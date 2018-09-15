//
// powerball.cpp 
//
// Created by apostolos on Thu Sep 13 13:40:18 PDT 2018
//

#include "powerball.hpp"  
    

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
        for (size_t i = 0; i < TICKET_LENGTH; i++){
            eosio_assert(ticket[i] <= MAX_POWERBALL_NUMBER, 
                    "Invalid number for ball!");
        }
        eosio_assert(ticket[TICKET_LENGTH-1] < MAX_POWERBALL_NUMBER, 
                "Invalid Length of powerball!");
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
                t.ticket_table[buyer].push_back(ticket);
        });
    }

    //add the amount paid to the smart contract balance
    this->balance += amount_paid;
}

//@abi action
void powerball::draw_numbers(const int64_t& round_num){
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
    const auto my_numbers = itr->ticket_table[claimer];
    eosio_assert(my_numbers.size() > 0, 
                "Player doesn't exist!");
    eosio_assert(itr->winning_nums[0] != 0, "Numbers not drawn yet!");

    //TODO: use BOOST for map
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
            case 5:
                payout += 40000; //1000 ETH
                break;
            case 4:
                if (powerball_matches){
                    payout += 2000; //50 ETH
                } else {
                    payout += 4; //0.1 ETH
                }
                break;
            case 3:
                if (powerball_matches){
                    payout += 4; //0.1 ETH
                } else {
                    payout += 0.28; //0.007 ETH
                }
                break;
            case 2:
                if (powerball_matches){
                    payout += 0.28; //0.007 ETH
                } 
                break;
            default:
                if (powerball_matches){
                    payout += 0.16; //0.004 ETH
                }
                break;
        }
            
    }
    //TODO: Transfer money
    this->balance -= payout;
    itr->ticket_table.erase(claimer);

}
//@abi action
std::vector<ticket_t> powerball::tickets_for(const int64_t& round_num,
                                             const account_name& user) const {
    //Assert current round_num exists
    auto itr = this->rounds.find(round_num);
    eosio_assert(itr != this->rounds.end(), "Round doesn't exist");
    return itr->ticket_table[user];
}

//@abi action
ticket_t powerball::winning_nums(const int64_t& round_num){
    //Assert current round_num exists
    auto itr = this->rounds.find(round_num);
    eosio_assert(itr != this->rounds.end(), "Round doesn't exist");

    return itr->winning_nums;
}





EOSIO_ABI( powerball, (buy)(draw_numbers))
