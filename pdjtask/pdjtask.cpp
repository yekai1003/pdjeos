#include "pdjtask.hpp"


void pdjtoken::sub_balance( account_name owner, asset value, const stat& st ) {
    print("sub_balance->",value.symbol);
   accounts from_acnts( _self, owner );
   //auto fromacc =  from_acnts.find(value.symbol.name());
 
   const auto& from = from_acnts.get( value.symbol.name() );
   eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

   if( has_auth( owner ) ) {
      eosio_assert( !st.can_freeze || !from.frozen, "account is frozen by issuer" );
      eosio_assert( !st.can_freeze || !st.is_frozen, "all transfers are frozen by issuer" );
      eosio_assert( !st.enforce_whitelist || from.whitelist, "account is not white listed" );
   } else if( has_auth( st.issuer ) ) {
      eosio_assert( st.can_recall, "issuer may not recall token" );
   } else {
      eosio_assert( false, "insufficient authority" );
   }
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
      eosio_assert( !st.enforce_whitelist, "can only transfer to white listed accounts" );
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = value;
      });
   } else {
      eosio_assert( !st.enforce_whitelist || to->whitelist, "receiver requires whitelist by issuer" );
      to_acnts.modify( to, 0, [&]( auto& a ) {
        a.balance += value;
      });
   }
}

void pdjtoken::create( account_name issuer,
                    asset        maximum_supply,
                    uint8_t      can_freeze,
                    uint8_t      can_recall,
                    uint8_t      can_whitelist )
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
       s.can_freeze    = can_freeze;
       s.can_recall    = can_recall;
       s.can_whitelist = can_whitelist;
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



void pdjtask::createtk( account_name creator, account_name worker, asset taskBonus, string memo )
{
    //创建任务-指定作者-奖金数量-描述
    //检查creator 和 worker是否都存在
    require_auth(creator);
    require_auth(worker);

    auto sym = taskBonus.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );

    tasks tk( _self, _self );
         
    tk.emplace(creator,[&](auto &t){
        t.creator = creator;
        t.worker  = worker;
        t.taskID  = tk.available_primary_key();
        t.bonus   = taskBonus;
        t.remark  = memo;
    });

}

void pdjtask::commit( uint64_t taskID, account_name worker, string memo )
{
    //提交任务者必须与任务分配者是一个人
    print("hi,",name{_self});
    require_auth(worker);
    tasks tk( _self, _self );
    auto tkobj = tk.find(taskID);
    eosio_assert( tkobj != tk.end(), "taskid not exists" );
    eosio_assert( tkobj->worker == worker, "worker not same" );
    tk.modify(tkobj,worker,[&](auto &t){
        t.status = 1;
        t.comment = memo;
    });

}

void pdjtask::confirm( uint64_t taskID, account_name creator, uint8_t ok )
{
    require_auth(creator);
    tasks tk( _self, _self );
    auto tkobj = tk.find(taskID);
    eosio_assert( tkobj != tk.end(), "taskid not exists" );
    uint8_t status = 2;
    if ( !ok ) {
        // re do 
        status = 0;
    }
    tk.modify(tkobj,creator,[&](auto &t){
        t.status = status;
        t.comment = "well done!";
    });
    
    if ( ok ){
        //发币刺激
        transfer( creator, tkobj->worker, tkobj->bonus, "very good!" );
    }
    
}


EOSIO_ABI( pdjtask, (createtk)(commit)(create)(issue)(transfer)(confirm) )
