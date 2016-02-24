#include "gtest/gtest.h"
#include "infra/util/public/BoundedString.hpp"

TEST(BoundedStringTest, TestConstructedEmpty)
{
    infra::BoundedString::WithStorage<5> string;

    EXPECT_TRUE(string.empty());
    EXPECT_FALSE(string.full());
    EXPECT_EQ(0, string.size());
    EXPECT_EQ(5, string.max_size());
}

TEST(BoundedStringTest, TestConstructFromNChars)
{
    infra::BoundedString::WithStorage<5> string(3, 'a');
    
    EXPECT_EQ('a', string[0]);
    EXPECT_EQ('a', string[1]);
    EXPECT_EQ('a', string[2]);
    EXPECT_EQ(3, string.size());
    EXPECT_FALSE(string.empty());
}

TEST(BoundedStringTest, TestConstructFromOtherString)
{
    infra::BoundedString::WithStorage<5> string(3, 'a');
    infra::BoundedString::WithStorage<4> copy(string, 0, 3);

    EXPECT_EQ('a', copy[0]);
    EXPECT_EQ(3, copy.size());
}

TEST(BoundedStringTest, TestConstructFromCharP)
{
    char a[] = { 'a', '\0', 'a' };
    infra::BoundedString::WithStorage<3> string(a, 3);

    EXPECT_EQ('a', string[0]);
    EXPECT_EQ('\0', string[1]);
    EXPECT_EQ('a', string[2]);
    EXPECT_EQ(3, string.size());
}

TEST(BoundedStringTest, TestConstructFromZeroTerminatedString)
{
    char a[] = { 'a', '\0', 'a' };
    infra::BoundedString::WithStorage<3> string(a);

    EXPECT_EQ('a', string[0]);
    EXPECT_EQ(1, string.size());
}

TEST(BoundedStringTest, TestConstructFromRange)
{
    char a[] = { 'a', '\0', 'a' };
    infra::BoundedString::WithStorage<3> string(a, a + 3);

    EXPECT_EQ('a', string[0]);
    EXPECT_EQ('\0', string[1]);
    EXPECT_EQ('a', string[2]);
    EXPECT_EQ(3, string.size());
}

TEST(BoundedStringTest, TestConstructFromInitializerList)
{
    infra::BoundedString::WithStorage<3> string({ 'a', '\0', 'a' });

    EXPECT_EQ('a', string[0]);
    EXPECT_EQ('\0', string[1]);
    EXPECT_EQ('a', string[2]);
    EXPECT_EQ(3, string.size());
}

TEST(BoundedStringTest, TestAssignFromString)
{
    infra::BoundedString::WithStorage<5> string(3, 'a');
    infra::BoundedString::WithStorage<5> copy;

    copy = string;

    EXPECT_EQ(string, copy);
}

TEST(BoundedStringTest, TestAssignFromZeroTerminated)
{
    infra::BoundedString::WithStorage<5> string;
    string = "abc";

    EXPECT_EQ("abc", string);
    EXPECT_EQ(3, string.size());
}

TEST(BoundedStringTest, TestAssignFromChar)
{
    infra::BoundedString::WithStorage<5> string;
    string = 'a';

    EXPECT_EQ("a", string);
}

TEST(BoundedStringTest, TestFront)
{
    infra::BoundedString::WithStorage<5> string("abc");
    EXPECT_EQ('a', string.front());
}

TEST(BoundedStringTest, TestBack)
{
    infra::BoundedString::WithStorage<5> string("abc");
    EXPECT_EQ('c', string.back());
}

TEST(BoundedStringTest, TestData)
{
    infra::BoundedString::WithStorage<5> string("abc");
    EXPECT_EQ('a', *string.data());
}

TEST(BoundedStringTest, TestBegin)
{
    infra::BoundedString::WithStorage<5> string("abc");
    EXPECT_EQ('a', *string.begin());
}

TEST(BoundedStringTest, TestEnd)
{
    infra::BoundedString::WithStorage<5> string("abc");
    EXPECT_EQ('c', *std::prev(string.end()));
}

TEST(BoundedStringTest, TestRBegin)
{
    infra::BoundedString::WithStorage<5> string("abc");
    EXPECT_EQ('c', *string.rbegin());
}

TEST(BoundedStringTest, TestREnd)
{
    infra::BoundedString::WithStorage<5> string("abc");
    EXPECT_EQ('a', *std::prev(string.rend()));
}

TEST(BoundedStringTest, TestClear)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.clear();
    EXPECT_EQ("", string);
    EXPECT_TRUE(string.empty());
}

