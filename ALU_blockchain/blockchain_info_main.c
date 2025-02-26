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
    alu_account *account = deserialize_alu_account();
    if (!account)
    {
        fprintf(stderr, "Could not get alu account info\n");
    }
    print_blockchain(blockchain);
    if (account)
    {
        print_alu_account(account);
        free_alu_account(account);
    }

    free_blockchain(blockchain);
    return 0;
}