Question 1: `Creating a Blockchain-based Job Directory Application in C`

- Create a blockchain-based job directory application in C that allows applicants to search and manage job listings. The application should simulate a blockchain system where each job listing is treated as a block containing information such as job title, company, location, and job description. Each block should be cryptographically hashed to ensure data integrity and linked to the previous block, mimicking the structure of a blockchain. Implement functions to add new job listings, search for jobs by keywords, and verify the integrity of the job directory. Demonstrate the immutability and integrity of the blockchain by detecting any unauthorized modification to job listings."


**HOW TO COMPILE AND RUN THE PROGRAM:**

1. **Source File**: The program's main entry point is in the `main.c` file.
2. **Compilation**: Use the `gcc` compiler, ensuring you include the linker flags `-lssl` and `-lcrypto`.
   
   **Example Command**:
   ```bash
   gcc -o job_directory.out blockchain_functions.c main.c -lssl -lcrypto
   ```
3. **Execution**: Run the resulting executable by typing the following command in your Linux terminal:
   ```bash
   ./main
   ```

This program implements a blockchain-like structure to store job postings securely.
It follows blockchain principles through the following mechanisms:

1. **Chaining Blocks with Cryptographic Hashing**  
   Each block contains a SHA-256 hash of its data and a reference (`prev_hash`) to the previous block's hash, ensuring immutability and integrity.

2. **Genesis Block**  
   The blockchain is initialized with a "Genesis Block," which serves as the first entry in the chain, similar to Bitcoin's blockchain structure.

3. **Tamper Detection**  
   The `validate_blockchain` function recalculates hashes and verifies that each block’s `prev_hash` matches the `curr_hash` of the previous block. If a block is modified (`tamper_block` function), this validation will fail, ensuring data integrity.

4. **Decentralized Verification**  
   Although this implementation does not use a distributed network, the integrity of the blockchain can be validated at any time by recalculating and comparing hashes, similar to how blockchain nodes verify transactions.

5. **Data Transparency and Searchability**  
   The blockchain enables job listings to be added, retrieved (`print_blockchain`), and searched (`search_job`), ensuring a secure and transparent record of job postings.


`Submission Requirements:`

`Source Code:`
- Submit the complete C source code files (.c and .h if applicable) with all functions implemented as required.
    - Ensure the code is well-commented to explain the logic and functionality of each function.
- ReadMe File:
Instructions on how to compile and run the program.
A brief description of how the program uses blockchain principles.
An explanation of the hashing mechanism and how it ensures data integrity.
Provide a README.md file that includes:
- Example Output:
Adding job listings to the blockchain.
Searching for jobs by keyword.
Verifying the integrity of the job directory.
Detecting integrity breaches if any job listing is tampered with.
Include a sample output file (output.txt) that demonstrates:
- Documentation:
An overview of how blockchain principles are implemented in the application.
The cryptographic hashing mechanism used and its importance in maintaining data integrity.
Any challenges faced and how they were resolved.
Write a document (documentation.pdf) that covers:
- Executable File:
Submit a compiled executable file (job_directory.exe or job_directory.out) for easy testing of the program.
- Code Quality:
Ensure that the code is clean, follows standard naming conventions, and is free of memory leaks or runtime errors.
Use tools like valgrind or AddressSanitizer to check for memory issues.