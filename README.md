# payment-application

![image](https://github.com/marwawageeh/payment-application/assets/173267798/f400a0bc-9813-429c-b74e-ae0c682a1753)




# Overview



This project demonstrates a secure payment application designed using C programming language, with distinct modules for handling card operations, terminal interactions, and server communications




# Programming Language



•	C: The core logic of the application is implemented in C.



# Components Description



1. Card Module (Card.c)
Purpose: Manages all card-related operations, including reading cardholder information and card data.
2. Terminal Module (Terminal.c)
Purpose: Manages user interactions, including displaying messages, capturing inputs, and validating card and transaction data
3. Server Module (Server.c)
Purpose: Handles backend operations, including validating accounts, checking available balance, and recording transactions.



# Workflow


 
1.	Card Reading:
o	Use getCardHolderName, getCardExpiryDate, and getCardPAN functions to read and encrypt card data.
2.	User Interaction:
o	Display messages and capture inputs using getTransactionDate, isCardExpired, isValidCardPAN, getTransactionAmount, isBelowMaxAmount, and setMaxAmount.
3.	Data Transmission:
o	Use receiveTransactionData in Server.c to send the transaction data to the backend server.
4.	Backend Processing:
o	Validate account and balance using isValidAccount and isAmountAvailable.
o	Save the transaction using saveTransaction.
o	Retrieve transaction details using getTransaction.
o	Check if the account is blocked using isBlockedAccount.
