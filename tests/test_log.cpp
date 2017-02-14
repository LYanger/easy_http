#include "log.h"
#include <stdio.h>

int main()
{
    int ret = log_init("../conf", "log.conf");
    if (ret != 0) {
        printf("log init error!");
        return 0;
    }		
	LOG_DEBUG("%s", "wocao");
	LOG_WARN("%s", "wocao");
	LOG_INFO("%s", "wocao");
	LOG_ERROR("%s", "wocao");
}
