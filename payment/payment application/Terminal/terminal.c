#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>

 
#include "C:/Users/MaKaNaK/Desktop/payment/payment application/Card/card.h"
#include "C:/Users/MaKaNaK/Desktop/payment/payment application/Terminal/terminal.h"


/*
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{

    uint16_t sum = 0;
    uint16_t PANlen;
    uint16_t counter = 1;
    uint16_t PANdigits[20] = { 0 };

    PANlen = strlen(cardData->primaryAccountNumber);   

    for (uint16_t i = PANlen-1 ; i > 0; i--)            
    {
        if (counter % 2 == 0)                         
        {
            uint16_t temp = (cardData->primaryAccountNumber[i] - '0') * 2; 

            if ((temp / 10) > 0)                        
            {
                temp = (temp % 10) + 1;
                PANdigits[i] = temp;
            }
            else
            {
                PANdigits[i] = temp;
            }
        }
        else 
        {
            PANdigits[i] = (cardData->primaryAccountNumber[i] - '0');
        }
        counter++;
    }
    

    if (counter % 2 == 0)
    {
        uint16_t temp = (cardData->primaryAccountNumber[0] - '0') * 2;;

        if ((temp / 10) > 0)
        {
            temp = (temp % 10) + 1;
            PANdigits[0] = temp;
        }
        else
        {
            PANdigits[0] = temp;
        }
    }
    else
    {
        PANdigits[0] = (cardData->primaryAccountNumber[0] - '0');
    }

   
    for (uint16_t i = 0; i < PANlen; i++)
    {
        sum += PANdigits[i];
    }
    
    
    if (sum % 10 == 0)
    {
        printf("Valid card\n");
        return OK;
    }
    else
        return INVALID_CARD;
    
}
*/
EN_terminalError_t getTransactionDate(ST_terminalData_t *termData)
{
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    char transactionDate[11] = {0};

    printf("\n Enter the transaction date (DD/MM/YYYY): ");

    fgets(transactionDate, sizeof(transactionDate), stdin); 

    uint8_t TransactionDateLength = strlen(transactionDate);

    if (TransactionDateLength != 10 || transactionDate[2] != '/' || transactionDate[5] != '/')
    {
        ThisFunction_ErrorState = WRONG_DATE;
    }
    else
    {
        strcpy(termData->transactionDate, transactionDate);
    }

    return ThisFunction_ErrorState;

    
}

EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    uint8_t CardExpirationMonth = ((cardData.cardExpirationDate[0] - '0') * 10) + (cardData.cardExpirationDate[1] - '0');
    uint8_t CardExpirationYear = ((cardData.cardExpirationDate[3] - '0') * 10) + (cardData.cardExpirationDate[4] - '0');

    uint8_t TransactionMonth = ((termData.transactionDate[3] - '0') * 10) + (termData.transactionDate[4] - '0');
    uint8_t TransactionYear = ((termData.transactionDate[8] - '0') * 10) + (termData.transactionDate[9] - '0');

    if (CardExpirationYear < TransactionYear || (CardExpirationYear == TransactionYear && CardExpirationMonth < TransactionMonth))
    {
        ThisFunction_ErrorState = EXPIRED_CARD;
    }

    return ThisFunction_ErrorState;
}



EN_terminalError_t setMaxAmount(ST_terminalData_t *termData)
{
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;
    
    float maxTransAmount = 0.0f;

    printf("\n Enter transaction maximum amount: ");
    scanf("%f", &maxTransAmount);

    if( ( maxTransAmount <= 0) )
    {
        ThisFunction_ErrorState = INVALID_MAX_AMOUNT;
    }
    else
    {
        termData->maxTransAmount = maxTransAmount;
    }

    return ThisFunction_ErrorState;
}

EN_terminalError_t getTransactionAmount(ST_terminalData_t *termData)
{
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    float transAmount = 0;

    printf("\n Enter transaction amount: ");
    scanf("%f", &transAmount);

    if (transAmount <= 0)
    {
        ThisFunction_ErrorState = INVALID_AMOUNT;
    }
    else
    {
        termData->transAmount = transAmount;
    } 
    return ThisFunction_ErrorState;
}

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t *termData)
{
    EN_terminalError_t ThisFunction_ErrorState = TERMINAL_OK;

    if (termData->transAmount > termData->maxTransAmount)
    {
        ThisFunction_ErrorState = EXCEED_MAX_AMOUNT;
    }

    return ThisFunction_ErrorState;
}


