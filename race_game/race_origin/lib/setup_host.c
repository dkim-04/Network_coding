#include <stdio.h>
#include <string.h> 

#include "snet.h"

int input_hostname(char hostname[], size_t size)
{
    printf("Input server's host name: ");
    fflush(stdout);

    if (fgets(hostname, size, stdin) == NULL)
        return -1;

    hostname[strcspn(hostname, "\n")] = '\0';

    return 0;
}
