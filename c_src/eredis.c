#include "eredis.h"

static ErlNifResourceType* eleveldb_db_RESOURCE;
static ERL_NIF_TERM ATOM_TRUE;
static ERL_NIF_TERM ATOM_FALSE;
static ERL_NIF_TERM ATOM_OK;
static ERL_NIF_TERM ATOM_ERROR;
static ERL_NIF_TERM ATOM_CREATE_IF_MISSING;
static ERL_NIF_TERM ATOM_ERROR_IF_EXISTS;
static ERL_NIF_TERM ATOM_WRITE_BUFFER_SIZE;
static ERL_NIF_TERM ATOM_MAX_OPEN_FILES;
static ERL_NIF_TERM ATOM_BLOCK_SIZE;
static ERL_NIF_TERM ATOM_BLOCK_RESTART_INTERVAL;
static ERL_NIF_TERM ATOM_ERROR_DB_OPEN;
static ERL_NIF_TERM ATOM_ERROR_DB_PUT;
static ERL_NIF_TERM ATOM_NOT_FOUND;
static ERL_NIF_TERM ATOM_VERIFY_CHECKSUMS;
static ERL_NIF_TERM ATOM_FILL_CACHE;
static ERL_NIF_TERM ATOM_SYNC;
static ERL_NIF_TERM ATOM_ERROR_DB_DELETE;
static ERL_NIF_TERM ATOM_CLEAR;
static ERL_NIF_TERM ATOM_PUT;
static ERL_NIF_TERM ATOM_DELETE;
static ERL_NIF_TERM ATOM_ERROR_DB_WRITE;
static ERL_NIF_TERM ATOM_BAD_WRITE_ACTION;
static ERL_NIF_TERM ATOM_KEEP_RESOURCE_FAILED;
static ERL_NIF_TERM ATOM_ITERATOR_CLOSED;
static ERL_NIF_TERM ATOM_FIRST;
static ERL_NIF_TERM ATOM_LAST;
static ERL_NIF_TERM ATOM_NEXT;
static ERL_NIF_TERM ATOM_PREV;
static ERL_NIF_TERM ATOM_INVALID_ITERATOR;
static ERL_NIF_TERM ATOM_CACHE_SIZE;
static ERL_NIF_TERM ATOM_PARANOID_CHECKS;
static ERL_NIF_TERM ATOM_ERROR_DB_DESTROY;
static ERL_NIF_TERM ATOM_KEYS_ONLY;
static ERL_NIF_TERM ATOM_COMPRESSION;
static ERL_NIF_TERM ATOM_ERROR_DB_REPAIR;



static void eredis_resource_cleanup(ErlNifEnv* env, void* arg);
static ErlNifFunc nif_funcs[] =
{
    {"open", 1, eredis_open},
    {"put", 3, eredis_put},
    {"get", 2, eredis_get},
    {"delete", 2, eredis_delete},
};


ERL_NIF_TERM eredis_put(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  eredis_handle * handle;
  ErlNifBinary key;
  ErlNifBinary value;
  if(enif_get_resource(env,argv[0],eleveldb_db_RESOURCE,(void **)&handle) &&
     enif_inspect_binary(env,argv[1],&key) && enif_inspect_binary(env,argv[2],&value)){ 
    redisDb* db=handle->db[0];
    robj * k1 = createStringObject(( char*)key.data,key.size,1,1);
    robj * v1 = createStringObject(( char*)value.data,value.size,1,1);     
    
    // robj * k1 = createStringObject("123",3);
    //robj * v1 = createStringObject("456",3);     
    
    dbReplace(db,k1,v1);
    return ATOM_OK;
    }
  return ATOM_ERROR;
}


ERL_NIF_TERM eredis_get(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  eredis_handle * handle;
  ErlNifBinary key;
  if(enif_get_resource(env,argv[0],eleveldb_db_RESOURCE,(void **)&handle) &&
     enif_inspect_binary(env,argv[1],&key)){ 
    redisDb* db=handle->db[0];
    robj * k1 = createStringObject((char*)key.data,key.size,1,1);
    uint16_t  version =1 ;
	
    robj * v1 = lookupKeyWithVersion(db,k1,&version);
    if(v1==NULL){
      return ATOM_NOT_FOUND;
    }else{
      const size_t size =sdslen(v1->ptr);
      ERL_NIF_TERM value_bin;
      unsigned char * value=enif_make_new_binary(env,size,&value_bin);
      memcpy(value,v1->ptr,sdslen(v1->ptr));
      return enif_make_tuple2(env,ATOM_OK,value_bin);
    }
    }
  return ATOM_ERROR;
}


ERL_NIF_TERM eredis_delete(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  eredis_handle * handle;
  ErlNifBinary key;
  if(enif_get_resource(env,argv[0],eleveldb_db_RESOURCE,(void **)&handle) &&
     enif_inspect_binary(env,argv[1],&key)){ 
    redisDb* db=handle->db[0];
    robj * k1 = createStringObject((char*)key.data,key.size,1,1);
    int ret = dbDelete(db,k1);
    if(ret==0){
      return ATOM_ERROR_DB_DELETE;
    }else{

      return ATOM_OK;
    }
    }
  return ATOM_ERROR;
}

