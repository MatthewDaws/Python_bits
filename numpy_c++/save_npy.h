/** Simple class to save .npy files
 *
 *  Currently only handles arrays (of any dimension) consisting of simple datatypes.
 *  Only handles datatypes of little endian, integer or float, or 4 or 8 bytes.
 *   (so codes <i4 <i8 <f4 <f8 only)
 *  Assumes C ordering of the data (not Fortran)
 *
 *  TODO: Some template metaprogramming trickery??
 *
 *  Throws exceptions of type Save_npy::failure (subclass of runtime_error) on parsing
 *  errors, and rethrows i/o errors.
 *  
 */

#ifndef __save_npy_h
#define __save_npy_h

#include <ostream>
#include <string>
#include <stdexcept>
#include <vector>

class Save_npy {
public:
	class failure : public std::runtime_error {
	public:
		failure(const char* msg) : runtime_error(msg) {}
	};

	Save_npy(std::ostream& _out, const std::vector<int> shape, const char* format);
	void write_row(void *buffer);
	void write(void *buffer);
private:
	std::ostream& out;
	bool intdata;
	int bytes;
	int rowlength, height;
};

void save_npy(const std::string filename, const std::vector<int> shape, const char* format, void *buf);

#endif // __save_npy_h
