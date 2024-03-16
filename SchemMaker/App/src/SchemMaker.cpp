#include <iostream>
#include <zlib.h>

#include "NBT_Value.h"
#include "NBT_Exception.h"
#include <fstream>

using namespace NBT;
using namespace std;

int main() {

	NBT_Value x{ "5","aaa","aaad"};

	NBT_Value y{ 1_b,2_b,3_b };
	
	try {
		y[1_B] = 10;
		cout << x.to_string();
	}
	catch (NBT_Exception e) {
		cerr << e.what();
	}

	
}