ERL_NIF_TERM eredis_open(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{

  // Setup handle
  eredis_handle* handle =
    (eredis_handle*) enif_alloc_resource(eleveldb_db_RESOURCE,
					      sizeof(eredis_handle));
  // redisDb * db = (redisDb *)malloc(sizeof(redisDb));
  redisDb * db =enif_alloc_resource(eleveldb_db_RESOURCE,sizeof(redisDb));
  if(handle !=NULL ){
    memset(db, '\0', sizeof(redisDb));
  
    extern dictType dbDictType;
    extern dictType keyptrDictType;
    //extern dictType keylistDictType;
    //extern dictType keylistDictType;
    db->dict = dictCreate(&dbDictType,NULL);
    db->expires =  dictCreate(&keyptrDictType,NULL);
   // db->blocking_keys = dictCreate(&keylistDictType,NULL);
   // db->watched_keys = dictCreate(&keylistDictType,NULL);
    db->id = 1;
    memset(handle, '\0', sizeof(eredis_handle));
    handle->db[0] = db;
    ERL_NIF_TERM result = enif_make_resource(env, handle);
    enif_release_resource(handle);
    return enif_make_tuple2(env, ATOM_OK, result);
  }
  return ATOM_ERROR;
}


#define ATOM(Id,Value) {Id = enif_make_atom(env,Value);}

static int on_load(ErlNifEnv* env, void** priv_data, ERL_NIF_TERM load_info)
{
    ErlNifResourceFlags flags = (ErlNifResourceFlags)(ERL_NIF_RT_CREATE | ERL_NIF_RT_TAKEOVER);
    eleveldb_db_RESOURCE = enif_open_resource_type(env, NULL, "eleveldb_db_resource",
                                                    &eredis_resource_cleanup,
                                                    flags, NULL);
//    eleveldb_itr_RESOURCE = enif_open_resource_type(env, NULL, "eleveldb_itr_resource",
//                                                     &eleveldb_itr_resource_cleanup,
//                                                     flags, NULL);

    // Initialize common atoms
    ATOM(ATOM_OK, "ok");
    ATOM(ATOM_ERROR, "error");
    ATOM(ATOM_TRUE, "true");
    ATOM(ATOM_FALSE, "false");
    ATOM(ATOM_CREATE_IF_MISSING, "create_if_missing");
    ATOM(ATOM_ERROR_IF_EXISTS, "error_if_exists");
    ATOM(ATOM_WRITE_BUFFER_SIZE, "write_buffer_size");
    ATOM(ATOM_MAX_OPEN_FILES, "max_open_files");
    ATOM(ATOM_BLOCK_SIZE, "block_size");
    ATOM(ATOM_BLOCK_RESTART_INTERVAL, "block_restart_interval");
    ATOM(ATOM_ERROR_DB_OPEN,"db_open");
    ATOM(ATOM_ERROR_DB_PUT, "db_put");
    ATOM(ATOM_NOT_FOUND, "not_found");
    ATOM(ATOM_VERIFY_CHECKSUMS, "verify_checksums");
    ATOM(ATOM_FILL_CACHE,"fill_cache");
    ATOM(ATOM_SYNC, "sync");
    ATOM(ATOM_ERROR_DB_DELETE, "db_delete");
    ATOM(ATOM_CLEAR, "clear");
    ATOM(ATOM_PUT, "put");
    ATOM(ATOM_DELETE, "delete");
    ATOM(ATOM_ERROR_DB_WRITE, "db_write");
    ATOM(ATOM_BAD_WRITE_ACTION, "bad_write_action");
    ATOM(ATOM_KEEP_RESOURCE_FAILED, "keep_resource_failed");
    ATOM(ATOM_ITERATOR_CLOSED, "iterator_closed");
    ATOM(ATOM_FIRST, "first");
    ATOM(ATOM_LAST, "last");
    ATOM(ATOM_NEXT, "next");
    ATOM(ATOM_PREV, "prev");
    ATOM(ATOM_INVALID_ITERATOR, "invalid_iterator");
    ATOM(ATOM_CACHE_SIZE, "cache_size");
    ATOM(ATOM_PARANOID_CHECKS, "paranoid_checks");
    ATOM(ATOM_ERROR_DB_DESTROY, "error_db_destroy");
    ATOM(ATOM_ERROR_DB_REPAIR, "error_db_repair");
    ATOM(ATOM_KEYS_ONLY, "keys_only");
    ATOM(ATOM_COMPRESSION, "compression");
    return 0;
}

static void eredis_resource_cleanup(ErlNifEnv* env, void* arg)
{
    // Delete any dynamically allocated memory stored in eleveldb_db_handle
    eredis_handle* handle = (eredis_handle*)arg;
     enif_release_resource(handle->db[0]);
    // delete handle->db;
}


ERL_NIF_INIT(eredis, nif_funcs, &on_load, NULL, NULL, NULL);


