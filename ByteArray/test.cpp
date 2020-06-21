/**
 * @file	test.cpp
 * @brief	
 * @author	My name is SunWu
 * @version $Id$
 * @date	2020-05-28
 */

#include <stdio.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <memory>
#include <unistd.h>

#include <map>
#include <vector>
#include <list>
#include <sys/epoll.h>

#include "monster/ringbuffer.h"
#include "monster/log.h"
#include "monster/monster.h"
#include "monster/socket.h"

static bool isFinal = false;

void mwrite(std::shared_ptr<Monster::RingBuffer> mq, int i)
{
	char buf[1024] = {};
	//std::cout<<"ptr usecount: "<<mq.use_count()<<std::endl;
	sprintf(buf, "%s : %d", "hello this is my bytearray",i);
	while(!isFinal) {
		mq->write(buf, strlen(buf));
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void mread(std::shared_ptr<Monster::RingBuffer> mq)
{
	while(!isFinal) {
		char rcvb[1024] = {};
		mq->read(rcvb, mq->get_size());
		std::cout<<rcvb<<std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void test_bytearray()
{
	auto mq = std::make_shared<Monster::RingBuffer>(10);
	std::vector<std::thread> tvec;
	tvec.resize(1);
	for (int i = 0; i < 3; i++) {
		if (i == 2) {
			tvec[i] = std::thread(mread, mq);
		} else {
			tvec[i] = std::thread(mwrite, mq, i);
		}
	}

	//sleep(20);
	isFinal = true;

	for (auto& it : tvec) {
		it.join();
	}

	mwrite(mq,2);

	char rcvb[1024] = {};
	mq->read(rcvb, mq->get_size());
	std::cout<<rcvb<<std::endl;
	mwrite(mq,4);
	mwrite(mq,3);

	mq->clear();
}

std::list<Monster::Socket::ptr> waitclientlist;
std::mutex waitlock;

std::map<int, Monster::Socket::ptr> okclientlist;
struct EpollData
{
	int sock = -1;
};

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
					snprintf(buf, 1024*4, "%s", "server hello this is a send");
					sockptr->send(buf, 1024*4);
					LOG_FMT_DEBUG("send msg : {}", buf);
				}
			}
		}
		
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void test_socket_server()
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

	//bind
	if (!socket->bind(address)) {
		LOG_FMT_ERROR("test_socket: bind socket error");
		return;
	}

	//listen
	if (!socket->listen()) {
		LOG_FMT_ERROR("test_socket: listen socket error");
		return;
	}
	
	auto epfd = epoll_create(1);
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = nullptr;
	assert(0 == epoll_ctl(epfd, EPOLL_CTL_ADD, socket->get_socket(), &ev));

	auto t1 = std::thread(handle_client);

	//accept
	while(true) {
		struct epoll_event ev2;
		int ret = epoll_wait(epfd, &ev2, 1, 2100);
		if (ret == 1 && ev2.events & EPOLLIN) {
			auto accsock = socket->accept();
			if (accsock) {
				std::lock_guard<std::mutex> tlock(waitlock);
				waitclientlist.push_back(accsock);
			}
		}
	}
	t1.join();
}

int main()
{
	Monster::logger = std::make_shared<Monster::SSpdLog>("ringbuff", "./logs/ringbuff.log");
	Monster::init_global();
	Monster::logger->set_level(spdlog::level::trace);
	Monster::logger->set_is_sink(true);

	//测试字节流缓冲区
	//test_bytearray();
	test_socket_server();

	//Monster::error(Monster::logger, "hello this is my log test:{}\n", 5);
	LOG_FMT_ERROR("hello this is my log test:");


	Monster::final_global();

	return 0;
}
