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
#include "spdlog/sinks/hour_file_sink.h"
#include "spdlog/async.h"
#include "spdlog/async_logger.h"

#include <thread>
#include <chrono>
#include <iostream>

#include "unistd.h"
#include "vartype.h"

using spdlogType = std::shared_ptr<spdlog::async_logger>;

static spdlogType aslog = nullptr;

void ThreadPrint(int i)
{
	auto tlog = aslog;
	std::cout<<"spdth:"<<i<<" "<<tlog.use_count()<<std::endl;
	for (int i = 0;i < 100; i++){
		tlog->info(" hello this is spdlog {}", i);
	}
	//tlog->flush();
	//sleep(1);
}
int main()
{
	std::string filename = "./logs/testspd.log";
	std::string logname = "SunWu";
	//定义输出对象
	std::vector<spdlog::sink_ptr> sink_vec;
	sink_vec.push_back(std::make_shared<spdlog::sinks::hour_file_sink_mt>(filename));
	//sink_vec.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
	//创建异步写入log
	//auto tp = std::make_shared<spdlog::details::thread_pool>(1024,1);
	spdlog::init_thread_pool(1024,1);
	auto tp = spdlog::thread_pool();
	auto hour_logger = std::make_shared<spdlog::async_logger>(logname, std::begin(sink_vec), std::end(sink_vec), tp);
	//设置日志输出等级
	hour_logger->set_level(spdlog::level::trace);
	//设置日志格式
	hour_logger->set_pattern("%^[%Y%m%d %H:%M:%S.%e][%l][%n]%v%$");
	//注册到日志管理器中
	spdlog::register_logger(hour_logger);


	//hour_logger->debug("debug hello this is spdlog {}", i);
	aslog = hour_logger;




	std::vector<std::thread> th;
	for (int i =0; i < 10; i++) {
		std::cout<<"for:"<<i<<" "<<aslog.use_count()<<std::endl;
		th.emplace_back(std::thread([&] { ThreadPrint(i); }));
	}


	for (auto& it : th) {
		it.join();
	}

	unsigned char	a = 11;
	unsigned short	b = 12;
	unsigned int	c = 13;
	unsigned long	d = 14;

	char	a1 = 21;
	short	b1 = 22;
	int		c1 = 23;
	long	d1 = 24;

	float a2 = 31;
	double b2 = 32;
	bool c2 = true;


	{
		Monster::VarType var(a);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(b);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(c);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(d);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}

	{
		Monster::VarType var(a1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(b1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(c1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(d1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}

	{
		Monster::VarType var(a2);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(b2);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::VarType var(c2);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}


	spdlog::drop_all();
	return 0;
}
