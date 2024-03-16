#include "pch.h"
#include "CppUnitTest.h"

#include "../SchemMaker/NBT/include/NBT_Value.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace NBT;
using tag = NBT::NBT_Value::tag;

namespace UnitTestNBT
{

	TEST_CLASS(UnitTestNBT)
	{
	public:
		
		TEST_METHOD(Test_Zlib)
		{
			Assert::IsNotNull(ZLIB_VERSION);
		}

		TEST_METHOD(Test_Init_NBT)
		{
			NBT_Value nbt_end;
			Assert::AreEqual((int)nbt_end.get_tag(), (int)tag::TAG_End);

			NBT_Value nbt_byte_1(1_b);
			NBT_Value nbt_byte_2((int8_t)20);
			NBT_Value nbt_byte_3((NBT::Byte)20); //等价于上一行
			Assert::AreEqual((int)nbt_byte_1.get_tag(), (int)tag::TAG_Byte);
			Assert::AreEqual((int)nbt_byte_2.get_tag(), (int)tag::TAG_Byte);

			NBT_Value nbt_short_1(1_s);
			NBT_Value nbt_short_2((int16_t)20);
			NBT_Value nbt_short_3((NBT::Short)20); //等价于上一行
			Assert::AreEqual((int)nbt_short_1.get_tag(), (int)tag::TAG_Short);
			Assert::AreEqual((int)nbt_short_2.get_tag(), (int)tag::TAG_Short);

			NBT_Value nbt_int_1(1_i);
			NBT_Value nbt_int_2((int32_t)20);
			NBT_Value nbt_int_3((NBT::Int)20); //等价于上一行
			Assert::AreEqual((int)nbt_int_1.get_tag(), (int)tag::TAG_Int);
			Assert::AreEqual((int)nbt_int_2.get_tag(), (int)tag::TAG_Int);
			NBT_Value nbt_int_4(20);	//OK But Bad, 默认推断为int，其宽度取决于编译器实现

			NBT_Value nbt_long_1(1_l);
			NBT_Value nbt_long_2((int64_t)20);
			NBT_Value nbt_long_3((NBT::Long)20); //等价于上一行
			Assert::AreEqual((int)nbt_long_1.get_tag(), (int)tag::TAG_Long);
			Assert::AreEqual((int)nbt_long_2.get_tag(), (int)tag::TAG_Long);

			NBT_Value nbt_float_1_1(1_f), nbt_float_1_2(1.5_f);
			NBT_Value nbt_float_2((float)20);
			NBT_Value nbt_float_3((NBT::Float)20); //等价于上一行
			Assert::AreEqual((int)nbt_float_1_1.get_tag(), (int)tag::TAG_Float);
			Assert::AreEqual((int)nbt_float_2.get_tag(), (int)tag::TAG_Float);

			NBT_Value nbt_double_1_1(1_d), nbt_double_1_2(1.5_d);
			NBT_Value nbt_double_2((double)20);
			NBT_Value nbt_double_2_2(20.0); //默认推断为double
			NBT_Value nbt_double_3((NBT::Double)20); //等价于上一行
			Assert::AreEqual((int)nbt_double_1_1.get_tag(), (int)tag::TAG_Double);
			Assert::AreEqual((int)nbt_double_2.get_tag(), (int)tag::TAG_Double);

			NBT_Value nbt_byte_array{ 1_b,2_b,3_b };
			Assert::AreEqual((int)nbt_byte_array.get_tag(), (int)tag::TAG_Byte_Array);
			Assert::AreEqual((int)nbt_byte_array.get_element_tag(), (int)tag::TAG_Byte);

			NBT_Value nbt_string_1("atring");
			std::string s("aaaa");
			NBT_Value nbt_string_2(s);
			Assert::AreEqual((int)nbt_string_1.get_tag(), (int)tag::TAG_String);
			Assert::AreEqual((int)nbt_string_2.get_tag(), (int)tag::TAG_String);

			NBT_Value nbt_int_array_1{ 1_i,2_i,3_i };
			NBT_Value nbt_int_array_2{ 10,20,30 }; //默认推断为Int
			Assert::AreEqual((int)nbt_int_array_1.get_tag(), (int)tag::TAG_Int_Array);
			Assert::AreEqual((int)nbt_int_array_1.get_element_tag(), (int)tag::TAG_Int);
			Assert::AreEqual((int)nbt_int_array_2.get_tag(), (int)tag::TAG_Int_Array);
			Assert::AreEqual((int)nbt_int_array_2.get_element_tag(), (int)tag::TAG_Int);

			NBT_Value nbt_long_array{ 1_l,2_l,3_l };
			Assert::AreEqual((int)nbt_long_array.get_tag(), (int)tag::TAG_Long_Array);
			Assert::AreEqual((int)nbt_long_array.get_element_tag(), (int)tag::TAG_Long);
		}

		TEST_METHOD(Test_Init_List) {

		}

		TEST_METHOD(Test_Init_Compound) {

		}

	};
}
