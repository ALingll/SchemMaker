#pragma once

#include <exception>
#include <string>

namespace NBT {
	class NBT_Exception :public std::exception {
	public:                               
		NBT_Exception(std::string str) :exception(str.c_str()) {}

	};
}

