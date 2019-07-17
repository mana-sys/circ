#include <stdio.h>

#include "msgtok.h"

int main()
{
    size_t toklen;
    char *msg[] = {"NICK ben\r\n", "NICK :ben llanes\r\n", "USER ben * * :Ben Llanes\r\n", "NICK\r\n"};
    char *tok;
    char tokbuf[512];

    for (size_t i = 0; i < sizeof(msg) / sizeof(char *); i++) {
        printf("Testing message: %s", msg[i]);
        tok = msgtok(msg[i], &toklen);
        while (tok != NULL) {
            memset(tokbuf, 0, sizeof(tokbuf));
            memcpy(tokbuf, tok, toklen);
            printf("Token: %s (length: %lu)\n", tokbuf, toklen);
            tok = msgtok(NULL, &toklen);
        }
    }
}