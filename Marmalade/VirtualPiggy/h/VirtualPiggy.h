#ifndef _VIRTUAL_PIGGY_H_
#define _VIRTUAL_PIGGY_H_

#include "VpCommon.h"
#include "VpHttp.h"

#define	VIRTUAL_PIGGY	(VirtualPiggy::getInstance())

//
// VpChild - Contains an entity
//
struct VpChild
{
	VpString	Identifier;				// Child identifier
	VpString	Name;					// Name of entity
	VpString	Type;					// Type
};

//
// VpPaymentAccount - Contains a users payment account
//
struct VpPaymentAccount
{
	VpString	Identifier;				// Payment identifier
	VpString	Name;					// Name of entity
	VpString	Type;					// Type
//	VpString	Url;					// Url ????
};

//
// VpBasketItem - Represents a basket item that the user has added to their basket
//
struct VpBasketItem
{
	VpString	Name;					// Name of item
	VpString	Description;			// Description of item
	float		Price;					// Price of item
	int			Quantity;				// Number of this item

	VpBasketItem() : Price(0), Quantity(0) {}

	VpBasketItem(const char* name, const char* description, float price, int quantity)
	{
		Name = name;
		Description = description;
		Price = price;
		Quantity = quantity;
	}
};

//
// VpBasket - Represents a complete basket of data that the user is wishing to purchase
//
struct VpBasket
{
protected:
	CIwList<VpBasketItem*>		Items;			// Items in the basket
public:
	typedef CIwList<VpBasketItem*>::iterator _Iterator;
	_Iterator					begin() { return Items.begin(); }
	_Iterator					end() { return Items.end(); }
	~VpBasket()					{ removeAllItems(); }

	VpString					Currency;		// Currency used in the transation (USD, GBP for example)
	float						TotalCost;		// Total cost of all items in this transaction

	void						addItem(VpBasketItem* item)
	{
		Items.push_back(item);
	}
	void						removeItem(VpBasketItem* item)
	{
		for (_Iterator it = begin(); it != end(); ++it)
		{
			if (*it == item)
			{
				Items.erase(it);
				delete *it;
				return;
			}
		}
	}
	void						removeAllItems()
	{
		for (_Iterator it = begin(); it != end(); ++it)
			delete *it;
		Items.clear();
	}
	void						CalculateTotalCost()
	{
		TotalCost = 0;
		for (_Iterator it = begin(); it != end(); ++it)
			TotalCost += (*it)->Price * (float)(*it)->Quantity;
	}
	int							getItemCount() const			{ return Items.size(); }
};

//
// VpSubscription - Represents a subscription
//
struct VpSubscription
{
	VpString	InitialCostCurrency;	// Name of currency that will be used for initial cost
	float		InitialCostValue;		// The initial cost
	VpString	Period;					// How long the subscription lasts (e.g. Weekly, Monthly)
	int			ExpiryInstances;		// ????
	VpBasket	Basket;					// Details of the subscription item

	VpSubscription() : InitialCostValue(0), ExpiryInstances(0) {}

	VpSubscription(const char* currency, float price)
	{
		InitialCostCurrency = currency;
		InitialCostValue = price;
	}
};

//
// VpAuthenticateChildResult - Contains AuthenticateChild result
//
struct VpAuthenticateChildResult
{
	VpString	ErrorMessage;		// Error message returned, empty if none
	bool		Status;				// true if success, false if not
	VpString	Token;				// Session token, you should store this as it will be required for future calls to the API
	VpString	TransactionStatus;	// ????
};
typedef int (*VpAuthenticateChildCallback)(VpAuthenticateChildResult* result);

//
// VpAuthenticateUserResult - Contains AuthenticateUser result
//
struct VpAuthenticateUserResult
{
	VpString	ErrorMessage;		// Error message returned, empty if none
	bool		Status;				// true if success, false if not
	VpString	Token;				// Session token, you should store this as it will be required for future calls to the API
	VpString	TransactionStatus;	// ????
};
typedef int (*VpAuthenticateUserCallback)(VpAuthenticateUserResult* result);

//
// VpProcessTransactionResult - Contains ProcessTransaction result
//
struct VpProcessTransactionResult
{
	VpString	DataXml;				// Returned data
	VpString	ErrorMessage;			// Error message returned, empty if none
	bool		Status;					// true if success, false if not
	VpString	TransactionIdentifier;	// ????
	VpString	TransactionStatus;		// ????
};
typedef int (*VpProcessTransactionCallback)(VpProcessTransactionResult* result);

