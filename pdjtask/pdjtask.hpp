#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
//#include "pdjtoken.hpp"

#include <string>


using namespace eosio ;

using std::string;

class pdjtoken : public contract {
    public:
        pdjtoken( account_name self ):contract(self){}
    /// @abi action
        void create( account_name issuer,
                    asset        maximum_supply,
                    uint8_t      can_freeze,
                    uint8_t      can_recall,
                    uint8_t      can_whitelist );


        void issue( account_name to, asset quantity, string memo );

        void transfer( account_name from,
                    account_name to,
                    asset        quantity,
                    string       memo );

    private:

        //friend pdjtask;

        inline asset get_supply( symbol_name sym )const;
        
        inline asset get_balance( account_name owner, symbol_name sym )const;

    private:
        /// @abi table accounts i64
        struct account {
            asset    balance;
            uint8_t frozen    = 0;
            uint8_t     whitelist = 1;

            uint64_t primary_key()const { return balance.symbol.name(); }
        };
    /// @abi table stats i64 
        struct stat {
            asset          supply;
            asset          max_supply;
            account_name   issuer;
            uint8_t           can_freeze         = 1;
            uint8_t           can_recall         = 1;
            uint8_t           can_whitelist      = 1;
            uint8_t           is_frozen          = 0;
            uint8_t           enforce_whitelist  = 0;

            uint64_t primary_key()const { return supply.symbol.name(); }
        };

        typedef eosio::multi_index<N(accounts), account> accounts;
        typedef eosio::multi_index<N(stats), stat> stats;

        void sub_balance( account_name owner, asset value, const stat& st );
        void add_balance( account_name owner, asset value, const stat& st,
                        account_name ram_payer );

    public:
    /*
        struct transfer_args {
        account_name  from;
        account_name  to;
        asset         quantity;
        string        memo;
        };
        */
};

asset pdjtoken::get_supply( symbol_name sym )const
{
    stats statstable( _self, sym );
    const auto& st = statstable.get( sym );
    return st.supply;
}

asset pdjtoken::get_balance( account_name owner, symbol_name sym )const
{
    accounts accountstable( _self, owner );
    const auto& ac = accountstable.get( sym );
    return ac.balance;
}


class pdjtask : public pdjtoken {
    public:
        pdjtask( account_name self ):  pdjtoken(self){
            
        }
        /// @abi action
        //创建任务
        void createtk( account_name creator, account_name worker, asset taskBonus, string memo );
        //提交任务
        void commit( uint64_t taskID, account_name worker, string memo );
        //验收任务
        void confirm( uint64_t taskID, account_name creator, uint8_t ok = 1 );

        private:
        /// @abi table tasks i64
        
        struct task {
            uint64_t taskID;
            account_name creator;
            account_name worker;
            asset    bonus;
            uint8_t  status = 0;
            string  remark;
            string  comment;
            uint64_t primary_key()const { return taskID; }
        };

        typedef eosio::multi_index<N(tasks), task> tasks;

};


