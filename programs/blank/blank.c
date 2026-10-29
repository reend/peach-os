#include "../stdlib/src/peachos.h"
#include "../stdlib/src/stdlib.h"
#include "../stdlib/src/stdio.h"
#include "../stdlib/src/string.h"

int main(int argc, char** argv)
{
    char str[] = "hello world";
    struct command_argument* root_command = peachos_parse_command(str, sizeof(str));
    printf("%s\n", root_command->argument);
    printf("%s\n", root_command->next->argument);

    while(1) 
    {
    }

    return 0;
}