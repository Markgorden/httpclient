/*##############################################
# wrtbox is a Swiss Army knife for WRTnode  
# WRTnode's busybox
# This file is part of wrtbox.
# Author: 39514004@qq.com (huamanlou,alais name intel inside)
#
# This library is free software; under the terms of the GPL
#
##############################################*/

#ifndef __HTTPCLIENT_H_
#define __HTTPCLIENT_H_

#include <pthread.h>
#include <string.h>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <sys/timex.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <signal.h>
#include <linux/types.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include <net/if_arp.h>
#include <sys/soundcard.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <glob.h>
#include <lualib.h>
#include <lauxlib.h>

//------------------------------------------------------------------------------
#define LINE_SIZE				1024
#define my_safe_free(a) { if (a) {free(a); a = NULL;} }
#ifndef __cplusplus 
typedef enum {false = 0, true = !false} bool;
#endif

int http_client_get(const char * url,char ** result_content, unsigned int * result_content_len);

#endif