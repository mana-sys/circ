#include <stdio.h>

#include "vendor/unity.h"
#include "../src/msgtok.h"

#define MSG_SIZE 512

void setUp()
{

}

void tearDown()
{

}

//static void test_handles_one_token()
//{
//    char msg[] = "NICK\r\n";
//    char *tok;
//    size_t toklen;
//    struct {
//        char *tok;
//        size_t toklen;
//    } tokens;
//}

static void test_handles_nick_msg()
{
    char msg[] = "NICK ben\r\n";
    char *tok;
    size_t toklen;
    int numTokens = 0;
    struct {
        char *tok;
        size_t toklen;
    } tokens[] = {
            {.tok = "NICK", .toklen = 4},
            {.tok =  "ben", .toklen = 3},
    };

    tok = msgtok(msg, &toklen);
    while (tok != NULL) {
        TEST_ASSERT_EQUAL_STRING(tokens[numTokens].tok, tok);
        TEST_ASSERT_EQUAL_UINT(tokens[numTokens].toklen, toklen);
        numTokens++;

        tok = msgtok(NULL, &toklen);
    }

    TEST_ASSERT_EQUAL_INT(2, numTokens);
}

static void test_handles_user_msg()
{
    char msg[] = "USER ben * * :Ben Llanes\r\n";
    char *tok;
    size_t toklen;
    int numTokens = 0;
    struct {
        char *tok;
        size_t toklen;
    } tokens[] = {
            {.tok = "USER", .toklen = 4},
            {.tok =  "ben", .toklen = 3},
            {.tok =    "*", .toklen = 1},
            {.tok =    "*", .toklen = 1},
            {.tok = "Ben Llanes", .toklen = 10},
    };

    tok = msgtok(msg, &toklen);
    while (tok != NULL) {
        TEST_ASSERT_EQUAL_STRING(tokens[numTokens].tok, tok);
        TEST_ASSERT_EQUAL_UINT(tokens[numTokens].toklen, toklen);
        numTokens++;

        tok = msgtok(NULL, &toklen);
    }

    TEST_ASSERT_EQUAL_INT(5, numTokens);
}

int main()
{
    UnityBegin("test_msgtok.c");
    RUN_TEST(test_handles_nick_msg);
    RUN_TEST(test_handles_user_msg);
    return UnityEnd();
}