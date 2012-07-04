
all: compile

compile:
	./c_src/build_deps.sh
	./rebar compile

test: compile
	./rebar eunit

clean:
	./rebar clean
