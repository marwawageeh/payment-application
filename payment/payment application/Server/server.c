#include "server.h"
#include <stdio.h>
#include <string.h>
#include "../Card/card.h"
#include "../Terminal/terminal.h"



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


EN_serverError_t isValidAccount(ST_cardData_t* cardData) {
    for (num = 0; num < DB_ACCOUNTS_MAX; num++) {
        if (strcmp((char *)cardData->primaryAccountNumber, (char *)accountsDB[num].primaryAccountNumber) == 0) {
            return SERVER_OK;
        }
    }
    return ACCOUNT_NOT_FOUND;
}