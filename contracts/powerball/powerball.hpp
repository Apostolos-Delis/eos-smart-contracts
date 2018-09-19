//
// powerball.hpp 
//
// Created by apostolos on Thu Sep 13 13:40:18 PDT 2018
//

#ifndef POWERBALL_HPP
#define POWERBALL_HPP

#include <string>

#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp> //for tapos_block_num()
#include <eosiolib/crypto.h> //For randomization
using namespace eosio;

//Set the value of eth = 40 eos
constexpr long double operator"" _eos( long double amt) { return amt; }
constexpr long double operator"" _eth( long double amt) { return amt * 40; }

const double TICKET_PRICE = 0.5_eos;  //Set to 0.5 EOS
const uint8_t MAX_NUMBER = 69;
const uint8_t MAX_POWERBALL_NUMBER = 26;
const uint8_t ROUND_LENGTH = 15;
const uint8_t TICKET_LENGTH = 6;

typedef std::array<int32_t, TICKET_LENGTH> ticket_t;

/**
 *  @defgroup powerballcontract Powerball Contract
 *  @brief Defines an implementation of powerball on the blockchain
 * 
 *  @details
 *  For each powerball round:
 *  - Each player can select 6 numbers corresponding to their entry
 *  - After the time expires, people with correct powerball numbers win
 */
class powerball : public eosio::contract {
public:
    powerball( account_name self ):contract(self), rounds(_self, _self) {
        this->balance = 0;
        this->current_round = 1; 
        this->rounds.emplace(current_round,
                [&](auto& r){ r.end_time = now() + ROUND_LENGTH; });
    }

    /**
     * @brief Information related to a single round
     * @abi table games i64
     */
    struct Round {
        Round(){
            //Initialize array to zeros
            winning_nums.ticket_t::fill(0);
        }

        int64_t                               round_num;
        time                                  end_time;
        int64_t                               draw_block;
        ticket_t                              winning_nums;
        std::map<account_name, std::vector<ticket_t>> ticket_table;

        int64_t primary_key() const { return round_num; }
        EOSLIB_SERIALIZE( Round, (round_num)(end_time)(draw_block)(winning_nums)(ticket_table))
    };

   
    /**
     * @brief Information related to a single round
     * @abi table games i64
     */
    //TODO: Create Table Struct

    /// @brief Keep track of the current round 
    int64_t current_round;
    int64_t balance;

    ///@brief Store existing rounds in a table
    typedef eosio::multi_index< N(Round), Round> round_table;

    ///@brief Round table instance
    round_table rounds;

    //@abi action
    void buy(const account_name& buyer, 
             const std::vector<ticket_t>& tickets,
             const int64_t& amount_paid);

    //@abi action
    void draw_numbers(const int64_t& round_num);

    //@abi action
    void claim(const account_name& claimer, 
               const int64_t& round_num);

    //@abi action
    std::vector<ticket_t> tickets_for(const int64_t& round_num,
                                      const account_name& user) const;

    //@abi action
    ticket_t winning_nums(const int64_t& round_num) const;
};

#endif // POWERBALL_HPP
