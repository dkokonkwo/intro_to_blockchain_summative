#include "blockchain.h"

/**
 * calculate_hash - calculates hash of block
 * @block: pointer to block to calcualate hash
 * @hash: buffer to store hash
 * Return: 1 on sucess else 0 on failure
 */
int calculate_hash(block_t *block, unsigned char *hash)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        return 0;
    }

     /* Initialize SHA-256 hashing */
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), NULL) != 1) {
        fprintf(stderr, "Failed to initialize hash\n");
        EVP_MD_CTX_free(ctx);
        return 0;
    }

    /* Adding block elements to hash calculation */
    if (EVP_DigestUpdate(ctx, &block->index, sizeof(block->index)) != 1 ||
        EVP_DigestUpdate(ctx, block->timestamp, strlen(block->timestamp)) != 1 ||
        EVP_DigestUpdate(ctx, block->prev_hash, SHA256_DIGEST_LENGTH) != 1 ||
        EVP_DigestUpdate(ctx, block->title, strlen(block->title)) != 1 ||
        EVP_DigestUpdate(ctx, block->company, strlen(block->company)) != 1 ||
        EVP_DigestUpdate(ctx, block->location, strlen(block->location)) != 1 ||
        EVP_DigestUpdate(ctx, block->desc, strlen(block->desc)) != 1) {
        fprintf(stderr, "Failed to update hash\n");
        EVP_MD_CTX_free(ctx);
        return 0;
    }

    /* Finialize and store hash */
    if (EVP_DigestFinal_ex(ctx, hash, NULL) != 1) {
        fprintf(stderr, "Failed to finalize hash\n");
        EVP_MD_CTX_free(ctx);
        return 0;
    }

    EVP_MD_CTX_free(ctx);
    return 1;
}

/**
 * create_block - create a new block
 * @index: block index
 * @title: job title
 * @company: job company
 * @location: job location
 * @description: job description
 * @prev_hash: previous block's hash
 * Return: pointer to new block else NULL
 */
block_t *create_block(int index, const char *title, const char *company,
const char *location, const char *description, unsigned char *prev_hash)
{
    block_t *block = (block_t*)malloc(sizeof(block_t));
    if (!block)
        return NULL;
    
    block->index = index;

    strncpy(block->title, title, DATAMAX);
    block->title[DATAMAX - 1] = '\0';

    strncpy(block->company, company, DATAMAX);
    block->company[DATAMAX - 1] = '\0';

    strncpy(block->location, location, DATAMAX);
    block->location[DATAMAX - 1] = '\0';

    strncpy(block->desc, description, DATAMAX);
    block->desc[DATAMAX - 1] = '\0';

    strncpy(block->timestamp, ctime(&(time_t){time(NULL)}), 25);

    if (prev_hash)
        memcpy(block->prev_hash, prev_hash, SHA256_DIGEST_LENGTH);
    else
        memset(block->prev_hash, 0, SHA256_DIGEST_LENGTH);
    memset(block->curr_hash, 0, SHA256_DIGEST_LENGTH);
    block->next = NULL;
    
    if (!calculate_hash(block, block->curr_hash))
    {
        free(block);
        fprintf(stderr, "Could not calculate block hash\n");
        return NULL;
    }
    return block;
}

/**
 * init_blockchain - initializes new blockchain with genesis block
 * Return: pointer to blockchain else Null on failure
 */
Blockchain *init_blockchain(void)
{
    block_t *genesis_block;
    unsigned char genesis_hash[SHA256_DIGEST_LENGTH] = {0};
    Blockchain *blockchain = (Blockchain *)malloc(sizeof(Blockchain));
    if (!blockchain)
        return NULL;

    genesis_block = create_block(0, "Genesis", "This Blockchain", "First Block", "Genesis block of blockchain", genesis_hash);

    if (!genesis_block)
    {
        fprintf(stderr, "Could not create genesis block\n");
        free(blockchain);
        return NULL;
    }

    blockchain->head = genesis_block;
    blockchain->tail = genesis_block;
    blockchain->length = 1;
    return blockchain;
}

/**
 * add_block - adds block(job posting) to blockchcain
 * @blockchcain: pointer to blockchain to add block
 * @block: pointer to block to add
 */
void add_block(Blockchain *blockchain, block_t *block)
{
    if (!blockchain || !block)
        return;
    
    if (blockchain->tail)
        blockchain->tail->next = block;
    else
        blockchain->head = block;
    blockchain->tail = block;
    blockchain->length++;
}

/**
 * print_blockchain - list all job postings in blockchain
 * @blockchain: pointer to blockchain
 */
void print_blockchain(Blockchain *blockchain)
{
    if (!blockchain)
        return;
    block_t *current = blockchain->head;
    while (current) {
        printf("Index: %d\nTimestamp: %s\nTitle: %s\nCompany: %s\nLocation: %s\nDescription: %s\n\n",
               current->index, current->timestamp, current->title,
               current->company, current->location, current->desc);
        current = current->next;
    } 
}

