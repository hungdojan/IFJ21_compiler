/**
 * @brief Testovani ruznych modulu kompilatoru IFJ21
 *
 * @author Hung Do (xdohun00)
 *
 * @date 04.10.2021
 */

#include <gtest/gtest.h>
extern "C" {
#include <string.h>
#include <stdio.h>
#include "scanner.h"
#include "istring.h"
#include "token.h"
};
#include <string>

/**
 * @brief Test struktury Istring
 */
class IstringStruct : public ::testing::Test {
protected:
    Istring str;

    virtual void SetUp() {
        string_Init(&str);
    }

    virtual void TearDown() {
        string_Free(&str);
    }
};

class LexemAnalysis : public ::testing::Test {
protected:
    FILE *f = NULL;
    token_t *token = NULL;
    virtual void SetUp() {
    }

    virtual void TearDown() {
        if (f != NULL)      fclose(f);
        if (token != NULL)  token_delete(&token);
    }
};

TEST_F(IstringStruct, Init) {
    EXPECT_EQ(str.allocSize, STRING_BASE_SIZE);
    EXPECT_EQ(str.length, 0);
    EXPECT_STREQ(str.value, "");
}

TEST_F(IstringStruct, AddChar) {
    char *my_str = "ahoj";
    for (int i = 0; my_str[i]; i++) {
        string_Add_Char(&str, my_str[i]);
    }
    string_Add_Char(&str, '\0');
    EXPECT_GE(str.allocSize, strlen(my_str));
    EXPECT_EQ(str.length, 5);
    EXPECT_STREQ(str.value, my_str);
}

TEST_F(LexemAnalysis, ValidTerms) {
    f = fopen("./test/IFJ_good.txt", "r");

    for (int i = 0; i < 12; i++) {
        EXPECT_EQ(get_token(f, &token), 0);
        EXPECT_EQ(token->type, TYPE_IDENTIFIER);
        switch (i) {

            case 1:     EXPECT_STREQ(token->value.str_val, "_"); break;
            case 4:     EXPECT_STREQ(token->value.str_val, "wo_rd"); break;
            case 6:     EXPECT_STREQ(token->value.str_val, "word42"); break;
            case 9:     EXPECT_STREQ(token->value.str_val, "word_42"); break;
            case 10:    EXPECT_STREQ(token->value.str_val, "_4word2"); break;
            case 11:    EXPECT_STREQ(token->value.str_val, "wo4_rd2"); break;
        }

        token_delete(&token);
    }

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(get_token(f, &token), 0);
        EXPECT_EQ(token->type, TYPE_INTEGER);

        if (i == 2)     EXPECT_EQ(token->value.int_val, 123);
        if (i == 5)     EXPECT_EQ(token->value.int_val, 12);
        if (i == 9)     EXPECT_EQ(token->value.int_val, 10123);
        token_delete(&token);
    }

    for (int i = 0; i < 15; i++) {
        EXPECT_EQ(get_token(f, &token), 0);

        switch(i) {
            case 0:     EXPECT_EQ(token->type, TYPE_KW_DO); break;
            case 1:     EXPECT_EQ(token->type, TYPE_KW_GLOBAL); break;
            case 2:     EXPECT_EQ(token->type, TYPE_KW_NIL); break;
            case 3:     EXPECT_EQ(token->type, TYPE_KW_ELSE); break;
            case 4:     EXPECT_EQ(token->type, TYPE_KW_END); break;
            case 5:     EXPECT_EQ(token->type, TYPE_KW_FUNCTION); break;
            case 6:     EXPECT_EQ(token->type, TYPE_KW_IF); break;
            case 7:     EXPECT_EQ(token->type, TYPE_KW_INTEGER); break;
            case 8:     EXPECT_EQ(token->type, TYPE_KW_LOCAL); break;
            case 9:     EXPECT_EQ(token->type, TYPE_KW_NUMBER); break;
            case 10:    EXPECT_EQ(token->type, TYPE_KW_REQUIRE); break;
            case 11:    EXPECT_EQ(token->type, TYPE_KW_RETURN); break;
            case 12:    EXPECT_EQ(token->type, TYPE_KW_STRING); break;
            case 13:    EXPECT_EQ(token->type, TYPE_KW_THEN); break;
            case 14:    EXPECT_EQ(token->type, TYPE_KW_WHILE); break;
        }

        token_delete(&token);
    }

    for (int i = 0; i < 9; i++) {
        EXPECT_EQ(get_token(f, &token), 0);

        if (i < 3)  EXPECT_EQ(token->type, TYPE_NUMBER);
        else        EXPECT_EQ(token->type, TYPE_NUMBER);
        token_delete(&token);
    }

    for (int i = 0; i < 15; i++) {
        EXPECT_EQ(get_token(f, &token), 0);

        EXPECT_EQ(token->type, TYPE_STRING);

        switch (i) {
            case 0:     EXPECT_STREQ(token->value.str_val, ""); break;
            case 1:     EXPECT_STREQ(token->value.str_val, "word"); break;
            case 2:     EXPECT_STREQ(token->value.str_val, "double word"); break;
            case 13:    EXPECT_STREQ(token->value.str_val, "Ahoj\n\"Sve'te \\\""); break;
            case 14:    EXPECT_STREQ(token->value.str_val, "{"); break;
        }
        token_delete(&token);
    }

    for (int i = 0; i < 20; i++) {
        EXPECT_EQ(get_token(f, &token), 0);

        switch(i) {
            case 0:    EXPECT_EQ(token->type, TYPE_PLUS); break;
            case 3: case 5:
                       EXPECT_EQ(token->type, TYPE_STRLEN); break;
            case 11:   EXPECT_EQ(token->type, TYPE_LESSER_OR_EQ); break;
            case 12:   EXPECT_EQ(token->type, TYPE_GREATER_OR_EQ); break;
            case 14:   EXPECT_EQ(token->type, TYPE_NOT_EQ); break;
        }
        token_delete(&token);
    }

    for (int i = 0; i < 5; i++)
    {
        EXPECT_EQ(get_token(f, &token), 0);

        switch (i) {
            case 0: case 2: case 4:
                EXPECT_EQ(token->type, TYPE_IDENTIFIER); break;
            case 1:     EXPECT_EQ(token->type, TYPE_ASSIGN); break;
            case 3:     EXPECT_EQ(token->type, TYPE_PLUS); break;
        }
        token_delete(&token);
    }

    for (int i = 0; i < 3; i++) {
        EXPECT_EQ(get_token(f, &token), EOF);
    }
}

#if 0 // Neni jeste dopsane
TEST_F(LexemAnalysis, InvalidTerms) {
    f = fopen("./test/IFJ_bad.txt", "r");
    // TODO:
}
#endif
