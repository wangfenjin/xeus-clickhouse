/***************************************************************************
 * Copyright (c) 2020, Mariana Meireles                                     *
 * Copyright (c) 2020, QuantStack                                           *
 *                                                                          *
 * Distributed under the terms of the BSD 3-Clause License.                 *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

#ifndef XEUS_CLICKHOUSE_INTERPRETER_HPP
#define XEUS_CLICKHOUSE_INTERPRETER_HPP

#include "clickhouse/client.h"
#include "nlohmann/json.hpp"
#include "xeus/xinterpreter.hpp"
#include "xeus_clickhouse_config.hpp"

namespace nl = nlohmann;

namespace xeus_clickhouse {
class XEUS_CLICKHOUSE_API interpreter : public xeus::xinterpreter {
public:
    interpreter()          = default;
    virtual ~interpreter() = default;

private:
    void configure_impl() override;
    nl::json execute_request_impl(int execution_counter, const std::string& code, bool silent, bool store_history,
                                  nl::json user_expressions, bool allow_stdin) override;
    nl::json complete_request_impl(const std::string& code, int cursor_pos) override;
    nl::json inspect_request_impl(const std::string& code, int cursor_pos, int detail_level) override;
    nl::json is_complete_request_impl(const std::string& code) override;
    nl::json kernel_info_request_impl() override;
    void shutdown_request_impl() override;

    /**
     * Separetes the code on spaces so it's easier to execute the commands.
     */
    std::vector<std::string> tokenizer(const std::string& code);

    /**
     * Returns true if the code input is magic and false if isn't.
     */
    bool is_magic(std::vector<std::string>& tokenized_code);

    /**
     * Parse magic and calls the correct function.
     */
    void parse_code(int execution_counter, const std::vector<std::string>& tokenized_code);

    // connect to ch db
    void connect_db(const std::vector<std::string> tokenized_code);

    std::unique_ptr<clickhouse::Client> client;

    void on_exception(const clickhouse::Exception& e);
    void on_progress(const clickhouse::Progress& progress);
    void on_data(const clickhouse::Block& block);

    std::string output_format;
    bool output_stacktrace;
};

/**
 * Cleans the code from inputs that are acceptable in a jupyter notebook.
 */
std::string sanitize_string(const std::string& code);
}  // namespace xeus_clickhouse

#endif
