#include <stdio.h> 

#include "lauxlib.h" 
#include "lualib.h" 

int main (void) { 

    printf ("Hey luarcas! you were there ... yeahughs!\n");

    char line[256]; 
    lua_State *L = luaL_newstate(); /* create a new state */ 
    luaL_openlibs(L); /* open the standard libraries */ 

    /* reads lines and executes them */ 
    while (fgets(line, sizeof(line), stdin) != NULL) { 
        luaL_loadstring(L, line); /* compile line to a function */ 
        lua_pcall(L, 0, 0, 0); /* call the function */ 
    } 

    lua_close(L); 
    return 0; 
} 
