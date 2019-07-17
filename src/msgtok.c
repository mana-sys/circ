#include <string.h>
#include <stdio.h>

#include "msgtok.h"

static char *msgtok_ptr;

char *msgtok(char * str, size_t *toklen)
{
    // Index of \r\n
    char *end, *i;

    if (str == NULL) {
        if (msgtok_ptr == NULL) {
            return NULL;
        }
        str = msgtok_ptr;
    }

    end = strstr(str, CRLF);
    if (end == NULL) {
        return NULL;
    }

    i = str;

    // If the current character is a colon, then the current token is the last
    // parameter of the message. Therefore, the last token consists of all the
    // characters after the token until CRLF.
    if (*i == ':') {
        *toklen = end - (i + 1);
        *end = '\0';
        msgtok_ptr = NULL;
        return str + 1;
    } else {

        // Search for spaces in the string until we reach a null character or CRLF.
        // If we come across a space, set the token length to the location of the space
        // minus the start location, and return the start location.
        // Additionally, msgtok_ptr
        // skips to the next non-space character.
        for (i = str; *i != 0 && i != end; i++) {
            if (*i == ' ') {
                *i = '\0';
                *toklen = i - str;
                msgtok_ptr = i + 1;
//                while (*msgtok_ptr == ' ') msgtok_ptr++;
                return str;
            }
        }

        // The final token has been parsed, so we set the token length to be the number
        // of characters from the start of the token until CRLF, and return the start
        // of the token. Additionally, msgtok_ptr is set to NULL, so that all subsequent calls
        // return NULL.
        *end = '\0';
        *toklen = end - str;
        msgtok_ptr = NULL;
        return str;
    }
}

char *msgtok_r(char *str, size_t *toklen, char **saveptr)
{
    char *end, *i;

//    if (saveptr == NULL)
    return 0;
}
