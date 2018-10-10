//
// powerball.cpp 
//
// Created by apostolos on Thu Sep 13 13:40:18 PDT 2018
//

#include "powerball.hpp"  

//Allow for quick lookup of an account's tickets in
// a single round
template<typename T, typename K>
std::size_t find(std::vector<T> vec, K key){

    for (size_t i = 0; i < vec.size(); i++){
        if (vec[i].primary_key() == key) {
            return i;
        }
    }
    return -1;
}            


//@abi action
void powerball::buy(const account_name& buyer,
                    const std::vector<ticket_t>& tickets,
                    const int64_t& amount_paid){
    require_auth(buyer);
    eosio_assert(tickets.size() * TICKET_PRICE == amount_paid, 
            "Amount Paid does not match!");


    //Make sure powerball ticket numbers are in correct range
    for (auto& ticket : tickets){
        for (size_t i = 0; i < TICKET_LENGTH; i++){
            eosio_assert(ticket[i] > 0, "Ticket numbers must be positive!");
        }
        for (size_t i = 0; i < TICKET_LENGTH - 1; i++){
            eosio_assert(ticket[i] <= MAX_NUMBER, 
                    "Invalid number for ball!");
        }
        eosio_assert(ticket[TICKET_LENGTH-1] < MAX_POWERBALL_NUMBER, 
                "Invalid Length of powerball!");
		
        for (size_t i = 0; i < TICKET_LENGTH; i++){
            
            for (size_t j = i+1; j < TICKET_LENGTH ; j++){

				eosio_assert(ticket[i] == ticket[j] && i != j,
					"Must have unique elements for the ticket entry");
			}
		}
    }

    auto itr = this->rounds.find(this->current_round);
    //Check to see if round has expired
    if (now() > itr->end_time){

        this->rounds.modify(itr, buyer, [&](auto& t){
                t.draw_block = tapos_block_num() + 5;
        });

        this->current_round++;

        this->rounds.emplace(this->current_round, [&](auto& t){
                t.end_time =  now() + ROUND_LENGTH;
        });
        itr = this->rounds.find(this->current_round);
    }

    for (auto& ticket : tickets){
        this->rounds.modify(itr, buyer, [&](auto& t){
                size_t account_entry_index = find(t.ticket_table, buyer);

                //if account hasn't bought anything yet, register account
                if (account_entry_index == -1) {
                    std::vector<ticket_t> buyer_entry;
                    buyer_entry.push_back(ticket);
                    t.ticket_table.push_back({buyer, buyer_entry}); 
                   
                } else {
                    //add ticket to vector for that user
                    t.ticket_table[account_entry_index].account_tickets.push_back(ticket);
                }
        });
    }

    //TODO: Actually add the value to the smart contract balance
    //add the amount paid to the smart contract balance
    //eosio::tr
    this->balance += amount_paid;
}

//@abi action
void powerball::drawnumbers(const int64_t& round_num){
    //Assert current round_num exists
    auto itr = this->rounds.find(round_num);
    eosio_assert(itr != this->rounds.end(), "Round doesn't exist");

    int64_t draw_block = itr->draw_block;
    
    eosio_assert(now() > itr->end_time, "Time has ended");
    eosio_assert(tapos_block_num() >= draw_block, "Round hasn't ended!");
    eosio_assert(itr->winning_nums[0] == 0, "Numbers already draw!");
    
    checksum256 rand;
    for (size_t i = 0; i < TICKET_LENGTH - 1; i++){
        //generate a hash
        sha256((char*) ((int)&round_num + draw_block + i),
                sizeof(int64_t), &rand);
        int64_t num_draw = rand.hash[0] % MAX_NUMBER + 1;

        //TODO: Self is probably not what should go here?
        this->rounds.modify(itr, _self, [&](auto& t) {
                t.winning_nums[i] = num_draw;
        });
    }
    
    sha256((char*) ((int)&round_num + draw_block + TICKET_LENGTH), 
            sizeof(int64_t), &rand);
    int64_t num_draw = rand.hash[0] % MAX_POWERBALL_NUMBER + 1;
    //TODO: Self is probably not what should go here?
    this->rounds.modify(itr, _self, [&](auto& t) {
            t.winning_nums[TICKET_LENGTH - 1] = num_draw;
    });
}

//@abi action
void powerball::claim(const account_name& claimer,
                      const int64_t& round_num){
    //Assert current round_num exists
    auto itr = this->rounds.find(round_num);
    eosio_assert(itr != this->rounds.end(), "Round doesn't exist");
    
    size_t claimer_index = find(itr->ticket_table, claimer);
    const auto my_numbers = itr->ticket_table[claimer_index].account_tickets;

    eosio_assert(claimer_index != -1, "Player doesn't exist!");
    eosio_assert(itr->winning_nums[0] != 0, "Numbers not drawn yet!");

    int64_t payout = 0;
    for (auto&& ticket : my_numbers){
        unsigned int num_matches = 0;
        
        for (size_t i = 0; i < TICKET_LENGTH - 1; i++){
            
            for (size_t j = 0; j < TICKET_LENGTH - 1; j++){

                if (ticket[i] == itr->winning_nums[j]){
                    num_matches++;
                }
            }
        }
        bool powerball_matches =
            (ticket[TICKET_LENGTH -1] == itr->winning_nums[TICKET_LENGTH - 1]);

        //Win Conditions 
        //Prices set at 1ETH == 40 EOS
        if ((num_matches == 5) && powerball_matches){
            payout = this->balance;
            break;
        }

        switch (num_matches){
            case TICKET_LENGTH - 1:
                payout += 1000.0_eth; //1000 ETH
                break;
            case TICKET_LENGTH - 2:
                if (powerball_matches){
                    payout += 50.0_eth; //50 ETH
                } else {
                    payout += 0.1_eth; //0.1 ETH
                }
                break;
            case TICKET_LENGTH - 3:
                if (powerball_matches){
                    payout += 0.1_eth; //0.1 ETH
                } else {
                    payout += 0.007_eth; //0.007 ETH
                }
                break;
            case TICKET_LENGTH -4:
                if (powerball_matches){
                    payout += 0.007_eth; //0.007 ETH
                } 
                break;
            default:
                if (powerball_matches){
                    payout += 0.004_eth; //0.004 ETH
                }
                break;
        }
            
    }
    //TODO: Transfer money
    this->balance -= payout;
    //TODO: get const iterator to be regular iterator
    auto claimer_iterator = itr->ticket_table.begin() + claimer_index;
    //itr->ticket_table.erase(claimer_iterator);

}


EOSIO_ABI( powerball, (buy)(drawnumbers)(claim))
