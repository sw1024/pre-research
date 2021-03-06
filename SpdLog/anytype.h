/**
 * @file	anytype.h
 * @brief	
 * @author	My name is SunWu
 * @version $Id$
 * @date	2020-05-25
 */

#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <any>

namespace Monster
{

class AnyType {

public:

	AnyType() {}

	~AnyType() {
		clear();
	}

	AnyType(AnyType const& p) {

		clear();
		free = p.free;
		copy = p.copy;
		type_id = p.type_id;
		if (p.type_id) ptr = p.copy(p.ptr);

	}

	AnyType(char const* i) {

		clear();
		std::string* p = new std::string(i);
		ptr = static_cast<void*>(p);
		type_id = get_type_id<std::string>();
		free = [](void* ptr) {
			delete static_cast<std::string*>(ptr);
		};
		copy = [](void* ptr) -> void* {
			return static_cast<void*>(new std::string(*static_cast<std::string*>(ptr)));
		};

	}

	template <typename T>
	AnyType(T* p) {

		clear();
		ptr = static_cast<void*>(p);
		type_id = get_type_id<T>();
		free = [](void* ptr) {
			delete static_cast<T*>(ptr);
		};
		copy = [](void* ptr) -> void* {
			return static_cast<void*>(new T(*static_cast<T*>(ptr)));
		};
	}


	template <typename T>
	explicit AnyType(T const& i) {

		clear();
		T* p = new T(i);
		ptr = static_cast<void*>(p);
		type_id = get_type_id<T>();
		free = [](void* ptr) {
			delete static_cast<T*>(ptr);
		};
		copy = [](void* ptr) -> void* {
			return static_cast<void*>(new T(*static_cast<T*>(ptr)));
		};
	}

	AnyType& operator=(AnyType const& p) {

		clear();
		free = p.free;
		copy = p.copy;
		type_id = p.type_id;
		if (p.type_id) ptr = p.copy(p.ptr);
		return *this;

	}

	template <typename T>
	AnyType& operator=(T const& p) {
		set(p);
		return *this;
	}

	template <typename T, typename Callable>
	AnyType& match(Callable const& func) {

		if (type_id == get_type_id<T>()) {

			func(*static_cast<T*>(ptr));

		}

		return *this;

	}


	template <typename T>
	bool match() {

		return type_id == get_type_id<T>();

	}

	template <typename T>
	void set(T const& i) {

		clear();
		T* p = new T(i);
		ptr = static_cast<void*>(p);
		type_id = get_type_id<T>();
		free = [](void* ptr) {
			delete static_cast<T*>(ptr);
		};
		copy = [](void* ptr) -> void* {
			return static_cast<void*>(new T(*static_cast<T*>(ptr)));
		};

	}

	void set(char const* i) {

		clear();
		std::string* p = new std::string(i);
		ptr = static_cast<void*>(p);
		type_id = get_type_id<std::string>();
		free = [](void* ptr) {
			delete static_cast<std::string*>(ptr);
		};
		copy = [](void* ptr) -> void* {
			return static_cast<void*>(new std::string(*static_cast<std::string*>(ptr)));
		};

	}

	template <typename T>
	void set(T* p) {

		clear();
		ptr = static_cast<void*>(p);
		type_id = get_type_id<T>();
		free = [](void* ptr) {
			delete static_cast<T*>(ptr);
		};
		copy = [](void* ptr) -> void* {
			return static_cast<void*>(new T(*static_cast<T*>(ptr)));
		};

	}

	template <typename T>
	T& value() const {
		if (type_id == get_type_id<T>()) {
			return *static_cast<T*>(ptr);
		}
		else {
			throw std::runtime_error("Invalid AnyType value.");
		}
	}

	void clear() {

		if (type_id && ptr != nullptr) {
			free(ptr);
			ptr = nullptr;
		}
		type_id = 0;

	}
	
	bool null() {
		return !type_id;
	}

private:

	long long int type_id = 0;

	void* ptr;
	void(*free)(void*);
	void* (*copy)(void*);

	template <typename T>
	static long long int get_type_id() {

		static const char addr;

		return reinterpret_cast<long long int>(&addr);

	}

};

};
