/**
 * This file defines C functions for sending smap data to an archiver
 */
#include<libstorm.h>
#include<time.h>
#include "lrodefs.h"
#include<libmsgpack.c>
int smap_send(lua_State *L);
int udpsocket_callback(lua_State *L);
int smap_close(lua_State *L);
int smap_create_msg(lua_State *L);
int smap_init(lua_State *L);


#define SMAP_SYMBOLS \
	{ LSTRKEY("init"), LFUNCVAL(smap_init) }, \
	{ LSTRKEY("send"), LFUNCVAL(smap_send) },\
	{ LSTRKEY("create_msg"), LFUNCVAL(smap_create_msg) },\
	{ LSTRKEY("close"), LFUNCVAL(smap_close) },

static const LUA_REG_TYPE smap_meta_map[] =
{
   // { LSTRKEY( "init" ), LFUNCVAL ( smap_init ) },
    { LSTRKEY( "send" ), LFUNCVAL ( smap_send ) },
    { LSTRKEY( "create_msg" ), LFUNCVAL ( smap_create_msg ) },
    { LSTRKEY( "close" ), LFUNCVAL ( smap_close ) },
 { LNILKEY, LNILVAL }
};


//struct for smap
struct smap
{
 //remains consistent throughout a session
 char *uuid;
 uint16_t cport;
 storm_socket_t *csock;
 const char *archiver_ip;
 uint16_t archiver_port;
 char *key;
 //changes for each entry
 float data;
};

//Meta map of smap

////////////////// BEGIN FUNCTIONS /////////////////////////////
int udpsocket_callback(lua_State *L)
{
 //get parameter values from wherever the function is called, in this case libstorm_net_udpsocket
 char *pay= lua_tostring(L,1); 
 const char *srcip= lua_tostring(L,2); 
 uint16_t srcport= (uint16_t)lua_isnumber(L,3);
 printf("Incoming message on cport\n");
 printf("Message: %s\n Source IP: %d \n Source Port: %u \n", pay, srcip, srcport);
 
}

//smap_init(cport, key, archiver_ip, archiver_port, frequency)
//cport: port on which you want to create the socket on client side
int smap_init(lua_State *L)
{
 uint16_t cport;
 struct smap *obj=lua_newuserdata(L, sizeof(struct smap));
 int obj_index=lua_gettop(L); //store value of object for setting meta table
 int top;
 if(lua_isnil(L,1))
        obj->cport= 49152;
 else
        obj->cport=lua_tonumber(L,1); //pass value of cport, can be hard-coded

 obj->key= luaL_checkstring(L,2);
 obj->archiver_ip= luaL_checkstring(L,3);
 obj->archiver_port= luaL_checknumber(L,4);

 lua_pushlightfunction(L,libstorm_net_udpsocket);
 lua_pushvalue(L,cport);
 lua_pushlightfunction(L,udpsocket_callback);
 lua_call(L,2,0);

 obj->csock = lua_touserdata(L, lua_gettop(L));
 top= lua_gettop(L);
 lua_pop(L, top-obj_index); //pop all items added after creation of object

 lua_pushrotable(L, (void*)smap_meta_map);
 lua_setmetatable(L,-2);

 return 1;

}

int smap_create_msg(lua_State *L)
{
 char *smap_msg;
 struct smap *obj= lua_touserdata(L,1);

//format table to pack
 sprintf(smap_msg, "{key= %s, UUID = %s,  Readings = { %d, %f }, }",obj->key, obj->uuid, (unsigned)time(NULL), obj->data);

//pack message
 lua_pushlightfunction(L, libmsgpack_mp_pack);
 lua_pushstring(L, smap_msg);
 lua_call(L, 1, 1);

 return 1;//returns packed smap msg

}
 
//smap_send(uuid,data)
//uuid:string or char*
//data: float

int smap_send (lua_State *L)
{

 struct smap *obj= lua_touserdata(L,1);
 obj->uuid= lua_tostring(L,2);
 obj->data= (float)lua_tonumber(L,3);

 lua_pushlightfunction(L, libstorm_net_sendto);
 lua_pushlightuserdata(L, obj->csock);
 lua_pushlightfunction(L, smap_create_msg);
 lua_call(L,0,1);//call smap_create_message, pushes packed smap msg onto stack 
 lua_pushvalue(L, obj->archiver_ip);
 lua_pushvalue(L, obj->archiver_port);
 lua_call(L, 4, 1); // call libstorm_net_sendto(csock, pay, srcip, srcport)

 if(lua_tonumber(L,-1)!=1)
 {
  printf("Send failure\n");
 }

 /*
 //call cord.await, send data every 1 second
 lua_pushlightfunction(L,libstorm_os_invoke_later);
 lua_pushnumber(L, (obj->frequency)*SECOND_TICKS);
 lua_call(L, 1, 0);
*/
}

//end smap session
int smap_close(lua_State *L)
{
 lua_pushlightfunction(L, libstorm_net_close);
 lua_call(L,0,0);
}


