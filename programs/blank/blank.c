#include "../stdlib/src/peachos.h"
#include "../stdlib/src/stdlib.h"
#include "../stdlib/src/stdio.h"
#include "../stdlib/src/string.h"

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }

    char* ptr = (char*)0x00;
    *ptr = 0x50;

    while(1) {}

    return 0;
}