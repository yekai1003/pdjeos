#include "pdjtoken.hpp"
#include <eosiolib/types.hpp>


void pdjtoken::sub_balance( account_name owner, asset value, const stat& st ) {
    print("sub_balance->",value.symbol);
   accounts from_acnts( _self, owner );
   //auto fromacc =  from_acnts.find(value.symbol.name());
 
   const auto& from = from_acnts.get( value.symbol.name() );
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

    print(",modify->",from.balance,",",value);
  
   from_acnts.modify( from, owner, [&]( auto& a ) {
       a.balance -= value;
   });

}

void pdjtoken::add_balance( account_name owner, asset value, const stat& st, account_name ram_payer )
{
    print("add_balance->",value.symbol);
   accounts to_acnts( _self, owner );
   auto to = to_acnts.find( value.symbol.name() );
   if( to == to_acnts.end() ) {
      //eosio_assert( !st.enforce_whitelist, "can only transfer to white listed accounts" );
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = value;
      });
   } else {
      //eosio_assert( !st.enforce_whitelist || to->whitelist, "receiver requires whitelist by issuer" );
      to_acnts.modify( to, 0, [&]( auto& a ) {
        a.balance += value;
      });
   }
}

void pdjtoken::create( account_name issuer,
                    asset        maximum_supply, 
                    uint64_t days,
                    string prekey )
{
    require_auth( _self );

    auto sym = maximum_supply.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( maximum_supply.is_valid(), "invalid supply");
    eosio_assert( maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable( _self, _self );
    auto existing = statstable.find( sym.name() );
    eosio_assert( existing == statstable.end(), "token with symbol already exists" );

    statstable.emplace( _self, [&]( auto& s ) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
       s.lockduration  = time_point_sec(days * 86400) ;
       s.attrPrefix    = prekey;
    });
}

void pdjtoken::issue( account_name to, asset quantity, string memo )
{
    //print( "issue" );
    auto sym = quantity.symbol.name();
    print("issue=>",name{sym});
    stats statstable( _self, _self );
    const auto& st = statstable.get( sym );

    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity" );
    eosio_assert( quantity <= st.max_supply - st.supply, "quantity exceeds available supply");

    statstable.modify( st, 0, [&]( auto& s ) {
       s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st, st.issuer );

    if( to != st.issuer )
    {
       SEND_INLINE_ACTION( *this, transfer, {st.issuer,N(active)}, {st.issuer, to, quantity, memo} );
    }
}


void pdjtoken::transfer( account_name from,
                      account_name to,
                      asset        quantity,
                      string       /*memo*/ )
{
//    print( "transfer" );
    require_auth( from );
    auto sym = quantity.symbol.name();
    print("transfer",name{sym});
    
    stats statstable( _self, _self );
    const auto& st = statstable.get( sym );

    require_recipient( from );
    require_recipient( to );

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must transfer positive quantity" );

    sub_balance( from, quantity, st );

    add_balance( to, quantity, st, from );

 
}

//根据代币符号去查找前缀key值，自动生成attrkey
void pdjtoken::addattr( asset sym, string key, string val )
{  

   stats st( _self, _self );
   auto &sy = st.get( sym.symbol.name() );

   attrs attr( _self, _self );

   string attrkey = sy.attrPrefix + key;

   
   //const auto& at = attr.get( N(sym) );
   //name{_self};
   auto existing = attr.find( string_to_name(attrkey.c_str()) );
   eosio_assert( existing == attr.end() , "this key already exists!" );

   print(N(sym+key),",",N(sym),",",sym,string_to_name(attrkey.c_str()),"---",string_to_name(key.c_str()));
   attr.emplace( current_receiver(),[&]( auto& s ){
          s.attrkey = attrkey ;
          s.val = val ;
          s.attrPrefix = sy.attrPrefix ;
   });

//    if ( existing == attr.end() ){
//        //需要初始化
//        attr.emplace( _self,[&]( auto& s ){
//           s.sym = sym;
//           attrdata data;
//           data.key = key;
//           data.val = val;
//           s.vattr.push_back(data);
//       });
//    }
//    else {
//        //直接追加即可
//        auto &at = *existing;
//        attr.modify( at, _self, [&](auto & a){
//            attrdata data;
//            data.key = key;
//            data.val = val;
//            a.vattr.push_back(data);
//        } );
//    }
   //eosio_assert( existing != attr.end(), "sym data not find!" );
}



EOSIO_ABI( pdjtoken, (create)(issue)(transfer)(addattr) )
