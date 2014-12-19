/** Simple class to load .npy files
 *
 *  Currently only handles arrays (of any dimension) consisting of simple datatypes.
 *  Only handles datatypes of little endian, integer or float, or 4 or 8 bytes.
 *   (so codes <i4 <i8 <f4 <f8 only)
 *  Assumes C ordering of the data (not Fortran)
 *  Typical usage would be to open the file, use Load_npy to parse the header, then
 *  check that the format is as expected, and then load.
 *
 *  """
 *  file.open("double64.npy", std::ios::binary);
 *  Load_npy npy(file);
 *  if ( npy.check_format("f8") and npy.dimensions()==2 and npy.get_shape()[1]==2 ) {
 *     struct point { double x,y; };
 *     std::vector<point> buffer(npy.get_shape()[0]);
 *     npy.read(file, &buffer[0]);
 *  }
 *  """
 *
 *  Throws exceptions of type Load_npy::failure (subclass of runtime_error) on parsing
 *  errors, and rethrows i/o errors.
 *  
 */

#ifndef __load_npy_header
#define __load_npy_header

#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

/** Client class to load an npy file. */
class Load_npy {
public:
	class failure : public std::runtime_error {
	public:
		failure(const char* msg) : runtime_error(msg) {}
	};
	enum class Order { C, Fortran, Other };

	Load_npy(std::istream &input) { init(input); }
	Load_npy(std::string filename);
	void read(std::istream &input, void *buf)const;
	void readrow(std::istream &input, void *row)const;
	void read(void *buf) { read(static_cast<std::istream&>(ifile), buf); }
	void readrow(void *row) { readrow(static_cast<std::istream&>(ifile), row); }
	std::string info()const;
	/** Return a const std::vector showing the shape of the array */
	const std::vector<int>& get_shape()const { return shape; }
	/** Returns dimensions, same as size() of get_shape() */
	int dimensions()const { return shape.size(); }
	/** Is the datatype an integer (otherwise float) */
	bool is_int()const { return integer; }
	/** Number of bytes an entry, assumed 4 or 8 */
	int bytes_per_entry()const { return bytes; }
	bool check_format(std::string format)const;
private:
	void init(std::istream &input);
private:
	std::string header;
	std::string dtype;
	Order ordering;
	std::vector<int> shape;
	bool little_endian;
	bool integer;
	int bytes;
	std::ifstream ifile;
};

template<typename strT>
std::vector<strT> load_numpy_structured_rows(std::string filename, int row_width, const char* format)
{
	Load_npy npy(filename);
	std::vector<strT> buffer;
	if ( npy.check_format(format) and npy.dimensions()==2 and npy.get_shape()[1]==row_width ) {
		buffer.resize(npy.get_shape()[0]);
		npy.read(&buffer[0]);
	}
	return buffer;
}

#endif // __load_npy_header