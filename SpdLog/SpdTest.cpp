/**
 * @file	SpdTest.cpp
 * @brief	
 * @author	My name is SunWu
 * @version $Id$
 * @date	2020-05-16
 */

#include "spdlog/spdlog.h"
#include "spdlog/common.h"
#include "spdlog/cfg/env.h"
#include "spdlog/sinks/daily_file_sink.h"

int main()
{
	auto daily_logger = spdlog::daily_logger_mt("daily_logger", "./daily.log", 2, 30);
	daily_logger->set_level(spdlog::level::trace);
	daily_logger->debug("debug hello this is spdlog");
	daily_logger->info("info hello this is spdlog");

	return 0;
}
