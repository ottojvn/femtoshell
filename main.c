#include <stdio.h>
#include <string.h>

#define TRUE 1
#define BUFFER_SIZE 256

int main(void)
{
    char buffer[BUFFER_SIZE];

    while(TRUE)
    {
        printf("$ ");
        
        fgets(buffer, BUFFER_SIZE, stdin);
        char *token = strtok(buffer, " ");

        do
        {
            printf("%s\n", token);
        }
        while((token = strtok(NULL, " ")));
    }

    return 0;
}
