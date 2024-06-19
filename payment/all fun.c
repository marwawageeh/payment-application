#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>
#include "payment application/Card/card.h"
#include "payment application/Terminal/terminal.h"
#include "payment application/server/server.h"

#define MAX_NAME_LENGTH 24
#define MIN_NAME_LENGTH 20
#define EXPIRY_DATE_LENGTH 5
#define MAX_PAN_LENGTH 20
#define MIN_PAN_LENGTH 16

EN_cardError_t getCardHolderName(ST_cardData_t *cardData) {
    printf("\nEnter the card holder name (between 20 and 24 characters): ");

    if (fgets(cardData->cardHolderName, sizeof(cardData->cardHolderName), stdin) == NULL) {
        return WRONG_NAME;
    }

    size_t len = strlen(cardData->cardHolderName);

    if (len > 0 && cardData->cardHolderName[len - 1] == '\n') {
        cardData->cardHolderName[len - 1] = '\0'; 
        len--;
    }

    if (len < MIN_NAME_LENGTH || len > MAX_NAME_LENGTH) {
        return WRONG_NAME;
    }

    return CARD_OK;
}

EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData) {
    printf("\nEnter the card expiration date (format: MM/YY): ");

    if (fgets(cardData->cardExpirationDate, sizeof(cardData->cardExpirationDate), stdin) == NULL) {
        return WRONG_EXP_DATE;
    }

    size_t len = strlen(cardData->cardExpirationDate);

    if (len != EXPIRY_DATE_LENGTH || cardData->cardExpirationDate[2] != '/') {
        return WRONG_EXP_DATE;
    }

    return CARD_OK;
}

EN_cardError_t getCardPAN(ST_cardData_t *cardData) {
    printf("\nEnter the card primary account number (between 16 and 20 digits): ");

    if (fgets(cardData->primaryAccountNumber, sizeof(cardData->primaryAccountNumber), stdin) == NULL) {
        return WRONG_PAN;
    }

    size_t len = strlen(cardData->primaryAccountNumber);

    if (len > 0 && cardData->primaryAccountNumber[len - 1] == '\n') {
        cardData->primaryAccountNumber[len - 1] = '\0'; 
        len--;
    }

    if (len < MIN_PAN_LENGTH || len > MAX_PAN_LENGTH) {
        return WRONG_PAN;
    }

    return CARD_OK;
}

EN_terminalError_t getTransactionDate(ST_terminalData_t *termData) {
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    char transactionDate[11] = {0};

    printf("\nEnter the transaction date (DD/MM/YYYY): ");

    fgets(transactionDate, sizeof(transactionDate), stdin); 

    uint8_t TransactionDateLength = strlen(transactionDate);

    if (TransactionDateLength > 0 && transactionDate[TransactionDateLength - 1] == '\n') {
        transactionDate[TransactionDateLength - 1] = '\0';
        TransactionDateLength--;
    }

    if (TransactionDateLength != 10 || transactionDate[2] != '/' || transactionDate[5] != '/') {
        ThisFunction_ErrorState = WRONG_DATE;
    } else {
        strcpy(termData->transactionDate, transactionDate);
    }

    return ThisFunction_ErrorState;
}

EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData) {
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    uint8_t CardExpirationMonth = ((cardData.cardExpirationDate[0] - '0') * 10) + (cardData.cardExpirationDate[1] - '0');
    uint8_t CardExpirationYear = ((cardData.cardExpirationDate[3] - '0') * 10) + (cardData.cardExpirationDate[4] - '0');

    uint8_t TransactionMonth = ((termData.transactionDate[3] - '0') * 10) + (termData.transactionDate[4] - '0');
    uint8_t TransactionYear = ((termData.transactionDate[8] - '0') * 10) + (termData.transactionDate[9] - '0');

    if (CardExpirationYear < TransactionYear || (CardExpirationYear == TransactionYear && CardExpirationMonth < TransactionMonth)) {
        ThisFunction_ErrorState = EXPIRED_CARD;
    }

    return ThisFunction_ErrorState;
}

EN_terminalError_t setMaxAmount(ST_terminalData_t *termData) {
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;
    
    float maxTransAmount = 0.0f;

    printf("\nEnter transaction maximum amount: ");
    scanf("%f", &maxTransAmount);

    if (maxTransAmount <= 0) {
        ThisFunction_ErrorState = INVALID_MAX_AMOUNT;
    } else {
        termData->maxTransAmount = maxTransAmount;
    }

    return ThisFunction_ErrorState;
}

EN_terminalError_t getTransactionAmount(ST_terminalData_t *termData) {
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    float transAmount = 0;

    printf("\nEnter transaction amount: ");
    scanf("%f", &transAmount);

    if (transAmount <= 0) {
        ThisFunction_ErrorState = INVALID_AMOUNT;
    } else {
        termData->transAmount = transAmount;
    } 
    return ThisFunction_ErrorState;
}

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t *termData) {
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    if (termData->transAmount > termData->maxTransAmount) {
        ThisFunction_ErrorState = EXCEED_MAX_AMOUNT;
    }

    return ThisFunction_ErrorState;
}

