-module(eredis).
-export([open/1,
	 put/3,
	 get/2,
	 delete/2
	]).
-export([init/0]).



-on_load(init/0).


-spec init()->ok|{error,any()}.

init()->
    SoName=case code:priv_dir(?MODULE) of
	       {error,bad_name}->
		   case code:which(?MODULE) of 
		       FileName when is_list(FileName)->
			   filename:join([filename:dirname(FileName),"../priv","eredis"]);
		       _ ->
			   filename:join("../priv","eredis")
		   end;
	       Dir ->
		   filename:join(Dir,"eredis")
	   end,
    erlang:load_nif(SoName,0).
open(_Name)->
    erlang:nif_error({error,not_loader})    
.

put(_Ref,_Key,_Value)->	      
    erlang:nif_error({error,not_loader}).

get(_Ref,_Key)->
    erlang:nif_error({error,not_loader}).

delete(_Ref,_Key)->
    erlang:nif_error({error,not_loader}).