//
// VpProcessParentTransactionResult - Contains ProcessParentTransaction result
//
struct VpProcessParentTransactionResult
{
	VpString	DataXml;				// Returned data
	VpString	ErrorMessage;			// Error message returned, empty if none
	bool		Status;					// true if success, false if not
	VpString	TransactionIdentifier;	// ????
	VpString	TransactionStatus;		// ????
};
typedef int (*VpProcessParentTransactionCallback)(VpProcessParentTransactionResult* result);

//
// VpGetAllChildrenResult - Contains GetAllChildren result
//
struct VpGetAllChildrenResult
{
	typedef CIwList<VpChild*>::iterator	_Iterator;
	_Iterator begin()	{ return Children.begin(); }
	_Iterator end()		{ return Children.end(); }
	CIwList<VpChild*>	Children;		// List of children

	~VpGetAllChildrenResult()
	{
		for (_Iterator it = Children.begin(); it != Children.end(); ++it)
			delete *it;
	}
	VpChild*				findChild(const char* child_name)
	{
		unsigned int name_hash = VpString::CalculateHash(child_name);
		for (_Iterator it = Children.begin(); it != Children.end(); ++it)
		{
			if ((*it)->Name.getHash() == name_hash)
				return *it;
		}
		return NULL;
	}
};
typedef int (*VpGetAllChildrenCallback)(VpGetAllChildrenResult* result);

//
// VpGetPaymentAccountsResult - Contains GetPaymentAccounts result
//
struct VpGetPaymentAccountsResult
{
	typedef CIwList<VpPaymentAccount*>::iterator	_Iterator;
	_Iterator begin()	{ return Accounts.begin(); }
	_Iterator end()		{ return Accounts.end(); }
	CIwList<VpPaymentAccount*>	Accounts;		// List of accounts

	~VpGetPaymentAccountsResult()
	{
		for (_Iterator it = Accounts.begin(); it != Accounts.end(); ++it)
			delete *it;
	}
	VpPaymentAccount*		findAccount(const char* account_name)
	{
		unsigned int name_hash = VpString::CalculateHash(account_name);
		for (_Iterator it = Accounts.begin(); it != Accounts.end(); ++it)
		{
			if ((*it)->Name.getHash() == name_hash)
				return *it;
		}
		return NULL;
	}
};
typedef int (*VpGetPaymentAccountsCallback)(VpGetPaymentAccountsResult* result);

//
// VpProcessSubscriptionResult - Contains ProcessSubscription result
//
struct VpProcessSubscriptionResult
{
	VpString	Identifier;				// An identity identifier
	VpString	Name;					// Name of entity
	VpString	Type;					// Type
};
typedef int (*VpProcessSubscriptionCallback)(VpProcessSubscriptionResult* result);

//
// VpProcessParentSubscriptionResult - Contains ProcessParentSubscription result
//
struct VpProcessParentSubscriptionResult
{
	VpString	Identifier;				// An identity identifier
	VpString	Name;					// Name of entity
	VpString	Type;					// Type
};
typedef int (*VpProcessParentSubscriptionCallback)(VpProcessParentSubscriptionResult* result);

//
// VpGetChildAddressResult - Contains GetChildAddress result
//
struct VpGetChildAddressResult
{
	VpString	Address;			// Child address
	VpString	AttentionOf;		// Attention of Name
	VpString	City;				// City of residence
	VpString	Country;			// Country of residence
	VpString	ErrorMessage;		// Error message returned, empty if none
	VpString	Name;				// 
	VpString	ParentEmail;		// Parents email address
	VpString	ParentPhone;		// Parents telephone number
	VpString	State;				// State of residence
	VpString	Status;				// Status
	VpString	Zip;				// Zip code
};
typedef int (*VpGetChildAddressCallback)(VpGetChildAddressResult* result);

//
// VpGetChildGenderAgeResult - Contains GetChildGenderAge result
//
struct VpGetChildGenderAgeResult
{
	int			Age;				// Childs age
	VpString	ErrorMessage;		// Error message returned, empty if none
	VpString	Gender;				// Gender of child
	bool		Status;				// true if success, false if not
};
typedef int (*VpGetChildGenderAgeCallback)(VpGetChildGenderAgeResult* result);

//
// VpGetLoyaltyBalanceResult - Contains GetLoyaltyBalance result
//
struct VpGetLoyaltyBalanceResult
{
	int			Balance;			// Childs loyalty balance points
};
typedef int (*VpGetLoyaltyBalanceCallback)(VpGetLoyaltyBalanceResult* result);

