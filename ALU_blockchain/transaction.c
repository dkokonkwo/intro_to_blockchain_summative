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
int add_transaction(unsigned char *sender, unsigned char *receiver, int amount)
{
    utxo_t *unspent;
    Transaction *new_trans;
    if (!sender || !receiver || amount <= 0)
    {
        fprintf(stderr, "Wrong details\n");
        return 0;
    }

    if (!verify_transaction(sender, receiver))
    {
        fprintf(stderr, "Could not verify transaction\n");
        return 0;
    }
    printf("Details verified\n");

    unspent = deserialize_utxo();
    if (!unspent)
    {
        fprintf(stderr, "Error deserializing unspent transactions\n");
        printf("Creating new unspent file\n");
        unspent = (utxo_t *)malloc(sizeof(utxo_t));
        if (!unspent)
        {
            fprintf(stderr, "Could not allocate mmemory for new unspent\n");
            return 0;
        }
        unspent->head = unspent->tail = NULL;
        unspent->nb_trans = 0;
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

    memcpy(new_trans->sender, sender, ADDRESS_SIZE);
    memcpy(new_trans->receiver, receiver, ADDRESS_SIZE);
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
int verify_transaction(unsigned char *sender, unsigned char *receiver)
{
    user_t *curr, *recv;
    if (!sender || !receiver)
    {
        fprintf(stderr, "Wrong details\n");
        return 0;
    }
    // check to see if sender's address matches current logged in user
    curr = get_user(NULL);
    if (!curr)
    {
        fprintf(stderr, "Sender details not valid\n");
        return 0;
    }
    if (!curr->wallet)
    {
        fprintf(stderr, "Sender has no wallet\n");
        free(curr);
        return 0;
    }
    if (memcmp(curr->wallet->address, sender, ADDRESS_SIZE) !=0)
    {
        fprintf(stderr, "Unverified sender\n");
        return 0;
    }
    free_user(curr);
    recv = get_user(receiver);
    if (!recv)
    {
        fprintf(stderr, "Receiver details not valid\n");
        return 0;
    }
    if (!recv->wallet)
    {
        fprintf(stderr, "Recevier has no wallet\n");
        free(recv);
        return 0;
    }
    free_user(recv);
    return 1;
}