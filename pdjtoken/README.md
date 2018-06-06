yekaideMacBook-Pro:pdjtoken yk$ cleos push action  yekai create '["yekai","100000.000 PTK",86400,"ptk"]' -pyekai
executed transaction: 0a97f2f10232bb88cb9cc38636a13f879004e535152087372097c13b482e16a5  128 bytes  558 us
#         yekai <= yekai::create                {"issuer":"yekai","maximum_supply":"100000.000 PTK","days":86400,"prekey":"ptk"}
warning: transaction executed locally, but may not be confirmed by the network yet
yekaideMacBook-Pro:pdjtoken yk$ 
yekaideMacBook-Pro:pdjtoken yk$ 
yekaideMacBook-Pro:pdjtoken yk$ cleos get table yekai yekai stats
{
  "rows": [{
      "supply": "0.000 PTK",
      "max_supply": "100000.000 PTK",
      "issuer": "yekai",
      "lockduration": "2070-06-14T17:31:44",
      "attrPrefix": "ptk"
    }
  ],
  "more": false
}
yekaideMacBook-Pro:pdjtoken yk$ 
yekaideMacBook-Pro:pdjtoken yk$ 
yekaideMacBook-Pro:pdjtoken yk$ cleos push action yekai addattr '["0.000 PTK","age","30"]' -p yekai
executed transaction: cf57f6ba93b5eb9e0ef5bd21bfabb44d1c0e1ecb3c4878d52d1f151b61caad86  120 bytes  775 us
#         yekai <= yekai::addattr               {"sym":"0.000 PTK","key":"age","val":"30"}
>> 14385632093726048256,14385623109728206848,0.000 PTK12565155282348408832---3680566795468537856
warning: transaction executed locally, but may not be confirmed by the network yet
yekaideMacBook-Pro:pdjtoken yk$ 
yekaideMacBook-Pro:pdjtoken yk$ 
yekaideMacBook-Pro:pdjtoken yk$ cleos get table yekai yekai attrs
{
  "rows": [{
      "attrkey": "ptkage",
      "val": "30",
      "attrPrefix": "ptk"
    }
  ],
  "more": false
}
yekaideMacBook-Pro:pdjtoken yk$ cleos push action yekai addattr '["0.000 PTK","sex","man"]' -p yekai
executed transaction: f0d8c716d0ee50c169792e46fbdf807d3dc0be063274144fc78a723c77f64682  120 bytes  675 us
#         yekai <= yekai::addattr               {"sym":"0.000 PTK","key":"sex","val":"man"}
>> 14385632093726048256,14385623109728206848,0.000 PTK12565471168603095040---14031527589026201600
warning: transaction executed locally, but may not be confirmed by the network yet
yekaideMacBook-Pro:pdjtoken yk$ cleos get table yekai yekai attrs
{
  "rows": [{
      "attrkey": "ptkage",
      "val": "30",
      "attrPrefix": "ptk"
    },{
      "attrkey": "ptksex",
      "val": "man",
      "attrPrefix": "ptk"
    }
  ],
  "more": false
}

