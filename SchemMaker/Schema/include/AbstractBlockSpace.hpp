#pragma once

#include <vector>

namespace Schema {
	template<typename T>
	class AbstractBlockSpace{
	public:
		struct Shape {
			unsigned short width;
			unsigned short height;
			unsigned short lenth;
		};
		struct Pos {
			unsigned short x;
			unsigned short y;
			unsigned short z;
		};
		
	private:
		std::vector<T> _data;
		Shape _shape;

	public:
		AbstractBlockSpace(const std::vector<T>& block_space, unsigned short width, unsigned short height, unsigned short lenth) :
			_data(block_space), _shape({width,height,lenth}) {}

		AbstractBlockSpace(std::vector<T>&& block_space, unsigned short width, unsigned short height, unsigned short lenth) :
			_data(std::move(block_space)), _width(width), _height(height), _lenth(lenth) {}

		bool is_legal() { return _shape.width * _shape.height * _shape.lenth == _data.size(); }
	};
}



