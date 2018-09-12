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

WALLET_KEY=PW5KYZMUPboZPPhHMj52VyrBWRmcfs5C5wdN2ePZGfDSANxHJkcFr
PRIVATE_KEY=5K7rA7Xmkwh66ScwMVvmKFJWiS5SDy1c4rt1At2AMGZnYCifWUv
PUBLIC_KEY=EOS84EtLNLFyFvYi4v3ZdhyNba9dg3C4FsaKg85s32F6jiGfbZeAj


CONTRACT_NAME := $(notdir $(CURDIR))
ACCOUNT_NAME=tic.tac.toe

WAST_FILE := $(CONTRACT_NAME).wast
ABI_FILE := $(CONTRACT_NAME).abi
WASM_FILE := $(CONTRACT_NAME).wasm
CPP_FILE := $(CONTRACT_NAME).cpp
HPP_FILE := $(CONTRACT_NAME).hpp


all: $(ABI_FILE)

$(ABI_FILE): $(WAST_FILE)
	eosiocpp --genabi $(ABI_FILE) $(HPP_FILE)
	cleos set contract $(CONTRACT_NAME) ../$(CONTRACT_NAME)\
		-p $(CONTRACT_NAME)@active

$(WAST_FILE): $(CPP_FILE)
	eosiocpp --outname $(WAST_FILE) $(CPP_FILE)

test: $(ABI_FILE)
	cleos push action $(CONTRACT_NAME) hi '["user"]' -p user@active

create:
	cleos create account eosio $(ACCOUNT_NAME) $(PUBLIC_KEY)
	cleos create account eosio tester $(PUBLIC_KEY)
	cleos create account eosio user $(PUBLIC_KEY) 

node:
	gnome-terminal -- nodeos -e -p eosio --plugin eosio::chain_api_plugin\
		--plugin eosio::history_api_plugin --contracts-console
	
clean:
	echo $(ACCOUNT_NAME)
	rm -f $(WAST_FILE) $(ABI_FILE) $(WASM_FILE)

unlock:
	cleos wallet unlock --password $(WALLET_KEY)
	cleos wallet import --private-key $(PRIVATE_KEY)
