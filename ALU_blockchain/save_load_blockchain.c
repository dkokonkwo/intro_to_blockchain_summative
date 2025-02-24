#include "blockchain.h"

/**
 * serialize_blockchain - serializes(backs up) a blockchain to a file
 * @blockchain: pointer to blockchain to serialize
 * Return: 1 on success else 0 on failure
 */
int serialize_blockchain(Blockchain *blockchain)
{
    FILE *file = fopen(BLOCKCHAIN_DATABASE, "wb");
    if (!file)
    {
        fprintf(stderr, "Failed to open file for serialization\n");
        return 0;
    }

    fwrite(&blockchain->difficulty, sizeof(blockchain->difficulty), 1, file);

    Block *current = blockchain->head;
    while (current) {
        fwrite(&current->index, sizeof(current->index), 1, file);
        fwrite(current->timestamp, sizeof(current->timestamp), 1, file);
        fwrite(&current->nonce, sizeof(current->nonce), 1, file);
        fwrite(current->previous_hash, SHA256_DIGEST_LENGTH, 1, file);
        fwrite(current->current_hash, SHA256_DIGEST_LENGTH, 1, file);

         if (current->transactions)
        {
            fwrite(&current->transactions->nb_trans, sizeof(current->transactions->nb_trans), 1, file);

            Transaction *trans = current->transactions->head;
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

        current = current->next;
    }

    fclose(file);
    freeBlockchain(blockchain);
    return 1;
}


/**
 * deserialize_blockchain - deserializes blockchain from a file
 * Return: pointer to blockchain or NULL on failure
 */
Blockchain *deserialize_blockchain(void)
{
    FILE *file = fopen(BLOCKCHAIN_DATABASE, "rb");
    if (!file)
    {
        perror("Failed to open blockchain file, initializing a new blockchain...");
        return init_blockchain();
    }

    Blockchain *blockchain = (Blockchain *)malloc(sizeof(Blockchain));
    if (!blockchain)
    {
        perror("Failed to allocate memory for blockchain");
        fclose(file);
        return NULL;
    }

    blockchain->head = blockchain->tail = NULL;
    blockchain->length = 0;

    if (fread(&blockchain->difficulty, sizeof(blockchain->difficulty), 1, file) != 1)
    {
        perror("Failed to read blockchain difficulty");
        free(blockchain);
        fclose(file);
        return NULL;
    }

    Block *prevBlock = NULL;
    while (1)
    {
        Block *block = (Block *)malloc(sizeof(Block));
        if (!block)
        {
            perror("Failed to allocate memory for block");
            break;
        }

        if (fread(&block->index, sizeof(block->index), 1, file) != 1)
        {
            free(block);
            break;
        }

        fread(block->timestamp, sizeof(block->timestamp), 1, file);
        fread(&block->nonce, sizeof(block->nonce), 1, file);
        fread(block->previous_hash, SHA256_DIGEST_LENGTH, 1, file);
        fread(block->current_hash, SHA256_DIGEST_LENGTH, 1, file);

        utxo_t *transactions = (utxo_t *)malloc(sizeof(utxo_t));
        if (!transactions)
        {
            perror("Failed to allocate memory for transactions");
            free(block);
            break;
        }

        if (fread(&transactions->nb_trans, sizeof(transactions->nb_trans), 1, file) != 1)
        {
            free(transactions);
            free(block);
            break;
        }

        transactions->head = transactions->tail = NULL;

        for (int i = 0; i < transactions->nb_trans; i++)
        {
            Transaction *trans = (Transaction *)malloc(sizeof(Transaction));
            if (!trans)
            {
                perror("Failed to allocate memory for transaction");
                break;
            }

            if (fread(&trans->index, sizeof(trans->index), 1, file) != 1 ||
                fread(trans->sender, sizeof(trans->sender), 1, file) != 1 ||
                fread(trans->receiver, sizeof(trans->receiver), 1, file) != 1 ||
                fread(&trans->amount, sizeof(trans->amount), 1, file) != 1 ||
                fread(&trans->status, sizeof(trans->status), 1, file) != 1)
            {
                free(trans);
                break;
            }

            trans->next = NULL;
            if (transactions->head == NULL)
            {
                transactions->head = transactions->tail = trans;
            }
            else
            {
                transactions->tail->next = trans;
                transactions->tail = trans;
            }
        }

        block->transactions = transactions;
        block->next = NULL;

        if (blockchain->head == NULL)
            blockchain->head = blockchain->tail = block;
        else
        {
            prevBlock->next = block;
            blockchain->tail = block;
        }

        prevBlock = block;
        blockchain->length++;
    }

    fclose(file);
    return blockchain;
}