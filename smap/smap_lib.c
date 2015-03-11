/**
 * This file defines C functions for sending smap data to an archiver
 */
#include<libstorm.c>
#include<time.h>

int smap_send(lua_State *L);
int udpsocket_callback(lua_State *L);
int smap_close(lua_State *L);
int smap_create_msg(lua_State *L);
int smap_init(lua_State *L);


#define SMAP_SYMBOLS \
	{ LSTRKEY("smap_init"), LFUNCVAL(smap_init) }, \

static const LUA_REG_TYPE smap_meta_map[] =
{
   // { LSTRKEY( "init" ), LFUNCVAL ( smap_init ) },
    { LSTRKEY( "smap_send" ), LFUNCVAL ( smap_send ) },
    { LSTRKEY( "smap_close" ), LFUNCVAL ( smap_close ) },
    { LSTRKEY( "__index"), LROVAL ( smap_meta_map)},
 { LNILKEY, LNILVAL }
};


//struct for smap
struct smap
{
 //remains consistent throughout a session
 uint16_t cport;
 const char *archiver_ip;
 uint16_t archiver_port;
 const char *key;
 storm_socket_t *csock;
 char *uuid;
 uint16_t data;
};

//Meta map of smap

////////////////// BEGIN FUNCTIONS /////////////////////////////
int udpsocket_callback(lua_State *L)
{
 //get parameter values from wherever the function is called, in this case libstorm_net_udpsocket
 char *pay=  (char*)lua_tostring(L,1);
 char *srcip= (char*)lua_tostring(L,2);
 uint16_t srcport= (uint16_t)lua_isnumber(L,3);
 printf("Incoming message on cport\n");
 printf("Message: %s\n Source IP: %s \n Source Port: %u \n", pay, srcip, srcport);
 return 0; 
}


//smap_init(key, archiver_ip, archiver_port,cport)
//cport: port on which you want to create the socket on client side
int smap_init(lua_State *L)
{
 
 struct smap *obj=lua_newuserdata(L, sizeof(struct smap));
 size_t s_key,s_ip;
 
 //get values of parameters passed from lua stack
 const char *temp_key = lua_tolstring(L,1, &s_key);
 const char *temp_ip = lua_tolstring(L,2,&s_ip);
 const uint16_t temp_aport = (uint16_t)luaL_checknumber(L,3);
 uint16_t cport_alloc;

 if(lua_isnil(L,4))
	cport_alloc = 49152;
 else
      	cport_alloc =  (uint16_t)lua_tonumber(L,4);
 
 const uint16_t temp_cport= cport_alloc;
 obj->key= malloc(s_key+1);
 obj->archiver_ip=malloc(s_ip+1);
 obj->archiver_port= malloc(sizeof(uint16_t));
 obj->cport = malloc(sizeof(uint16_t));

 //terminating '\0' for strings
 memset(obj->archiver_ip, 0, s_ip+1);
 memset(obj->key, 0, s_key+1);
 
 memcpy(obj->key,temp_key, s_key+1);
 memcpy(obj->archiver_ip, temp_ip, s_ip);
 memcpy(&(obj->archiver_port), &temp_aport, sizeof(uint16_t));
 memcpy(&(obj->cport), &temp_cport, sizeof(uint16_t));

 lua_pushlightfunction(L,libstorm_net_udpsocket);
 lua_pushnumber(L,obj->cport);
 lua_pushlightfunction(L,udpsocket_callback);
 lua_call(L,2,1);
 obj->csock = lua_touserdata(L, -1);
 lua_pop(L, 1); //pop all items added after creation of object
 lua_pushrotable(L, (void*)smap_meta_map);
 lua_setmetatable(L,-2);
 printf("Key: %s Archiver IP: %s Arch port: %u cport: %u\n", obj->key, obj->archiver_ip, obj->archiver_port, obj->cport);
  return 1;

}

//smap_send(uuid,data)
//uuid:string or char*
//data: float

int smap_send (lua_State *L)
{
 printf("Entered smap_send\n");
 struct smap *obj= lua_touserdata(L,1);
 printf("Key = %s\n", obj->key); 
 char smap_msg[100];
 printf("Key= %s UUID= %s, Data= %f\n", obj->key, obj->uuid, obj->data);
printf("Address of object: 0x%x\n", (uint32_t)obj);
 obj->uuid= (char*)lua_tostring(L,2);
 obj->data= (uint16_t)lua_tonumber(L,3);

 printf("Key= %s UUID= %s, Data= %u\n", obj->key, obj->uuid, obj->data);
 lua_pushlightfunction(L, libstorm_net_sendto);
 lua_pushlightuserdata(L, obj->csock);

//format table to pack
 sprintf(smap_msg, "{key= %s, UUID = %s,  Readings = { %d, %u }, }",obj->key, obj->uuid, (unsigned)time(NULL), obj->data);
 printf("Unpacked msg:");
 puts(smap_msg);
//pack message
 lua_pushlightfunction(L, libmsgpack_mp_pack);
 lua_pushstring(L, smap_msg);
 lua_call(L, 1, 1);

 printf("Packed msg: %s\n", lua_tostring(L,-1)); 
 lua_pushstring(L, obj->archiver_ip);
 lua_pushnumber(L, obj->archiver_port);
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
 return 0;
}

//end smap session
int smap_close(lua_State *L)
{
 lua_pushlightfunction(L, libstorm_net_close);
 lua_call(L,0,0);
 return 0;
}


