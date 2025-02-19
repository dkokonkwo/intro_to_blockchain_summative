#include "blockchain.h"

void add_job(Blockchain *blockchain);
void search_job_by_keyword(Blockchain *blockchain);
void tamper_with_block(Blockchain *blockchain);

int main(void)
{
    int choice;
    Blockchain *blockchain = create_job_postings();
    
    if (!blockchain)
    {
        fprintf(stderr, "Failed to create job postings.\n");
        return EXIT_FAILURE;
    }

    while (1)
    {
        printf("\n=== Job Directory (Blockchain) ===\n");
        printf("1. Add a Job\n");
        printf("2. Search for a Job by Keyword\n");
        printf("3. Check Blockchain Integrity\n");
        printf("4. Tamper with a Block\n");
        printf("5. Print Blockchain\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline left in buffer

        switch (choice)
        {
        case 1:
            add_job(blockchain);
            break;
        case 2:
            search_job_by_keyword(blockchain);
            break;
        case 3:
            if (validate_blockchain(blockchain))
                printf("Blockchain integrity verified. No tampering detected.\n");
            else
                printf("Blockchain has been tampered with!\n");
            break;
        case 4:
            tamper_with_block(blockchain);
            break;
        case 5:
            print_blockchain(blockchain);
            break;
        case 6:
            free_blockchain(blockchain);
            printf("Exiting...\n");
            return EXIT_SUCCESS;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }

    return EXIT_SUCCESS;
}

/**
 * add_job - Prompts user for job details and adds a new job to the blockchain
 */
void add_job(Blockchain *blockchain)
{
    char title[DATAMAX], company[DATAMAX], location[DATAMAX], description[DATAMAX];

    printf("Enter job title: ");
    fgets(title, DATAMAX, stdin);
    title[strcspn(title, "\n")] = '\0';

    printf("Enter company name: ");
    fgets(company, DATAMAX, stdin);
    company[strcspn(company, "\n")] = '\0';

    printf("Enter job location: ");
    fgets(location, DATAMAX, stdin);
    location[strcspn(location, "\n")] = '\0';

    printf("Enter job description: ");
    fgets(description, DATAMAX, stdin);
    description[strcspn(description, "\n")] = '\0';

    block_t *prev_block = blockchain->tail;
    block_t *new_block = create_block(prev_block->index + 1, title, company, location, description, prev_block->curr_hash);

    if (!new_block)
    {
        fprintf(stderr, "Failed to add job.\n");
        return;
    }

    add_block(blockchain, new_block);
    printf("Job successfully added!\n");
}

/**
 * search_job_by_keyword - Searches for jobs containing a keyword
 */
void search_job_by_keyword(Blockchain *blockchain)
{
    char keyword[DATAMAX];

    printf("Enter keyword to search for: ");
    fgets(keyword, DATAMAX, stdin);
    keyword[strcspn(keyword, "\n")] = '\0';

    search_job(blockchain, keyword);
}

/**
 * tamper_with_block - Allows user to modify a block's data
 */
void tamper_with_block(Blockchain *blockchain)
{
    int block_index;
    block_t *current = blockchain->head;

    printf("Enter block index to tamper with: ");
    scanf("%d", &block_index);
    getchar(); // Consume newline

    while (current)
    {
        if (current->index == block_index)
        {
            tamper_block(current);
            printf("Block %d has been tampered with.\n", block_index);
            return;
        }
        current = current->next;
    }

    printf("Block with index %d not found.\n", block_index);
}
