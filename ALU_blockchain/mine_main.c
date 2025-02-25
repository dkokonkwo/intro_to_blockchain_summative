#include "blockchain.h"

/**
 * main - mines new block and adds it to blockchain
 * return: 0 always
 */
int main(void)
{
    Blockchain *blockchain;
    Block *newBlock;
    uint64_t startTime, endTime;
    utxo_t *unspent;

    blockchain = deserialize_blockchain();
    if (!blockchain)
    {
        fprintf(stderr, "Could not deserialize blockchain\n");
        exit(EXIT_FAILURE);
    }

    if (!blockchain->tail)
    {
        fprintf(stderr, "Blockchain is empty. Initializing new blockchain...\n");
        free_blockchain(blockchain);
        blockchain = init_blockchain();
    }

    unspent = deserialize_utxo();
    if (!unspent)
    {
        fprintf(stderr, "Could not deserialize unspent transactions\n");
        free_blockchain(blockchain);
        exit(EXIT_FAILURE);
    }

    if (unspent->nb_trans == 0)
    {
        fprintf(stderr, "No transactions to mine\n");
        free_blockchain(blockchain);
        free_transactions(unspent);
        exit(EXIT_FAILURE);
    }

    utxo_t *block_txs = tx_for_mining(unspent);
    if (!block_txs)
    {
        fprintf(stderr, "Error getting transactions for mining\n");
        free_blockchain(blockchain);
        free_transactions(unspent);
        exit(EXIT_FAILURE);
    }

    printf("------MINING BLOCK------\n");
    startTime = (uint64_t)time(NULL);

    newBlock = create_block(blockchain->length, block_txs, blockchain->tail ? blockchain->tail->current_hash : NULL, blockchain->difficulty);
    if (!newBlock)
    {
        fprintf(stderr, "Could not create new block\n");
        free_blockchain(blockchain);
        free_transactions(block_txs);
        exit(EXIT_FAILURE);
    }

    int result = finalize_mining(newBlock);
    if (!result)
    {
        fprintf(stderr, "Could not finish mining\n");
        free_blockchain(blockchain);
        free_transactions(block_txs);
        exit(EXIT_FAILURE);
    }

    endTime = (uint64_t)time(NULL);
    addBlock(blockchain, newBlock);
    printf("Time taken to mine block: %lu seconds\n", (endTime - startTime));
    printf("\n\n");

    blockchain->difficulty = adjustDifficulty(startTime, endTime, blockchain->difficulty);
    printf("New Difficulty Level: %d\n", blockchain->difficulty);

    printf("\n------VERIFYING BLOCKCHAIN INTERGRITY-------\n");
    if (!validate_chain(blockchain))
    {
        fprintf(stderr, "Blockchain is not valid\n");
        free_blockchain(blockchain);
        free_transactions(block_txs);
        exit(EXIT_FAILURE);
    }

    printf("Blockchain is valid\n");

    if (!serialize_blockchain(blockchain))
    {
        fprintf(stderr, "Blockchain with new block could not be serialized\n");
        free_blockchain(blockchain);
        free_transactions(block_txs);
        exit(EXIT_FAILURE);
    }

    printf("Serialized new blockchain\n");
    free_transactions(block_txs);

    printf("MINING COMPLETE. NEW BLOCK ADDED TO BLOCKCHAIN\n");
    return 0;
}