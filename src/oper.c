//
// Created by mana on 10/2/19.
//

#include "log.h"
#include "oper.h"

#define PASSWD_LINE_MAX 512

int parse_oper_passwd(const char *passwd_file, GHashTable *oper)
{
    size_t line_len;
    FILE *passwd;
    char *delim;
    char line[PASSWD_LINE_MAX];

    passwd = fopen(passwd_file, "r");
    if (passwd == NULL)
        return -1;

    while (fgets(line, PASSWD_LINE_MAX, passwd)) {

        line_len = strlen(line);

        if (line[line_len - 1] == '\n')
            line[line_len - 1] = '\0';

        delim = strchr(line, ',');

        if (delim == NULL) {
            Log_Debug("invalid OPER passwd file format");
            fclose(passwd);
            return -1;
        }

        *delim = '\0';

        g_hash_table_insert(oper, g_strdup(line), g_strdup(delim + 1));
    }

    fclose(passwd);

    return 0;
}
