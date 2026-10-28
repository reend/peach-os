#include "../stdlib/src/peachos.h"
#include "../stdlib/src/stdlib.h"

int main(int argc, char** argv)
{
    print("stdlib. test main func\n");

    print(itoa(8763));
    
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