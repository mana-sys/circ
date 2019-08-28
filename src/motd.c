//
// Created by mana on 8/27/19.
//

#include "motd.h"

char * Motd_Get(const char *path, size_t *len)
{
    int result;
    long offset;
    size_t numRead;
    char *message;
    FILE *f;

    /*
     * Find the length of the file by seeking to the end getting the offset.
     */
    f = fopen(path, "r");
    if (f == NULL)
        return NULL;

    if (fseek(f, 0, SEEK_END) != 0)
        return NULL;

    if ((offset = ftell(f)) == -1)
        return NULL;

    /*
     * Allocate memory for the string.
     */
    if ((message = calloc(1, offset + 1)) == NULL)
        return NULL;

    /*
     * Read the contents of the file into the string, and return the string.
     */
    if (fseek(f, 0, SEEK_SET) != 0)
        return NULL;

    if (fread(message, offset, 1, f) != 1)
        return NULL;

    fclose(f);

    if (len != NULL)
        *len = offset;
    return message;
}