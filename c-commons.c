#include <stdio.h>
#include <stdlib.h>
#include "test.h"

#define COMMONS_MAIN_LOG(...) COMMONS_LOG("MAIN",__VA_ARGS__);


int main()
{
    testProgramProcess();

    return 0;
}
