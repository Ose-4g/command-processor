#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "command-processor.h"



TEST(CommandProcessorTest, processShouldCallAddedFunction) {
    ose4g::CommandProcessor cp("name");
}
TEST(CommandProcessorTest, processShouldFailIfFunctionNotAdded) {}
TEST(CommandProcessorTest, addShouldFailIfCommandIsInvalid) {}
TEST(CommandProcessorTest, addShouldAddCommandSuccessfully) {}

struct ParseTestInfo{
    std::string d_input;
    ose4g::Command d_command;
    ose4g::Args d_args;
    ParseTestInfo(std::string inp, ose4g::Command cmd, ose4g::Args args):d_input(inp),d_command(cmd),d_args(args){}
};

class ParseStatementPassTest : public testing::TestWithParam<ParseTestInfo>
{
};
class ParseStatementFailTest : public testing::TestWithParam<ParseTestInfo>
{
};

TEST_P(ParseStatementPassTest, parseStatementShouldPassForValidStatment) {
    ose4g::CommandProcessor cp("name");
    ParseTestInfo value = GetParam();
    ose4g::Command command;
    ose4g::Args args;
    EXPECT_TRUE(cp.parseStatement(value.d_input, command, args));
}

TEST_P(ParseStatementFailTest, parseStatementShouldFailForInvalidStatements) {
    ose4g::CommandProcessor cp("name");
    ParseTestInfo value = GetParam();
    ose4g::Command command;
    ose4g::Args args;
    EXPECT_FALSE(cp.parseStatement(value.d_input, command, args));
}

INSTANTIATE_TEST_SUITE_P(TestSuite,
                         ParseStatementFailTest,
                         testing::Values(
                             ParseTestInfo{
                                 "send hello 'world",
                                 "send",
                                 std::vector<std::string>{}},
                             ParseTestInfo{
                                 "send 'hello' \"world",
                                 "send",
                                 std::vector<std::string>{}}));

INSTANTIATE_TEST_SUITE_P(TestSuite,
                         ParseStatementPassTest,
                         testing::Values(
                            ParseTestInfo{
                                "",
                                "",
                                std::vector<std::string>{}},
                             ParseTestInfo{
                                 "send",
                                 "send",
                                 std::vector<std::string>{}},
                             ParseTestInfo{
                                 "send hello world",
                                 "send",
                                 std::vector<std::string>{"hello", "world"}},
                             ParseTestInfo{
                                 "send 'hello' \"world\"",
                                 "send",
                                 std::vector<std::string>{"hello", "world"}},
                             ParseTestInfo{
                                 "send 'hello world'",
                                 "send",
                                 std::vector<std::string>{"hello world"}}));