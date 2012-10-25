#include "s3e.h"
#include "VirtualPiggy.h"

#define VIRTUAL_PIGGY_API_KEY		"mat@pocketeers.co.uk123"
#define VIRTUAL_PIGGY_MERCHANT_ID	"32a01284-6812-4f5a-8f91-1b16b515be0a"

VpString					g_Token;
VpString					g_ChildID;
VpBasket					g_Basket;
VpSubscription				g_Subscription;
VpString					g_SubscriptionID;

//
//
// Utility functins
//
//
void MySetupBasket(VpBasket* backet)
{
	backet->Currency = "USD";
	backet->removeAllItems();
	backet->addItem(new VpBasketItem("10 Coins", "10 Golden Coins", 0.99f, 1));
	backet->CalculateTotalCost();
}

void MySetupSubscription(VpSubscription* subscription)
{
	subscription->InitialCostCurrency = "USD";
	subscription->InitialCostValue = 1.99f;
	subscription->Period = "Weekly";
	subscription->ExpiryInstances = 1;
	MySetupBasket(&subscription->Basket);
}

//
//
//
// Test child authentication and purchase
// Tests the following:
// - AuthenticateChild
// - ProcessTransaction
//
//
//
int MyProcessTransactionCallback(VpProcessTransactionResult* result)
{
	// Display result
	printf(">>>> MyProcessTransactionCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- DataXml: %s\n", result->DataXml.c_str());
	printf("---- TransactionIdentifier: %s\n", result->TransactionIdentifier.c_str());
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());

	return 1;
}
int MyAuthenticateChildForTransactionCallback(VpAuthenticateChildResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateChildCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	// Make a purchase
	MySetupBasket(&g_Basket);
	VIRTUAL_PIGGY->ProcessTransaction(result->Token.c_str(), "Test", &g_Basket, MyProcessTransactionCallback);

	return 1;
}
void TEST_ChildPurchase()
{
	printf("******** TEST_ChildPurchase:\n");
	VIRTUAL_PIGGY->AuthenticateChild("Mat", "potter2012", MyAuthenticateChildForTransactionCallback);
}

//
//
//
// Test parent authentication and purchase
// Tests the following:
// - AuthenticateUser
// - GetAllChildren
// - GetPaymentAccounts
// - ProcessParentTransaction
//
//
//
int MyProcessParentTransactionCallback(VpProcessParentTransactionResult* result)
{
	// Display result
	printf(">>>> MyProcessParentTransactionCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- DataXml: %s\n", result->DataXml.c_str());
	printf("---- TransactionIdentifier: %s\n", result->TransactionIdentifier.c_str());
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());

	return 1;
}
int MyGetPaymentAccountsCallback(VpGetPaymentAccountsResult* result)
{
	// Display list of payment accounts
	printf(">>>> MyGetPaymentAccountsCallback:\n");
	for (VpGetPaymentAccountsResult::_Iterator it = result->begin(); it != result->end(); ++it)
		printf("---- Account: %s with ID = %s\n", (*it)->Name.c_str(), (*it)->Identifier.c_str());

	// Get first available payment account
	VpPaymentAccount* account = *(result->begin());
	if (account != NULL)
	{
		// Make a purchase for the child
		MySetupBasket(&g_Basket);
		VIRTUAL_PIGGY->ProcessParentTransaction(g_Token.c_str(), "Test", g_ChildID.c_str(), account->Identifier.c_str(), &g_Basket, MyProcessParentTransactionCallback);
	}

	return 1;
}
int MyGetAllChildrenCallback(VpGetAllChildrenResult* result)
{
	// Display list of children
	printf(">>>> MyGetAllChildrenCallback:\n");
	for (VpGetAllChildrenResult::_Iterator it = result->begin(); it != result->end(); ++it)
		printf("---- Chlld: %s with ID = %s\n", (*it)->Name.c_str(), (*it)->Identifier.c_str());

	// Find the Mat child and make a purchase for them
	VpChild* child = result->findChild("Mat");
	if (child != NULL)
	{
		// Get payment accounts
		g_ChildID = child->Identifier;
		VIRTUAL_PIGGY->GetPaymentAccounts(g_Token.c_str(), MyGetPaymentAccountsCallback);
	}

	return 1;
}
int MyAuthenticateUserCallback(VpAuthenticateUserResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateUserCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	g_Token = result->Token;

	// Get list of children that parent can purchase items for
	VIRTUAL_PIGGY->GetAllChildren(g_Token.c_str(), MyGetAllChildrenCallback);

	return 1;
}
void TEST_ParentPurchase()
{
	printf("******** TEST_ParentPurchase:\n");
	VIRTUAL_PIGGY->AuthenticateUser("harry", "potter2012", MyAuthenticateUserCallback);
}

