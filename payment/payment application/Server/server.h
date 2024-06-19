#pragma once
#ifndef _SERVER_H_
#define _SERVER_H_

//#include "../std/std_type.h"
#include "../Card/card.h"
#include"../Terminal/terminal.h"

#define DB_ACCOUNTS_MAX            (255u)
#define DB_TRANSACTIONS_MAX        (255u)

typedef enum EN_transState_t
{
    APPROVED, 
    DECLINED_INSUFFECIENT_FUND, 
    DECLINED_STOLEN_CARD, 
    INTERNAL_SERVER_ERROR
}EN_transState_t;


typedef struct ST_transaction_t
{
    ST_cardData_t cardHolderData;
    ST_terminalData_t terminalData;
    EN_transState_t transState;
    uint32_t transactionSequenceNumber;
}ST_transaction_t;


typedef enum EN_serverError_t
{
    SERVER_OK, 
    SAVING_FAILED, 
    TRANSACTION_NOT_FOUND, 
    ACCOUNT_NOT_FOUND, 
    LOW_BALANCE,
    BLOCKED_ACCOUNT
}EN_serverError_t ;

typedef enum EN_accountState_t
{
    RUNNING,
    BLOCKED,
    
}EN_accountState_t;



typedef struct ST_accountsDB_t
{
    float balance;
    EN_accountState_t state;
    uint8_t primaryAccountNumber[20];
}ST_accountsDB_t;




EN_transState_t recieveTransactionData(ST_transaction_t *transData);
EN_serverError_t isValidAccount(ST_cardData_t* cardData);
EN_serverError_t isAmountAvailable(ST_terminalData_t *termData);
EN_serverError_t saveTransaction(ST_transaction_t *transData);
EN_serverError_t getTransaction(uint32_t transactionSequenceNumber, ST_transaction_t *transData);
EN_serverError_t isBlockedAcount(ST_accountsDB_t *accountReference);



#endif 