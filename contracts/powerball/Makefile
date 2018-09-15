# Makefile Commands:
#		all			Compile all source code to web assembly and create an abi
#					file 
#		test		tests the smart contract must be implemented on a per
#					contract basis
#		node		starts an instance of nodeos to allow for smart contract 
#					execution
#		create		creates a smart contract account on the blockchain as well
#					as a 'user' and 'tester' account, uses the Public key and
#					Private Key provided
#		unlock		Unlocks the wallet with the current Wallet key and imports
#					the private key provided 
#		clean		removes all files that are added during the compilation 
#					process 
#		get			Shows all the accounts currently registered to the public 
# 					key
#		push		push the smart contract onto the eos blockchain

WALLET_KEY=PW5KYZMUPboZPPhHMj52VyrBWRmcfs5C5wdN2ePZGfDSANxHJkcFr
PRIVATE_KEY=5J6RgZ1K4dEARF6unvCRGa56Qoe9vstAKXN8ggufk3CrBPStE7r
PUBLIC_KEY=EOS7qCdhMB48Nna82tTpKQwwu4W2EWtuLAsVwxuKxFredTutFYM5C


CONTRACT_NAME := $(notdir $(CURDIR))
ACCOUNT_NAME := $(CONTRACT_NAME) 

WAST_FILE := $(CONTRACT_NAME).wast
ABI_FILE := $(CONTRACT_NAME).abi
WASM_FILE := $(CONTRACT_NAME).wasm
CPP_FILE := $(CONTRACT_NAME).cpp
HPP_FILE := $(CONTRACT_NAME).hpp


all: $(ABI_FILE)

$(ABI_FILE): $(WAST_FILE)
	@#eosiocpp --genabi $(ABI_FILE) $(HPP_FILE)

$(WAST_FILE): $(CPP_FILE)
	eosiocpp --outname $(WAST_FILE) $(CPP_FILE)

test: $(ABI_FILE)
	@echo "Must implement!"

create:
	cleos create account eosio pwrball.user $(PUBLIC_KEY)
	cleos create account eosio pwrball.test $(PUBLIC_KEY) 
	cleos create account eosio $(ACCOUNT_NAME) $(PUBLIC_KEY)

node:
	@gnome-terminal -- nodeos -e -p eosio --plugin eosio::chain_api_plugin\
		--plugin eosio::history_api_plugin --contracts-console
	
clean:
	rm -f $(WAST_FILE) $(ABI_FILE) $(WASM_FILE)

push:
	cleos set contract $(CONTRACT_NAME) ../$(CONTRACT_NAME)\
		-p $(CONTRACT_NAME)@active

unlock:
	cleos wallet unlock --password $(WALLET_KEY)
	cleos wallet import --private-key $(PRIVATE_KEY)

get:
	cleos get accounts $(PUBLIC_KEY)
