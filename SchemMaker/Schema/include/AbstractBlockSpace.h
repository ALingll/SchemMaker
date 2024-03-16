#pragma once

#include <vector>

namespace Schema {
	template<typename T>
	class AbstractBlockSpace{

	private:
		std::vector<T> _block_space;
		unsigned short _width;
		unsigned short _height;
		unsigned short _lenth;

	public:
		AbstractBlockSpace(const std::vector<T>& block_space, unsigned short width, unsigned short height, unsigned short lenth) :
			_block_space(block_space), _width(width), _height(height), _lenth(lenth) {}

		AbstractBlockSpace(std::vector<T>&& block_space, unsigned short width, unsigned short height, unsigned short lenth) :
			_block_space(std::move(block_space)), _width(width), _height(height), _lenth(lenth) {}

		bool is_legal() { return _width * _height * _lenth == _block_space.size(); }
	};
}



