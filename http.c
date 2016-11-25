#include "http.h"

int is_http_request(const char *payload, int size)
{
    const int MAX_METHOD_LEN = 10;
    char method_buf[MAX_METHOD_LEN];
    char space = ' ';

    int index = 0;
    while (index < size && index < MAX_METHOD_LEN)
    {
        if (payload[index] == space)
        {
            method_buf[index] = '\0';
            break;
        }
        method_buf[index] = payload[index];

        index++;
    }

    if (payload[index] == space)
    {
        int method_count = sizeof(http_methods) / sizeof(char *);
        for (int i = 0; i < method_count; i++)
        {
            if (strcmp(method_buf, http_methods[i]) == 0)
                return 1;
        }
    }

    return 0;
}
