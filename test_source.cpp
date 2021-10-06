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
#include "scanner.h"
#include "istring.h"
// #include "token.h"
};

/**
 * @brief Test struktury Istring
 */
class IstringStruct : public ::testing::Test {
protected:
    Istring str;

    virtual void SetUp() {
        string_Init(&str);
    };

    virtual void TearDown() {
        string_Free(&str);
    };
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