//
//
//
// Test child authentication and subscription
// Tests the following:
// - AuthenticateChild
// - ProcessSubscription
//
//
//
int MyProcessSubscriptionCallback(VpProcessSubscriptionResult* result)
{
	// Display result
	printf(">>>> VpProcessSubscriptionResult:\n");
	printf("---- Identifier: %s\n", result->Identifier.c_str());
	printf("---- Name: %s\n", result->Name.c_str());
	printf("---- Type: %s\n", result->Type.c_str());

	return 1;
}
int MyAuthenticateChildForSubscriptionCallback(VpAuthenticateChildResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateChildForSubscriptionCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	// Make a purchase
	MySetupSubscription(&g_Subscription);
	VIRTUAL_PIGGY->ProcessSubscription(result->Token.c_str(), "Test", &g_Subscription, MyProcessSubscriptionCallback);

	return 1;
}
void TEST_ChildPurchaseSubscription()
{
	printf("******** TEST_ChildPurchaseSubscription:\n");
	VIRTUAL_PIGGY->AuthenticateChild("Mat", "potter2012", MyAuthenticateChildForSubscriptionCallback);
}

//
//
//
// Test parent authentication and purchase
// Tests the following:
// - AuthenticateUser
// - GetAllChildren
// - GetPaymentAccounts
// - ProcessParentSubscription
//
//
//
int MyProcessParentSubscriptionCallback(VpProcessParentSubscriptionResult* result)
{
	// Display result
	printf(">>>> MyProcessParentSubscriptionCallback:\n");
	printf("---- Identifier: %s\n", result->Identifier.c_str());
	printf("---- Name: %s\n", result->Name.c_str());
	printf("---- Type: %s\n", result->Type.c_str());

	return 1;
}
int MyGetPaymentAccountsForSubscriptionCallback(VpGetPaymentAccountsResult* result)
{
	// Display list of children
	printf(">>>> MyGetPaymentAccountsForSubscriptionCallback:\n");
	for (VpGetPaymentAccountsResult::_Iterator it = result->begin(); it != result->end(); ++it)
		printf("---- Account: %s with ID = %s\n", (*it)->Name.c_str(), (*it)->Identifier.c_str());

	// Get first available payment account
	VpPaymentAccount* account = *(result->begin());
	if (account != NULL)
	{
		// Make a purchase for the child
		MySetupSubscription(&g_Subscription);
		VIRTUAL_PIGGY->ProcessParentSubscription(g_Token.c_str(), "Test", g_ChildID.c_str(), account->Identifier.c_str(), &g_Subscription, MyProcessParentSubscriptionCallback);
	}

	return 1;
}
int MyGetAllChildrenForSubscriptionCallback(VpGetAllChildrenResult* result)
{
	// Display list of children
	printf(">>>> MyGetAllChildrenForSubscriptionCallback:\n");
	for (VpGetAllChildrenResult::_Iterator it = result->begin(); it != result->end(); ++it)
		printf("---- Chlld: %s with ID = %s\n", (*it)->Name.c_str(), (*it)->Identifier.c_str());

	// Find the Mat child and make a purchase for them
	VpChild* child = result->findChild("Mat");
	if (child != NULL)
	{
		// Get payment accounts
		g_ChildID = child->Identifier;
		VIRTUAL_PIGGY->GetPaymentAccounts(g_Token.c_str(), MyGetPaymentAccountsForSubscriptionCallback);
	}

	return 1;
}
int MyAuthenticateUserForSubscriptionCallback(VpAuthenticateUserResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateUserForSubscriptionCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	g_Token = result->Token;

	// Get list of children that parent can purchase items for
	VIRTUAL_PIGGY->GetAllChildren(g_Token.c_str(), MyGetAllChildrenForSubscriptionCallback);

	return 1;
}
void TEST_ParentPurchaseSubscription()
{
	printf("******** TEST_ParentPurchaseSubscription:\n");
	VIRTUAL_PIGGY->AuthenticateUser("harry", "potter2012", MyAuthenticateUserForSubscriptionCallback);
}