/**
 * free_blockchain: frees all blocks in blockchain and blockchain
 * @blockchain: pointer to blockchain to free
 */
void free_blockchain(Blockchain *blockchain) {
    if (!blockchain)
        return;
    block_t *current = blockchain->head;
    while (current) {
        block_t *temp = current;
        current = current->next;
        free(temp);
    }
    free(blockchain);
}

/**
 * seach_job - find jobs(blocks) that hav keyword in them
 * @blockchain: pointer to blockchain to search
 * @keyword: keyword to search for
 */
void search_job(Blockchain *blockchain, char *keyword)
{
    if (!blockchain || !blockchain->head)
        return;
    block_t *current = blockchain->head;
    while (current)
    {
        if (strstr(current->title, keyword) || strstr(current->company, keyword) || strstr(current->location, keyword)) {
            printf("Found Job:\nIndex: %d\nTitle: %s\nCompany: %s\nLocation: %s\n\n",
                   current->index, current->title, current->company, current->location);
        }
        current = current->next;
    }
}

/**
 * tamper_block - change block hash to mess with validity
 * @block: pointer to block to tamper with
 */
void tamper_block(block_t *block) {
    if (block) {
        strcpy(block->desc, "This job posting has been tampered with!");
        if (!calculate_hash(block, block->curr_hash))
            fprintf(stderr, "Could not recalculate tamper block hash\n");
    }
}

/**
 * validate_blockchain - ensures that previous block's hash matches with new block's hash
 * @blockchain: pointer to blockchain to validate
 * Return: 1 if valid, or 0 if invalid
 */
int validate_blockchain(Blockchain *blockchain)
{
    if (!blockchain || !blockchain->head)
        return 0;
    unsigned char tmp_hash[SHA256_DIGEST_LENGTH] = {0};
    unsigned char calculated_hash[SHA256_DIGEST_LENGTH];
    block_t *current = blockchain->head;

    while (current)
    {
        calculate_hash(current, calculated_hash);
        if (memcmp(current->curr_hash, calculated_hash, SHA256_DIGEST_LENGTH) != 0 || memcmp(current->prev_hash, tmp_hash, SHA256_DIGEST_LENGTH) != 0)
        {
            return 0;
        }    
        memcpy(tmp_hash, current->curr_hash, SHA256_DIGEST_LENGTH);
        current = current->next;
    }
    return 1;
}

/**
 * create_job_postings - creates a blockchain and adds 15 job postings
 * Return: pointer to the blockchain, or NULL on failure
 */
Blockchain *create_job_postings(void)
{
    Blockchain *blockchain = init_blockchain();
    if (!blockchain)
    {
        fprintf(stderr, "Failed to initialize blockchain\n");
        return NULL;
    }

    /* Sample job postings */
    char *titles[] = {
        "Software Engineer", "Data Scientist", "Backend Developer",
        "Frontend Developer", "DevOps Engineer", "Cybersecurity Analyst",
        "AI Researcher", "Product Manager", "Cloud Architect",
        "Mobile Developer", "Embedded Systems Engineer", "Database Administrator",
        "Game Developer", "Systems Analyst", "Network Engineer"
    };

    char *companies[] = {
        "TechCorp", "DataX", "CloudSolutions",
        "WebWorks", "DevOps Ltd", "SecureNet",
        "AI Innovations", "Product Inc", "CloudMasters",
        "AppDev", "EmbeddedTech", "DataSys",
        "GameWorld", "IT Solutions", "NetSecure"
    };

    char *locations[] = {
        "New York, NY", "San Francisco, CA", "Seattle, WA",
        "Austin, TX", "Boston, MA", "Chicago, IL",
        "Los Angeles, CA", "Denver, CO", "Atlanta, GA",
        "Miami, FL", "Houston, TX", "San Diego, CA",
        "Portland, OR", "Phoenix, AZ", "Dallas, TX"
    };

    char *descriptions[] = {
        "Develop and maintain software applications.",
        "Analyze large datasets and create machine learning models.",
        "Build robust backend systems and APIs.",
        "Create beautiful and responsive web interfaces.",
        "Manage infrastructure and automate deployments.",
        "Protect systems from cyber threats.",
        "Develop AI-driven applications and models.",
        "Lead product development and strategy.",
        "Design and implement cloud-based solutions.",
        "Develop mobile applications for iOS and Android.",
        "Work on embedded firmware and hardware integration.",
        "Manage databases and optimize queries.",
        "Develop and optimize video game software.",
        "Analyze business requirements and optimize systems.",
        "Design and maintain network infrastructures."
    };

    for (int i = 0; i < 15; i++)
    {
        block_t *prev_block = blockchain->tail;
        block_t *new_block = create_block(i + 1, titles[i], companies[i], locations[i], descriptions[i], prev_block->curr_hash);

        if (!new_block)
        {
            fprintf(stderr, "Failed to create job block %d\n", i + 1);
            free_blockchain(blockchain);
            return NULL;
        }

        add_block(blockchain, new_block);
    }

    return blockchain;
}
