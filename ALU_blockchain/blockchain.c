#include "blockchain.h"

/**
 * create_genesis_transaction: create list of transactions for a new block main use for genesis block
 * @sender: sender details
 * @receiver: receiver details
 * @amount: transaction amount
 */
utxo_t *create_genesis_transaction(const char *sender, const char *receiver, int amount)
{
    utxo_t *new_list;
    Transaction *new_trans = (Transaction *)malloc(sizeof(Transaction));
    if (!new_trans)
    {
        perror("Could not allocate memory for transaction");
        exit(EXIT_FAILURE);
    }
    strncpy(new_trans->sender, sender, DATASIZE_MAX - 1);
    new_trans->sender[DATASIZE_MAX - 1] = '\0';
    
    strncpy(new_trans->receiver, receiver, DATASIZE_MAX - 1);
    new_trans->receiver[DATASIZE_MAX - 1] = '\0';

    new_trans->amount = amount;

    new_trans->next = NULL;
    new_trans->index = 0;

    new_list = (utxo_t *)malloc(sizeof(utxo_t));
    if (!new_list)
    {
        perror("Could not allocate memory for data");
        free(new_trans);
        exit(EXIT_FAILURE);
    }
    new_list->head = new_trans;
    new_list->tail = new_trans;
    new_list->nb_trans = 1;
    return new_list;
}


/**
 * create_block - creates new block
 * @index: block index
 * @transactions: pointer to transactions to add to block
 * @previous_hash: previous block hash
 * @difficulty: Proof of Work difficulty level
 */
Block *create_block(int index, utxo_t *transactions, const unsigned char *previous_hash, int difficulty)
{
    Block *new_block = (Block *)malloc(sizeof(Block));
    if (!new_block) {
        printf("Failed to allocate memory for block\n");
        return NULL;
    }

    new_block->index = index;
    strcpy(new_block->timestamp, ctime(&(time_t){time(NULL)}));
    new_block->transactions = transactions;
    if (previous_hash)
        memcpy(new_block->previous_hash, previous_hash, SHA256_DIGEST_LENGTH);
    else
        memset(new_block->previous_hash, 0, SHA256_DIGEST_LENGTH); 
    memset(new_block->current_hash, 0, SHA256_DIGEST_LENGTH);
    new_block->next = NULL;
    new_block->nonce = 0;

    mine_block(new_block, difficulty);
    return new_block;
}

/**
 * add_block - adds block to blockchain
 * @blockchain: pointer to blockchain
 * @block: pointer to mined block
 * Return: Nothing
 */
void add_block(Blockchain *blockchain, Block *block)
{
    if (!block)
        return;
    if (!blockchain->head)
        blockchain->head = blockchain->tail = block;
    else
    {
        blockchain->tail->next = block;
        blockchain->tail = block;
    }
    blockchain->length++;
}

/**
 * init_blockchain - initializes new blockchain with genesis block
 * Return: pointer to blockchain
 */
Blockchain *init_blockchain(void)
{
    Blockchain *blockchain = (Blockchain *)malloc(sizeof(Blockchain));
    if (!blockchain) {
        perror("Failed to allocate memory for blockchain");
        exit(EXIT_FAILURE);
    }

    blockchain->difficulty = INITIAL_DIFFICULTY;

    // Create the genesis block
    utxo_t *genesis_transactions = create_genesis_transaction("Genesis", "Blockchain", 10);
    Block *genesisBlock = createBlock(0, genesis_transactions, NULL, blockchain->difficulty);

    blockchain->head = blockchain->tail = genesisBlock;
    blockchain->length = 1;

    return blockchain;
}


/**
 * validate_chain - ensures that previous block's hash matches with new block's hash
 * @blockchain: pointer to blockchain to validate
 * Return: 1 if valid, or 0 if invalid
 */
int validate_chain(Blockchain *blockchain)
{
    if (!blockchain || !blockchain->head)
        return 0;
    unsigned char tmpHash[SHA256_DIGEST_LENGTH] = {0};
    unsigned char calculatedHash[SHA256_DIGEST_LENGTH];
    Block *current = blockchain->head;

    while (current)
    {
        calculate_hash(current, current->nonce, calculatedHash);
        if (memcmp(current->current_hash, calculatedHash, SHA256_DIGEST_LENGTH) != 0 || memcmp(current->previous_hash, tmpHash, SHA256_DIGEST_LENGTH) != 0) {
            return 0;
        }    
        memcpy(tmpHash, current->current_hash, SHA256_DIGEST_LENGTH);
        current = current->next;
    }
    return 1;
}

/**
 * print_blockchain: prints blocks in blockchain
 * @blockchain: pointer to blockchain
 * Return: Nothing
 */
void print_blockchain(Blockchain *blockchain)
{
    Block *current = blockchain->head;
    while (current) {
        printf("Block %d\n", current->index);
        printf("Timestamp: %lu\n", current->timestamp);
        Transaction *trans = current->transactions->head;
        while (trans)
        {
            printf("  Transaction %d: %s -> %s, Amount: %d Status: %d\n", trans->index, trans->sender, trans->receiver, trans->amount, status_str[trans->status]);
            trans = trans->next;
        }

        printf("Previous Hash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", current->previous_hash[i]);
        }
        printf("\n");

        printf("Current Hash: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", current->current_hash[i]);
        }
        printf("\n\n");

        current = current->next;
    }
}

/**
 * free_blockchain - free block in blockchain and then the blockchain itself
 * @blockchain: pointer to blockchain
 * Return: Nothing
 */
void free_blockchain(Blockchain *blockchain)
{
    Block *current = blockchain->head;
    while (current) {
        Block *next = current->next;
        free_transactions(current->transactions);
        free(current);
        current = next;
    }
    free(blockchain);
}

/**
 * adjust_difficulty - adjusts mining difficulty based on block time
 * @prevTime: timestamp of previous block
 * @currentTime: timestamp of current block
 * @currentDifficulty: current difficulty level
 * Return: new difficulty level
 */
int adjust_difficulty(uint64_t prevTime, uint64_t currentTime, int currentDifficulty)
{
    double timeDiff = difftime(currentTime, prevTime);
    if (timeDiff < 10)
        return currentDifficulty + 1;  // Increase difficulty
    else if (timeDiff > 40 && currentDifficulty > 1)
        return currentDifficulty - 1;  // Decrease difficulty
    return currentDifficulty;
}