//
//
//
// Test get child address
// Tests the following:
// - AuthenticateChild
// - GetChildAddress
//
//
//
int MyGetChildAddressCallback(VpGetChildAddressResult* result)
{
	// Display result
	printf(">>>> MyGetChildAddressCallback:\n");
	printf("---- Address: %s\n", result->Address.c_str());
	printf("---- AttentionOf: %s\n", result->AttentionOf.c_str());
	printf("---- City: %s\n", result->City.c_str());
	printf("---- Country: %s\n", result->Country.c_str());
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Name: %s\n", result->Name.c_str());
	printf("---- ParentEmail: %s\n", result->ParentEmail.c_str());
	printf("---- ParentPhone: %s\n", result->ParentPhone.c_str());
	printf("---- State: %s\n", result->State.c_str());
	printf("---- Status: %s\n", result->Status.c_str());
	printf("---- Zip: %s\n", result->Zip.c_str());

	return 1;
}
int MyAuthenticateChildForGetChildAddressCallback(VpAuthenticateChildResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateChildForGetChildAddressCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	// Get childs address
	VIRTUAL_PIGGY->GetChildAddress(result->Token.c_str(), MyGetChildAddressCallback);

	return 1;
}
void TEST_GetChildAddress()
{
	printf("******** TEST_GetChildAddress:\n");
	VIRTUAL_PIGGY->AuthenticateChild("Mat", "potter2012", MyAuthenticateChildForGetChildAddressCallback);
}

//
//
//
// Test get child age and gender
// Tests the following:
// - AuthenticateChild
// - GetChildGenderAge
//
//
//
int MyGetChildGenderAgeCallback(VpGetChildGenderAgeResult* result)
{
	// Display result
	printf(">>>> MyGetChildGenderAgeCallback:\n");
	printf("---- Age: %d\n", result->Age);
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Gender: %s\n", result->Gender.c_str());
	printf("---- Status: %s\n", result->Status ? "true" : "false");

	return 1;
}
int MyAuthenticateChildForGetChildGenderAgeCallback(VpAuthenticateChildResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateChildForGetChildGenderAgeCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	// Get childs age and gender
	VIRTUAL_PIGGY->GetChildGenderAge(result->Token.c_str(), MyGetChildGenderAgeCallback);

	return 1;
}
void TEST_GetChildGenderAge()
{
	printf("******** TEST_GetChildGenderAge:\n");
	VIRTUAL_PIGGY->AuthenticateChild("Mat", "potter2012", MyAuthenticateChildForGetChildGenderAgeCallback);
}

//
//
//
// Test get childs loyalty balance
// Tests the following:
// - AuthenticateChild
// - GetLoyaltyBalance
//
//
//
int MyGetLoyaltyBalanceCallback(VpGetLoyaltyBalanceResult* result)
{
	// Display result
	printf(">>>> MyGetLoyaltyBalanceCallback:\n");
	printf("---- Balance: %d\n", result->Balance);

	return 1;
}
int MyAuthenticateChildForGetLoyaltyBalanceCallback(VpAuthenticateChildResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateChildForGetLoyaltyBalanceCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	// Get childs loyalty balance
	VIRTUAL_PIGGY->GetLoyaltyBalance(result->Token.c_str(), MyGetLoyaltyBalanceCallback);

	return 1;
}
void TEST_GetLoyaltyBalance()
{
	printf("******** TEST_GetLoyaltyBalance:\n");
	VIRTUAL_PIGGY->AuthenticateChild("Mat", "potter2012", MyAuthenticateChildForGetLoyaltyBalanceCallback);
}

