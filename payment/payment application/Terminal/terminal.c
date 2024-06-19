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


/*
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../Card/card.h"

#define MAX_NAME_LENGTH 24
#define MIN_NAME_LENGTH 20
#define EXPIRY_DATE_LENGTH 5
#define MAX_PAN_LENGTH 20
#define MIN_PAN_LENGTH 16


EN_cardError_t getCardHolderName(ST_cardData_t *cardData)
{
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

EN_cardError_t getCardExpiryDate(ST_cardData_t *cardData)
{
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


EN_cardError_t getCardPAN(ST_cardData_t *cardData)
{
    printf("\nEnter the card primary account number (between 16 and 20 digits): ");

    if (fgets(cardData->primaryAccountNumber, sizeof(cardData->primaryAccountNumber), stdin) == NULL) {
        return WRONG_PAN;
    }

    size_t len = strlen(cardData->primaryAccountNumber);

    if (len < MIN_PAN_LENGTH || len > MAX_PAN_LENGTH) {
        return WRONG_PAN;
    }

    return CARD_OK;
}

*/
/*
int main (){
    
    //EN_cardError_t cardHolderName_f;
	//EN_cardError_t cardExpiryDate_f;
	EN_cardError_t cardPAN_f;

	ST_cardData_t cardHolderData;
	ST_cardData_t* cardData = &cardHolderData;
    
   

 if ((cardPAN_f = getCardPAN(cardData) == WRONG_PAN))
	{
		printf("Wrong PAN !");
		
        return 0;
	}
    
	if ((cardHolderName_f = getCardHolderName(cardData) == WRONG_NAME))
	{
		printf("Wrong Name !");
		return 0;
	}
    
	 EN_terminalError_t transactionDate_f;
	EN_terminalError_t cardEpired_f;
	//EN_terminalError_t maxAmount_f;
	//EN_terminalError_t transactionAmount_f;
	//EN_terminalError_t belowMaxAmount_f;

	
	ST_terminalData_t terminalData;
	ST_terminalData_t* termData = &terminalData; 
  

	if ((transactionDate_f = getTransactionDate(termData) == WRONG_DATE))
	{
		printf("Wrong Date !");
		return 0;
	}

    if ((cardExpiryDate_f = getCardExpiryDate(cardData) == WRONG_EXP_DATE))
	{
		printf("Wrong Expiry Date !");
		return 0;
	}

	if ((cardEpired_f = isCardExpired(cardHolderData, terminalData) == EXPIRED_CARD))
	{
		printf("Your Card is Expired !, Please Refer To The Bank To Get Your New Card");
		return 0;
	}

	if ((maxAmount_f = setMaxAmount(termData) == INVALID_MAX_AMOUNT))
	{
		printf("Invalid Max Amount");
		return 0;
	}

	if ((transactionAmount_f = getTransactionAmount(termData) == INVALID_AMOUNT))
	{
		printf("Invalid Amount !, Please Enter A Valid Amount");
		return 0;
	}

	if ((belowMaxAmount_f = isBelowMaxAmount(termData) == EXCEED_MAX_AMOUNT))
	{
		printf("Exceeded Max Amount !, Please Enter A Valid Amount ");
		return 0;
	}
    


 */