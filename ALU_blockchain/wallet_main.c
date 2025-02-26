#include "blockchain.h"

/**
 * main - create wallet
 * Return: 0 always 
 */
int main(void)
{
    user_t *user = get_user(NULL);
    lusers *users = deserialize_users();
    user_t *curr = users->head;
    while (curr)
    {
        if (strcmp(curr->name, user->name) == 0 && curr->index == user->index)
            break;
        curr = curr->next;
    }
    if (!curr)
    {
        fprintf(stderr, "Could not get user details\n");
        exit(EXIT_FAILURE);
    }
    if (!create_wallet(curr))
    {
        printf("Could not create wallet\n");
        exit(EXIT_FAILURE);
    }
    if (!serialize_users(users))
    {
        fprintf(stderr, "Could not serilaize users with new wallet\n");
        free_user(user);
        free_users(users);
        exit(EXIT_FAILURE);
    }
    free_user(user);
    return 0;
}