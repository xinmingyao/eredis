-module(eredis_SUITE).
-compile(export_all).
-include_lib("common_test/include/ct.hrl").


init_per_suite(Config)->    
    
    Config.

end_per_suite(Config)->

    Config.

init_per_testcase(_,Config)->
    Config
    .

end_per_testcase(Config)->
    
    ok.

all()->
    [put,get].

put1(Config)->
%    Db=?config(db,Config),
    {ok,Db}=eredis:open(a)
    .

put(Config)->
%    Db=?config(db,Config),
    {ok,Db}=eredis:open(a),
    ok=eredis:put(Db,<<"a1">>,<<"b">>).

get(Config)->

%    Db=?config(db,Config),
    {ok,Db}=eredis:open(a),
    ok=eredis:put(Db,<<"get">>,<<"v1">>),
    {ok,<<"v1">>}=eredis:get(Db,<<"get">>).


main1([])->
    {ok,Db}=eredis:open(a),
    loop(100,Db).
    

loop(0,_Db)->
    ok;
loop(N,Db) ->
    ok=eredis:put(Db,<<"a1">>,<<"b">>),
    
loop(N-1,Db).

