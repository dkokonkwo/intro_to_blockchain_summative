#include "blockchain.h"

/**
 * serialize_utxo - serialize unspent transactions to a file
 * @unspent: pointer to list of unspent transactions
 * Return: 1 on sucess else 0 on failure
 */
int serialize_utxo(utxo_t *unspent)
{
    FILE *file = fopen(UTXO_DATABASE, "wb");
    if (!file)
    {
        printf("Failed to open file for serialization\n");
        fclose(file);
        return 0;
    }
    Transaction *current = unspent->head;
    while (current)
    {
        fwrite(&current->index, sizeof(current->index), 1, file);
        fwrite(current->sender, sizeof(current->sender), 1, file);
        fwrite(current->receiver, sizeof(current->receiver), 1, file);
        fwrite(&current->amount, sizeof(current->amount), 1, file);
        current = current->next;
    }
    fclose(file);
    return 1;
}

/**
 * deserialize_utxo - get unpsent transactions from pool(file)
 * Return: pointer to list of unpsent transactions else exit
 */
utxo_t *deserialize_utxo(void)
{
    FILE *file = fopen(UTXO_DATABASE, "rb");
    if (!file) {
        perror("Failed to open file for deserialization of unpsent transactions");
        return NULL;
    }

    utxo_t *unspent_transactions = (utxo_t *)malloc(sizeof(utxo_t));
    if (!unspent_transactions) {
        perror("Failed to allocate memory for unspent transactions");
        fclose(file);
        return NULL;
    }
    
    unspent_transactions->head = unspent_transactions->tail = NULL;
    unspent_transactions->nb_trans = 0;

    while (!feof(file))
    {
        Transaction *transaction = (Transaction *)malloc(sizeof(Transaction));
        if (!transaction)
        {
            perror("Failed to allocate memory for transaction");
            fclose(file);
            free(unspent_transactions);
            return NULL;
        }

        if (fread(&transaction->index, sizeof(transaction->index), 1, file) != 1) {
            free(transaction);
            break;
        }
        fread(transaction->sender, sizeof(transaction->sender), 1, file);
        fread(transaction->receiver, sizeof(transaction->receiver), 1, file);
        fread(&transaction->amount, sizeof(transaction->amount), 1, file);

        transaction->next = NULL;
        if (unspent_transactions->head == NULL) {
            unspent_transactions->head = unspent_transactions->tail = transaction;
        } else {
            unspent_transactions->tail->next = transaction;
            unspent_transactions->tail = transaction;
        }

        unspent_transactions->nb_trans++;
    }

    fclose(file);
    return unspent_transactions;
}

/**
 * add_transaction - adds transaction to unspent transactions pool(file)
 * @sender: sender details
 * @receiver: receiver details
 * @amount: amount of transaction
 * Return: 1 on success or 0 on failure
 */
int add_transaction(const char *sender, const char *receiver, int amount)
{
    utxo_t *unspent;
    Transaction *new_trans;
    if (!sender || !receiver || !amount)
    {
        fprintf(stderr, "Wrong details\n");
        return 0;
    }

    unspent = deserialize_utxo();
    if (!unspent)
    {
        fprintf(stderr, "Error deserializing unspent transactions\n");
        return 0;
    }

    new_trans = (Transaction *)malloc(sizeof(Transaction));
     if (!new_trans)
    {
        fprintf(stderr, "Failed to allocate memory for new transaction\n");
        free(unspent);
        return 0;
    }
    new_trans->index = unspent->nb_trans;
    unspent->nb_trans++;

    strncpy(new_trans->sender, sender, DATASIZE_MAX - 1);
    new_trans->sender[DATASIZE_MAX - 1] = '\0';
    strncpy(new_trans->receiver, receiver, DATASIZE_MAX - 1);
    new_trans->receiver[DATASIZE_MAX - 1] = '\0';

    new_trans->amount = amount;
    new_trans->next = NULL;

    if (!unspent->head)
        unspent->head = unspent->tail = new_trans;
    else
    {
        unspent->tail->next = new_trans;
        unspent->tail = new_trans;
    }

    if (!serialize_utxo(unspent))
    {
        fprintf(stderr, "Could not serialize unspent with new transaction\n");
        free(new_trans);
        free(unspent);
        return 0;
    }

    printf("Transaction saved!\n");
    free(new_trans);
    free(unspent);
    return 1;
}

/**
 * free_transactions - frees list of transactions
 * transactions: pointer to list of transactions
 */
void free_transactions(utxo_t *transactions)
{
    if(!transactions)
        return;
    Transaction *current = transactions->head;
    while (current)
    {
        Transaction *next = current->next;
        free(current);
        current = next;
    }
    free(transactions);
}


/**
 * verify_transaction - means of user authentication
 * @sender: sender's address to verify
 * @receiver: receiver's address to verify
 * Return: 1 if addresses are verified else 0 on failure
 */
int verify_transaction(const char *sender, const char *receiver)
{
    user_t *curr, *recv;
    lusers *all_users;
    if (!sender || !receiver)
    {
        fprintf(stderr, "Wrong details\n");
        return 0;
    }
    // check to see if sender's address matches current logged in user
    curr = get_user(NULL);
    if (!curr || !curr->wallet)
    {
        fprintf(stderr, "Sender has no wallet\n");
        return 0;
    }
    if (strcmp(curr->wallet->address, sender) !=0)
    {
        fprintf(stderr, "Unverified sender\n");
        return 0;
    }
    all_users = deserialize_users();
    if (!all_users || !all_users->length)
    {
        fprintf(stderr, "Problem verifying receiver address\n");
        return 0;
    }
    for (recv = all_users->head; recv; recv = recv->next)
    {
        if (recv->wallet)
        {
            if (strcmp(recv->wallet->address, receiver) == 0)
            {
                printf("Transaction verified\n");
                return 1;
            }
        }
    }

    fprintf(stderr, "Could not verify receiver or receiver address\n");
    return 0;
}