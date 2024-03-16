#include "NBT_Value.h"

#include <functional>
#include <string>
#include <algorithm>

namespace NBT {

	std::partial_ordering NBT::operator<=>(const NBT_Value& v1, const NBT_Value& v2) { return v1._value <=> v2._value; }
	auto NBT::operator==(const NBT_Value& v1, const NBT_Value& v2) { return v1._value == v2._value; }

	tag_builder operator ""_tag(const char* v, size_t n) { return tag_builder(v); }
	NBT_Value::byte_array_visitor operator ""_B(unsigned long long v) { return { (int16_t)v }; }
	NBT_Value::int_array_visitor operator ""_I(unsigned long long v) { return { (int16_t)v }; }
	NBT_Value::long_array_visitor operator ""_L(unsigned long long v) { return { (int16_t)v }; }

	NBT::NBT_Value::NBT_Value(std::initializer_list<std::pair<std::string, NBT_Value>> ils)
	{
		Compound cmp;
		for (auto& pair : ils) {
			cmp[pair.first] = pair.second;
		}
		_value = std::move(cmp);
	}

	std::string NBT::NBT_Value::to_string() const
	{
		struct {
			std::string operator()(const End v) {
				return "";
			};
			std::string operator()(const Byte v) {
				return std::to_string(v) + "B";
			};
			std::string operator()(const Short v) {
				return std::to_string(v) + "S";
			};
			std::string operator()(const Int v) {
				return std::to_string(v);
			};
			std::string operator()(const Long v) {
				return std::to_string(v) + "L";
			};
			std::string operator()(const Float v) {
				return std::to_string(v) + "F";
			};
			std::string operator()(const Double v) {
				return std::to_string(v);
			};
			std::string operator()(const Byte_Array v) {
				std::string r{ "[B;" };
				if (!v.empty()) {
					r += std::to_string(v[0]) + "B";
					std::for_each(++v.begin(), v.end(),
						[&r](const Byte& a) {
							r += ",";
							r += std::to_string(a) + "B";
						}
					);
				}
				return r + "]";
			};
			std::string operator()(const String v) {
				return v;
			};
			std::string operator()(const List v) {
				std::string r{ "[" };
				if (!v.empty()) {
					r += v[0].to_string();
					std::for_each(++v.begin(), v.end(),
						[&r](const NBT_Value& a) {
							r += ",";
							r += a.to_string();
						}
					);
				}
				return r + "]";
			};
			std::string operator()(const Compound v) {
				std::string r{ "{" };
				r += (*v.begin()).first + ":";
				r += (*v.begin()).second.to_string();
				std::for_each(++v.begin(), v.end(),
					[&r](const std::pair<std::string, NBT_Value>& a) {
						r += ",\n";
						r += a.first + ":";
						r += a.second.to_string();
					}
				);
				return r + "}";
			};
			std::string operator()(const Int_Array v) {
				std::string r{ "[I;" };
				r += std::to_string(v[0]);
				std::for_each(++v.begin(), v.end(),
					[&r](const Int& a) {
						r += ",";
						r += std::to_string(a);
					}
				);
				return r + "]";
			};
			std::string operator()(const Long_Array v) {
				std::string r{ "[L;" };
				r += std::to_string(v[0]) + "L";
				std::for_each(++v.begin(), v.end(),
					[&r](const Long& a) {
						r += ",";
						r += std::to_string(a) + "L";
					}
				);
				return r + "]";
			};
		}str_make_visitor;
		return std::visit(str_make_visitor, _value);
	}

	NBT_Value::tag NBT_Value::get_tag() const {
		struct {
			tag operator()(End			) { return tag::TAG_End;		}
			tag operator()(Byte			) { return tag::TAG_Byte;		}
			tag operator()(Short		) { return tag::TAG_Short;		}
			tag operator()(Int			) { return tag::TAG_Int;		}
			tag operator()(Long			) { return tag::TAG_Long;		}
			tag operator()(Float		) { return tag::TAG_Float;		}
			tag operator()(Double		) { return tag::TAG_Double;		}
			tag operator()(Byte_Array	) { return tag::TAG_Byte_Array; }
			tag operator()(String		) { return tag::TAG_String;		}
			tag operator()(List			) { return tag::TAG_List;		}
			tag operator()(Compound		) { return tag::TAG_Compound;	}
			tag operator()(Int_Array	) { return tag::TAG_Int_Array;	}
			tag operator()(Long_Array	) { return tag::TAG_Long_Array; }
		}get_tag_visitor;
		return std::visit(get_tag_visitor, _value);
	}

	NBT_Value::tag NBT_Value::get_element_tag() const {
		struct {
			tag current_tag;
			tag operator()(End			) { return current_tag;			}
			tag operator()(Byte			) { return current_tag;			}
			tag operator()(Short		) { return current_tag;			}
			tag operator()(Int			) { return current_tag;			}
			tag operator()(Long			) { return current_tag;			}
			tag operator()(Float		) { return current_tag;			}
			tag operator()(Double		) { return current_tag;			}
			tag operator()(Byte_Array	) { return tag::TAG_Byte;		}
			tag operator()(String		) { return tag::TAG_String;		}
			tag operator()(List	l		) {
				return l.empty() ? l[0].get_tag() : current_tag; 
			}
			tag operator()(Compound		) { return tag::TAG_Compound;	}
			tag operator()(Int_Array	) { return tag::TAG_Int;		}
			tag operator()(Long_Array	) { return tag::TAG_Long;		}
		}get_tag_visitor;
		get_tag_visitor.current_tag = get_tag();
		return std::visit(get_tag_visitor, _value);
	}