TEST(BoundedStringTest, TestAppendWithChars)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.append(2, 'd');
    EXPECT_EQ("abcdd", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestAppendWithString)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.append(infra::BoundedString::WithStorage<5>("de"));
    EXPECT_EQ("abcde", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestAppendWithSubString)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.append(infra::BoundedString::WithStorage<5>("defgh"), 1, 2);
    EXPECT_EQ("abcef", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestAppendWithCharP)
{
    infra::BoundedString::WithStorage<6> string("abc");
    string.append("d\0ef", 3);
    EXPECT_EQ(infra::BoundedString::WithStorage<6>("abcd\0e", 6), string);
    EXPECT_EQ(6, string.size());
}

TEST(BoundedStringTest, TestAppendWithZeroTerminated)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.append("d\0ef");
    EXPECT_EQ("abcd", string);
    EXPECT_EQ(4, string.size());
}

TEST(BoundedStringTest, TestAppendWithRange)
{
    char a[] = "def";
    infra::BoundedString::WithStorage<5> string("abc");
    string.append(a, a + 3);
    EXPECT_EQ("abcde", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestInsertChars)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.insert(1, 2, 'd');

    EXPECT_EQ("addbc", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestInsertZeroTerminated)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.insert(1, "de");

    EXPECT_EQ("adebc", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestInsertCharP)
{
    infra::BoundedString::WithStorage<6> string("abc");
    string.insert(1, "d\0ef", 3);
    EXPECT_EQ(infra::BoundedString::WithStorage<6>("ad\0ebc", 6), string);
    EXPECT_EQ(6, string.size());
}

TEST(BoundedStringTest, TestInsertString)
{
    infra::BoundedString::WithStorage<5> string("abc");
    infra::BoundedString::WithStorage<3> other("de");
    string.insert(1, other);
    EXPECT_EQ("adebc", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestInsertCharAtIterator)
{
    infra::BoundedString::WithStorage<5> string("abc");
    infra::BoundedString::WithStorage<5>::const_iterator i = string.insert(string.begin() + 1, 'd');
    EXPECT_EQ(string.begin() + 2, i);
    EXPECT_EQ("adbc", string);
    EXPECT_EQ(4, string.size());
}

TEST(BoundedStringTest, TestInsertCharsAtIterator)
{
    infra::BoundedString::WithStorage<5> string("abc");
    infra::BoundedString::WithStorage<5>::const_iterator i = string.insert(string.begin() + 1, 2, 'd');
    EXPECT_EQ(string.begin() + 3, i);
    EXPECT_EQ("addbc", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestInsertRangeAtIterator)
{
    char a[] = "de";
    infra::BoundedString::WithStorage<5> string("abc");
    infra::BoundedString::WithStorage<5>::const_iterator i = string.insert(string.begin() + 1, a, a + sizeof(a));
    EXPECT_EQ(string.begin() + 3, i);
    EXPECT_EQ("adebc", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestEraseCount)
{
    infra::BoundedString::WithStorage<5> string("abcd");
    string.erase(1, 2);
    EXPECT_EQ("ad", string);
    EXPECT_EQ(2, string.size());
}

TEST(BoundedStringTest, TestEraseChar)
{
    infra::BoundedString::WithStorage<5> string("abcd");
    infra::BoundedString::WithStorage<5>::const_iterator i = string.erase(string.begin() + 1);
    EXPECT_EQ("acd", string);
    EXPECT_EQ(3, string.size());
}

TEST(BoundedStringTest, TestEraseRange)
{
    infra::BoundedString::WithStorage<5> string("abcd");
    infra::BoundedString::WithStorage<5>::const_iterator i = string.erase(string.begin() + 1, string.begin() + 3);
    EXPECT_EQ("ad", string);
    EXPECT_EQ(2, string.size());
}

TEST(BoundedStringTest, TestPushBack)
{
    infra::BoundedString::WithStorage<5> string("abcd");
    string.push_back('e');
    EXPECT_EQ("abcde", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestPopBack)
{
    infra::BoundedString::WithStorage<5> string("abcd");
    string.pop_back();
    EXPECT_EQ("abc", string);
    EXPECT_EQ(3, string.size());
}

TEST(BoundedStringTest, TestCompareEqual)
{
    infra::BoundedString::WithStorage<5> string1("abcd");
    infra::BoundedString::WithStorage<10> string2("abcd");
    EXPECT_EQ(0, string1.compare(string2));
}

TEST(BoundedStringTest, TestCompareLess)
{
    infra::BoundedString::WithStorage<5> string1("abbb");
    infra::BoundedString::WithStorage<10> string2("abcd");
    EXPECT_EQ(-1, string1.compare(string2));
}

TEST(BoundedStringTest, TestCompareLessBySize)
{
    infra::BoundedString::WithStorage<5> string1("abc");
    infra::BoundedString::WithStorage<10> string2("abcd");
    EXPECT_EQ(-1, string1.compare(string2));
}

TEST(BoundedStringTest, TestCompareGreater)
{
    infra::BoundedString::WithStorage<5> string1("abde");
    infra::BoundedString::WithStorage<10> string2("abcd");
    EXPECT_EQ(1, string1.compare(string2));
}

TEST(BoundedStringTest, TestCompareGreaterBySize)
{
    infra::BoundedString::WithStorage<5> string1("abcde");
    infra::BoundedString::WithStorage<10> string2("abcd");
    EXPECT_EQ(1, string1.compare(string2));
}

TEST(BoundedStringTest, TestReplaceByString)
{
    infra::BoundedString::WithStorage<5> string1("abcde");
    infra::BoundedString::WithStorage<10> string2("kl");
    string1.replace(1, 3, string2);
    EXPECT_EQ("akle", string1);
}

TEST(BoundedStringTest, TestReplaceByChars)
{
    infra::BoundedString::WithStorage<5> string("abcd");
    string.replace(1, 2, 3, 'k');
    EXPECT_EQ("akkkd", string);
}

TEST(BoundedStringTest, TestSubstr)
{
    infra::BoundedString::WithStorage<5> string("abcde");
    EXPECT_EQ("bcd", string.substr(1, 3));
}

TEST(BoundedStringTest, TestCopy)
{
    char buffer[3];
    infra::BoundedString::WithStorage<5> string("abcde");
    EXPECT_EQ(3, string.copy(buffer, 3, 1));
    EXPECT_EQ('b', buffer[0]);
}

TEST(BoundedStringTest, TestResize)
{
    infra::BoundedString::WithStorage<5> string("abc");
    string.resize(5, 'k');
    EXPECT_EQ("abckk", string);
    EXPECT_EQ(5, string.size());
}

TEST(BoundedStringTest, TestSwap)
{
    infra::BoundedString::WithStorage<4> string1("abc");
    infra::BoundedString::WithStorage<5> string2("defg");
    swap(string1, string2);
    EXPECT_EQ("defg", string1);
    EXPECT_EQ("abc", string2);
}

TEST(BoundedStringTest, TestFind)
{
    infra::BoundedString::WithStorage<10> string("abcdefgh");
    EXPECT_EQ(3, string.find("def"));
}

TEST(BoundedStringTest, TestFindNotFound)
{
    infra::BoundedString::WithStorage<10> string("abcdefgh");
    EXPECT_EQ(infra::BoundedString::WithStorage<10>::npos, string.find("fed"));
}

TEST(BoundedStringTest, TestFindEmptyReturnsPos)
{
    infra::BoundedString::WithStorage<10> string("abcdefgh");
    EXPECT_EQ(4, string.find("", 4));
}

TEST(BoundedStringTest, TestRFind)
{
    infra::BoundedString::WithStorage<10> string("abcdefgh");
    EXPECT_EQ(3, string.rfind("def"));
}

TEST(BoundedStringTest, TestRFindNotFound)
{
    infra::BoundedString::WithStorage<10> string("abcdefgh");
    EXPECT_EQ(infra::BoundedString::WithStorage<10>::npos, string.rfind("fed"));
}

TEST(BoundedStringTest, TestRFindEmptyReturnsPos)
{
    infra::BoundedString::WithStorage<10> string("abcdefgh");
    EXPECT_EQ(4, string.rfind("", 4));
}

TEST(BoundedStringTest, TestFindFirstOf)
{
    char search[] = "dcb";
    infra::BoundedString::WithStorage<5> string("abcde");
    EXPECT_EQ(1, string.find_first_of(search));
}

TEST(BoundedStringTest, TestFindFirstNotOf)
{
    char search[] = "dca";
    infra::BoundedString::WithStorage<5> string("abcde");
    EXPECT_EQ(1, string.find_first_not_of(search));
}

TEST(BoundedStringTest, TestFindLastOf)
{
    char search[] = "cdb";
    infra::BoundedString::WithStorage<5> string("abcde");
    EXPECT_EQ(3, string.find_last_of(search));
}

TEST(BoundedStringTest, TestFindLastNotOf)
{
    char search[] = "dce";
    infra::BoundedString::WithStorage<5> string("abcde");
    EXPECT_EQ(1, string.find_last_not_of(search));
}
