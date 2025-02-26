#include "blockchain.h"

/**
 * main - log in user for new session
 * Return: 0 always
 */
int main(void)
{
    char name[DATASIZE_MAX];
    int userID;
    printf("Enter Your full name: ");
    if (!fgets(name, sizeof(name), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter your userID: ");
    if (scanf("%d%*c", &userID) != 1)
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    /* Removing newlines */
    name[strcspn(name, "\n")] = '\0';

    if (!load_user(name, userID))
    {
        fprintf(stderr, "Login failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Login successful!\n");
    return 0;
}