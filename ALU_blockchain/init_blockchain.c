#include "blockchain.h"

/**
 * main - initializes blockchain from scratch
 * Return: 0 always
 */
int main(void)
{
    Blockchain *blockchain = init_blockchain();
    if (!blockchain)
    {
        fprintf(stderr, "Could not initialize blockchain\n");
        exit(EXIT_FAILURE);
    }
    if (!serialize_blockchain(blockchain))
    {
        fprintf(stderr, "Could not serialize created blockchain\n");
        fflush(stdout);
        free_blockchain(blockchain);
        exit(EXIT_FAILURE);
    }

    setup_blockchain();

    printf("Blockchain created!\n");
    fflush(stdout);
    return 0;
}