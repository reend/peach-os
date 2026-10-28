#include "../stdlib/src/peachos.h"

int main(int argc, char** argv)
{
    print("stdlib. test main func\n");
    
    while(1) 
    {
        if (getkey() != 0)
        {
            print("stdlib. test getkey: key wass pressed\n");
        }
    }

    return 0;
}