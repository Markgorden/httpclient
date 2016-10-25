##############################################
# wrtbox is a Swiss Army knife for WRTnode  
# WRTnode's busybox
# This file is part of wrtbox.
# Author: 39514004@qq.com (huamanlou,alais name intel inside)
#
# This library is free software; under the terms of the GPL
#
##############################################

HTTPCLIENT_BACKENDS    = $(BACKENDS)
HTTPCLIENT_CFLAGS      = $(CFLAGS) -std=gnu99 -fstrict-aliasing -Iinclude -fPIC -Wall 

#LDFLAGS = -L/work/openwrt/OpenWrt-SDK-ramips-for-linux-i686-gcc-4.8-linaro_uClibc-0.9.33.2/staging_dir/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2/lib -lpthread -ldl -lm -lc -llua

HTTPCLIENT_LIB         = libhttpclient.so
HTTPCLIENT_LIB_LDFLAGS = $(LDFLAGS) -shared
HTTPCLIENT_LIB_OBJ     = httpclient_lib.o

HTTPCLIENT_LUA         = httpclient.so
HTTPCLIENT_LUA_LDFLAGS = $(LDFLAGS) -shared -L./ -lhttpclient -llua
HTTPCLIENT_LUA_OBJ     = httpclient_lua.o

HTTPCLIENT_CLI         = httpclient
HTTPCLIENT_CLI_LDFLAGS = $(LDFLAGS) -L./ -lhttpclient
HTTPCLIENT_CLI_OBJ     = httpclient_cli.o

%.o: %.c
	$(CC) $(HTTPCLIENT_CFLAGS) $(FPIC) -c -o $@ $<

compile: clean $(HTTPCLIENT_LIB_OBJ) $(HTTPCLIENT_LUA_OBJ) $(HTTPCLIENT_CLI_OBJ)
	$(CC) $(HTTPCLIENT_LIB_LDFLAGS) -o $(HTTPCLIENT_LIB) $(HTTPCLIENT_LIB_OBJ)
	$(CC) $(HTTPCLIENT_LUA_LDFLAGS) -o $(HTTPCLIENT_LUA) $(HTTPCLIENT_LUA_OBJ)
	$(CC) $(HTTPCLIENT_CLI_LDFLAGS) -o $(HTTPCLIENT_CLI) $(HTTPCLIENT_CLI_OBJ)

clean:
	rm -f *.o $(HTTPCLIENT_LIB) $(HTTPCLIENT_LUA) $(HTTPCLIENT_CLI)
