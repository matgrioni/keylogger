#include <stdio.h>

#include "process.h"

int main()
{
    int pid;
    while (1)
    {
        scanf("%d", &pid);
        printf("%d\n", exists_task_by_id(pid));
    }
    return 0;
}