//
//
//
// Test get parent address
// Tests the following:
// - AuthenticateUser
// - GetParentAddress
//
//
//
int MyGetParentAddressCallback(VpGetParentAddressResult* result)
{
	// Display result
	printf(">>>> MyGetParentAddressCallback:\n");
	printf("---- Address: %s\n", result->Address.c_str());
	printf("---- AttentionOf: %s\n", result->AttentionOf.c_str());
	printf("---- City: %s\n", result->City.c_str());
	printf("---- Country: %s\n", result->Country.c_str());
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Name: %s\n", result->Name.c_str());
	printf("---- ParentEmail: %s\n", result->ParentEmail.c_str());
	printf("---- ParentPhone: %s\n", result->ParentPhone.c_str());
	printf("---- State: %s\n", result->State.c_str());
	printf("---- Status: %s\n", result->Status.c_str());
	printf("---- Zip: %s\n", result->Zip.c_str());

	return 1;
}
int MyAuthenticateForGetParentAddressCallback(VpAuthenticateUserResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateForGetParentAddressCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	// Get parents address
	VIRTUAL_PIGGY->GetParentAddress(result->Token.c_str(), MyGetParentAddressCallback);

	return 1;
}
void TEST_GetParentAddress()
{
	printf("******** TEST_GetParentAddress:\n");
	VIRTUAL_PIGGY->AuthenticateUser("harry", "potter2012", MyAuthenticateForGetParentAddressCallback);
}

//
//
//
// Test get parent address
// Tests the following:
// - AuthenticateUser
// - GetAllChildren
// - GetParentChildAddress
//
//
//
int MyGetParentChildAddressCallback(VpGetParentChildAddressResult* result)
{
	// Display result
	printf(">>>> MyGetParentChildAddressCallback:\n");
	printf("---- Address: %s\n", result->Address.c_str());
	printf("---- AttentionOf: %s\n", result->AttentionOf.c_str());
	printf("---- City: %s\n", result->City.c_str());
	printf("---- Country: %s\n", result->Country.c_str());
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Name: %s\n", result->Name.c_str());
	printf("---- ParentEmail: %s\n", result->ParentEmail.c_str());
	printf("---- ParentPhone: %s\n", result->ParentPhone.c_str());
	printf("---- State: %s\n", result->State.c_str());
	printf("---- Status: %s\n", result->Status.c_str());
	printf("---- Zip: %s\n", result->Zip.c_str());

	return 1;
}
int MyGetAllChildrenForGetParentChildAddressCallback(VpGetAllChildrenResult* result)
{
	// Display list of children
	printf(">>>> MyGetAllChildrenForGetParentChildAddressCallback:\n");
	for (VpGetAllChildrenResult::_Iterator it = result->begin(); it != result->end(); ++it)
		printf("---- Chlld: %s with ID = %s\n", (*it)->Name.c_str(), (*it)->Identifier.c_str());

	// Find the Mat child
	VpChild* child = result->findChild("Mat");
	if (child != NULL)
	{
		// Get child address
		VIRTUAL_PIGGY->GetParentChildAddress(g_Token.c_str(), child->Identifier.c_str(), MyGetParentChildAddressCallback);
	}

	return 1;
}
int MyAuthenticateForGetParentChildAddressCallback(VpAuthenticateUserResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateForGetParentChildAddressCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	g_Token = result->Token;

	// Get list of children for paremt
	VIRTUAL_PIGGY->GetAllChildren(result->Token.c_str(), MyGetAllChildrenForGetParentChildAddressCallback);

	return 1;
}
void TEST_GetParentChildAddress()
{
	printf("******** TEST_GetParentChildAddress:\n");
	VIRTUAL_PIGGY->AuthenticateUser("harry", "potter2012", MyAuthenticateForGetParentChildAddressCallback);
}

