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
			_data(std::move(block_space)), _shape({ width,height,lenth }) {}

		bool is_legal() { return _shape.width * _shape.height * _shape.lenth == _data.size(); }

		T& get(Pos p) { return _data[p.x + p.z * _shape.width + p.y * _shape.width * _shape.length]; }
		T& get(int x, int y, int z) { return _data[x + z * _shape.width + y * _shape.width * _shape.length]; }
		T& operator[](Pos p) { return _data[p.x + p.z * _shape.width + p.y * _shape.width * _shape.length]; }
		T& operator[](int x, int y, int z) { return _data[x + z * _shape.width + y * _shape.width * _shape.length]; }

		Shape report_shape() { return _shape; }

	};
}



