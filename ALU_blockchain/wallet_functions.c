#include "blockchain.h"

/**
 * get_address - calculates new address for user
 * @user: user to give address
 * @address: buffer to store address
 */
void get_address(user_t *user, unsigned char *address);


/**
 * create_wallet - creates new wallet user
 * @user: user to create wallet for
 * Return: 1 on success else 0 on failure
 */
int create_wallet(user_t *user)
{
    Wallet *new_wallet;
    if (!user)
    {
        fprintf(stderr, "User invalid\n");
        return 0;
    }
    if (user->wallet)
    {
        fprintf(stderr, "User already has wallet\n");
        return 0;
    }
    new_wallet = (Wallet *)malloc(sizeof(Wallet));
    if (!new_wallet)
    {
        fprintf(stderr, "Could not allocate memory for new wallet\n");
        return 0;
    }
    new_wallet->balance = 0;
    new_wallet->nb_trans = 0;
    new_wallet->transactions = NULL;
    get_address(user, new_wallet->address);
    user->wallet = new_wallet;
    printf("Your new wallet address is: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH / 2; i++)
        printf("%02x", user->wallet->address[i]);
    printf("\n");

    return 1;
}

/**
 * view_balance - get user wallet balance
 * @user: pointer to user to get balance
 */
void view_balance(void)
{
    user_t *user = get_user(NULL);
    if (!user)
    {
        fprintf(stderr, "User invalid\n");
        return;
    }
    if (!user->wallet)
    {
        fprintf(stderr, "User has not wallet\n");
        return;
    }
    printf("Balance: %d\n", user->wallet->balance);
    free_user(user);
}