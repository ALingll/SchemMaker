#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <map>
#include <type_traits>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <algorithm>
#include <functional>
#include <optional>

#include <zlib.h>

#include "NBT_Exception.h"

#define LIST NBT_Value
#define CMP NBT_Value


namespace NBT {

	constexpr auto NBT_BY_000_Ver = "Alpha 0.2";

	std::string decompressString(const std::string&);
	std::string compressString(const std::string&);

	extern class NBT_Value;

	using End		 = std::monostate;
	using Byte		 = int8_t;
	using Short		 = int16_t;
	using Int		 = int32_t;
	using Long		 = int64_t;
	using Float		 = float;
	using Double	 = double;
	using Byte_Array = std::vector<Byte>;
	using String	 = std::string;
	using List		 = std::vector<NBT_Value>;
	using Compound	 = std::map<String, NBT_Value>;
	using Int_Array	 = std::vector<Int>;
	using Long_Array = std::vector<Long>;

	template<typename T>
	concept NBT_Type =
		std::is_same_v<T, End		> ||
		std::is_same_v<T, Byte		> ||
		std::is_same_v<T, Short		> ||
		std::is_same_v<T, Int		> ||
		std::is_same_v<T, Long		> ||
		std::is_same_v<T, Float		> ||
		std::is_same_v<T, Double	> ||
		std::is_same_v<T, Byte_Array> ||
		std::is_same_v<T, String	> ||
		std::is_same_v<T, List		> ||
		std::is_same_v<T, Compound	> ||
		std::is_same_v<T, Int_Array	> ||
		std::is_same_v<T, Long_Array>;

	template<typename T>
	concept NBT_Not_Null =
		!std::is_same_v<T, End>;

	template<typename T>
	concept NBT_Used_Type =
		std::is_integral_v<T> ||
		std::is_floating_point_v<T> ||
		std::is_same_v<T, const char*>;

	template<typename T>
	concept NBT_Surpported_Type =
		NBT_Type<T> ||
		NBT_Used_Type<T>;

	class NBT_Value {
	public:

		friend std::ifstream& operator>>(std::ifstream&, NBT_Value&);
		friend std::ofstream& operator<<(std::ofstream&, NBT_Value&);
		friend std::partial_ordering operator<=>(const NBT_Value&, const NBT_Value&);
		friend auto operator==(const NBT_Value&, const NBT_Value&);

		struct byte_array_visitor {
			int16_t index;
		};
		struct int_array_visitor {
			int16_t index;
		};
		struct long_array_visitor {
			int16_t index;
		};

		enum class tag {
			TAG_End			 = 0x00,
			TAG_Byte		 = 0x01,
			TAG_Short		 = 0x02,
			TAG_Int			 = 0x03,
			TAG_Long		 = 0x04,
			TAG_Float		 = 0x05,
			TAG_Double		 = 0x06,
			TAG_Byte_Array	 = 0x07,
			TAG_String		 = 0x08,
			TAG_List		 = 0x09,
			TAG_Compound	 = 0x0a,
			TAG_Int_Array	 = 0x0b,
			TAG_Long_Array	 = 0x0c
		};

		static std::string tag_string(tag t);

		tag get_tag() const;
		tag get_element_tag() const;

		static constexpr int use_gz = 0x0001;
		static constexpr int use_zip = 0x0002;

		static constexpr int snbt_str = 0x0010;
		static constexpr int json_str = 0x0020;
		static constexpr int tree_str = 0x0040;

	private:
		std::variant<
			End, Byte, Short,
			Int, Long, Float,
			Double, Byte_Array, String,
			List, Compound, Int_Array,
			Long_Array
		> _value;

		int _state;
		tag _list_type;
		std::optional<tag> _should_be_tag;

		NBT_Value& set_list_type(tag type) { _list_type = type; return *this; }

		NBT_Value& set_should_be_tag(tag type) { _should_be_tag = type; return *this; }

