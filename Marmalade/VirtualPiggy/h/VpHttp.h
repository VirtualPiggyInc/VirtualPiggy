#if !defined(_VP_HTTP_H_)
#define _VP_HTTP_H_

#include <IwList.h> 
#include "s3e.h"
#include "IwHTTP.h"

#include "VpCommon.h"
#include "VpString.h"

class VpHttpManager;

//
//
// VpHttpHeader - An http header
// 
//
class VpHttpHeader
{
	// Properties
private:
	VpString			Header;			// Header name
	VpString			HeaderData;		// header data
public:
	void		setHeader(const char* header)
	{
		Header.setString(header);
	}
	VpString&			getHeader()		{ return Header; }
	void				setHeaderData(const char* header_data)
	{
		HeaderData.setString(header_data);
	}
	const char*			getHeaderData() const { return HeaderData.c_str(); }
	// Properties end
public:
	VpHttpHeader()
	{
		Header.setAutoHash(true);
	}
	~VpHttpHeader()
	{
	}
};
typedef CIwList<VpHttpHeader*>::iterator VpHttpHeaderIterator;

//
//
// VpHttpPostData - http POST data
// 
//
class VpHttpPostData
{
private:
	CIwList<VpHttpHeader*>	Headers;		// Headers collection
	VpString				Body;			// POST body
public:
	VpHttpPostData() {}
	~VpHttpPostData()
	{
		ClearHeaders();
	}

	void SetHeader(const char* header, const char* header_data)
	{
		unsigned int header_hash = VpString::CalculateHash(header);

		// Check to see if header already present, if so update the header info
		for (VpHttpHeaderIterator it = Headers.begin(); it != Headers.end(); it++)
		{
			if (header_hash == (*it)->getHeader().getHash())
			{
				(*it)->setHeaderData(header_data);
				return;
			}
		}

		// Header was not already present so add new header
		VpHttpHeader* head = new VpHttpHeader();
		head->setHeader(header);
		head->setHeaderData(header_data);
		Headers.push_back(head);
	}
	void ClearHeaders()
	{
		for (VpHttpHeaderIterator it = Headers.begin(); it != Headers.end();  ++it)
			delete (*it);
		Headers.clear();
	}
	void setBody(const char* body)
	{
		Body.setString(body);
	}
	const VpString& getBody() const { return Body; }

	void ApplyHeaders(CIwHTTP* http_object)
	{
		for (VpHttpHeaderIterator it = Headers.begin(); it != Headers.end(); ++it)
		{
			http_object->SetRequestHeader((*it)->getHeader().c_str(), (*it)->getHeaderData());
		}
	}
};

//
//
// VpHttpRequest - An http request including the header, body and response
//
//
class VpHttpRequest
{
	// Properties
private:
	VpString				URI;			// URI end point
	VpString				Content;		// Receieved content
	int						ContentLength;	// Received content lenngth
	bool					Processed;		// Processed marker (becomes true when Http Manager has processed the request, this does not mean result is available)
	int						Error;			// Any errors that occured or 0 for none
	VpHttpPostData*			PostData;		// Collection of headers and the main body to POST
	VpCallback				ContentAvailableCallback;		// Callback to be called when content is available
	void*					ContentAvailableCallbackData;	// Data to be passed to the callback
	bool					Post;			// True if request is a POST, otherwise a GET
	bool					AutoDelete;		// Request is auto deleted if true
public:
	const VpString&				getURI() const							{ return URI; }
	void						setURI(const char* uri)
	{
		URI.setString(uri);
	}
	VpString&					getContent()							{ return Content; }
	void						setContent(char* content, int len)
	{
		Content.allocString(len);
		Content.Copy(content, 0, len);
		ContentLength = len;
	}
	void						allocContent(int len)
	{
		Content.allocString(len);
		Content.setLength(len);
		ContentLength = len;
	}
	void						reallocContent(int new_len)
	{
		Content.reallocString(new_len);
		Content.setLength(new_len);
		ContentLength = new_len;
	}
	int							getContentLength() const							{ return ContentLength; }
	bool						getProcessed() const								{ return Processed; }
	void						setProcessed(bool processed)						{ Processed = processed; }
	void						SetHeader(const char* header, const char* header_data) { PostData->SetHeader(header, header_data); }
	void						ClearHeaders()										{ PostData->ClearHeaders(); }
	void						setBody(const char* body)							{ PostData->setBody(body); }
	const VpString&				getBody() const										{ return PostData->getBody(); }
	void						setContentAvailableCallback(VpCallback callback, void *data)	{ ContentAvailableCallback = callback; ContentAvailableCallbackData = data; }
	void						setPOST()											{ Post = true; }
	void						setGET()											{ Post = false; }
	bool						isPOST() const										{ return Post; }
	void						setAutoDelete(bool enable)							{ AutoDelete = enable; }
	// Properties end

private:

public:
	VpHttpRequest() : ContentLength(0), Processed(false), Error(0), ContentAvailableCallback(NULL), ContentAvailableCallbackData(NULL), Post(false), AutoDelete(false) { PostData = new VpHttpPostData(); }
	~VpHttpRequest()
	{
		SAFE_DELETE(PostData)
	}

