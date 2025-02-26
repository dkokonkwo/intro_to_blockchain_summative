#include "blockchain.h"

/**
 * create_user - Creates a new user and adds it to the user list
 * @name: Name of the user
 * @role: Role of the user
 * Return: 1 on success, 0 on failure
 */
int create_user(const char *name, int role)
{
    if (!name)
    {
        fprintf(stderr, "No user name\n");
        return 0;
    }

    user_t *new_user = (user_t *)malloc(sizeof(user_t));
    if (!new_user)
    {
        fprintf(stderr, "Failed to allocate memory for user\n");
        return 0;
    }

    lusers *users = deserialize_users();
    if (!users)
    {
        printf("Could not get user from file...Creating new list\n");
        users = (lusers *)malloc(sizeof(lusers));
        if (!users)
        {
            fprintf(stderr, "Could not allocate memory for new users list\n");
            return 0;
        }
        users->head = NULL;
        users->tail = NULL;
        users->length = 0;
    }

    new_user->role = (Role)role;
    strncpy(new_user->name, name, DATASIZE_MAX - 1);
    new_user->name[DATASIZE_MAX - 1] = '\0'; // null termination
    new_user->index = users->length;
    new_user->wallet = NULL; // Not all users have wallets
    new_user->next = NULL;

    // Add to linked list
    if (users->head == NULL)
    {
        users->head = users->tail = new_user;
    }
    else
    {
        users->tail->next = new_user;
        users->tail = new_user;
    }

    users->length++;
    if (!serialize_users(users))
    {
        fprintf(stderr, "Could not serialize users with new user\n");
        free_users(users);
        return 0;
    }
    printf("User created\n");
    int result = load_user(name, new_user->index);
    return result;
}

/**
 * serialize_users - Serializes user data to a file
 * @users: List of users
 * Return: 1 on success, 0 on failure
 */
int serialize_users(lusers *users)
{
    if (!users)
    {
        fprintf(stderr, "no users allocated\n");
        return 0;
    }

    FILE *file = fopen(USERS_DATABASE, "wb");
    if (!file)
    {
        perror("Failed to open file for serialization");
        return 0;
    }

    fwrite(&users->length, sizeof(users->length), 1, file);

    user_t *current = users->head;
    while (current)
    {
        fwrite(&current->role, sizeof(current->role), 1, file);
        fwrite(current->name, sizeof(current->name), 1, file);
        fwrite(&current->index, sizeof(current->index), 1, file);

        int has_wallet = (current->wallet != NULL);
        fwrite(&has_wallet, sizeof(has_wallet), 1, file);

        if (has_wallet)
        {
            fwrite(current->wallet->address, sizeof(current->wallet->address), 1, file);
            fwrite(&current->wallet->balance, sizeof(current->wallet->balance), 1, file);
        }

        current = current->next;
    }

    fclose(file);
    free_users(users);
    return 1;
}

/**
 * deserialize_users - Deserializes user data from a file
 * Return: Pointer to lusers list or NULL on failure
 */
lusers *deserialize_users(void)
{
    FILE *file = fopen(USERS_DATABASE, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open users file\n");
        return NULL;
    }

    lusers *users = (lusers *)malloc(sizeof(lusers));
    if (!users)
    {
        fprintf(stderr, "Failed to allocate memory for users list");
        fclose(file);
        return NULL;
    }

    users->head = users->tail = NULL;
    users->length = 0;

    if (fread(&users->length, sizeof(users->length), 1, file) != 1)
    {
        perror("Failed to read users length");
        free(users);
        fclose(file);
        return NULL;
    }

    user_t *prevUser = NULL;
    for (int i = 0; i < users->length; i++)
    {
        user_t *user = (user_t *)malloc(sizeof(user_t));
        if (!user)
        {
            perror("Failed to allocate memory for user");
            break;
        }

        fread(&user->role, sizeof(user->role), 1, file);
        fread(user->name, sizeof(user->name), 1, file);
        fread(&user->index, sizeof(user->index), 1, file);

        int has_wallet;
        fread(&has_wallet, sizeof(has_wallet), 1, file);
        if (has_wallet)
        {
            user->wallet = (Wallet *)malloc(sizeof(Wallet));
            if (!user->wallet)
            {
                perror("Failed to allocate memory for wallet");
                free(user);
                break;
            }
            fread(user->wallet->address, sizeof(user->wallet->address), 1, file);
            fread(&user->wallet->balance, sizeof(user->wallet->balance), 1, file);
        }
        else
            user->wallet = NULL;

        user->next = NULL;

        if (users->head == NULL)
            users->head = users->tail = user;
        else
        {
            prevUser->next = user;
            users->tail = user;
        }

        prevUser = user;
    }

    fclose(file);
    return users;
}

