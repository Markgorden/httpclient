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

int main(int argc, char **argv, char** envp)
{
	char * result_content = NULL;
	unsigned int result_content_len;
	int result =  http_client_get(argv[1],&result_content, &result_content_len);
	// 写入到文件
	char * filename = NULL;
	//把数据写入文件中
	filename =(char *)malloc(30);
	strncpy(filename,"/tmp/",5);
	strcpy(filename+5,"http_plugin.json");

	FILE *new_file;
	if((new_file=fopen(filename,"wt+"))==NULL)
		printf("Can't fopen file %s \n",filename);

	int write_length = 0;
	write_length = fwrite(result_content,sizeof(char),result_content_len,new_file);

	if(write_length < result_content_len)
		printf("Write file failed \n");
	//printf("write_length = %d \n result_content_len = \n",write_length,result_content_len);
    printf("Write file successes! (%s)\n",filename);
	
	fflush(new_file);
	fclose(new_file);
	my_safe_free(filename);	
	my_safe_free(result_content);
	return 1;
}
