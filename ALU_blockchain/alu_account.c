#include "blockchain.h"

/**
 * create_alu_account - Creates a new ALU account with a wallet and token
 * Return: Pointer to newly created alu_account else NULL on failure
 */
alu_account *create_alu_account(void)
{
    alu_account *account = (alu_account *)malloc(sizeof(alu_account));
    if (!account)
    {
        fprintf(stderr, "Memory allocation failed for ALU account\n");
        return NULL;
    }

    strcpy(account->name, "African Leadership University Ltd");

    account->wallet = (Wallet *)malloc(sizeof(Wallet));
    if (!account->wallet)
    {
        fprintf(stderr, "Memory allocation failed for Wallet\n");
        free(account);
        return NULL;
    }
    get_address(account->name, account->wallet->address);
    account->wallet->balance = 0; 

    account->token = (Token *)malloc(sizeof(Token));
    if (!account->token)
    {
        fprintf(stderr, "Memory allocation failed for Token\n");
        free(account->wallet);
        free(account);
        return NULL;
    }
    strcpy(account->token->token_name, "ALU Token");
    account->token->total_supply = 1000000000;  // 1 billion tokens
    account->token->circulating_supply = 0;     // No tokens in circulation initially

    return account;
}

/**
 * serialize_alu_account - Saves ALU account data to a file
 * @account: Pointer to the ALU account
 * Return: 1 on success, 0 on failure
 */
int serialize_alu_account(alu_account *account)
{
    FILE *file = fopen(ALU_ACCOUNT_FILE, "wb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file for writing\n");
        return 0;
    }

    fwrite(account->name, sizeof(account->name), 1, file);
    fwrite(account->wallet, sizeof(Wallet), 1, file);
    fwrite(account->token, sizeof(Token), 1, file);

    fclose(file);
    return 1;
}

/**
 * deserialize_alu_account - Loads ALU account data from a file
 * Return: Pointer to the loaded ALU account, NULL on failure
 */
alu_account *deserialize_alu_account(void)
{
    FILE *file = fopen(ALU_ACCOUNT_FILE, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file for reading\n");
        return NULL;
    }

    alu_account *account = (alu_account *)malloc(sizeof(alu_account));
    if (!account)
    {
        fprintf(stderr, "Memory allocation failed for ALU account\n");
        fclose(file);
        return NULL;
    }

    account->wallet = (Wallet *)malloc(sizeof(Wallet));
    account->token = (Token *)malloc(sizeof(Token));
    if (!account->wallet || !account->token)
    {
        fprintf(stderr, "Memory allocation failed for wallet or token\n");
        free(account->wallet);
        free(account->token);
        free(account);
        fclose(file);
        return NULL;
    }

    fread(account->name, sizeof(account->name), 1, file);
    fread(account->wallet, sizeof(Wallet), 1, file);
    fread(account->token, sizeof(Token), 1, file);

    fclose(file);
    return account;
}

/**
 * load_alu_account - create and save alu_account
 * Return: pointer to alu account else NULL
 */
alu_account *load_alu_account(void)
{
    alu_account *account = create_alu_account();
    if (!account)
    {
        fprintf(stderr, "Could not create alu account\n");
        return NULL;
    }
    if (!serialize_alu_account(account))
    {
        fprintf(stderr, "Could not serialize alu account\n");
        free_alu_account(account);
        return NULL;
    }
    return account;
}

/**
 * free_alu_account - Frees memory allocated for an ALU account
 * @account: Pointer to the ALU account
 */
void free_alu_account(alu_account *account)
{
    if (!account)
        return;

    free(account->wallet);
    free(account->token);
    free(account);
}

/**
 * print_alu_account - Prints the ALU account details
 * @account: Pointer to the ALU account
 */
void print_alu_account(alu_account *account)
{
    if (!account)
    {
        printf("No account data to display\n");
        return;
    }

    printf("ALU Account Details:\n");
    printf("Name: %s\n", account->name);
    printf("Wallet Address: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", account->wallet->address[i]);
    printf("\nBalance: %d\n", account->wallet->balance);
    printf("Token Name: %s\n", account->token->token_name);
    printf("Total Supply: %u\n", account->token->total_supply);
    printf("Circulating Supply: %u\n", account->token->circulating_supply);
}

/**
 * transfer_tokens - create transaction to send toeke from alu account to user
 * @account: pointer to alu account
 * @address: user wallet address
 */
void transfer_tokens(alu_account *account, user_t *user)
{
    if (!user->wallet)
    {
        fprintf(stderr, "Receiver does not have a wallet.\n");
        return;
    }

    if (account->token->total_supply < account->token->circulating_supply + GIFT_TOKENS)
    {
        fprintf(stderr, "Insufficient tokens.\n");
        return;
    }

    account->token->circulating_supply += GIFT_TOKENS;
    user->wallet->balance += GIFT_TOKENS;
    serialize_alu_account(account);
    printf("Tokens transferred\n");
}
