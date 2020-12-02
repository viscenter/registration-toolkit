#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "rt/util/String.hpp"

using namespace rt;

TEST(String, ToUpper)
{
    std::string input{"abcde!@"};
    to_upper(input);
    EXPECT_EQ(input, "ABCDE!@");
}

TEST(String, ToUpperCopy)
{
    std::string input{"abcde!@"};
    EXPECT_EQ(to_upper_copy(input), "ABCDE!@");
    EXPECT_NE(input, "ABCDE!@");
}

TEST(String, ToLower)
{
    std::string input{"ABCDE!@"};
    to_lower(input);
    EXPECT_EQ(input, "abcde!@");
}

TEST(String, ToLowerCopy)
{
    std::string input{"ABCDE!@"};
    EXPECT_EQ(to_lower_copy(input), "abcde!@");
    EXPECT_NE(input, "abcde!@");
}

TEST(String, TrimLeft)
{
    std::string input{" \r \n  \r\n left trim "};
    trim_left(input);
    EXPECT_EQ(input, "left trim ");
}

TEST(String, TrimLeftCopy)
{
    std::string input{" \r \n \r\n left trim "};
    EXPECT_EQ(trim_left_copy(input), "left trim ");
    EXPECT_NE(input, "left trim ");
}

TEST(String, TrimRight)
{
    std::string input{" right trim \r \n  \r\n "};
    trim_right(input);
    EXPECT_EQ(input, " right trim");
}

TEST(String, TrimRightCopy)
{
    std::string input{" right trim \r \n  \r\n "};
    EXPECT_EQ(trim_right_copy(input), " right trim");
    EXPECT_NE(input, " right trim");
}

TEST(String, Trim)
{
    std::string input{" \r \n  \r\n trim \r \n  \r\n "};
    trim(input);
    EXPECT_EQ(input, "trim");
}

TEST(String, TrimCopy)
{
    std::string input{" \r \n  \r\n trim \r \n  \r\n "};
    EXPECT_EQ(trim_copy(input), "trim");
    EXPECT_NE(input, "trim");
}

TEST(String, Split)
{
    // Setup expected
    std::vector<std::string> expected{"a", "b", "c"};

    // Space separated (implicit)
    EXPECT_EQ(split("a b c"), expected);

    // Multiple spaces (implicit)
    EXPECT_EQ(split("  a  b  c  "), expected);

    // Space separated (explicit)
    EXPECT_EQ(split("a b c", ' '), expected);

    // Comma separated
    EXPECT_EQ(split("a,b,c", ','), expected);

    // Multi-delimited
    EXPECT_EQ(split("a+b-c", '+', '-'), expected);
}