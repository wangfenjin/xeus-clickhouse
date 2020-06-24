/***************************************************************************
 * Copyright (c) 2020, Fenjin Wang                                          *
 *                                                                          *
 * Distributed under the terms of the BSD 3-Clause License.                 *
 *                                                                          *
 * The full license is in the file LICENSE, distributed with this software. *
 ****************************************************************************/

#include <cctype>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "tabulate/table.hpp"
#include "xeus/xinterpreter.hpp"

#include "xeus-clickhouse/xeus_clickhouse_interpreter.hpp"

#include "logger.h"

namespace xeus_clickhouse {

std::string sanitize_string(const std::string& code) {
    /*
      Cleans the code from inputs that are acceptable in a jupyter notebook.
    */
    std::string aux = code;
    aux.erase(std::remove_if(aux.begin(), aux.end(), [](char const c) { return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }),
              aux.end());
    return aux;
}

std::vector<std::string> interpreter::tokenizer(const std::string& code) {
    /*
      Separetes the code on spaces so it's easier to execute the commands.
    */
    std::stringstream input(sanitize_string(code));
    std::string segment;
    std::vector<std::string> tokenized_str;
    std::string is_magic(1, input.str()[0]);
    tokenized_str.push_back(is_magic);

    while (std::getline(input, segment, ' ')) {
        tokenized_str.push_back(segment);
    }

    return tokenized_str;
}

bool interpreter::is_magic(std::vector<std::string>& tokenized_code) {
    /*
      Returns true if the code input is magic and false if isn't.
    */
    if (tokenized_code[0] == "%") {
        tokenized_code[1].erase(0, 1);
        std::transform(tokenized_code[1].begin(), tokenized_code[1].end(), tokenized_code[1].begin(), ::toupper);
        return true;
    } else {
        return false;
    }
}

void interpreter::connect_db(const std::vector<std::string> tokenized_code) {
    // https://clickhouse.tech/docs/en/interfaces/cli/#command-line-options
    // --host, -h -– The server name, ‘localhost’ by default. You can use either the name or the IPv4 or IPv6 address.
    // --port – The port to connect to. Default value: 9000. Note that the HTTP interface and the native interface use different ports.
    // --user, -u – The username. Default value: default.
    // --password – The password. Default value: empty string.
    // --query, -q – The query to process when using non-interactive mode.
    // --database, -d – Select the current default database. Default value: the current database from the server settings (‘default’ by
    // default).
    // --multiline, -m – If specified, allow multiline queries (do not send the query on Enter).
    // --multiquery, -n – If specified, allow processing multiple queries separated by semicolons.
    // --format, -f – Use the specified default format to output the result.
    // --vertical, -E – If specified, use the Vertical format by default to output the result. This is the same as ‘–format=Vertical’. In
    // this format, each value is printed on a separate line, which is helpful when displaying wide tables.
    // --time, -t – If specified, print the query execution time to ‘stderr’ in non-interactive mode.
    // --stacktrace – If specified, also print the stack trace if an exception occurs.
    // --config-file – The name of the configuration file.
    // --secure – If specified, will connect to server over secure connection.
    // --param_<name> — Value for a query with parameters.
    output_format = "html";
    auto options  = clickhouse::ClientOptions();
    size_t size   = tokenized_code.size();
    for (size_t i = 2; i < size; i++) {
        if (tokenized_code[i] == "--host" || tokenized_code[i] == "-h") {
            if (i + 1 < size) {
                options.SetHost(tokenized_code[i + 1]);
                i++;
            } else {
                throw std::runtime_error("No host after " + tokenized_code[i]);
            }
        } else if (tokenized_code[i] == "--port" || tokenized_code[i] == "-p") {
            if (i + 1 < size) {
                try {
                    int number = std::stoi(tokenized_code[i + 1]);
                    options.SetPort(number);
                    i++;
                } catch (std::exception& e) {
                    throw std::runtime_error("Can't parse port number " + tokenized_code[i + 1]);
                }
            } else {
                throw std::runtime_error("No port after " + tokenized_code[i]);
            }
        } else if (tokenized_code[i] == "--user" || tokenized_code[i] == "-u") {
            if (i + 1 < size) {
                options.SetUser(tokenized_code[i + 1]);
                i++;
            } else {
                throw std::runtime_error("No user after " + tokenized_code[i]);
            }
        } else if (tokenized_code[i] == "--password") {
            if (i + 1 < size) {
                options.SetPassword(tokenized_code[i + 1]);
                i++;
            } else {
                throw std::runtime_error("No password after " + tokenized_code[i]);
            }
        } else if (tokenized_code[i] == "--query" || tokenized_code[i] == "-q") {
            throw std::runtime_error("direct query not supported for now");
        } else if (tokenized_code[i] == "--database" || tokenized_code[i] == "-d") {
            if (i + 1 < size) {
                options.SetDefaultDatabase(tokenized_code[i + 1]);
                i++;
            } else {
                throw std::runtime_error("No database name after " + tokenized_code[i]);
            }
        } else if (tokenized_code[i] == "--format" || tokenized_code[i] == "-f") {
            if (i + 1 < size) {
                if (tokenized_code[i + 1] != "html" && tokenized_code[i + 1] != "plain") {
                    throw std::runtime_error(tokenized_code[i + 1] + " format is not supported. We only support html or plain");
                }
                output_format = tokenized_code[i + 1];
                i++;
            } else {
                throw std::runtime_error("No format after " + tokenized_code[i]);
            }
        } else if (tokenized_code[i] == "--stacktrace") {
            output_stacktrace = true;
        } else if (tokenized_code[i] == "--secure" || tokenized_code[i] == "--config-file" || tokenized_code[i].rfind("--params_", 0)) {
            throw std::runtime_error(tokenized_code[i] + " not supported for now");
        }
        // others are ignored as not related to jupyter
    }
    client = std::make_unique<clickhouse::Client>(options);
}

void interpreter::parse_code(int /*execution_counter*/, const std::vector<std::string>& tokenized_code) {
    if (tokenized_code[1] == "CONNECT") {
        return connect_db(tokenized_code);
    }
    throw std::runtime_error("Load a database to run this command.");
}

void interpreter::configure_impl() {
}

nl::json interpreter::execute_request_impl(int execution_counter, const std::string& code, bool /*silent*/, bool /*store_history*/,
                                           nl::json /*user_expressions*/, bool /*allow_stdin*/) {
    /*
      Executes either Clickhouse code or Jupyter Magic.
    */

    nl::json pub_data;
    std::string result = "";
    std::vector<std::string> traceback;
    std::stringstream html_table("");
    std::vector<std::string> tokenized_code = tokenizer(code);

    try {
        // Runs non-Clickhouse code
        if (is_magic(tokenized_code)) {
            parse_code(execution_counter, tokenized_code);
        } else {  // clickhouse
            std::clock_t begin = std::clock();

            nl::json pub_data;
            tabulate::Table plain_table;
            std::stringstream html_table("");
            uint64_t rowset = 0, rows = 0, bytes = 0;

            std::function<double(double, double)> convtime = [](std::clock_t begin, std::clock_t end) {
                return double(end - begin) / CLOCKS_PER_SEC;
            };

            auto on_exception = [&](const clickhouse::Exception& e) { log(logERROR) << e.stack_trace; };
            auto on_progress  = [&](const clickhouse::Progress& p) {
                rows  = p.rows;
                bytes = p.bytes;
            };
            auto on_data = [&](const clickhouse::Block& block) {
                // TODO: seems multiple response will received
                if (block.GetRowCount() == 0) {
                    return;
                }
                rowset += block.GetRowCount();

                std::vector<std::variant<std::string, tabulate::Table>> column_names;
                html_table << "<table>\n<tr>\n";
                int columnCount = 0;
                for (clickhouse::Block::Iterator bi(block); bi.IsValid(); bi.Next()) {
                    std::string name = bi.Name();
                    column_names.push_back(name);
                    columnCount += 1;
                    html_table << "<th>" << name << "</th>\n";
                }
                plain_table.add_row(column_names);
                html_table << "</tr>\n";

                for (size_t i = 0; i < block.GetRowCount(); ++i) {
                    html_table << "<tr>\n";
                    std::vector<std::variant<std::string, tabulate::Table>> row;
                    for (int column = 0; column < columnCount; column++) {
                        std::stringstream ss;
                        block[column]->Dump(ss, i);
                        std::string cell = ss.str();
                        row.push_back(cell);
                        html_table << "<td>" << cell << "</td>\n";
                    }
                    html_table << "</tr>\n";
                    plain_table.add_row(row);
                }

                html_table << "</table>";
            };
            client->Execute(clickhouse::Query(code).OnData(on_data).OnException(on_exception).OnProgress(on_progress));

            std::clock_t end = std::clock();
            std::ostringstream out;
            out.precision(2);
            out << rowset << " rows in set. Elapsed: " << convtime(begin, end) << "sec. Processed: " << rows << " rows, " << bytes
                << "B (0 rows/s, 0.0B/s)";
            html_table << "<tfoot> <tr><p style=\"color:green;display:inline\">Ok.</p> " << out.str() << " </tr> </tfoot>";

            if (output_format == "html") {
                pub_data["text/html"] = html_table.str();
            }
            pub_data["text/plain"] = plain_table.str() + "\nOk. " + out.str();
            publish_execution_result(execution_counter, std::move(pub_data), nl::json::object());
        }

        nl::json jresult;
        jresult["status"]           = "ok";
        jresult["payload"]          = nl::json::array();
        jresult["user_expressions"] = nl::json::object();
        return jresult;
    }

    catch (const std::runtime_error& err) {
        nl::json jresult;
        jresult["status"] = "error";
        jresult["ename"]  = "Error";
        jresult["evalue"] = err.what();
        traceback.push_back((std::string)jresult["ename"] + ": " + (std::string)err.what());
        publish_execution_error(jresult["ename"], jresult["evalue"], traceback);
        traceback.clear();
        // connect_db(tokenized_code);
        return jresult;
    }
}

nl::json interpreter::complete_request_impl(const std::string& /*code*/, int /*cursor_pos*/) {
    nl::json jresult;
    jresult["status"] = "ok";
    return jresult;
};

nl::json interpreter::inspect_request_impl(const std::string& /*code*/, int /*cursor_pos*/, int /*detail_level*/) {
    nl::json jresult;
    jresult["status"] = "ok";
    return jresult;
};

nl::json interpreter::is_complete_request_impl(const std::string& /*code*/) {
    nl::json jresult;
    jresult["status"] = "complete";
    return jresult;
};

nl::json interpreter::kernel_info_request_impl() {
    nl::json result;
    result["implementation"]         = "xclickhouse";
    result["implementation_version"] = "0.1.0";

    /* The jupyter-console banner for xeus-clickhouse is the following:
      __  _____ _   _ ___
      \ \/ / _ \ | | / __|
       >  <  __/ |_| \__ \
      /_/\_\___|\__,_|___/
      xeus-clickhouse: a Jupyter kernel for Clickhouse
    */

    // clang-format off
    std::string banner =
        ""
        "  __  _____ _   _ ___\n"
        "  \\ \\/ / _ \\ | | / __|\n"
        "   >  <  __/ |_| \\__ \\\n"
        "  /_/\\_\\___|\\__,_|___/\n"
        "\n"
        "  xeus-clickhouse: a Jupyter kernel for Clickhouse\n"
        "  Clickhouse ";
    // clang-format on

    result["banner"]                           = banner;
    result["language_info"]["name"]            = "clickhouse";
    result["language_info"]["codemirror_mode"] = "sql";
    // TODO: get version from clickhouse
    result["language_info"]["version"]        = "1.0.0";
    result["language_info"]["mimetype"]       = "";
    result["language_info"]["file_extension"] = "";
    return result;
}

void interpreter::shutdown_request_impl() {
}

}  // namespace xeus_clickhouse
