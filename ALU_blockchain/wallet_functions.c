#include "blockchain.h"

/**
 * get_address - calculates new address for user
 * @name: name to give address
 * @address: buffer to store address
 */
void get_address(const char *name, unsigned char *address)
{
    if (!name)
    {
        fprintf(stderr, "No name given");
        return;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        return;
    }

     /*SHA-256 hashing */
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1 ||
        EVP_DigestUpdate(ctx, name, strlen(name)) != 1)
    {
        fprintf(stderr, "Failed to update hash\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    unsigned int digest_len;
    if (EVP_DigestFinal_ex(ctx, address, &digest_len) != 1)
    {
        fprintf(stderr, "Failed to finalize hash\n");
        EVP_MD_CTX_free(ctx);
        return;
    }

    EVP_MD_CTX_free(ctx);
}


/**
 * create_wallet - creates new wallet user
 * @user: user to create wallet for
 * Return: 1 on success else 0 on failure
 */
int create_wallet(user_t *user)
{
    if (!user)
    {
        fprintf(stderr, "User invalid\n");
        return 0;
    }
    Wallet *new_wallet;
    if (user->wallet)
    {
        fprintf(stderr, "User already has wallet\n");
        for (int i = 0; i < ADDRESS_SIZE; i++)
            printf("%02x", user->wallet->address[i]);
        printf("\n");
        return 0;
    }
    new_wallet = (Wallet *)malloc(sizeof(Wallet));
    if (!new_wallet)
    {
        fprintf(stderr, "Could not allocate memory for new wallet\n");
        return 0;
    }

    new_wallet->balance = 0;

    get_address(user->name, new_wallet->address);
    user->wallet = new_wallet;
    printf("Your new wallet address is: ");
    for (int i = 0; i < ADDRESS_SIZE; i++)
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
    printf("Balance: %dalunium(ALU)\n", user->wallet->balance);
    free_user(user);
}

/**
 * print_wallet - prints wallet address and balance
 * @wallet: pointer to wallet to print
 */
void print_wallet(Wallet *wallet)
{
    printf("Wallet address: ");
    for (int i = 0; i < ADDRESS_SIZE; i++)
        printf("%02x", wallet->address[i]);
    printf("\n");
    printf("Balance: %d\n", wallet->balance);
}