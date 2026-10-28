#include "../stdlib/src/peachos.h"
#include "../stdlib/src/stdlib.h"
#include "stdio.h"

int main(int argc, char** argv)
{
    printf("My age is %i\n", 98);
    
    print("stdlib. test main func\n");

    print(itoa(8763));

    putchar('Z');
    
    void* ptr = malloc(512);
    free(ptr);

    while(1) 
    {
        if (getkey() != 0)
        {
            print("stdlib. test getkey: key wass pressed\n");
        }
    }

    return 0;
}