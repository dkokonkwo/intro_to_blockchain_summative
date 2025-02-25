#include "blockchain.h"

/**
 * main - view wallet balance
 * Return: 0 always
 */
int main(void)
{
    user_t *sesh_user = get_user(NULL);
    if (!sesh_user)
    {
        fprintf(stderr, "Could not get session user\n");
        exit(EXIT_FAILURE);
    }
    if (!sesh_user->wallet)
    {
        fprintf(stderr, "User has no wallet\n");
        free(sesh_user);
        exit(EXIT_FAILURE);
    }
    printf("Wallet balance: %dalunium(ALU)\n", sesh_user->wallet->balance);
    free_user(sesh_user);
    return 0;
}