//
// VpGetParentAddressResult - Contains GetParentAddress result
//
struct VpGetParentAddressResult
{
	VpString	Address;			// Parent address
	VpString	AttentionOf;		// Attention of Name
	VpString	City;				// City of residence
	VpString	Country;			// Country of residence
	VpString	ErrorMessage;		// Error message returned, empty if none
	VpString	Name;				// 
	VpString	ParentEmail;		// Parents email address
	VpString	ParentPhone;		// Parents telephone number
	VpString	State;				// State of residence
	VpString	Status;				// Status
	VpString	Zip;				// Zip code
};
typedef int (*VpGetParentAddressCallback)(VpGetParentAddressResult* result);

//
// VpGetParentChildAddressResult - Contains GetParentChildAddress result
//
struct VpGetParentChildAddressResult
{
	VpString	Address;			// Childs address
	VpString	AttentionOf;		// Attention of Name
	VpString	City;				// City of residence
	VpString	Country;			// Country of residence
	VpString	ErrorMessage;		// Error message returned, empty if none
	VpString	Name;				// 
	VpString	ParentEmail;		// Parents email address
	VpString	ParentPhone;		// Parents telephone number
	VpString	State;				// State of residence
	VpString	Status;				// Status
	VpString	Zip;				// Zip code
};
typedef int (*VpGetParentChildAddressCallback)(VpGetParentChildAddressResult* result);

//
// VpMerchantCancelSubscriptionResult - Contains MerchantCancelSubscription result
//
struct VpMerchantCancelSubscriptionResult
{
	VpString	Result;				// Cancel subscription result
};
typedef int (*VpMerchantCancelSubscriptionCallback)(VpMerchantCancelSubscriptionResult* result);

//
// VpPingHeadersResult - Contains PingHeaders result
//
struct VpPingHeadersResult
{
	bool		Success;			// True if ping succeeded or false if not
};
typedef int (*VpPingHeadersCallback)(VpPingHeadersResult* result);

//
//
//
// VirtualPiggy - Main contol class
//
//
//
class VirtualPiggy
{
	// Singleton - for single instance and global access
	static VirtualPiggy* _instance;
	VirtualPiggy() {}
	~VirtualPiggy() {}
	VirtualPiggy(const VirtualPiggy &);
	VirtualPiggy& operator=(const VirtualPiggy &);
public:
	static void Create();
	static void Destroy();
	static VirtualPiggy* getInstance();
	// Singleton end

	// Properties
private:
	VpString			MerchantID;			// The merchants ID
	VpString			APIKey;				// The merchants API key
	VpString			LastResponse;		// The last SOAP response (can be used by developers to see exceptions)
public:
	bool				isAvailable() const;
	void				setMerchantID(const char* merchant_id)	{ MerchantID = merchant_id; BuildSoapHeader(); }
	// Properties end

	// Internal
	void				setLastResponse(const VpString& response)		{ LastResponse = response; }

private:
	VpHttpManager*		HttpManager;
	VpString			SoapHeader;
	bool				BuildSoapHeader();
	void				CallSoapMethod(const char* method, VpString& soap, VpCallback response_callback, void *callback_data);
	void				BuildBasketXML(VpString& basket_xml, VpBasket* basket);
	void				BuildSubscriptionXML(VpString& subscription_xml, VpString& basket_xml, VpSubscription* subscription);

public:
	bool	Init(const char* api_key);
	void	Release();
	void	Update();

	VpHttpManager*		getHttpManager()	{ return HttpManager; }

	//
	// Authentication
	//
	// AuthenticateChild - Authenticates a child
	// * user_name		- Childs user name
	// * password		- Childs password
	// * callback		- A callback that is called when the authentication is complete or fails
	void				AuthenticateChild(const char* user_name, const char* password, VpAuthenticateChildCallback callback);
	// AuthenticateUser - Authenticates a user
	// * user_name		- Users user name
	// * password		- Users password
	// * callback		- A callback that is called when the authentication is complete or fails
	void				AuthenticateUser(const char* user_name, const char* password, VpAuthenticateUserCallback callback);

