#include "blockchain.h"

/**
 * setup_blockchain - adds data to blockchain
 */
void setup_blockchain(void)
{
    /* Create users */
    alu_account *account = load_alu_account();
    if (!account)
    {
        fprintf(stderr, "Could not load alu account\n");
        return;
    }
    char *names[] = {"Alice", "Bob", "Charlie", "David", "Eve"};
    int roles[] = {0, 0, 1, 2, 1};
    for (int i = 0; i < 5; i++)
    {
        create_user(names[i], roles[i]);
        lusers *users = deserialize_users();
        if (!create_wallet(users->tail))
            printf("Could not create wallet for %s\n", names[i]);
        else
            printf("Wallet created for %s.\n", users->tail->name);
        transfer_tokens(account, users->tail);
        serialize_users(users);
    }
    free_alu_account(account);
}