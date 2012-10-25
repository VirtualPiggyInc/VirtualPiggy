#ifndef __VP_STRING_H__
#define __VP_STRING_H__

#include <string>
#include "IwList.h"
#include "IwHashString.h"
#include "VpCommon.h"

//
//
// VpString - A simple named string builder style class
//
// NOTE: If you are building strings then its best to create a string of the size you think you will need then concatenate it, this will save lots of memory allocations and fragmentation
// In short, new a VpString() and then allocate some space for it using allocString(size_you_think_you_will_need)
//
//

class VpString
{
	// Properties
protected:
	char*			Data;			// The string data
	int				Length;			// Length of the string
	int				Size;			// Size of memory reserved for the string
	unsigned int	DataHash;		// Hash value of the data (for fast comparison)
	bool			AutoHash;		// true to calculate hash value every time string is updated
	int				FindIndex;		// Used to track multiple searches

public:
	void			reallocString(int len);
	void			allocString(int len);
	void			reset();
	void			setString(const char *str);
	void			setString(const char *str, int len);
	char*			getString()									{ return Data; }
	VpString		getSubString(int start, int max_chars);
	VpString		getSubString(int start);
	unsigned int	getHash() const								{ return DataHash; }
	void			setAutoHash(bool enable);
	void			setLength(int length)						{ Length = length; }
	bool			isAutohash() const							{ return AutoHash; }
	// Properties end

private:

public:
	VpString() : Data(NULL), Length(0), Size(0), AutoHash(true), FindIndex(0), DataHash(0) {}
	VpString(const VpString &string);
	VpString(const char *pString, int start, int num_chars);
	VpString(const char *pString);
	VpString(int nNum);
	VpString(unsigned int nNum);
	VpString(float fNum);
	VpString(bool value);
	virtual ~VpString()
	{
		SAFE_DELETE_ARRAY(Data);
	}

	// Query
	int				GetSize() const { return Size; }
	int				GetLength() const { return Length; }
	char*			GetCharPtr() { return Data; }
	int				GetAsInt() const;
	bool			GetAsBool() const;
	float			GetAsFloat() const;
	int				GetAsListOfInt(int *int_pool);
	int				GetAsListOfFloat(float* float_pool);
	bool			IsEmpty() const { return Data == NULL || Length == 0; }
	
	/// Copy
	void			Copy(const char* pString);
	void			Copy(VpString& pString);
	void			Copy(const char *pString, int start, int count);

	// Operators
	VpString&		operator=	(const VpString& op);
	VpString&		operator=	(const char *op);
	char			operator[]	(int nIndex);
	VpString&		operator+=	(const VpString& op);
	VpString&		operator+=	(const char* op);
	VpString&		operator=	(int nNum);
	VpString&		operator=	(float nNum);
	VpString&		operator=	(char chr);
	VpString&		operator+=	(int nNum);
	VpString&		operator+=	(float nNum);
	VpString&		operator+=	(char chr);
	bool			operator==	(const VpString& op);
	bool			operator==	(const char* op);
	bool			operator==	(unsigned int hash);
	char*			str() const { return Data; }
	const char*		c_str() const { return (const char *)Data; }

	bool			Compare(const char* pString, int len) const;
	bool			Compare(int start, const char* pString, int len) const;

	// Searching
	int				Find(const char* string);											// Simple string search
	int				FindNext(const char* string, int len);								// Searches from last find position for text string
	int				FindNext(const char* string);										// Searches from last find position for text string
	int				FindNext(char chr);													// Searches from last find position for the specified character
	int				FindNext2(char chr1, char chr2);									// Searches from last find position for the specified characters
	void			FindReset();														// Resets the find position to start of string
	int				StepFindIndex(int amount);											// Adjust the find position by the specified
	int				StepFindIndexNoneWhiteSpace();										// Adjust the find position to the next none white space
	int				StepFindIndexWhiteSpace();											// Adjust the find position to the next white space
	void			setFindIndex(int index) { FindIndex = index; }						// Sets the current find index
	int				getFindIndex() const { return FindIndex; }							// Gets the current find index
	int				GetNextMarkerOffset(char marker);									// Returns length to the end of paramneter marker
	int				GetNextString();													// Returns the next string
	int				GetNextMarkedString(char start_mark, char end_mark, int &offset);	// Returns a string marked by start and end marker characters
	int				GetNextMarkedStringAfterString(const char* search_string, char start_mark, char end_mark, VpString& out_string);	// Returns the next marked string after finding a certain string

	// Utility
	int				Replace(const char* string, const char* with);
	void			Replace(char chr, char with);
	bool			ContainsSpecialCharacters() const;
	int				Contains(char c) const;
	void			Encode();
	void			HTMLEncode();
	void			HTMLDecode();
	void			URLEncode(const char* str);
	void			URLEncode();
	void			URLDecode();
	bool			ExtractValueFromTag(const char* tag, VpString& value);
	bool			ExtractValueFromTag(const char* tag, bool& value);
	bool			ExtractValueFromTag(const char* tag, float& value);
	bool			ExtractValueFromTag(const char* tag, int& value);

	static unsigned int	CalculateHash(const char* pString, int hash = 5381);

	inline static bool IsNumber(char c)
	{
		if (c >= '0' && c <= '9')
			return true;
		return false;
	}

	inline static bool IsAlpha(char c)
	{
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
			return true;
		return false;
	}

	inline static bool IsLower(char c)
	{
		if (c >= 'a' && c <= 'z')
			return true;
		return false;
	}

	inline static bool IsUpper(char c)
	{
		if (c >= 'A' && c <= 'Z')
			return true;
		return false;
	}

	inline static int GetValueFromHexDigit(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;
		if (c >= 'A' && c <= 'F')
			return c - 'A' + 10;

		return 0;
	}
	
	void		ToUpper();
	void		ToLower();
};


#endif	// __VP_STRING_H__
