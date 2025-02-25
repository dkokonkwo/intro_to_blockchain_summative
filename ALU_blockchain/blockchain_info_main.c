#include "blockchain.h"

/**
 * main - display blockchain information
 * Return: 0 always
 */
int main(void)
{
    Blockchain *blockchain = deserialize_blockchain();
    if (!blockchain)
    {
        fprintf(stderr, "Could not get blockchain info\n");
        exit(EXIT_FAILURE); 
    }
    if (!blockchain->head)
    {
        printf("Blockchain is empty\n");
        return 0;
    }
    print_blockchain(blockchain);
    free_blockchain(blockchain);
    return 0;
}