/**
 * load_user - sign in user
 * @name: user name
 * @idx: user index in lusers
 * Return: 1 on success else 0
 */
int load_user(const char *name, int idx)
{
    FILE *file;
    if (!name)
    {
        fprintf(stderr, "Name not valid\n");
        return 0;
    }
    lusers *all_users = deserialize_users();
    if (!all_users)
    {
        fprintf(stderr, "Could not get all users for loading\n");
        return 0;
    }
    user_t *curr = all_users->head;
    while (curr)
    {
        if (strcmp(curr->name, name) == 0 && curr->index == idx)
        {
            file = fopen(SESSION_USER, "wb");
            if (!file)
            {
                fprintf(stderr, "Failed to open current user file for serialization\n");
                free_users(all_users);
                return 0;
            }
            fwrite(&curr->role, sizeof(curr->role), 1, file);
            fwrite(&curr->index, sizeof(curr->index), 1, file);
            fwrite(curr->name, sizeof(curr->name), 1, file);

            fclose(file);
            return 1;
        }
        curr = curr->next;
    }

    fprintf(stderr, "Could not verify user\n");
    free_users(all_users);
    return 0;
}


/**
 * get_user - get user account
 * @address: if address is given retrieve user with wallet address
 * Otherwise, retrieve session user
 * Return: pointer to user else NULL
 */
user_t *get_user(unsigned char *address)
{
    lusers *all_users;
    user_t *user = NULL, *sesh_user = NULL;
    
    if (!address)
    {
        FILE *file = fopen(SESSION_USER, "rb");
        if (!file)
        {
            fprintf(stderr, "Failed to open session user file...");
            return NULL;
        }

        sesh_user = (user_t *)malloc(sizeof(user_t));
        if (!sesh_user)
        {
            fprintf(stderr, "Failed to allocate memory for session user\n");
            fclose(file);
            return NULL;
        }

        sesh_user->wallet = NULL;
        sesh_user->next = NULL;

        if (fread(&sesh_user->role, sizeof(sesh_user->role), 1, file) != 1 ||
            fread(&sesh_user->index, sizeof(sesh_user->index), 1, file) != 1 ||
            fread(sesh_user->name, sizeof(sesh_user->name), 1, file) != 1)
        {
            fprintf(stderr, "Failed to read session user data\n");
            fclose(file);
            free(sesh_user);
            return NULL;
        }
        fclose(file);
    }

    all_users = deserialize_users();
    if (!all_users)
    {
        fprintf(stderr, "Failed to deserialize users\n");
        free(sesh_user);
        return NULL;
    }
    user = all_users->head;
    while (user)
    {
        if (address)
        {
            if (user->wallet && memcmp(user->wallet->address, address, ADDRESS_SIZE) == 0)
            {
                return user;
            }
        }
        else
        {
            if (sesh_user && strcmp(user->name, sesh_user->name) == 0 && user->index == sesh_user->index)
            {
                free(sesh_user);
                return user;
            }
        }
        user = user->next;
    }
    fprintf(stderr, "Could not find user with address:");
    for (int i = 0; i < ADDRESS_SIZE; i++)
    {
        printf("%02x", address[i]);
    }
    printf("\n\n");

    if (sesh_user)
        free(sesh_user);
    free_users(all_users);
    return user;
}

/**
 * print_current_user - prints current user info
 */
void print_current_user(void)
{
    user_t *user = get_user(NULL);
    if (user)
    {
        printf("Name: %s\nUser ID: %d\n", user->name, user->index);
        if (user->wallet)
            print_wallet(user->wallet);
        else
            printf("No wallet\n");
    }
}

/**
 * free_user - frees user struture from memory
 * @user: pointer to user to free
 */
void free_user(user_t *user)
{
    if (user)
    {
        if (user->wallet)
            free(user->wallet);
        free(user);
    }
}

/**
 * free_users - frees users list memory
 * @users: pointer to list of users to free
 */
void free_users(lusers *users)
{
    if(!users)
        return;
    user_t *curr = users->head;
    while (curr)
    {
        user_t *next = curr->next;
        free_user(curr);
        curr = next;
    }
    free(users);
}