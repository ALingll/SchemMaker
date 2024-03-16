#pragma once

#include <vector>

namespace Schema {
	template<typename T>
	class AbstractBlockSpace{
	public:
		struct Pos {
			unsigned short x;
			unsigned short z;
			unsigned short y;
 		};

	private:
		std::vector<T> _data;
		unsigned short _width;
		unsigned short _height;
		unsigned short _lenth;

	public:
		AbstractBlockSpace(const std::vector<T>& block_space, unsigned short width, unsigned short height, unsigned short lenth) :
			_data(block_space), _width(width), _height(height), _lenth(lenth) {}

		AbstractBlockSpace(std::vector<T>&& block_space, unsigned short width, unsigned short height, unsigned short lenth) :
			_data(std::move(block_space)), _width(width), _height(height), _lenth(lenth) {}

		bool is_legal() { return _width * _height * _lenth == _data.size(); }

		AbstractBlockSpace split(Pos, Pos) {

		}

		T& operator[](int x, int z, int y) {
			return _data[x + z * Width + y * Width * Length];
		}

		T& get(Pos p) {
			return _data[p.x + p.z * Width + p.y * Width * Length];
		}

		auto report_shape() {
			return struct {
				unsigned short width;
				unsigned short height;
				unsigned short lenth;
			}shape{ _width,_height,_lenth };
		}

	};
}



