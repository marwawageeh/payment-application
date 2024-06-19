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


	
   