	void setError(int error)				{ Error = error; }
	int getError() const					{ return Error; }	

	void EndRequest(int error);
	void ApplyHeaders(CIwHTTP* http_object) { PostData->ApplyHeaders(http_object); }

};

//
//
// VpHttpManager - Handles the queueing of http requests
//
//
class VpHttpManager
{
protected:
	typedef CIwList<VpHttpRequest*>::iterator _Iterator;
	CIwList<VpHttpRequest*> Requests;		// Request queue (caller owns requests)

	// Properties
	VpHttpRequest*			CurrentRequest;	// Current request thats being processed or null if not busy
	CIwHTTP*				HttpObject;		// The Marmalade SDK Http Object
	VpString				UserAgent;		// Browser style user-agent
	VpString				IPAddress;		// IP address of device
public:
	VpHttpRequest*			getCurrentRequest()						{ return CurrentRequest; }
	void					setNoRequest()							{ CurrentRequest = NULL; }
	bool					GetHeader(const char* header_name, VpString* header_data);
	CIwHTTP*				getHttpObject()							{ return HttpObject; }
	void					setUserAgent(const char* user_agent)	{ UserAgent.setString(user_agent); }	// Aoto determined but can be changed if needed
	VpString&				getUserAgent()							{ return UserAgent; }
	void					setIPAddress(const char* ip_address)	{ IPAddress.setString(ip_address); }	// Aoto determined but can be changed if needed
	VpString&				getIPAddress()							{ return IPAddress; }
	// Properties end

protected:
public:
	void					Init();
	void					Release();
	void					AddRequest(VpHttpRequest* request)
	{
		Requests.push_back(request);
		request->setProcessed(false);
	}
	void					RemoveRequest(VpHttpRequest* request)
	{
		for (_Iterator it = Requests.begin(); it != Requests.end(); it++)
		{
			if (*it == request)
			{
				Requests.erase(it);
				break;
			}
		}
	}
	void					ClearRequests()
	{
		Requests.clear();
	}
	void					CancelRequests()
	{
		ClearRequests();
		if (HttpObject != NULL)
			HttpObject->Cancel();
	}
	
	s3eResult				GetStatus() const			{ return HttpObject->GetStatus(); }
	int						GetContentReceived() const	{ return HttpObject->ContentReceived(); }
	int						GetContentLength() const	{ return HttpObject->ContentLength(); }
	int						GetContentExpected() const	{ return HttpObject->ContentExpected(); }
//	int						ReadContent(char *buf, int max_bytes, s3eCallback callback = NULL, void *cb_data = NULL) { return HttpObject->ReadContent(buf, max_bytes, callback, cb_data); }
	int						ReadContent(char *buf, int max_bytes, s3eCallback callback = NULL, void *cb_data = NULL) { HttpObject->ReadDataAsync(buf, max_bytes, 10000, callback); return 0; }
	void					Update();
	bool					IsEmpty() const				{ return Requests.size() > 0; }
};



#endif // _VP_HTTP_H_