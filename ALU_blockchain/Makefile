CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -pedantic

# linkers
LDFLAGS = -lssl -lcrypto

# Binary directory
BIN_DIR = /usr/bin

# Header files
HEADERS = blockchain.h

SRC = login_main.c nodes.c transaction_main.c balance_main.c blockchain_info_main.c mine_functions.c wallet_functions.c blockchain.c create_user_main.c mine_main.c transaction.c wallet_main.c sample_blockchain.c alu_account.c show_current_user.c

# Object files
OBJS = $(SRC:.c=.o)

# Default target: build all CLI tools
all: create_wallet initiate_transaction mine_block blockchain_info view_balance login_user create_user init_blockchain show_user

# Compile object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# CLI Commands (linking against object files)
create_wallet: wallet_main.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ wallet_main.c blockchain.c wallet_functions.c transaction.c mine_functions.c nodes.c $(LDFLAGS)

initiate_transaction: transaction_main.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ transaction_main.c transaction.c blockchain.c mine_functions.c nodes.c wallet_functions.c $(LDFLAGS)

mine_block: mine_main.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ mine_main.c mine_functions.c blockchain.c nodes.c save_load_blockchain.c transaction.c wallet_functions.c $(LDFLAGS)

blockchain_info: blockchain_info_main.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ blockchain_info_main.c blockchain.c save_load_blockchain.c nodes.c mine_functions.c alu_account.c transaction.c wallet_functions.c $(LDFLAGS)

view_balance: balance_main.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ wallet_functions.c blockchain.c balance_main.c mine_functions.c nodes.c transaction.c $(LDFLAGS)

login_user: login_main.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ login_main.c nodes.c blockchain.c mine_functions.c transaction.c wallet_functions.c $(LDFLAGS)

create_user: create_user_main.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ create_user_main.c nodes.c blockchain.c mine_functions.c transaction.c wallet_functions.c $(LDFLAGS)

init_blockchain: init_blockchain.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ init_blockchain.c save_load_blockchain.c blockchain.c mine_functions.c sample_blockchain.c transaction.c nodes.c alu_account.c wallet_functions.c $(LDFLAGS)

show_user: show_current_user.c $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ show_current_user.c save_load_blockchain.c blockchain.c mine_functions.c sample_blockchain.c transaction.c nodes.c alu_account.c wallet_functions.c $(LDFLAGS)

# Clean up the build
clean:
	rm -f *.o *.dat $(BIN_DIR)/mine_block $(BIN_DIR)/initiate_transaction $(BIN_DIR)/create_user $(BIN_DIR)/login_user $(BIN_DIR)/blockchain_info $(BIN_DIR)/view_balance $(BIN_DIR)/create_wallet $(BIN_DIR)/init_blockchain

# Rebuild everything
rebuild: clean all