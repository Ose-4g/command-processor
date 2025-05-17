#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "command-processor.h"

class AddCommandFailTest : public testing::TestWithParam<ose4g::Command>
{
};
INSTANTIATE_TEST_SUITE_P(TestSuite,
                         AddCommandFailTest,
                         testing::Values("2sendcommand", "*sendcommand", "send*command2", "send command2"));

TEST_P(AddCommandFailTest, addShouldFailIfCommandIsInvalid)
{
    ose4g::CommandProcessorImpl cp("name");
    EXPECT_THROW(cp.add(GetParam(), [](const ose4g::Args &) {}, "my description"), std::invalid_argument);
}

class AddCommandPassTest : public testing::TestWithParam<ose4g::Command>
{
};
INSTANTIATE_TEST_SUITE_P(TestSuite,
                         AddCommandPassTest,
                         testing::Values("send-command", "sendcommand"));
TEST_P(AddCommandPassTest, addShouldAddCommandSuccessfully)
{
    ose4g::CommandProcessorImpl cp("name");
    EXPECT_NO_THROW(cp.add(GetParam(), [](const ose4g::Args &) {}, "my description"));
}

class ProcessCommandTest : public testing::Test
{
public:
    bool called = false;
    void doStuff(const ose4g::Args &args)
    {
        called = true;
    }
};
TEST_F(ProcessCommandTest, processShouldCallAddedFunction)
{
    ose4g::CommandProcessorImpl cp("name");
    auto f = std::bind(&ProcessCommandTest::doStuff, this, std::placeholders::_1);
    EXPECT_NO_THROW(cp.add("mycommand", f, ""));
    EXPECT_NO_THROW(cp.process("mycommand", {}));
    EXPECT_TRUE(called);
}

TEST(CommandProcessorTest, processShouldFailIfFunctionNotAdded)
{
    ose4g::CommandProcessorImpl cp("name");
    EXPECT_THROW(cp.process("mycommand", {}), std::invalid_argument);
}

TEST_F(ProcessCommandTest, processShouldFailIfFunctionValidationFails)
{
    ose4g::CommandProcessorImpl cp("name");
    auto f = std::bind(&ProcessCommandTest::doStuff, this, std::placeholders::_1);
    ose4g::ArgCountRule<1, 5> rule1;
    EXPECT_NO_THROW(cp.add("mycommand", f, {&rule1}, ""));
    EXPECT_THROW(cp.process("mycommand", {}), std::invalid_argument);
}

struct ParseTestInfo
{
    std::string d_input;
    ose4g::Command d_command;
    ose4g::Args d_args;
    ParseTestInfo(std::string inp, ose4g::Command cmd, ose4g::Args args) : d_input(inp), d_command(cmd), d_args(args) {}
};

class ParseStatementPassTest : public testing::TestWithParam<ParseTestInfo>
{
};
class ParseStatementFailTest : public testing::TestWithParam<ParseTestInfo>
{
};

TEST_P(ParseStatementPassTest, parseStatementShouldPassForValidStatment)
{
    ose4g::CommandProcessorImpl cp("name");
    ParseTestInfo value = GetParam();
    ose4g::Command command;
    ose4g::Args args;
    EXPECT_TRUE(cp.parseStatement(value.d_input, command, args));
    EXPECT_EQ(command, value.d_command);
    EXPECT_EQ(args, value.d_args);
}

TEST_P(ParseStatementFailTest, parseStatementShouldFailForInvalidStatements)
{
    ose4g::CommandProcessorImpl cp("name");
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

class TestCout : public testing::Test
{
public:
    std::stringstream buffer;
    std::streambuf *sbuf;
    void SetUp() override
    {
        buffer.clear();
        sbuf = std::cout.rdbuf();
        std::cout.rdbuf(buffer.rdbuf());
    }

    void TearDown() override
    {
        std::cout.rdbuf(sbuf);
    }
};

TEST_F(TestCout, ShouldPrintsOnlyHelpByDefault)
{
    ose4g::CommandProcessorImpl cp("name");
    std::string helpMessage = "\t\033[1;34mhelp\033[0m: lists all commands and their description\n";
    helpMessage += "\t\033[1;34mclear\033[0m: clear screen\n";
    helpMessage += "\t\033[1;34mexit\033[0m: exit program\n";
    cp.help();
    EXPECT_EQ(buffer.str(), helpMessage);
}

TEST_F(TestCout, ShouldPrintDescriptionFromAllOtherCommands)
{
    ose4g::CommandProcessorImpl cp("name");
    EXPECT_NO_THROW(cp.add("send", [](const ose4g::Args &args) {}, "Usage send name args. Sends arg info"));
    EXPECT_NO_THROW(cp.add("list", [](const ose4g::Args &args) {}, "lists all active processes"));
    std::string helpMessage = "\t\033[1;34mhelp\033[0m: lists all commands and their description\n";
    helpMessage += "\t\033[1;34mclear\033[0m: clear screen\n";
    helpMessage += "\t\033[1;34mexit\033[0m: exit program\n";
    helpMessage += "\t\033[1;34mlist\033[0m: lists all active processes\n";
    helpMessage += "\t\033[1;34msend\033[0m: Usage send name args. Sends arg info\n";
    cp.help();
    EXPECT_EQ(buffer.str(), helpMessage);
}

TEST(ValidateTest, argCountRuleShouldFailWithLessThanRequiredArguments)
{
    ose4g::ArgCountRule<3> rule;
    ose4g::Args args = {"arg1"};
    auto res = rule.apply(args);
    EXPECT_FALSE(res.first);
    EXPECT_EQ(res.second, "Number of arguments should be between 3 and 10 But got 1");
}

TEST(ValidateTest, argCountRuleShouldFailWithMoreThanRequiredArguments)
{
    ose4g::ArgCountRule<1, 3> rule;
    ose4g::Args args = {"arg1", "arg2", "arg3", "arg4"};
    auto res = rule.apply(args);
    EXPECT_FALSE(res.first);
    EXPECT_EQ(res.second, "Number of arguments should be between 1 and 3 But got 4");
}

TEST(ValidateTest, argCountRuleShouldPassWithRequiredArguments)
{
    ose4g::ArgCountRule<1, 5> rule;
    ose4g::Args args = {"arg1", "arg2", "arg3"};
    auto res = rule.apply(args);
    EXPECT_TRUE(res.first);
}

TEST(ValidateTest, testUserRuleFailure)
{
    ose4g::UserRule rule([](const ose4g::Args &args)
                         {
        bool valid = args[0]=="3";
        return std::pair<bool, std::string>{valid, valid ? "" : "invalid arguments"}; });
    ose4g::Args args = {"arg1"};
    auto res = rule.apply(args);
    EXPECT_FALSE(res.first);
    EXPECT_EQ(res.second, "invalid arguments");
}

TEST(ValidateTest, testUserRuleSuccess)
{
    ose4g::UserRule rule([](const ose4g::Args &args)
                         {
        bool valid = args[0]=="3";
        return std::pair<bool, std::string>{valid, valid ? "" : "invalid arguments"}; });
    ose4g::Args args = {"3"};
    auto res = rule.apply(args);
    EXPECT_TRUE(res.first);
}
