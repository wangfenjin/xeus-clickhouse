#ifndef TEST_DB_HPP
#define TEST_DB_HPP

#include "gtest/gtest.h"

#include "xeus-clickhouse/xeus_clickhouse_interpreter.hpp"
#include "logger.h"

loglevel_e loglevel = logDEBUG;

namespace xeus_clickhouse
{

TEST(xeus_clickhouse_interpreter, sanitize_string_check)
{
    std::string code = "\n\n Some inp\nut\n";
    std::string sanitized_string;
    sanitized_string = sanitize_string(code);
    EXPECT_EQ(sanitized_string, " Some input");
}

// TEST(xeus_clickhouse_interpreter, tokenizer_check)
// {
//     std::string code = "%LOAD database.db rw";
//     std::vector<std::string> tokenized_code;
//     tokenized_code = tokenizer(code);
//     EXPECT_EQ(tokenized_code[2], "database.db");
// }

// TEST(xeus_clickhouse_interpreter, is_magic_check)
// {
//     std::string code = "%LOAD database.db rw";
//     EXPECT_TRUE(is_magic(code));

//     std::string code = "SELECT *";
//     EXPECT_FALSE(is_magic(code));
// }

// TEST(xeus_clickhouse_interpreter, create_destroy_db)
// {
//     std::vector<std::string> tokenized_code; 
//     tokenized_code[2] = ".";
//     tokenized_code[3] = "test.db";
//     tokenized_code = create_db(tokenized_code);
// }
}

#endif
