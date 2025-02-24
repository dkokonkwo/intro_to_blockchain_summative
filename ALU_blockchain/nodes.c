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
        fprintf(stderr, "Could not get user from file\n");
        free(new_user);
        return 0;
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
    serialize_users(users);
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
            

            if (current->wallet->transactions)
            {
                fwrite(&current->wallet->nb_trans, sizeof(current->wallet->nb_trans), 1, file);

                Transaction *trans = current->wallet->transactions->head;
                while (trans)
                {
                    fwrite(&trans->index, sizeof(trans->index), 1, file);
                    fwrite(trans->sender, sizeof(trans->sender), 1, file);
                    fwrite(trans->receiver, sizeof(trans->receiver), 1, file);
                    fwrite(&trans->amount, sizeof(trans->amount), 1, file);
                    fwrite(&trans->status, sizeof(trans->status), 1, file);
                    trans = trans->next;
                }
            }
            else
            {
                int zero_trans = 0;
                fwrite(&zero_trans, sizeof(zero_trans), 1, file);
            }
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
        fprintf(stderr, "Failed to open users file");
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

            int nb_trans;
            fread(&nb_trans, sizeof(nb_trans), 1, file);
            user->wallet->nb_trans = nb_trans;

            if (nb_trans > 0)
            {
                user->wallet->transactions = (utxo_t *)malloc(sizeof(utxo_t));
                if (!user->wallet->transactions)
                {
                    perror("Failed to allocate memory for transactions");
                    free(user->wallet);
                    free(user);
                    break;
                }

                user->wallet->transactions->head = NULL;
                user->wallet->transactions->nb_trans = nb_trans;

                Transaction *prevTrans = NULL;
                for (int j = 0; j < nb_trans; j++)
                {
                    Transaction *trans = (Transaction *)malloc(sizeof(Transaction));
                    if (!trans)
                    {
                        perror("Failed to allocate memory for transaction");
                        break;
                    }

                    fread(&trans->index, sizeof(trans->index), 1, file);
                    fread(trans->sender, sizeof(trans->sender), 1, file);
                    fread(trans->receiver, sizeof(trans->receiver), 1, file);
                    fread(&trans->amount, sizeof(trans->amount), 1, file);
                    fread(&trans->status, sizeof(trans->status), 1, file);
                    trans->next = NULL;

                    if (prevTrans == NULL)
                        user->wallet->transactions->head = trans;
                    else
                        prevTrans->next = trans;
                    prevTrans = trans;
                }
            }
            else
                user->wallet->transactions = NULL;
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
        fprintf(stderr, "Could not get all users\n");
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
 * get_user - get current session user
 * Return: pointer to to session user else NULL
 */
user_t *get_user(void)
{
    lusers *all_users;
    user_t *sesh_user, *current_user;
    FILE *file = fopen(SESSION_USER, "rb");
    if (!file)
    {
        fprintf(stderr, "Failed to open session user file...");
        return NULL;
    }

    sesh_user = (user_t *)malloc(sizeof(user_t));
    if (!sesh_user)
    {
        perror("Failed to allocate memory for session user");
        fclose(file);
        return NULL;
    }

    sesh_user->wallet = NULL;
    sesh_user->next = NULL;

    fread(&sesh_user->role, sizeof(sesh_user->role), 1, file);
    fread(&sesh_user->index, sizeof(sesh_user->index), 1, file);
    fread(sesh_user->name, sizeof(sesh_user->name), 1, file);
    fclose(file);

    all_users = deserialize_users();
    current_user = all_users->head;
    while (current_user)
    {
        if (strcmp(current_user->name, sesh_user->name) == 0 && current_user->index == sesh_user->index)
        {
            free(sesh_user);
            free_users(all_users);
            return current_user;
        }
        current_user = current_user->next;
    }
    fprintf(stderr, "Could not find user\n");
    free(sesh_user);
    free_users(all_users);
    return (current_user);
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
        if (curr->wallet)
        {
            if (curr->wallet->transactions)
            {
                Transaction *tx = curr->wallet->transactions->head;
                while (tx)
                {
                    Transaction *nxt = tx->next;
                    free(tx);
                    tx = nxt;
                }
            }
            free(curr->wallet);
        }
        free(curr);
        curr = next;
    }
    free(users);
}