//
//
//
// Test child authentication and subscription
// Tests the following:
// - AuthenticateChild
// - ProcessSubscription
// - MerchantCancelSubscription
//
//
int MyProcessSubscriptionForSubscriptionCancelCallback(VpMerchantCancelSubscriptionResult* result)
{
	// Display result
	printf(">>>> MyProcessSubscriptionForSubscriptionCancelCallback:\n");
//	printf("---- Result: %s\n", result->Result.c_str());

	return 1;
}
int MyProcessSubscriptionForSubscriptionCancelCallback(VpProcessSubscriptionResult* result)
{
	// Display result
	printf(">>>> MyProcessSubscriptionForSubscriptionCancelCallback:\n");
	printf("---- Identifier: %s\n", result->Identifier.c_str());
	printf("---- Name: %s\n", result->Name.c_str());
	printf("---- Type: %s\n", result->Type.c_str());

	g_SubscriptionID = result->Identifier;
	VIRTUAL_PIGGY->MerchantCancelSubscription(g_SubscriptionID.c_str(), MyProcessSubscriptionForSubscriptionCancelCallback);

	return 1;
}
int MyAuthenticateChildForSubscriptionCancelCallback(VpAuthenticateChildResult* result)
{
	// Display result
	printf(">>>> MyAuthenticateChildForSubscriptionCancelCallback:\n");
	printf("---- ErrorMessage: %s\n", result->ErrorMessage.c_str());
	printf("---- Status: %d\n", result->Status);
	printf("---- TransactionStatus: %s\n", result->TransactionStatus.c_str());
	printf("---- Token: %s\n", result->Token.c_str());
	

	// Make a purchase
	MySetupSubscription(&g_Subscription);
	VIRTUAL_PIGGY->ProcessSubscription(result->Token.c_str(), "Test", &g_Subscription, MyProcessSubscriptionForSubscriptionCancelCallback);

	return 1;
}
void TEST_MerchantCancelSubscription()
{
	printf("******** TEST_MerchantCancelSubscription:\n");
	VIRTUAL_PIGGY->AuthenticateChild("Mat", "potter2012", MyAuthenticateChildForSubscriptionCancelCallback);
}

//
//
//
// Test child authentication and subscription
// Tests the following:
// - pingHeaders
//
//
//
int MyPingHeadersCallback(VpPingHeadersResult* result)
{
	// Display result
	printf(">>>> VpPingHeadersResult:\n");
	printf("---- Success: %s\n", result->Success ? "true" : "false");

	VpString last_reponse = VIRTUAL_PIGGY->getLastResponse();

	return 1;
}
void TEST_PingHeaders()
{
	printf("******** TEST_PingHeaders:\n");
	VIRTUAL_PIGGY->PingHeaders(MyPingHeadersCallback);
}


int main()
{
	// Start up Virtual Piggy
	VirtualPiggy::Create();
	VIRTUAL_PIGGY->Init(VIRTUAL_PIGGY_API_KEY);
	VIRTUAL_PIGGY->setMerchantID(VIRTUAL_PIGGY_MERCHANT_ID);

	// TESTS: Run each test individually
//	TEST_ParentPurchase();
//	TEST_ChildPurchase();
//	TEST_ChildPurchaseSubscription();
//	TEST_ParentPurchaseSubscription();
//	TEST_GetChildAddress();
//	TEST_GetChildGenderAge();
//	TEST_GetLoyaltyBalance();
//	TEST_GetParentAddress();
//	TEST_GetParentChildAddress();
//	TEST_MerchantCancelSubscription();
	TEST_PingHeaders();

	// Marmalade main loop
	while (!s3eDeviceCheckQuitRequest())
	{
		VIRTUAL_PIGGY->Update();
		s3eDeviceYield(0);
	}

	// Shut down Virtual Piggy
	VIRTUAL_PIGGY->Release();
	VirtualPiggy::Destroy();

    return 0;
}

