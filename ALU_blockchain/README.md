Here is the question exactly as you requested:

**Question 2: Developing a Blockchain-Based Payment System for ALU in C**  

Create a blockchain based payment system for ALU that will provide secured payment service for any of this this payment transaction done on campus (payment of tuition fees, cafeteria payments, library fines etc.)  

**Submission Key Tasks and Requirements:**  

1. **System Design and Architecture**  
   - Define the objectives of the payment system (e.g., tuition fees, cafeteria payments, library fines).  
   - Determine the type of blockchain:  
     - Public (decentralized, open to all participants).  
     - Private (restricted to university stakeholders).  
   - Identify key participants: students, faculty, administrative staff, and external vendors.  
   - Create struct data types to represent key entities like:  
     - `struct Student` for student profiles.  
     - `struct Wallet` for user accounts.  
     - `struct Vendor` for external payment participants.  

2. **Blockchain Cryptography**  
   - Implement cryptographic algorithms:  
     - Use libraries like OpenSSL for generating hashes (e.g., SHA-256).  
     - Generate public and private keys for wallet addresses.  
   - Secure authentication:  
     - Write functions for user authentication (e.g., `verify_transaction()`).  

3. **Consensus Mechanism**  
   - Choose and implement the consensus algorithm:  
     - **Proof of Work (PoW):**  
       - Write a `mine_block()` function to simulate mining by solving hash puzzles.  
     - **Proof of Stake (PoS):**  
       - Implement a `select_validator()` function based on user wallet balances.  

4. **Token/Cryptocurrency Design**  
   - Define the university token structure:  
     ```c
     typedef struct { 
         char token_name[50]; 
         unsigned int total_supply; 
         unsigned int circulating_supply; 
     } Token;
     ```
   - Write functions to manage token distribution and balances (e.g., `transfer_tokens()`).  

5. **Mining and Block Creation**  
   - **Block Structure:**  
     ```c
     typedef struct { 
         unsigned int index; 
         char previous_hash[64]; 
         char timestamp[30]; 
         unsigned int nonce; 
         char transactions[1000]; // Transactions included in the block.  
         char current_hash[64]; 
     } Block;
     ```
   - Write functions to:  
     - Add transactions (`add_transaction()`).  
     - Create new blocks (`create_block()`).  
     - Validate blocks (`validate_block()`).  

6. **Data Structures**  
   - Create core data structures for:  
     - Blockchain (`struct Blockchain`).  
     - Transaction records (`struct Transaction`).  
     - User wallets (`struct Wallet`).  
   - Implement linked lists or dynamic arrays to maintain blockchains and transactions.  

7. **Command-Line Interface (CLI)**  
   - Develop a simple CLI for interacting with the blockchain:  
     - `create_wallet()`  
     - `view_balance()`  
     - `initiate_transaction()`  
     - `mine_block()`  
   - Use standard C libraries (`stdio.h`, `stdlib.h`) to handle I/O operations.  

8. **Security and Testing**  
   - Implement security measures:  
     - Hash validation for tamper-proofing (`validate_chain()` function).  
     - Prevent double-spending by marking spent transactions.  
   - Write unit tests for functions using a testing framework like Unity or custom assertions.  

9. **Deployment and Maintenance**  
   - Package the application into an executable.  
   - Use configuration files for system setup (`config.txt`).  
   - Develop a `backup_blockchain()` function to secure blockchain data.  