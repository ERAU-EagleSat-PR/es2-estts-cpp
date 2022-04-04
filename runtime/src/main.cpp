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

int main() {


    // Create stdout sink logger
    std::vector<spdlog::sink_ptr> sinks;
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    stdout_sink->set_level(spdlog::level::warn);
    stdout_sink->set_pattern("[%T] [thread %t] [%^%l%$] [%@] %v");
    sinks.push_back(stdout_sink);

    auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("estts_log", 23, 59);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%T] [thread %t] [%^%l%$] [%@] %v");
    sinks.push_back(file_sink);
    auto combined_logger = std::make_shared<spdlog::logger>("name", begin(sinks), end(sinks));
    spdlog::register_logger(combined_logger);
    spdlog::set_default_logger(combined_logger);


    spdlog::set_level(spdlog::level::trace);

    SPDLOG_TRACE("Bruh");
    SPDLOG_DEBUG("Bruh");
    SPDLOG_INFO("Bruh");
    SPDLOG_WARN("Bruh");
    SPDLOG_ERROR("Bruh");

    auto cosmos = new cosmos_handler();
    if (estts::ES_OK != cosmos->cosmos_init())
        return -1;
    cosmos->initialize_cosmos_daemon();
    return 0;
}