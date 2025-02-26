#include "blockchain.h"

/**
 * hash_to_hex - converts binary hash to hex string
 * @hash: pointer to hash
 * @output: pointer to buffer to store converted data
 * Return: Nothing
 */
void hash_to_hex(unsigned char *hash, char *output)
{
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[SHA256_DIGEST_LENGTH * 2] = '\0';
}

/**
 * is_valid_hash - checks to see that diffculty requisite is met for the hash
 * @hash: pointer to hash to check validity
 * @difficulty: PoW difficulty level
 * Return: 1 if valid else 0
 */
int is_valid_hash(unsigned char *hash, int difficulty)
{
    for (int i = 0; i < difficulty; i++) {
        if (hash[i] != 0)
            return 0;
    }
    return 1;
}

/**
 * calculate_hash - calculates the hash of a block
 * @block: pointer to block to calculate hash of
 * @hash: pointer to address to store hash
 * Return: Nothing
 */
void calculate_hash(Block *block, unsigned char *hash)
{
    Transaction *current_trans;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        exit(EXIT_FAILURE);
    }

    /* Initialize SHA-256 hashing */
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        fprintf(stderr, "Failed to initialize hash function\n");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    /* Hash Block metadata */
    uint32_t index_be = htonl(block->index);
    if (EVP_DigestUpdate(ctx, &index_be, sizeof(index_be)) != 1 ||
        EVP_DigestUpdate(ctx, block->timestamp, sizeof(block->timestamp)) != 1 ||
        EVP_DigestUpdate(ctx, block->previous_hash, SHA256_DIGEST_LENGTH) != 1 ||
        EVP_DigestUpdate(ctx, &block->nonce, sizeof(block->nonce)) != 1)
    {
        fprintf(stderr, "Failed to update hash with block metadata\n");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    if (block->transactions)
    {
        /* adding block's transactions to hash*/
        current_trans = block->transactions->head;
        while (current_trans) {
            if (EVP_DigestUpdate(ctx, current_trans->sender, ADDRESS_SIZE) != 1 ||
                EVP_DigestUpdate(ctx, current_trans->receiver, ADDRESS_SIZE) != 1 ||
                EVP_DigestUpdate(ctx, &current_trans->amount, sizeof(current_trans->amount)) != 1 ||
                EVP_DigestUpdate(ctx, &current_trans->status, sizeof(current_trans->status)) != 1)
            {
                fprintf(stderr, "Failed to update hash with transaction data\n");
                EVP_MD_CTX_free(ctx);
                exit(EXIT_FAILURE);
            }
            current_trans = current_trans->next;
        }
    }

    if (EVP_DigestFinal_ex(ctx, hash, NULL) != 1) {
        fprintf(stderr, "Failed to finalize hash\n");
        EVP_MD_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    EVP_MD_CTX_free(ctx);
}


/**
 * mine_block - mines a block in a blockchain
 * @block: pointer to block to mine
 * @difficulty: PoW difficulty level
 * Return: Nothing
 */
void mine_block(Block *block, int difficulty) 
{
    unsigned int nonce = 0;
    unsigned char hash[SHA256_DIGEST_LENGTH];

    printf("Mining block %d at difficulty %d...\n", block->index, difficulty);

    do {
        block->nonce = nonce;
        calculate_hash(block, hash);
        nonce++;
    } while (!is_valid_hash(hash, difficulty));

    memcpy(block->current_hash, hash, SHA256_DIGEST_LENGTH);

    printf("Block %d mined with nonce: %u\n", block->index, nonce - 1);
    
}

/**
 * finalize_transaction - accounting updates, utxo updates
 * @block: pointer to mined block
 * Return: 1 on success else 0
 */
int finalize_mining(Block *block)
{
    if (!block || !block->transactions)
    {
        fprintf(stderr, "Invalid block or not transaction in block\n");
        return 0;
    }
    user_t *miner = get_user(NULL);
    if (!miner || !miner->wallet)
    {
        fprintf(stderr, "Could next get miner details\n");
        return 0;
    }
    Transaction *trans = block->transactions->head;
    while (trans)
    {
        user_t *sender = get_user(trans->sender);
        if (!sender)
        {
            fprintf(stderr, "Could not get sender details\n");
            return 0;
        }
        user_t *receiver = get_user(trans->receiver);
        if (!receiver)
        {
            fprintf(stderr, "Could not get receiver details\n");
            free_user(sender);
            return 0;
        }
        sender->wallet->balance -= trans->amount + TRANSACTION_FEE;
        receiver->wallet->balance += trans->amount;
        miner->wallet->balance += TRANSACTION_FEE;
        trans = trans->next;
    }
    return 1;
}

/**
 * tx_for_mining - get transactions to add to block for mining
 * @total_unpsent: all unspent transactions in pool
 * Return: pointer to transactions else NULL on failure
 */
utxo_t *tx_for_mining(utxo_t *total_unspent)
{
    int max = 0;
    utxo_t *block_transactions;
    Transaction *curr, *prev = NULL, *next;
    if (!total_unspent)
    {
        fprintf(stderr, "total unspent is null\n");
        return NULL;
    }
    if (total_unspent->nb_trans == 0)
    {
        fprintf(stderr, "No transactions to mine\n");
        return NULL;
    }

    /* Allocate memory for block transactions */
    block_transactions = (utxo_t *)malloc(sizeof(utxo_t));
    if (!block_transactions)
    {
        fprintf(stderr, "Could not allocate memory for block transactions\n");
        return NULL;
    }
    block_transactions->head = block_transactions->tail = NULL;
    block_transactions->nb_trans = 0;

    curr = total_unspent->head;
    
    while (curr && max < TRANSACTION_VOLUME)
    {
        next = curr->next;  // Store next transaction before potential removal
        
        user_t *sender = get_user(curr->sender);
        if (!sender)
        {
            fprintf(stderr, "Could not get sender details\n");
            prev = curr;
            curr = next;
            continue;
        }

        /* Check if sender has enough balance */
        if (sender->wallet->balance < (curr->amount + TRANSACTION_FEE))
        {
            fprintf(stderr, "Insufficient balance for transaction from %s\n", sender->name);

            /* Remove transaction from total_unspent */
            if (prev)
                prev->next = next;
            else
                total_unspent->head = next;

            if (!next)
                total_unspent->tail = prev;

            free(curr); 
            total_unspent->nb_trans--;
            curr = next;
            continue;
        }

        /* Add transaction to block_transactions */
        curr->next = NULL;
        if (!block_transactions->head)
            block_transactions->head = block_transactions->tail = curr;
        else
        {
            block_transactions->tail->next = curr;
            block_transactions->tail = curr;
        }

        block_transactions->nb_trans++;
        total_unspent->nb_trans--;
        max++;

        prev = curr;
        curr = next;
    }
    serialize_utxo(total_unspent);
    return block_transactions;
}