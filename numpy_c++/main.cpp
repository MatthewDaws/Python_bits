/** Tester
 *  
 */

#include "load_npy.h"

#include <exception>
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

// Make this a bit more portable
#include <cstdint>

template <typename T>
void do_test(std::string filename, const char *format)
{
	std::ifstream file;
	file.open(filename, std::ios::binary);
	try {
		Load_npy npy(file);
		cout << npy.info() << endl;
		if ( npy.check_format(format) and npy.dimensions()==2 and npy.get_shape()[1]==2 ) {
			struct point { T x,y; };
			std::vector<point> buffer(npy.get_shape()[0]);
			npy.read(file, &buffer[0]);
			for (auto row : buffer) {
				cout << row.x << ", " << row.y << endl;
			}
		}
	}
	catch ( const std::exception& e )
	{
		cout << "Exception: " << e.what() << endl;
	}
}

int main()
{
	do_test<int32_t>("int32.npy", "i4");
	do_test<int64_t>("int64.npy", "i8");
	do_test<float>("float32.npy", "f4");
	do_test<double>("double64.npy", "f8");
}
