#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define DATAMAX 1024

/**
 * struct block_s - block structure for a job posting
 * @index: index of the block in Blockchain
 * @timestamp: time job was posted
 * @title: job title
 * @company: job company
 * @location: job location
 * @desc: job description
 * @prev_hash: previous block's hash
 * @curr_hash: current block's hash
 * @next: pointer to next block in blockchain
 */
typedef struct block_s {
    int index;
    char timestamp[26];
    char title[DATAMAX];
    char company[DATAMAX];
    char location[DATAMAX];
    char desc[DATAMAX];
    unsigned char prev_hash[SHA256_DIGEST_LENGTH];
    unsigned char curr_hash[SHA256_DIGEST_LENGTH];
    struct block_s *next;
} block_t;

/**
 * struct Blockchain - blockchain structure
 * @head: pointer to head block in blockchain
 * @tail: pointer to last block in blockchain
 * @length: length of blockchain
 */
typedef struct Blockchain {
    block_t *head;
    block_t *tail;
    int length;
} Blockchain;

Blockchain *init_blockchain(void);
int validate_blockchain(Blockchain *blockchain);
void print_blockchain(Blockchain *blockchain);
void free_blockchain(Blockchain *blockchain);

block_t *create_block(int index, const char *title, const char *company,
const char *location, const char *description, unsigned char *prev_hash);
void add_block(Blockchain *blockchain, block_t *block);
int calculate_hash(block_t *block, unsigned char *hash);

Blockchain *create_job_postings(void);
void tamper_block(block_t *block);
void search_job(Blockchain *blockchain, char *keyword);

#endif