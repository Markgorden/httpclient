/*##############################################
# wrtbox is a Swiss Army knife for WRTnode  
# WRTnode's busybox
# This file is part of wrtbox.
# Author: 39514004@qq.com (huamanlou,alais name intel inside)
#
# This library is free software; under the terms of the GPL
#
##############################################*/

#include "httpclient.h"


typedef struct tagHTTPFile
{
	FILE * fp;
	char * buffer;
} HTTP_STREAM;

static inline char * str_assign(const char * value)
{
	int len = strlen(value) + 1;
	unsigned char * p = (unsigned char *)malloc(len);
	if (! p)
		return NULL;
	memcpy(p, value, len);
	return (char *)p;
}

static HTTP_STREAM * http_stream_open(int sock);
static void http_stream_close(HTTP_STREAM * s);
static int http_stream_printf(HTTP_STREAM * s, const char * format, ... );
static char * http_stream_gets(char *buf, int bufsize, HTTP_STREAM * s);
static size_t http_stream_read(void *buffer, size_t count, HTTP_STREAM * s);

static HTTP_STREAM * http_stream_open(int sock)
{
	HTTP_STREAM * s = (HTTP_STREAM *)malloc(sizeof(HTTP_STREAM));
	if (! s)
		return NULL;
	memset(s, 0, sizeof(HTTP_STREAM));
	if (NULL == (s->fp = fdopen(sock,"r+t")))
		goto failed;
	return s;
	
failed:
	my_safe_free(s);
	return NULL;
}

static void http_stream_close(HTTP_STREAM * s)
{
	my_safe_free(s->buffer);
	if (s->fp)
		fclose(s->fp);
	free(s);
}

static int http_stream_printf(HTTP_STREAM * s, const char * format, ... )
{
	va_list ap;
	int ret;
	va_start(ap, format);
	if (0 < (ret = vfprintf(s->fp, format, ap)))
		fflush(s->fp);
	va_end(ap);
	return ret;
}

static char * http_stream_gets(char *buf, int bufsize, HTTP_STREAM * s)
{
	return fgets(buf, bufsize, s->fp);
}

static size_t http_stream_read(void *buffer, size_t count, HTTP_STREAM * s)
{
	return fread(buffer, 1, count, s->fp);
}

unsigned short parse_url(const char * url, char ** host, char ** path)
{
	unsigned short port;
	char * p;

	if ((! strncasecmp(url, "http://", 7)) || (! strncasecmp(url, "http:\\\\", 7)))
	{
		port = 80;
		url += 7;
	}
	else
	{
		printf("%s: not illegal url %s\n", __func__, url);
		return 0;
	}
	
	if (NULL == (* host = str_assign(url)))
	{
		printf("%s: can not allocate memory\n", __func__);
		return 0;
	}
	for (p = * host; * p;p ++)
	{
		if (* p == '\\')
			* p = '/';
	}
	if (NULL == (p = strstr(* host, "/")))
	{
		* path = str_assign("/");
	}
	else
	{
		* path = str_assign(p);
		* p = 0;
	}
	if (NULL == * path)
	{
		my_safe_free(* host);
		printf("%s: can not allocate memory\n", __func__);
		return 0;
	}
	if (NULL != (p = strstr(* host, ":")))
	{
		* p = 0;
		p ++;
		port = (unsigned short)atoi(p);
	}
	return port;
}

static inline void set_socket_timeout(int s, unsigned int w_timeout, unsigned int r_timeout)
{
	struct timeval tv;
	tv.tv_sec = w_timeout;
	tv.tv_usec = 0;
	setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv));
	tv.tv_sec = r_timeout;
	setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char *)&tv,sizeof(tv));
}

unsigned long my_gethostbyname(const char * server)
{
	unsigned long ip;
	struct hostent host, * lphost;
	char * dnsbuffer;
	int rc;

	if (INADDR_NONE != (ip = inet_addr(server)))
		return ip;
	if (NULL == (dnsbuffer = (char *)malloc(8192)))
		return INADDR_NONE;
	if (gethostbyname_r(server, &host, dnsbuffer, 8192, &lphost, &rc) || (! lphost))
	{
		res_init();
		if (gethostbyname_r(server, &host, dnsbuffer, 8192, &lphost, &rc) || (! lphost))
		{
			printf("Can not resolve this domain name of %s\n", server);
			ip = INADDR_NONE;
		}
		else
		{
			ip = ((struct in_addr *)(lphost->h_addr))->s_addr;
		}
	}
	else
	{
		ip = ((struct in_addr *)(lphost->h_addr))->s_addr;
	}

	free(dnsbuffer);
	return ip;
}

