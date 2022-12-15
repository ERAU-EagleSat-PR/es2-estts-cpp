/* Copyright © EagleSat II - Embry Riddle Aeronautical University - All rights reserved - 2022 */
//
// Created by Hayden Roszell on 1/11/22.
//

#include "constants.h"
#include "cosmos_handler.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "helper.h"

int main(int argc, char *argv[]) {

    auto stdoutsink = spdlog::level::trace;

    std::string cosmos_server_address;
    std::string working_log_directory;
    std::string telem_git_repo;
    std::string base_git_dir;

    for (int i = 1; i < argc; i += 2) {
        // --log-level trace --cosmos-server-addr 172.19.35.150 --working-log-dir /tmp/estts/ --base-git-dir /home/parallels/telemetry --telemetry-git-repo git@github.com:ERAU-EagleSat-PR/eaglesat-2-telemetry.git
        if (strcmp(argv[i], "--log-level") == 0) {
            if (strcmp(argv[i+1], "trace") == 0) {
                stdoutsink = spdlog::level::trace;
            } else if (strcmp(argv[i+1], "debug") == 0) {
                stdoutsink = spdlog::level::debug;
            } else if (strcmp(argv[i+1], "info") == 0) {
                stdoutsink = spdlog::level::info;
            } else if (strcmp(argv[i+1], "warn") == 0) {
                stdoutsink = spdlog::level::warn;
            } else if (strcmp(argv[i+1], "err") == 0) {
                stdoutsink = spdlog::level::err;
            } else if (strcmp(argv[i+1], "critical") == 0) {
                stdoutsink = spdlog::level::critical;
            } else if (strcmp(argv[i+1], "off") == 0) {
                stdoutsink = spdlog::level::off;
            }
        } else if (strcmp(argv[i], "--cosmos-server-addr") == 0) {
            cosmos_server_address = argv[i+1];
        } else if (strcmp(argv[i], "--working-log-dir") == 0) {
            working_log_directory = argv[i+1];
        } else if (strcmp(argv[i], "--base-git-dir") == 0) {
            base_git_dir = argv[i+1];
        } else if (strcmp(argv[i], "--telemetry-git-repo") == 0) {
            telem_git_repo = argv[i+1];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            SPDLOG_INFO("Help yourself loser");
            return 0;
        } else {
            SPDLOG_WARN("Unrecognized program argument: {}", argv[i]);
        }
    }

    if (cosmos_server_address.empty())
        cosmos_server_address = "172.19.35.150";

    if (working_log_directory.empty())
        working_log_directory = "/tmp/estts/";

    if (telem_git_repo.empty())
        base_git_dir = "/home/sbeve/telemetry";

    if (base_git_dir.empty())
        telem_git_repo = "git@github.com:ERAU-EagleSat-PR/eaglesat-2-telemetry.git";

    std::stringstream log_path;
    auto logname = "estts_log";
    log_path << working_log_directory;
    log_path << logname;

    // Create stdout sink logger
    std::vector<spdlog::sink_ptr> sinks;
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    stdout_sink->set_level(stdoutsink);
    stdout_sink->set_pattern("[%T] [thread %t] [%^%l%$] [%@] %v");
    sinks.push_back(stdout_sink);

    auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_path.str(), 23, 59);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%T] [thread %t] [%^%l%$] [%@] %v");
    sinks.push_back(file_sink);
    auto combined_logger = std::make_shared<spdlog::logger>("name", begin(sinks), end(sinks));
    spdlog::register_logger(combined_logger);
    spdlog::set_default_logger(combined_logger);

    spdlog::set_level(spdlog::level::trace);

    auto cosmos = new cosmos_handler();
    cosmos->set_cosmos_server_address(cosmos_server_address);
    cosmos->set_telem_git_repo(telem_git_repo);
    cosmos->set_base_git_dir(base_git_dir);

    if (estts::ES_OK != cosmos->cosmos_init())
        return -1;
    cosmos->initialize_cosmos_daemon();
    return 0;
}