#include <iostream>
#include <zlib.h>

#include "NBT_Value.h"
#include <fstream>

using namespace NBT;
using namespace std;

int main() {
	time_t start, end;
	start = clock();
	for (auto i = 0; i < 10000; i++) {
		//读入文件，注意必须要使用二进制格式打开文件
		NBT_Value nbt{};
		ifstream fin("test/lupine_01.schem", ios::binary);
		fin >> nbt.set_state(NBT_Value::use_gz);
	}
	end = clock();

	std::cout << "DBSCAN time: " << (end - start) << " ms" << std::endl;

	
	//获取元素
	//auto& palette = nbt["Schematic"].get<Compound>()["Palette"].get<Compound>();
	//auto& azure_bluet = palette["minecraft:azure_bluet"].get<int>();
	//cout << azure_bluet << endl;
	
	////输出文件，注意必须要使用二进制格式打开文件
	//cout << nbt.to_string();
	//ofstream fout("test/lupine_01.nbt.schem", ios::binary);
	//fout << nbt;

	//NBT_Value nbt{
	//	"atag"_tag << 10,
	//	"btag"_tag << 10.5,
	//	"ctag"_tag << LIST {10, 10, 10},
	//	"dtag"_tag << CMP {
	//		"ftag"_tag << "aaaa",
	//		"etag"_tag << 555
	//	}
	//};
	//cout << nbt.to_string() << "\n" << endl;
	//nbt["atag"] = 20;
	//nbt["btag"] = "aaaa";
	//nbt["ctag"][1] = 50;
	//nbt["dtag"]["tag"] = CMP{
	//	"gtag"_tag << 666,
	//	"htag"_tag << LIST{1.1,2.2,3.3}
	//};
	//cout << nbt.to_string() << endl;
	
}