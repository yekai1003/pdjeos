#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>

//#include "pdjtoken.hpp"

#include <string>


using namespace eosio ;
const uint32_t attrsize = 7;

using std::string;

class pdjtoken : public contract {
    public:
        pdjtoken( account_name self ):contract(self){}
    /// @abi action
        void create( account_name issuer,
                    asset        maximum_supply, 
                    uint64_t days,
                    string prekey );


        void issue( account_name to, asset quantity, string memo );

        void transfer( account_name from,
                    account_name to,
                    asset        quantity,
                    string       memo );
        void addattr( asset sym, string key, string val );
    private:

        //friend pdjtask;

        inline asset get_supply( symbol_name sym )const;
        
        inline asset get_balance( account_name owner, symbol_name sym )const;

    private:
        /// @abi table accounts i64
        struct account {
            asset    balance;
            uint64_t primary_key()const { return balance.symbol.name(); }
        };
    /// @abi table stats i64 
        struct stat {
            asset          supply;
            asset          max_supply;
            account_name   issuer;
            time_point_sec lockduration;
            string attrPrefix;//用于查找币对应的属性信息
            uint64_t primary_key()const { return supply.symbol.name(); }
            uint64_t get_prekey() const { return string_to_name(attrPrefix.c_str()); }
        };
        // struct attrdata {
        //     string  key;
        //     uint64_t val;
        // };
        /// @abi table attrs i64
         struct attribute {
            string attrkey;//不能超过12个长度，针对不同币采用不同前缀，范围 1-5,a-z
            string val;
            //vector<attrdata>  vattr;
            string  attrPrefix;
            uint64_t primary_key()const { return string_to_name(attrkey.c_str()); }
            //uint64_t get_prekey() const { return string_to_name(attrPrefix.c_str()); }
         };
        typedef eosio::multi_index<N(accounts), account> accounts;
        typedef eosio::multi_index<N(stats), stat> stats;
        typedef eosio::multi_index<N(attrs), attribute> attrs;

        void sub_balance( account_name owner, asset value, const stat& st );
        void add_balance( account_name owner, asset value, const stat& st,
                        account_name ram_payer );

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




