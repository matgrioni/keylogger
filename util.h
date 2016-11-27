#include <stdio.h>

#define ASSERT_ON_COMPILE(expn) typedef char __C_ASSERT__[(expn) ? 1 : -1]
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))


