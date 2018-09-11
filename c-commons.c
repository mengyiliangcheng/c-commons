#include <stdio.h>
#include "utils_string.h"
#include "string.h"
int main()
{
    printf("hello world");
    char* p = "123456";
    char buffer[128] = {0};
    strings_to_hex(p,strlen(p),buffer);
    return 0;
}
