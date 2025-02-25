#include "blockchain.h"

/**
 * main - Adds transaction to unspent transaction pool for PoW
 * Return: 0 always
 */
int main(void)
{
    char sender[DATASIZE_MAX], receiver[DATASIZE_MAX];
    int amount;
    printf("Sender: ");
    if (!fgets(sender, sizeof(sender), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    printf("Receiver: ");
    if (!fgets(receiver, sizeof(receiver), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    printf("Amount: ");
    if (scanf("%d", &amount) != 1)
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    /* Removing newlines */
    sender[strcspn(sender, "\n")] = '\0';
    receiver[strcspn(receiver, "\n")] = '\0';

    if (!add_transaction(sender, receiver, amount))
    {
        fprintf(stderr, "Could not add transactions to unspent pool\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}