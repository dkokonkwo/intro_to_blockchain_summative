#include "blockchain.h"

/**
 * hex_to_bytes - Convert a hex string to a byte array
 * @hex: input hex string
 * @bytes: output byte array
 * @size: number of bytes to convert
 * Return: 1 on success, 0 on failure
 */
int hex_to_bytes(const char *hex, unsigned char *bytes, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if (sscanf(hex + (i * 2), "%2hhx", &bytes[i]) != 1)
            return 0; // Failed conversion
    }
    return 1;
}

/**
 * main - Adds transaction to unspent transaction pool for PoW
 * Return: 0 always
 */
int main(void)
{
    char sender[ADDRESS_SIZE * 2 + 2], receiver[ADDRESS_SIZE * 2 + 2];
    unsigned char converted_sender[ADDRESS_SIZE];
    unsigned char converted_receiver[ADDRESS_SIZE]; 
    int amount;
    printf("Sender: ");
    if (!fgets(sender, sizeof(sender), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    /* Removing newlines */
    sender[strcspn(sender, "\n")] = '\0';

    if (strlen(sender) != ADDRESS_SIZE * 2)
    {
        fprintf(stderr, "Invalid sender address length\n");
        exit(EXIT_FAILURE);
    }

    printf("Receiver: ");
    if (!fgets(receiver, sizeof(receiver), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    receiver[strcspn(receiver, "\n")] = '\0';

    if (strlen(receiver) != ADDRESS_SIZE * 2)
    {
        fprintf(stderr, "Invalid receiver address length\n");
        exit(EXIT_FAILURE);
    }

    printf("Amount: ");
    if (scanf("%d%*c", &amount) != 1)
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    if (!hex_to_bytes(sender, converted_sender, ADDRESS_SIZE))
    {
        fprintf(stderr, "Invalid hex input sender\n");
        return 1;
    }

    if (!hex_to_bytes(receiver, converted_receiver, ADDRESS_SIZE))
    {
        fprintf(stderr, "Invalid hex input receiver\n");
        return 1;
    }

    if (!add_transaction(converted_sender, converted_receiver, amount))
    {
        fprintf(stderr, "Could not add transactions to unspent pool\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}