ST_accountsDB_t accountsDB[DB_ACCOUNTS_MAX] = {
    {3000.0, BLOCKED,"12345678901234567" },
    {10000.0,BLOCKED, "09876543211234567" },
    {200000.0,RUNNING, "13579246801357924"},
    {4000000.0, RUNNING,"24680135792468013"}
};

ST_transaction_t transactionDB[DB_TRANSACTIONS_MAX] = { 0 };

uint32_t transactionNumber = 0;
uint16_t num = 0;

EN_transState_t recieveTransactionData(ST_transaction_t* transData) {
    EN_serverError_t validAccountStatus = isValidAccount(&transData->cardHolderData);
    if (validAccountStatus == ACCOUNT_NOT_FOUND) {
        return DECLINED_STOLEN_CARD;
    }

    EN_serverError_t amountAvailableStatus = isAmountAvailable(&transData->terminalData);
    if (amountAvailableStatus == LOW_BALANCE) {
        return DECLINED_INSUFFECIENT_FUND;
    }

    EN_serverError_t saveTransactionStatus = saveTransaction(transData);
    if (saveTransactionStatus == SAVING_FAILED) {
        return INTERNAL_SERVER_ERROR;
    }

    printf("\nYour Balance before The Transaction: %.2f", accountsDB[num].balance);
    accountsDB[num].balance -= transData->terminalData.transAmount;
    printf("\nYour Balance After The Transaction: %.2f", accountsDB[num].balance);

    return APPROVED;
}

EN_serverError_t isValidAccount(ST_cardData_t* cardData) {
    for (num = 0; num < DB_ACCOUNTS_MAX; num++) {
        if (strcmp(cardData->primaryAccountNumber, accountsDB[num].primaryAccountNumber) == 0) {
            return SERVER_OK;
        }
    }
    return ACCOUNT_NOT_FOUND;
}

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData) {
    if (termData->transAmount <= accountsDB[num].balance) {
        return SERVER_OK;
    }
    return LOW_BALANCE;
}

EN_serverError_t saveTransaction(ST_transaction_t* transData) {
    if (transactionNumber < DB_TRANSACTIONS_MAX) {
        transactionDB[transactionNumber] = *transData;
        transactionDB[transactionNumber].transactionSequenceNumber = transactionNumber;
        transactionNumber++;
        return SERVER_OK;
    }
    return SAVING_FAILED;
}

EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t *transData) {
    if (transactionSequenceNumber >= transactionNumber) {
        return TRANSACTION_NOT_FOUND;
    }
    
    *transData = transactionDB[transactionSequenceNumber];
    
    return SERVER_OK;
}


int main() {
    ST_cardData_t cardHolderData;
    ST_terminalData_t terminalData;
    ST_transaction_t transactionData;
    
    // Get Card Holder Name
    if (getCardHolderName(&cardHolderData) == WRONG_NAME) {
        printf("Wrong Name!\n");
        return 0;
    }
     
    // Get Card PAN
    if (getCardPAN(&cardHolderData) == WRONG_PAN) {
        printf("Wrong PAN!\n");
        return 0;
    }
    
    // Get Transaction Date
    if (getTransactionDate(&terminalData) == WRONG_DATE) {
        printf("Wrong Date!\n");
        return 0;
    }
    
    /*
    // Get Card Expiry Date
    if (getCardExpiryDate(&cardHolderData) == WRONG_EXP_DATE) {
        printf("Wrong Expiry Date!\n");
        return 0;
    }*/

    // Check if the card is expired
    if (isCardExpired(cardHolderData, terminalData) == EXPIRED_CARD) {
        printf("Your Card is Expired");
        return 0;
    } 


    // Set Maximum Transaction Amount
    if (setMaxAmount(&terminalData) == INVALID_MAX_AMOUNT) {
        printf("Invalid Max Amount");
        return 0;
    }

    // Get Transaction Amount
    if (getTransactionAmount(&terminalData) == INVALID_AMOUNT) {
        printf("Invalid Amount");
        return 0;
    }

    // Check if the transaction amount is below the maximum amount
    if (isBelowMaxAmount(&terminalData) == EXCEED_MAX_AMOUNT) {
        printf("Exceeded Max Amount");
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
		printf("\nTransaction was successfull");
		break;
   
	case DECLINED_INSUFFECIENT_FUND:
		saveTransaction(transData);
		printf("\nDECLINED_INSUFFECIENT_FUND");
		break;

	case DECLINED_STOLEN_CARD:
		saveTransaction(transData);
		printf("\nTransaction Declined This Card Is Stolen");
		break;

	case INTERNAL_SERVER_ERROR:
		saveTransaction(transData);
		printf("\nTransaction Declined Due To Server Error ");
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
        //printf("\nCard Expiry Date: %s", retrievedTransaction.cardHolderData.cardExpirationDate);
        printf("\nTransaction Amount: %.2f", retrievedTransaction.terminalData.transAmount);
        printf("\nTransaction Date: %s", retrievedTransaction.terminalData.transactionDate);
    } else if (getTransactionStatus == TRANSACTION_NOT_FOUND) {
        printf("\nTransaction not found.");
    }
	*/
}
