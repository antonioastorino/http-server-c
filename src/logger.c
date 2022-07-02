#include "logger.h"
#include <stdlib.h>
#include <time.h>

#if LOG_LEVEL > LEVEL_NO_LOGS
void get_date_time(char* date_time_str)
{
    time_t ltime;
    struct tm result;
    ltime = time(NULL);
    localtime_r(&ltime, &result);
    asctime_r(&result, date_time_str);
    date_time_str[24] = 0;
}
#endif