bool socket_connect(int s, const char * host, unsigned short port, unsigned int timeout)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (INADDR_NONE == (addr.sin_addr.s_addr = my_gethostbyname(host)))
		return false;
	if (0 > connect(s, (struct sockaddr *)&addr, sizeof(addr))) 
	{
		printf("%s: Can't connect to %s:%d\n", __func__, host, port);
		return false;
	}
	return true;
}

int http_client_get(const char * url,char ** result_content, unsigned int * result_content_len)
{
	char * host = NULL, * path = NULL, * line = NULL, * temp;
	unsigned short port;
	int s = -1;
	HTTP_STREAM * stream = NULL;
	int value1,value2,value3;
	int ok = 0;
	unsigned int offset;
	int len;
	
	if (result_content)
	{
		* result_content = NULL;
		if (! result_content_len)
			return 0;
		* result_content_len = 0;
	}
	if (0 == (port = parse_url(url, &host, &path)))
	{
		printf("%s: Can not parse url %s\n", __func__, url);
		return 0;
	}
	if (0 > (s = socket(AF_INET,SOCK_STREAM,0)))
	{
		printf("%s: Can not create socket\n", __func__);
		goto quit;
	}
	set_socket_timeout(s, 30, 30);
	if (! socket_connect(s, host, port, 30))
	{
		printf("%s: Can not connect to %s:%d\n", __func__, host, port);
		goto quit;
	}
	if (NULL == (stream = http_stream_open(s)))
	{
		printf("%s: fdopen failed\n", __func__);
		goto quit;
	}

	printf("Path= %s\n",path);
	printf("Host= %s\n",host);

	http_stream_printf(stream, "GET %s HTTP/1.%d\r\nHost: %s\r\nUser-Agent: myclient/1.0 me@null.net\r\n\r\n",
		path, 1, host);
	
	if (NULL == (line = (char *)malloc(LINE_SIZE)))
	{
		printf("%s: Can not allocate memory\n", __func__);
		goto quit;
	}
	if (! http_stream_gets(line, LINE_SIZE, stream))
	{
		printf("%s: Can not get echo\n", __func__);
		goto quit;
	}
	
	printf("%s",line);

	if (3 != sscanf(line,"HTTP/%d.%d %d ",&value1,&value2,&value3)) 
	{
		printf("%s: Get incorrect echo:\n%s\n", __func__, line);
		goto quit;
	}
	
	ok = 1;
	if (result_content)
	{
		ok = 0;
		* result_content_len = 0;
		while (http_stream_gets(line,LINE_SIZE,stream))
		{
			if (strncmp(line,"\r\n",2) == 0)
			{
				ok = 1;
				break;
			}
			else if (strncasecmp(line,"Content-Length:",15) == 0) 
			{
				temp = line + 15;
				temp += strspn(temp," \t");
				* result_content_len = strtoul(temp,NULL,0);
			}
		}
		if (ok)
		{
			if (* result_content_len)
			{
				if ((* result_content = (char *)malloc(* result_content_len + 1)))
				{
					offset = 0;
					while (0 < (len = http_stream_read((* result_content) + offset, (* result_content_len) - offset, stream)))
						offset += len;
					* ((* result_content) + offset) = 0;
					* result_content_len = offset;
				}
				else
				{
					printf("%s: Can not allocate memory\n", __func__);
					ok = 0;
				}
			}
			else 
			{
				while (0 < (len = http_stream_read(line, LINE_SIZE, stream)))
				{
					if (! (* result_content_len))
						* result_content = (char *)malloc(len + 1);
					else
						* result_content = (char *)realloc(* result_content, * result_content_len + len + 1);
					if (! (* result_content))
					{
						* result_content_len = 0;
						ok = 0;
						break;
					}
					memcpy(* result_content + * result_content_len, line, len);
					* result_content_len += len;
					* (* result_content + * result_content_len) = 0;
				}
			}
		}
	}
	
quit:
	my_safe_free(host);
	my_safe_free(path);
	my_safe_free(line);
	if (stream) http_stream_close(stream);
	if (s >= 0) close(s);
	return ok?value3:ok;
}

