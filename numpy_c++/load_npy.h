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

#include <iosfwd>
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

	Load_npy(std::istream &input);
	void read(std::istream &input, void *buf)const;
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
	std::string header;
	std::string dtype;
	Order ordering;
	std::vector<int> shape;
	bool little_endian;
	bool integer;
	int bytes;
};



#endif // __load_npy_header