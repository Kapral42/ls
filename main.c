#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "directory.h"

int main(int argc, const char *argv[])
{
    int list_opt = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'l') {
                list_opt = 1;
            } else {
                printf("Usage: ls <Options(\"-l\" only)> directory\n");
                return (EXIT_FAILURE);
            }
        }
    }

    const char *path;
    if (argc == 1 || argv[argc - 1][0] == '-') {
        /* if path not found then set path to current folder */
        path = "./";
    } else {
        path = argv[argc - 1];
    }

    print_dir(path, list_opt);

    return (EXIT_SUCCESS);
}
