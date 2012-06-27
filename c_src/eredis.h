#ifndef _EREDIS_H
#define _EREDIS_H
#define  _YS_WAIT_H 
#include "erl_nif.h"
//#define _STDLIB_H
#include "redis.h"
typedef struct _eredis_handle
{
  redisDb * db[10];
} eredis_handle;

// Prototypes
ERL_NIF_TERM eredis_open(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM eredis_put(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM eredis_get(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
ERL_NIF_TERM eredis_delete(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

#endif

