//#include  "app.h"
#include <stdio.h>
#include "../Card/card.h"
#include"../Terminal/terminal.h"
#include "../Server/server.h"
#include "../Card/card.c"
#include"../Terminal/terminal.c"
#include "../Server/server.c"

void appStart(void);


#define appStart main

int appStart(void)
{

    EN_cardError_t cardHolderName_f;
	EN_cardError_t cardExpiryDate_f;
	EN_cardError_t cardPAN_f;

	ST_cardData_t cardHolderData;
	ST_cardData_t* cardData = &cardHolderData;
    
    

	if ((cardHolderName_f = getCardHolderName(cardData) == WRONG_NAME))
	{
		printf("Wrong Name !");
		return 0;
	}

    if ((cardPAN_f = getCardPAN(cardData) == WRONG_PAN))
	{
		printf("Wrong PAN !");
		return 0;
	}

    if ((cardExpiryDate_f = getCardExpiryDate(cardData) == WRONG_EXP_DATE))
	{
		printf("Wrong Expiry Date !");
		return 0;
	}
	

	 EN_terminalError_t transactionDate_f;
	EN_terminalError_t cardEpired_f;
	EN_terminalError_t maxAmount_f;
	EN_terminalError_t transactionAmount_f;
	EN_terminalError_t belowMaxAmount_f;

	
	ST_terminalData_t terminalData;
	ST_terminalData_t* termData = &terminalData;
  

	if ((transactionDate_f = getTransactionDate(termData) == WRONG_DATE))
	{
		printf("Wrong Date !");
		return 0;
	}

	if ((cardEpired_f = isCardExpired(cardHolderData, terminalData) == EXPIRED_CARD))
	{
		printf("Your Card is Expired ");
		return 0;
	}

	if ((maxAmount_f = setMaxAmount(termData) == INVALID_MAX_AMOUNT))
	{
		printf("Invalid Max Amount");
		return 0;
	}

	if ((transactionAmount_f = getTransactionAmount(termData) == INVALID_AMOUNT))
	{
		printf("Invalid Amount ");
		return 0;
	}

	if ((belowMaxAmount_f = isBelowMaxAmount(termData) == EXCEED_MAX_AMOUNT))
	{
		printf("Exceeded Max Amount ");
		return 0;
	}

	
EN_transState_t recieveTransaction_f;

	// Transaction Data Struct
	ST_transaction_t transactonData;
	ST_transaction_t* transData = &transactonData;

	// Server Module: Get Card Data, and Terminal Data
	transData->cardHolderData = cardHolderData;
	transData->terminalData = terminalData;

	recieveTransaction_f = recieveTransactionData(transData);

	switch (recieveTransaction_f)
	{
	case APPROVED:
		printf("\nTransaction was successfull, Thank You !!");
		break;

	case DECLINED_INSUFFECIENT_FUND:
		saveTransaction(transData);
		printf("\nTransaction Declined Due To Insuffecient Fund");
		break;

	case DECLINED_STOLEN_CARD:
		saveTransaction(transData);
		printf("\nTransaction Declined This Card Is Stolen, Please Return It To The Nearest Branch");
		break;

	case INTERNAL_SERVER_ERROR:
		saveTransaction(transData);
		printf("\nTransaction Declined Due To Server Error, Please Try Again Later .. ");
		break;

	default:
		break;
	}
/*
uint32_t transactionSeqNum;
    printf("\nEnter the transaction sequence number to retrieve: ");
    scanf("%u", &transactionSeqNum);

    ST_transaction_t retrievedTransaction;
    EN_serverError_t getTransactionStatus = getTransaction(transactionSeqNum, &retrievedTransaction);

    if (getTransactionStatus == SERVER_OK) {
        printf("\nTransaction Details Retrieved Successfully:");
        printf("\nTransaction Sequence Number: %u", retrievedTransaction.transactionSequenceNumber);
        printf("\nCard Holder Name: %s", retrievedTransaction.cardHolderData.cardHolderName);
        printf("\nPrimary Account Number: %s", retrievedTransaction.cardHolderData.primaryAccountNumber);
        printf("\nCard Expiry Date: %s", retrievedTransaction.cardHolderData.cardExpirationDate);
        printf("\nTransaction Amount: %.2f", retrievedTransaction.terminalData.transAmount);
        printf("\nTransaction Date: %s", retrievedTransaction.terminalData.transactionDate);
    } else if (getTransactionStatus == TRANSACTION_NOT_FOUND) {
        printf("\nTransaction not found.");
    }
	*/
}