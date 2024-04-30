#include "main.h"
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char base_directory[1024] = {0};
int port = 4221;

int main(int argc, char **argv)
{

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "--directory") == 0 || strcmp(argv[i], "-d") == 0) && i + 1 < argc)
        {
            strncpy(base_directory, argv[i + 1], sizeof(base_directory) - 1);
            base_directory[sizeof(base_directory) - 1] = '\0';
            i++;
        }
        else if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) && i + 1 < argc)
        {
            port = atoi(argv[i + 1]);
            i++;
        }
    }

    run_server();
    return 0;
}