	//
	// Transactions
	//
	// ProcessTransaction - Makes a purchase as a child user
	// * token			- Users token (returned from AuthenticateChild)
	// * description	- Description of transaction
	// * basket			- Goods that th child wants to purchase
	// * callback		- A callback that is called when the purchase is complete or fails
	void				ProcessTransaction(const char* token, const char* description, VpBasket* basket, VpProcessTransactionCallback callback);
	// ProcessParentTransaction - Makes a purchase as a parent user
	// * token			- Users token (returned from AuthenticateUser)
	// * description	- Description of transaction
	// * child_id		- ID of child that user is purchasing goods for (retrieved from GetAllChildren)
	// * payment_id		- ID of paynment that user is usnig to make purchase (retrieved from GetPaymentAccounts)
	// * basket			- Goods that user wants to purchasse for the child
	// * callback		- A callback that is called when the purchase is complete or fails
	void				ProcessParentTransaction(const char* token, const char* description, const char* child_id, const char* payment_id, VpBasket* basket, VpProcessParentTransactionCallback callback);

	//
	// Subscriptions
	//
	// ProcessSubscription - Buy a subscription as a child user
	// * token			- Users token (returned from AuthenticateChild)
	// * description	- Description of Subscription
	// * basket			- Goods that th child wants to purchase
	// * callback		- A callback that is called when the purchase is complete or fails
	void				ProcessSubscription(const char* token, const char* description, VpSubscription* subscription, VpProcessSubscriptionCallback callback);
	// ProcessParentSubscription - Buy a subscription as a parent user
	// * token			- Users token (returned from AuthenticateUser)
	// * description	- Description of Subscription
	// * child_id		- ID of child that user is purchasing goods for (retrieved from GetAllChildren)
	// * payment_id		- ID of paynment that user is usnig to make purchase  (retrieved from GetPaymentAccounts)
	// * basket			- Goods that user wants to purchasse for the child
	// * callback		- A callback that is called when the purchase is complete or fails
	void				ProcessParentSubscription(const char* token, const char* description, const char* child_id, const char* payment_id, VpSubscription* subscription, VpProcessParentSubscriptionCallback callback);
	// MerchantCancelSubscription - Cancels a subscription
	// * subscription_id- ID of subscription to cancel (the Identifier returned by ProcessParentSubscription and ProcessSubscription)
	// * callback		- A callback that is called when the details are available
	void				MerchantCancelSubscription(const char* subscription_id, VpMerchantCancelSubscriptionCallback callback);

	//
	// Query
	//
	// GetAllChildren - Gets a list of childrn associated with the user
	// * token			- Parents token (returned from AuthenticateUser)
	// * callback		- A callback that is called when the list is available
	void				GetAllChildren(const char* token, VpGetAllChildrenCallback callback);
	// GetPaymentAccounts - Gets a list of a parents payment accounts
	// * token			- Parents token (returned from AuthenticateUser)
	// * callback		- A callback that is called when the list is available
	void				GetPaymentAccounts(const char* token, VpGetPaymentAccountsCallback callback);
	// GetChildAddress - Gets the childs address details
	// * token			- Childs token (returned from AuthenticateChild)
	// * callback		- A callback that is called when the details are available
	void				GetChildAddress(const char* token, VpGetChildAddressCallback callback);
	// GetChildGenderAge - Gets the childs gender and age
	// * token			- Childs token (returned from AuthenticateChild)
	// * callback		- A callback that is called when the details are available
	void				GetChildGenderAge(const char* token, VpGetChildGenderAgeCallback callback);
	// GetLoyaltyBalance - Gets the childs loyalty balance
	// * token			- Childs token (returned from AuthenticateChild)
	// * callback		- A callback that is called when the details are available
	void				GetLoyaltyBalance(const char* token, VpGetLoyaltyBalanceCallback callback);
	// GetParentAddress - Gets the parents address details
	// * token			- Parents token (returned from AuthenticateUser)
	// * callback		- A callback that is called when the details are available
	void				GetParentAddress(const char* token, VpGetParentAddressCallback callback);
	// GetParentChildAddress - Gets the parents specified childs address details
	// * token			- Parents token (returned from AuthenticateUser)
	// * child_id		- ID of child to return address details for (retrieved from GetAllChildren)
	// * callback		- A callback that is called when the details are available
	void				GetParentChildAddress(const char* token, const char* child_id, VpGetParentChildAddressCallback callback);
	// PingHeaders - Pings the Virtual Piggy server
	void				PingHeaders(VpPingHeadersCallback callback);

	// Uitlity
	const VpString&		getLastResponse() const		{ return LastResponse; }

};



#endif	// _VIRTUAL_PIGGY_H_