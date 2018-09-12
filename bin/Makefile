WALLET_KEY=PW5KYZMUPboZPPhHMj52VyrBWRmcfs5C5wdN2ePZGfDSANxHJkcFr
PUBLIC_KEY=EOS7Wy8YG5VNnaZ3ueJKKKgx3JwJgayNRWUofY9uu3tWEN42TPWwF

CONTRACT_NAME := $(notdir $(CURDIR))

WAST_FILE := $(CONTRACT_NAME).wast
ABI_FILE := $(CONTRACT_NAME).abi
WASM_FILE := $(CONTRACT_NAME).wasm
CPP_FILE := $(CONTRACT_NAME).cpp


all: $(ABI_FILE)

$(ABI_FILE): $(WAST_FILE)
	eosiocpp --genabi $(ABI_FILE) $(CPP_FILE)
	cleos set contract $(CONTRACT_NAME).code ../$(CONTRACT_NAME)\
		-p $(CONTRACT_NAME).code@active

$(WAST_FILE): $(CPP_FILE)
	eosiocpp --outname $(WAST_FILE) $(CPP_FILE)

test: $(ABI_FILE)
	cleos push action $(CONTRACT_NAME).code hi '["user"]' -p user@active

create:
	gnome-terminal -- nodeos -e -p eosio --plugin eosio::chain_api_plugin\
		--plugin eosio::history_api_plugin --contracts-console

	cleos create account eosio $(CONTRACT_NAME).code $(PUBLIC_KEY)
	
clean:
	rm -f $(WAST_FILE) $(ABI_FILE) $(WASM_FILE)

unlock:
	cleos wallet unlock --password $(WALLET_KEY)
