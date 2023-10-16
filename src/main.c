#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "bwt.h"
#include "factorizations.h"
#include "utils.h"
#define BLOCK_SIZE 5000

/**
        MAIN TEST
**/

int main(int argc, char* argv[])
{
    /**
        TEST SUL CORPUS DELL'ARTICOLO
    **/
    fprintf( stderr, "BWTZ on " );
    if ( argc > 1 ) {
        freopen( argv[ 1 ], "rb", stdin );
        fprintf( stderr, "%s", argv[ 1 ] );
    } else
        fprintf( stderr, "stdin" );
    fprintf( stderr, " to " );
    if ( argc > 2 ) {
        freopen( argv[ 2 ], "wb", stdout );
        fprintf( stderr, "%s", argv[ 2 ] );
    } else
        fprintf( stderr, "stdout" );
    fprintf( stderr, "\n" );

    char buffer[BLOCK_SIZE];
    long itemsread;
    do {
        itemsread = fread (buffer, sizeof(char), BLOCK_SIZE, stdin);
        buffer[itemsread] = '\0';
        char* bwt = bwtz(buffer);
        fprintf(stdout, "%s", bwt);
    } while (itemsread);
    return 0;
}