		void check_should_be() const { 
			if (_should_be_tag.has_value() ? get_tag() != _should_be_tag.value() : false) {
				throw NBT_Exception(
					String("Bad type:") + "Assign " + tag_string(get_tag()) + 
					"to a variable that should be " + tag_string(_should_be_tag.value())
				);
			}
		}

		void check_should_be(tag t) const {
			if (_should_be_tag.has_value() ? t != _should_be_tag.value() : false) {
				throw NBT_Exception(
					String("Bad type:") + "Assign " + tag_string(t) +
					"to a variable that should be " + tag_string(_should_be_tag.value())
				);
			}
		}

#pragma region get_binary_data

		static tag get_binary_tag(std::istringstream& is) {
			unsigned char v;
			is >> v;
			return (tag)v;
		}

		static Byte get_binary_byte(std::istringstream& is) {
			unsigned char bytes[1]{};
			is >> bytes[0];
			Byte v = static_cast<Byte>(bytes[0]);
			return v;
		}

		static Short get_binary_short(std::istringstream& is) {
			unsigned char bytes[2]{};
			is >> bytes[1] >> bytes[0];
			Short v = 
				(static_cast<Short>(bytes[1]) << 8) |
				static_cast<Short>(bytes[0]);
			return v;
		}

		static Int get_binary_int(std::istringstream& is) {
			unsigned char bytes[4]{};
			is >> bytes[3] >> bytes[2] >> bytes[1] >> bytes[0];
			Int v =
				(static_cast<Int>(bytes[3]) << 24) |
				(static_cast<Int>(bytes[2]) << 16) |
				(static_cast<Int>(bytes[1]) << 8) |
				static_cast<Int>(bytes[0]);
			return v;
		}

		static Long get_binary_long(std::istringstream& is) {
			unsigned char bytes[8]{};
			is >> bytes[7] >> bytes[6] >> bytes[5] >> bytes[4]
				>> bytes[3] >> bytes[2] >> bytes[1] >> bytes[0];
			Long v =
				(static_cast<Long>(bytes[7]) << 56) |
				(static_cast<Long>(bytes[6]) << 48) |
				(static_cast<Long>(bytes[5]) << 40) |
				(static_cast<Long>(bytes[4]) << 32) |
				(static_cast<Long>(bytes[3]) << 24) |
				(static_cast<Long>(bytes[2]) << 16) |
				(static_cast<Long>(bytes[1]) << 8) |
				static_cast<Long>(bytes[0]);
			return v;
		}

		static Float get_binary_float(std::istringstream& is) {
			unsigned char bytes[4]{};
			is >> bytes[3] >> bytes[2] >> bytes[1] >> bytes[0];
			uint32_t integer_value =
				(static_cast<uint32_t>(bytes[3]) << 24) |
				(static_cast<uint32_t>(bytes[2]) << 16) |
				(static_cast<uint32_t>(bytes[1]) << 8) |
				static_cast<uint32_t>(bytes[0]);
			Float v;
			std::memcpy(&v, &integer_value, sizeof(v));
			return v;
		}

		static Double get_binary_double(std::istringstream& is) {
			unsigned char bytes[8]{};
			is >> bytes[7] >> bytes[6] >> bytes[5] >> bytes[4]
				>> bytes[3] >> bytes[2] >> bytes[1] >> bytes[0];
			uint64_t integer_value =
				(static_cast<uint64_t>(bytes[7]) << 56) |
				(static_cast<uint64_t>(bytes[6]) << 48) |
				(static_cast<uint64_t>(bytes[5]) << 40) |
				(static_cast<uint64_t>(bytes[4]) << 32) |
				(static_cast<uint64_t>(bytes[3]) << 24) |
				(static_cast<uint64_t>(bytes[2]) << 16) |
				(static_cast<uint64_t>(bytes[1]) << 8) |
				static_cast<uint64_t>(bytes[0]);
			Double v;
			std::memcpy(&v, &integer_value, sizeof(v));
			return v;
		}

