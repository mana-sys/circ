#include <stdio.h>

#include "unity.h"
#include "motd.h"

#define MESSAGE "Message of the day!"

void setUp()
{
}

void tearDown()
{
}

static void test_read_motd()
{
    char tmppath[] = "/tmp/motd-XXXXXX";
    char *message;
    FILE *f;

    mkstemp(tmppath);

    f = fopen(tmppath, "w");

    fputs(MESSAGE, f);
    fclose(f);

    message = Motd_Get(tmppath, NULL);

    TEST_ASSERT_EQUAL_STRING(MESSAGE, message);

    free(message);
}

int main(int argc, char *argv[])
{
    UnityBegin("test_motd.c");

    RUN_TEST(test_read_motd);

    return UnityEnd();
}