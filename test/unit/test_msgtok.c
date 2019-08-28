#include "msgtok.h"
#include "unity.h"

#define MSG_SIZE 512

void setUp()
{

}

void tearDown()
{

}

static void test_handles_invalid_message()
{
    char message[] = "NICK ben\r";
    char *token;
    size_t tokenLen;

    token = msgtok(message, &tokenLen);

    TEST_ASSERT_NULL(token);
    TEST_ASSERT_EQUAL_UINT(0, tokenLen);
}

static void test_handles_invalid_message_r()
{
    char message[] = "NICK ben\r";
    char *token, *saveptr;
    size_t tokenLen;

    token = msgtok_r(message, &tokenLen, &saveptr);

    TEST_ASSERT_NULL(token);
    TEST_ASSERT_EQUAL_UINT(0, tokenLen);
}

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

static void test_handles_nick_msg_r()
{
    char msg[] = "NICK ben\r\n";
    char *tok;
    char *saveptr;
    size_t toklen;
    int numTokens = 0;
    struct {
        char *tok;
        size_t toklen;
    } tokens[] = {
            {.tok = "NICK", .toklen = 4},
            {.tok =  "ben", .toklen = 3},
    };

    tok = msgtok_r(msg, &toklen, &saveptr);
    while (tok != NULL) {
        TEST_ASSERT_EQUAL_STRING(tokens[numTokens].tok, tok);
        TEST_ASSERT_EQUAL_UINT(tokens[numTokens].toklen, toklen);
        numTokens++;

        tok = msgtok_r(NULL, &toklen, &saveptr);
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

static void test_handles_user_msg_extra_space_front()
{
    char msg[] = "    USER ben * * :Ben Llanes\r\n";
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

static void test_handles_user_msg_extra_space_between_params()
{
    char msg[] = "    USER   ben    *   *   :Ben Llanes\r\n";
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

static void test_handles_user_message_extra_space_between_params_r()
{
    char msg[] = "    USER   ben    *   *   :Ben Llanes\r\n";
    char *tok, *saveptr;
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

    tok = msgtok_r(msg, &toklen, &saveptr);
    while (tok != NULL) {
        TEST_ASSERT_EQUAL_STRING(tokens[numTokens].tok, tok);
        TEST_ASSERT_EQUAL_UINT(tokens[numTokens].toklen, toklen);
        numTokens++;

        tok = msgtok_r(NULL, &toklen, &saveptr);
    }

    TEST_ASSERT_EQUAL_UINT(0, toklen);
    TEST_ASSERT_EQUAL_INT(5, numTokens);
}

static void test_handles_user_msg_r()
{
    char msg[] = "USER ben * * :Ben Llanes\r\n";
    char *tok, *saveptr;
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

    tok = msgtok_r(msg, &toklen, &saveptr);
    while (tok != NULL) {
        TEST_ASSERT_EQUAL_STRING(tokens[numTokens].tok, tok);
        TEST_ASSERT_EQUAL_UINT(tokens[numTokens].toklen, toklen);
        numTokens++;

        tok = msgtok_r(NULL, &toklen, &saveptr);
    }

    TEST_ASSERT_EQUAL_INT(5, numTokens);
}

int main()
{
    UnityBegin("test_msgtok.c");

    RUN_TEST(test_handles_invalid_message);
    RUN_TEST(test_handles_nick_msg);
    RUN_TEST(test_handles_user_msg);
    RUN_TEST(test_handles_user_msg_extra_space_front);
    RUN_TEST(test_handles_user_msg_extra_space_between_params);

    RUN_TEST(test_handles_invalid_message_r);
    RUN_TEST(test_handles_nick_msg_r);
    RUN_TEST(test_handles_user_msg_r);
    RUN_TEST(test_handles_user_message_extra_space_between_params_r);

    return UnityEnd();
}