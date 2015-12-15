// thread

#include "lua_thread.h"

//

LUAMOD_API int luaopen_thread( lua_State *L ) {
    lua_newmt(L, LUA_MT_THREAD, __thread_index, lua_thread_gc);

    luaL_newlib(L, __index);
    return 1;
}

// arg#1 - string - file to execute
// arg#2 - table - meta args
// arg#3 - table - meta keys to copy
static int lua_thread_start( lua_State *L ) {
    int type;

    lua_ud_thread *thread = (lua_ud_thread *)lua_newuserdata(L, sizeof(lua_ud_thread));

    if ( !thread ) {
        lua_fail(L, "lua_ud_thread alloc failed", 0);
    }

    thread->L = luaL_newstate();

    if ( !thread->L ) {
        lua_fail(L, "luaL_newstate alloc failed", 0);
    }

    thread->detached = 0;
    thread->id = inc_id();

    // meta id
    lua_pushnumber(thread->L, thread->id);
    lua_setfield(thread->L, LUA_REGISTRYINDEX, LUA_THREAD_ID_METAFIELD);
    //

    lua_atpanic(thread->L, lua_thread_atpanic);

    luaL_openlibs(thread->L);

    // meta args
    lua_newtable(thread->L);
    lua_setfield(thread->L, LUA_REGISTRYINDEX, LUA_THREAD_ARGS_METAFIELD);
    //

    // meta keys to copy
    if ( lua_istable(L, 3) ) {
        lua_pushnil(L);
        while ( lua_next(L, 3) != 0 ) {
            type = lua_getfield(L, LUA_REGISTRYINDEX, lua_tostring(L, -1));

            if ( type==LUA_TLIGHTUSERDATA ) {
                lua_pushlightuserdata(thread->L, lua_touserdata(L, -1));
                lua_setfield(thread->L, LUA_REGISTRYINDEX, lua_tostring(L, -2));
            }

            lua_pop(L, 2);
        }
    }
    //

printf("pp: %p\n", lua_thread_create_worker(L));

    luaL_setmetatable(L, LUA_MT_THREAD);
    lua_pushnumber(L, thread->id);
    return 2;
}

static int lua_thread_stop( lua_State *L ) {
    return 0;
}

static int lua_thread_join( lua_State *L ) {
    lua_ud_thread *thread = luaL_checkudata(L, 1, LUA_MT_THREAD);

    int r = pthread_join(thread->thread, NULL);

    if ( r != 0 ) {
        lua_fail(L, "thread join failed", r);
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int lua_thread_get_id( lua_State *L ) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_THREAD_ID_METAFIELD);
    return 1;
}

static int lua_thread_get_args( lua_State *L ) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_THREAD_ARGS_METAFIELD);
    return 1;
}

static int lua_thread_set_cancel_point( lua_State *L ) {
    pthread_testcancel();
    return 0;
}

static int lua_thread_gc( lua_State *L ) {
    lua_ud_thread *thread = luaL_checkudata(L, 1, LUA_MT_THREAD);

    if ( thread->L ) {
        lua_close(thread->L);
        thread->L = NULL;
    }

    return 0;
}

//

static uint64_t inc_id( void ) {
    static volatile uint64_t id = 1;
    return __sync_add_and_fetch(&id, 1);
}

static void *lua_thread_create_worker( void *arg ) {
    return arg;
}

static int lua_thread_atpanic( lua_State *L ) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_THREAD_ID_METAFIELD);
    fprintf(stderr, "lua thread #%zu: PANIC: %s\n", (size_t)lua_tonumber(L, -1), lua_tostring(L, -2));
    return 0;
}
