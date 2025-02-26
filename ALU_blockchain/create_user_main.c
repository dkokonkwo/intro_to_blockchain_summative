#include "blockchain.h"

/**
 * main - creates new user and loads user for new session
 * Return: 0 always
 */
int main(void)
{
    char name[DATASIZE_MAX];
    int role;
    printf("Enter Your full name: ");
    if (!fgets(name, sizeof(name), stdin))
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    printf("1. STUDENT\n2. FACULTY\n3. VENDOR\nEnter a number representing your role from above: ");
    if (scanf("%d", &role) != 1)
    {
        printf("Error: invalid input.\n");
        exit(EXIT_FAILURE);
    }

    /* Removing newlines */
    name[strcspn(name, "\n")] = '\0';
    if (!create_user(name, role - 1))
    {
        fprintf(stderr, "Could not create new user\n");
        exit(EXIT_FAILURE);
    }

    printf("New account created and Logged in successfully!\n");
    return 0;
}