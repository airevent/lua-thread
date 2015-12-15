// thread

#include <pthread.h>

#include "lua_pd.h"
#include "lualib.h"

//

#define LUA_MT_THREAD "mt.thread"

#define LUA_THREAD_ID_METAFIELD "_thread_id"
#define LUA_THREAD_ARGS_METAFIELD "_thread_args"

#define lua_thread_detach( thread ) { \
    if ( !thread->detached ) { \
        thread->detached = 1; \
        pthread_detach(thread->thread); \
    } \
}

//

typedef struct lua_ud_thread {
    pthread_t thread;
    lua_State *L;
    short detached;
    uint64_t id; // thread unique id (unique in process scope)
} lua_ud_thread;

//

LUAMOD_API int luaopen_thread( lua_State *L );

//

static int lua_thread_start( lua_State *L );
static int lua_thread_stop( lua_State *L );
static int lua_thread_join( lua_State *L );
static int lua_thread_get_id( lua_State *L );
static int lua_thread_get_args( lua_State *L );
static int lua_thread_set_cancel_point( lua_State *L );
static int lua_thread_gc( lua_State *L );

//

static uint64_t inc_id( void );
static void *lua_thread_create_worker( void *arg );
static int lua_thread_atpanic( lua_State *L );
static void lua_thread_xcopy( lua_State *fromL, lua_State *toL );
static int lua_custom_traceback( lua_State *L );
static int lua_custom_pcall( lua_State *L, int narg, int nres );

//

static const luaL_Reg __index[] = {
    {"start", lua_thread_start},
    {"stop", lua_thread_stop},
    {"join", lua_thread_join},
    {"get_id", lua_thread_get_id},
    {"get_args", lua_thread_get_args},
    {"set_cancel_point", lua_thread_set_cancel_point},
    {NULL, NULL}
};

static const luaL_Reg __thread_index[] = {
    {NULL, NULL}
};
