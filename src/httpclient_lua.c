/*
##############################################
# 
httpclient - httpclient Library - Lua Bindings

wrtbox is a Swiss Army knife for WRTnode  
# WRTnode's busybox
# This file is part of wrtbox.
# Author: 39514004@qq.com (huamanlou,alais name intel inside)
#
# This library is free software; under the terms of the GPL
#
##############################################
 */

#include "httpclient.h"

//------------------------------------------------------------------------------
static int http_get(lua_State *L){              /* C中的函数名 */
    char * url = lua_tostring(L, -1);      /* 从Lua虚拟机里取出一个变量，这个变量是string类型的 */
    printf("Top of get(), url=%s\n",url);
    
	char * result_content = NULL;
	unsigned int result_content_len;
	int result =  http_client_get(url,&result_content, &result_content_len);

    lua_pushstring(L,result_content);           /* 将返回值压回Lua虚拟机的栈中 */

    printf("Get http file successes!\n");
	my_safe_free(result_content);
    return 1;                              /* 这个返回值告诉lua虚拟机，我们往栈里放入了多少个返回值 */
}

int luaopen_httpclient(lua_State *L){
    lua_register(
            L,            /* Lua 状态机 */
            "http_get",        /*Lua中的函数名 */
            http_get           /*当前文件中的函数名 */
            );  
    //lua_register(L,"cube",icube);
    return 0;
}

