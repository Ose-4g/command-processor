#include <gtest/gtest.h>
#include "history.h"

TEST(HistoryTest, getPreviousShouldReturnFalseIfEmpty)
{
    ose4g::History history;
    auto f = history.getPrevious();
    EXPECT_FALSE(f.first);
}

TEST(HistoryTest, getPreviousShouldReturnFalseIfNoMoreHistory)
{
    ose4g::History history;
    history.addBack("history");
    history.getPrevious();
    auto f = history.getPrevious();
    EXPECT_FALSE(f.first);
}

TEST(HistoryTest, getPreviousShouldReturnCorrectHistory)
{
    ose4g::History history;
    history.addBack("history");
    auto f = history.getPrevious();
    EXPECT_TRUE(f.first);
    EXPECT_EQ(f.second, "history");
}

TEST(HistoryTest, getNextShouldReturnFalseIfEmpty)
{
    ose4g::History history;
    auto f = history.getNext();
    EXPECT_FALSE(f.first);
}

TEST(HistoryTest, getNextShouldReturnFalseIfNoMoreHistory)
{
    ose4g::History history;
    history.addFront("history");
    auto f = history.getNext();
    EXPECT_FALSE(f.first);
}

TEST(HistoryTest, getNextShouldReturnCorrectHistory)
{
    ose4g::History history;
    history.addFront("history");
    history.addFront("prehistory");
    auto f = history.getNext();
    EXPECT_TRUE(f.first);
    EXPECT_EQ(f.second, "history");
}