		static Byte_Array get_binary_byte_array(std::istringstream& is) {
			Byte_Array v;
			for (auto len = get_binary_int(is); len > 0; len--) {
				v.emplace_back(get_binary_byte(is));
			}
			return v;
		}

		static String get_binary_string(std::istringstream& is) {

			Short len = get_binary_short(is);
			char* c = new char[len + 1];
			c[len] = '\0';
			is.read(c, len);
			return c;
		}

		static NBT_Value get_binary_list(std::istringstream& is) {
			auto current_tag = get_binary_tag(is);
			auto len = get_binary_int(is);
			List v(len);
			switch (current_tag)
			{
			case NBT::NBT_Value::tag::TAG_End:
				break;
			case NBT::NBT_Value::tag::TAG_Byte:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_byte(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Short:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_short(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Int:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_int(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Long:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_long(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Float:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_float(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Double:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_double(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Byte_Array:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_byte_array(is); });
				break;
			case NBT::NBT_Value::tag::TAG_String:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_string(is); });
				break;
			case NBT::NBT_Value::tag::TAG_List:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_list(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Compound:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_compound(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Int_Array:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_int_array(is); });
				break;
			case NBT::NBT_Value::tag::TAG_Long_Array:
				std::for_each(v.begin(), v.end(), [&](NBT_Value& e) {e = get_binary_long_array(is); });
				break;
			default:
				break;
			}
			std::for_each(v.begin(), v.end(), [&](auto& _) {_.set_should_be_tag(current_tag); });
			return ((NBT_Value)std::move(v)).set_list_type(current_tag);
		}

		static Compound get_binary_compound(std::istringstream& is) {
			Compound v;
			bool end_flag = false;
			while (!end_flag) {
				switch (get_binary_tag(is)) {
				case NBT::NBT_Value::tag::TAG_Byte: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_byte(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Short: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_short(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Int: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_int(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Long: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_long(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Float: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_float(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Double: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_double(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Byte_Array: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_byte_array(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_String: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_string(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_List: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_list(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Compound: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_compound(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Int_Array: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_int_array(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_Long_Array: {
					auto current_name = get_binary_string(is);
					v[current_name] = get_binary_long_array(is);
					break;
				}
				case NBT::NBT_Value::tag::TAG_End: default: {
					end_flag = true;
					break;
				}
				}
			}
			return v;
		}

		static Int_Array get_binary_int_array(std::istringstream& is) {
			Int_Array v;
			for (auto len = get_binary_int(is); len > 0; len--) {
				v.emplace_back(get_binary_int(is));
			}
			return v;
		}

		static Long_Array get_binary_long_array(std::istringstream& is) {
			Long_Array v;
			for (auto len = get_binary_int(is); len > 0; len--) {
				v.emplace_back(get_binary_long(is));
			}
			return v;
		}

#pragma endregion

#pragma region put_binary_data

		static void put_binary_data(std::ostringstream& os, const NBT_Value& v) {
			struct {
				void put_binary_tag(std::ostringstream& os, tag _) {
					os << (unsigned char)_;
				}
				void operator()(std::ostringstream& os, const End& v, tag list_tag) {
					put_binary_tag(os, tag::TAG_End);
				}
				void operator()(std::ostringstream& os, const Byte& v, tag list_tag) {
					const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
					os << bytes[0];
				}
				void operator()(std::ostringstream& os, const Short& v, tag list_tag) {
					const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
					os << bytes[1] << bytes[0];
				}
				void operator()(std::ostringstream& os, const Int& v, tag list_tag) {
					const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
					os << bytes[3] << bytes[2] << bytes[1] << bytes[0];
				}
				void operator()(std::ostringstream& os, const Long& v, tag list_tag) {
					const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
					os << bytes[7] << bytes[6] << bytes[5] << bytes[4]
						<< bytes[3] << bytes[2] << bytes[1] << bytes[0];
				}
				void operator()(std::ostringstream& os, const Float& v, tag list_tag) {
					const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
					os << bytes[3] << bytes[2] << bytes[1] << bytes[0];
				}
				void operator()(std::ostringstream& os, const Double& v, tag list_tag) {
					const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
					os << bytes[7] << bytes[6] << bytes[5] << bytes[4]
						<< bytes[3] << bytes[2] << bytes[1] << bytes[0];
				}
				void operator()(std::ostringstream& os, const Byte_Array& v, tag list_tag) {
					this->operator()(os, (Int)v.size(), list_tag);
					for (auto& _ : v)
						this->operator()(os, _, list_tag);
				}
				void operator()(std::ostringstream& os, const String& v, tag list_tag) {
					uint16_t len = v.size();
					const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&len);
					os << bytes[1] << bytes[0];
					for (auto& _ : v)
						this->operator()(os, (Byte)_, list_tag);
				}
				void operator()(std::ostringstream& os, const List& v, tag list_tag) {
					put_binary_tag(os, list_tag);
					this->operator()(os, (Int)v.size(), list_tag);
					for (auto& _ : v)
						put_binary_data(os, _);
				}
				void operator()(std::ostringstream& os, const Compound& v, tag list_tag) {
					for (const auto& [tag, value] : v) {
						put_binary_tag(os, value.get_tag());
						put_binary_data(os, NBT_Value(tag));
						put_binary_data(os, value);
					}
					put_binary_tag(os, tag::TAG_End);
				}
				void operator()(std::ostringstream& os, const Int_Array& v, tag list_tag) {
					this->operator()(os, (Int)v.size(), list_tag);
					for (auto& _ : v)
						this->operator()(os, _, list_tag);
				}
				void operator()(std::ostringstream& os, const Long_Array& v, tag list_tag) {
					this->operator()(os, (Int)v.size(), list_tag);
					for (auto& _ : v)
						this->operator()(os, _, list_tag);
				}
			}put_binary_visitor;
			return std::visit([&](auto&& _) {
				return put_binary_visitor(os, _, v._list_type);
				}, v._value);
		}

#pragma endregion

	public:

		NBT_Value() :_state(0), _list_type(tag::TAG_End) {}

		template<NBT_Surpported_Type T>
		explicit NBT_Value(const T& value, int state = 0) : _value(value), _state(state) {
			_list_type = get_element_tag();
		}

		template<NBT_Surpported_Type T>
		explicit NBT_Value(T&& value, int state = 0) : _value(std::move(value)), _state(state), _list_type(tag::TAG_End) {
			_list_type = get_element_tag();
		}

		explicit NBT_Value(const char* s, int state = 0) :_value(s), _state(state), _list_type(tag::TAG_End) {}

		NBT_Value(std::initializer_list<std::pair<std::string, NBT_Value>>);

		template<typename T>
			requires NBT_Surpported_Type<T>	&& NBT_Not_Null<T>
		NBT_Value(std::initializer_list<T> il) {
			std::vector<NBT_Value> value(il.size());
			int i = 0;
			for (auto& v : il) {
				value[i] = v;
				i++;
			}
			_list_type = value.empty() ? tag::TAG_End : value[0].get_tag();
			std::for_each(value.begin(), value.end(), [&](auto& _) {_.set_should_be_tag(_list_type); });
			_value = std::move(value);
		}

		NBT_Value(std::initializer_list<const char*> il) {
			std::vector<NBT_Value> value;
			_list_type = tag::TAG_String;
			for (auto& v : il) {
				value.push_back(NBT_Value(v));
			}
			std::for_each(value.begin(), value.end(), [&](auto& _) {_.set_should_be_tag(_list_type); });
			_value = std::move(value);
		}

		NBT_Value(std::initializer_list<Byte> il) {
			std::vector<Byte> value;
			_list_type = tag::TAG_String;
			for (auto& v : il) {
				value.push_back(v);
			}
			_value = std::move(value);
		}

		NBT_Value(std::initializer_list<Int> il) {
			std::vector<Int> value;
			_list_type = tag::TAG_String;
			for (auto& v : il) {
				value.push_back(v);
			}
			_value = std::move(value);
		}

		NBT_Value(std::initializer_list<Long> il) {
			std::vector<Long> value;
			_list_type = tag::TAG_String;
			for (auto& v : il) {
				value.push_back(v);
			}
			_value = std::move(value);
		}

		NBT_Value(NBT_Value&& v) noexcept :
			_value(std::move(v._value)),_list_type(v._list_type),_should_be_tag(v._should_be_tag), _state(v._state) {}

		NBT_Value(const NBT_Value& v):
			_value(v._value), _list_type(v._list_type), _should_be_tag(v._should_be_tag), _state(v._state) {}

		template<typename T>
			requires  NBT_Surpported_Type<T>
		NBT_Value& operator=(const T& value) {
			_value = value;
			check_should_be();
			return *this;
		}

		template<typename T>
			requires  NBT_Surpported_Type<T>
		NBT_Value& operator=(T&& value) {
			_value = std::move(value);
			check_should_be();
			return *this;
		}

		NBT_Value& operator=(const char* value) {
			check_should_be(tag::TAG_String);
			_value = String(value);
			check_should_be();
			return *this;
		}

		NBT_Value& operator=(NBT_Value&& v) noexcept {
			check_should_be(v.get_tag());
			_value = std::move(v._value);
			_list_type = v._list_type;
			_should_be_tag = v._should_be_tag;
			return *this;
		}

		NBT_Value& operator=(const NBT_Value& v) {
			check_should_be(v.get_tag());
			_value = v._value;
			_list_type = v._list_type;
			_should_be_tag = v._should_be_tag;
			return *this;
		}

		template<NBT_Type T>
		T& get() {
			return std::get<T>(_value);
		}

		NBT_Value& set_state(const int state) { _state |= state; return *this; }

		NBT_Value& unset_state(const int state) { _state &= ~state; return *this; }

		std::string to_string() const;

		bool if_use_gz() const { return _state & use_gz; }

		NBT_Value& add_tag(std::string, NBT_Value);

		NBT_Value& operator[](std::string);

		NBT_Value& operator[](int);

		Byte& operator[](byte_array_visitor i);

		Int& operator[](int_array_visitor i);

		Long& operator[](long_array_visitor i);

	};

	class tag_builder :public std::string {
	public:
		tag_builder(std::string s) :std::string(s) {}

		template<class T>
			requires NBT_Surpported_Type<T> || std::convertible_to<T, NBT_Value> || std::is_same_v<T, const char *>
		friend std::pair<std::string, NBT_Value> operator<<(const tag_builder s, T t) {
			NBT_Value v = NBT_Value(t);
			return std::make_pair((std::string)s, v);
		}
	};

	std::ifstream& operator>>(std::ifstream&, NBT_Value&);

	NBT_Value::byte_array_visitor operator ""_B(unsigned long long v);
	NBT_Value::int_array_visitor operator ""_I(unsigned long long v);
	NBT_Value::long_array_visitor operator ""_L(unsigned long long v);

	constexpr Byte operator ""_b(unsigned long long v) { return Byte(v); }
	constexpr Short	operator ""_s(unsigned long long v) { return Short(v); }
	constexpr Int operator ""_i(unsigned long long v) { return Int(v); }
	constexpr Long operator ""_l(unsigned long long v) { return Long(v); }
	constexpr Float operator ""_f(long double v) { return Float(v); }
	constexpr Float operator ""_f(unsigned long long v) { return Float(v); }
	constexpr Double operator ""_d(long double v) { return Double(v); }
	constexpr Double operator ""_d(unsigned long long v) { return Double(v); }
	constexpr String operator ""_r(const char* v, size_t n) { return String(v); }

	tag_builder operator ""_tag(const char* v, size_t n);

}
