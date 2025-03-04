#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define DATASIZE_MAX 1024
#define BLOCKCHAIN_DATABASE "blockchain.dat"
#define UTXO_DATABASE "transaction.dat"
#define USERS_DATABASE "users.dat"
#define SESSION_USER "current_user.dat"
#define ALU_ACCOUNT_FILE "alu_account.dat"
#define TRANSACTION_FEE 250
#define TRANSACTION_VOLUME 5 /* Number of transaction to be mined in a block */
#define ADDRESS_SIZE (SHA256_DIGEST_LENGTH / 2)
#define GIFT_TOKENS 2000000

#define INITIAL_DIFFICULTY 1  /* Starting difficulty level */

typedef enum
{
    STUDENT,
    FACULTY,
    VENDOR,
} Role;

typedef enum
{
    INITIATED,
    SUCCESS,
    FAILED,
} Status;

/**
 * struct Transaction_s - transaction structure
 * @index: transaction index
 * @sender: sender address
 * @receiver: receiver address
 * @amount: transaction amount in token currency
 * @status: transaction status
 * @next: pointer to next transaction in list
 */
typedef struct Transaction_s {
    int index;
    unsigned char sender[ADDRESS_SIZE];
    unsigned char receiver[ADDRESS_SIZE];
    int amount;
    Status status;
    struct Transaction_s *next;
} Transaction;

/**
 * struct utxo_s - list of transaction (input and output) structure
 * @head: first transaction linked list
 * @tail: last transaction in list
 * @nb_trans: number of transactions in list
 */
typedef struct utxo_s {
    Transaction *head;
    Transaction *tail;
    int nb_trans;
} utxo_t;

/**
 * struct Block_s: block structure
 * @index: block index
 * @previous_hash: previous block hash
 * @timestamp: block creation timestamp
 * @nonce: block nonce
 * @transactions: list of transactions in block
 * @current_hash: block's hash
 * @next: pointer to next block in blockchain
 */
typedef struct Block_s {
    unsigned int index;
    unsigned char previous_hash[SHA256_DIGEST_LENGTH];
    char timestamp[30];
    unsigned int nonce;
    utxo_t *transactions; // Transactions included in the block.
    unsigned char current_hash[SHA256_DIGEST_LENGTH];
    struct Block_s *next;
} Block;

/**
 * struct Blockchain - blockchain structure
 * @head: first block in chain
 * @tail: last block in chain
 * @difficulty: current block difficulty
 * @length: length of blockchain
 */
typedef struct Blockchain {
    Block *head;
    Block *tail;
    int length;
    int difficulty;
} Blockchain;

/**
 * Wallet: user wallet structure
 * @address: user public address for wallet
 * @balance: wallet balance
 * @transactions: transactions with wallet
 * @nb_trans: number of transactions done with wallet
 */
typedef struct Wallet {
    unsigned char address[SHA256_DIGEST_LENGTH];
    int balance;
} Wallet;

/**
 * user_s - user structure for with type
 * @name: user full name
 * @role: user role
 * @index: user index
 * @wallet: user wallet
 * @next: pointer to next user in list
 */
typedef struct user_s {
    Role role;
    char name[DATASIZE_MAX];
    int index;
    Wallet *wallet;
    struct user_s *next;
} user_t;

/**
 * lusers - list of users(nodes) in blockchain
 * @head: first user in linked list
 * @tail: last user in linked list
 * @length: number ot users
 */
typedef struct lusers {
    user_t *head;
    user_t *tail;
    int length;
} lusers;

/**
 * Token: university token structure
 * @token_name: token name
 * @total_supply: total supply of token
 * @circulating_supply: token supply in circulation
 */
typedef struct {
    char token_name[50];
    unsigned int total_supply;
    unsigned int circulating_supply;
} Token;

/**
 * alu_account - school wallet
 * @name: school name
 * @wallet: pointer to school wallet
 */
typedef struct alu_account_s
{
    char name[DATASIZE_MAX];
    Wallet *wallet;
    Token *token;
} alu_account;

/* TRANSACTION FUNCTIONS */

int serialize_utxo(utxo_t *unspent);
utxo_t *deserialize_utxo(void);
int add_transaction(unsigned char *sender, unsigned char *receiver, int amount);
void free_transactions(utxo_t *transactions);
int verify_transaction(unsigned char *sender, unsigned char *receiver);

/* WALLET FUNCTIONS */

int create_wallet(user_t *user); //compute user address(hash) and print it. Add wallet to user structure
int delete_wallet(user_t *user); //may or may not be a function
void view_balance(void); //print out balance in user wallet
void get_address(const char *name, unsigned char *address);
void print_wallet(Wallet *wallet);

/* USER FUNCTIONS */

void print_current_user(void);
int load_user(const char *name, int idx);
int create_user(const char *name, int role);
int serialize_users(lusers *users);
lusers *deserialize_users(void);
user_t *get_user(unsigned char *address);
void free_users(lusers *users);
void free_user(user_t *user);

/* BLOCK FUNCTIONS */

Block *create_block(int index, utxo_t *transactions, const unsigned char *previous_hash, int difficulty);
void add_block(Blockchain *blockchain, Block *block);

/* BLOCK MINING FUNCTIONS */

void mine_block(Block *block, int difficulty);
void calculate_hash(Block *block, unsigned char *hash);
int is_valid_hash(unsigned char *hash, int difficulty);
void hash_to_hex(unsigned char *hash, char *output);
int finalize_mining(Block *block);
utxo_t *tx_for_mining(utxo_t *total_unpsent);

/* BLOCKCHAIN FUNCTIONS */

Blockchain *deserialize_blockchain(void);
int serialize_blockchain(Blockchain *blockchain); // backup_blockchain?
Blockchain *init_blockchain(void);
int validate_chain(Blockchain *blockchain);
void print_blockchain(Blockchain *blockchain);
void free_blockchain(Blockchain *blockchain);
utxo_t *create_genesis_transaction(unsigned char *sender, unsigned char *receiver, int amount);
int adjust_difficulty(uint64_t prevTime, uint64_t currentTime, int currentDifficulty);

/* ALU ACCOUNT FUNCTIONS */

void print_alu_account(alu_account *account);
void free_alu_account(alu_account *account);
alu_account *deserialize_alu_account(void);
int serialize_alu_account(alu_account *account);
alu_account *create_alu_account(void);
alu_account *load_alu_account(void);
void transfer_tokens(alu_account *account, user_t *user);
void setup_blockchain(void);

#endif