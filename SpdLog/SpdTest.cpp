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

#include "monster/monster.h"
#include "monster/ringbuffer.h"
#include "monster/log.h"
#include "monster/socket.h"

#include <map>
#include <vector>
#include <list>
#include <sys/epoll.h>

#include <thread>
#include <chrono>
#include <iostream>
/*
#include "unistd.h"
#include "anytype.h"

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
		Monster::AnyType var(a);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(b);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(c);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(d);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}

	{
		Monster::AnyType var(a1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(b1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(c1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(d1);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}

	{
		Monster::AnyType var(a2);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(b2);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}
	{
		Monster::AnyType var(c2);
		std::cout<<"testvar:"<<var._var_string<<std::endl;
	}

	LOG_FMT_DEBUG("this is a log test");

	spdlog::drop_all();
	Monster::final_global();
	return 0;
}
*/

std::list<Monster::Socket::ptr> waitclientlist;
std::mutex waitlock;

std::map<int, Monster::Socket::ptr> okclientlist;

void handle_client()
{
	std::vector<struct epoll_event> epfds;
	epfds.resize(256);
	auto epfd = epoll_create(256);
	while(true) {
		//先处理waitlist 添加监听事件
		if (!waitclientlist.empty()) {
			std::lock_guard<std::mutex> tlock(waitlock);
			for (auto& it : waitclientlist) {
				struct epoll_event ev;
				ev.events = EPOLLIN | EPOLLOUT | EPOLLERR |EPOLLPRI;
				ev.data.fd = it->get_socket();
				assert(0 == epoll_ctl(epfd, EPOLL_CTL_ADD, it->get_socket(), &ev));
				okclientlist[it->get_socket()] = it;
			}
			waitclientlist.clear();
		}

		//处理监听到的事件
		int ret = epoll_wait(epfd, &epfds[0], okclientlist.size(), 0);
		if (ret > 0) {
			for (int i = 0; i < ret; i++) {
				auto sockid = epfds[i].data.fd;
				auto sockptr = okclientlist[sockid];
				
				if (epfds[i].events & (EPOLLERR | EPOLLPRI)) { //套接口出错
					okclientlist.erase(sockid);
					struct epoll_event ev;
					ev.events = 0;
					ev.data.ptr = nullptr;
					epoll_ctl(epfd, EPOLL_CTL_DEL, sockid, &ev);
					continue;

				} else if (epfds[i].events & EPOLLIN) { //读取
					char buf[1024* 4] = {};
					sockptr->recv(buf, 1024*4);
					LOG_FMT_DEBUG("recv msg : {}", buf);
				} else if (epfds[i].events & EPOLLOUT) { //写入
					char buf[1024* 4] = {};
					snprintf(buf, 1024*4, "%s", "client hello this is a send");
					sockptr->send(buf, 1024*4);
					LOG_FMT_DEBUG("send msg : {}", buf);
				}
			}
		}
		
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void test_socket_client()
{
	auto address = Monster::IPAddress::create("127.0.0.1", 20001);
	if (!address) {
		LOG_FMT_ERROR("test_socket: create address error");
		return;
	}

	//create
	auto socket = Monster::Socket::create_tcp(address);
	if (!socket) {
		LOG_FMT_ERROR("test_socket: create socket error");
		return;
	}

	if (!socket->connect(address)) {
		LOG_FMT_ERROR("test_socket: connect socket error");
		return;
	}

	
	auto epfd = epoll_create(1);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = nullptr;
	assert(0 == epoll_ctl(epfd, EPOLL_CTL_ADD, socket->get_socket(), &ev));

	auto t1 = std::thread(handle_client);

	{
		std::lock_guard<std::mutex> tlock(waitlock);
		waitclientlist.push_back(socket);
	}

	t1.join();
}


int main()
{
	Monster::logger = std::make_shared<Monster::SSpdLog>("spdlog", "./logs/spdlog.log");
	Monster::init_global();
	Monster::logger->set_level(spdlog::level::trace);
	Monster::logger->set_is_sink(true);

	test_socket_client();
	LOG_FMT_DEBUG("this is a log test");

	Monster::final_global();
	return 0;
}