	NBT_Value& NBT_Value::add_tag(std::string s, NBT_Value v)
	{
		auto& cmp = std::get<Compound>(_value);
		cmp[s] = v;
		return *this;
	}

	NBT_Value& NBT_Value::operator[](std::string s)
	{
		if (get_tag() != tag::TAG_Compound)
			throw NBT_Exception("Bad Visit: *this is not a Compound");
		return std::get<Compound>(_value)[s];
	}

	NBT_Value& NBT_Value::operator[](int i)
	{
		if (get_tag() != tag::TAG_List)
			throw NBT_Exception("Bad Visit: *this is not a List");
		return std::get<List>(_value)[i];
	}

	Byte& NBT_Value::operator[](byte_array_visitor i)
	{
		if (get_tag() != tag::TAG_Byte_Array)
			throw NBT_Exception("Bad Visit: *this is not a Byte_Array");
		return std::get<Byte_Array>(_value)[i.index];
	}

	Int& NBT_Value::operator[](int_array_visitor i)
	{
		if (get_tag() != tag::TAG_Int_Array)
			throw NBT_Exception("Bad Visit: *this is not a Int_Array");
		return std::get<Int_Array>(_value)[i.index];
	}

	Long& NBT_Value::operator[](long_array_visitor i)
	{
		if (get_tag() != tag::TAG_Long_Array)
			throw NBT_Exception("Bad Visit: *this is not a Long_Array");
		return std::get<Long_Array>(_value)[i.index];
	}

	std::ifstream& operator>>(std::ifstream& in, NBT_Value& v)
	{
		auto s = std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
		NBT_Value temp_v;

		if(v.if_use_gz())
			s = decompressString(s);

		std::istringstream is(s, std::ios::binary);
		is >> std::noskipws;
		auto current_tag = NBT_Value::get_binary_tag(is);
		std::string current_name{""};
		switch (current_tag)
		{
		case NBT_Value::tag::TAG_Compound: {
			auto temp_cmp = Compound();
			auto temp_name = NBT_Value::get_binary_string(is);
			temp_cmp[temp_name] = NBT_Value::get_binary_compound(is);
			temp_v = std::move(temp_cmp);
			break;
		}
		default:
			break;
		}

		v = std::move(temp_v);

		return in;
	}

	std::ofstream& operator<<(std::ofstream& out, NBT_Value& v) {
		std::ostringstream os(std::ios::binary);
		NBT_Value::put_binary_data(os, v);

		auto s = os.str();

		if (v.if_use_gz())
			s = compressString(s);

		out << s;

		return out;
	}

	std::string decompressString(const std::string& compressed_data) {
		z_stream strm{};
		std::string uncompressed_data;

		// 初始化z_stream结构
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;

		// 初始化解压缩
		int ret = inflateInit2(&strm, 16 + MAX_WBITS); // 16表示自动检测gzip或者zlib格式
		if (ret != Z_OK) {
			return "";
		}

		// 设置输入
		strm.avail_in = compressed_data.size();
		strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_data.data()));

		// 解压缩循环
		do {
			char outbuffer[1024]{};
			strm.avail_out = sizeof(outbuffer);
			strm.next_out = reinterpret_cast<Bytef*>(outbuffer);

			// 解压缩数据
			ret = inflate(&strm, Z_NO_FLUSH);
			if (ret < 0) {
				inflateEnd(&strm);
				return "";
			}

			// 将解压后的数据拷贝到uncompressed_data中
			uncompressed_data.append(outbuffer, sizeof(outbuffer) - strm.avail_out);
		} while (strm.avail_out == 0);

		// 结束解压缩
		inflateEnd(&strm);

		return uncompressed_data;
	}

	std::string compressString(const std::string& uncompressed_data) {
		z_stream strm;
		std::string compressed_data;

		// 初始化z_stream结构
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;

		// 初始化压缩
		int ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
		if (ret != Z_OK) {
			return "";
		}

		// 设置输入
		strm.avail_in = uncompressed_data.size();
		strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(uncompressed_data.data()));

		// 压缩循环
		do {
			char outbuffer[1024];
			strm.avail_out = sizeof(outbuffer);
			strm.next_out = reinterpret_cast<Bytef*>(outbuffer);

			// 压缩数据
			ret = deflate(&strm, Z_FINISH);
			if (ret < 0) {
				deflateEnd(&strm);
				return "";
			}

			// 将压缩后的数据拷贝到compressed_data中
			compressed_data.append(outbuffer, sizeof(outbuffer) - strm.avail_out);
		} while (strm.avail_out == 0);

		// 结束压缩
		deflateEnd(&strm);

		return compressed_data;
	}

}