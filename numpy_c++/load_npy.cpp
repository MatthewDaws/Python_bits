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

#include "load_npy.h"

#include <istream>
#include <sstream>

/** Helper class to process the header of a npy file.  Not to be used by clients. */
class Load_npy_Header {
public:
	Load_npy_Header(const std::string &h) : header(h) {}
	std::string get_descr()const;
	static std::string Order_to_string(Load_npy::Order o);
	Load_npy::Order get_order()const;
	std::vector<int> get_shape()const;
private:
	std::string parse(const std::string &str)const;
	std::vector<std::string> split(const std::string str, const char c)const;
private:
	std::string header;
};

/** Convenience function to split a string */
std::vector<std::string> Load_npy_Header::split(const std::string str, const char c)const
{
	std::vector<std::string> ret;
	std::size_t start = 0;
	do {
		auto found = str.find_first_of(c, start);
		if ( found == std::string::npos ) {
			ret.push_back( str.substr(start, std::string::npos) );
			break;
		}
		ret.push_back( str.substr(start,found) );
		start = found + 1;
	} while ( true );
	return ret;
}

/** Searches for "'str':" in header and returns _rest_ of header after this.
 *  We need to do this as otherwise we have to completely parse nested commas, which
 *  seems like a lot of work!
 */
std::string Load_npy_Header::parse(const std::string &str)const
{
	std::string search = "'" + str + "':";
	auto found = header.find(search);
	if ( found == std::string::npos ) {
		return std::string();
	}
	return header.substr(found+search.length(),std::string::npos);
}

/** Find the data type description: expects a simple code '<i4' etc.
 *  and will deliberately return an empty string otherwise
 */
std::string Load_npy_Header::get_descr()const
{
	std::string d = parse("descr");
	auto start = d.find_first_not_of(' ');
	if ( start == std::string::npos ) { return std::string(); }
	d = d.substr(start, std::string::npos);
	if ( d[0] != '\'' ) { return std::string(); }
	auto end = d.find_first_of('\'', start+1);
	if ( end == std::string::npos ) { return std::string(); }
	return d.substr(1, end-1);
}

/** Parse the fortran_order: True/False part */
Load_npy::Order Load_npy_Header::get_order()const
{
	auto order = parse("fortran_order");
	auto found = order.find_first_of(',');
	if ( found != std::string::npos ) {
		order = order.substr(0,found);
	}
	if ( order.find("False") != std::string::npos ) {
		return Load_npy::Order::C;
	}
	if ( order.find("True") != std::string::npos ) {
		return Load_npy::Order::Fortran;
	}
	return Load_npy::Order::Other;
}

/** Convenience function to pretty print the Order enumerate class */
std::string Load_npy_Header::Order_to_string(Load_npy::Order o)
{
	switch ( o ) {
		case Load_npy::Order::C: return "C";
		case Load_npy::Order::Fortran: return "Fortran";
	}
	return "Other";
}

/** Read the shape part, expects (20,2,5) etc. */
std::vector<int> Load_npy_Header::get_shape()const
{
	std::string shape =  parse("shape");
	std::vector<int> ret;
	auto start = shape.find_first_of('(');
	if ( start == std::string::npos ) { return ret; }
	auto end = shape.find_first_of(')');
	if ( end == std::string::npos ) { return ret; }
	shape = shape.substr(start+1, end-start-1);
	auto nums = split(shape, ',');
	for (auto s : nums) {
		int x = -1;
		if ( s.length()==0 ) { break; } // Deal with Python convention for a
			// tuple of length 1: (20,) not (20).
		std::stringstream(s) >> x;
		ret.push_back( x );
	}
	return ret;
}




// ==========================================================================

/** Read, parse and validate the header.
 *  On failure throws Load_npy::failure class, which extends the runtime_error class
 *  and provides an entry for what()
 *  Any I/O exceptions are rethrown to be caught by client.
 */
void Load_npy::init(std::istream &input)
{
	auto oldexc = input.exceptions();
	input.exceptions(std::ios::badbit | std::ios::eofbit | std::ios::failbit);
	try {
		// Magic 0x93NUMPY
		unsigned char magic = 0;
		input.read(reinterpret_cast<char*>(&magic), 1);
		if ( magic != 0x93 ) { throw failure("Load_npy: Format error: Magic number not 0x93"); }
		char sig[5] = {0,0,0,0,0};
		input.read(sig, 5);
		if ( sig[0]!='N' or sig[1]!='U' or sig[2]!='M' or sig[3]!='P' or sig[4]!='Y' ) {
			throw failure("Load_npy: Format error: Magic code not 'NUMPY'");
		}
		// Version 1.0  ??
		unsigned char version[2] = {2,2};
		input.read(reinterpret_cast<char*>(&version[0]), 1);
		input.read(reinterpret_cast<char*>(&version[1]), 1);
		if ( version[0] != 1 or version[1] != 0 ) {
			throw failure("Load_npy: Format error: Version is not 1.0");
		}
		// Read header
		unsigned short int header_len;
		input.read(reinterpret_cast<char*>(&header_len), 2);
		std::vector<char> _header(header_len);
		input.read(reinterpret_cast<char*>(&_header[0]), header_len);
		header = std::string(&_header[0]);
		// Parse header
		Load_npy_Header header_parse(header);
		dtype = header_parse.get_descr();
		ordering = header_parse.get_order();
		shape = header_parse.get_shape();
		// Now internally parse
		if ( dtype.length() != 3 ) {
			throw failure("Load_npy: Cannot parse data type.");
		}
		if ( dtype[0] == '<' ) {
			little_endian = true;
		} else {
			throw failure("Load_npy: Cannot currently handle Big Endian byte ordering.");
		}
		switch ( dtype[1] ) {
			case 'i': integer = true; break;
			case 'f': integer = false; break;
			default: throw failure("Load_npy: Unknown data type, can only handle integer and float.");
		}
		switch ( dtype[2] ) {
			case '4': bytes = 4; break;
			case '8': bytes = 8; break;
			default: throw failure("Load_npy: Unknown data type size, can only handle 4 or 8.");
		}
		if ( ordering != Order::C ) {
			throw failure("Load_npy: Currently can only handle C ordering of data.");
		}
		for (int x : shape) {
			if ( x<=0 ) {
				throw failure("Load_npy: Shape contains 0 or negative number?");
			}
		}
	}
	catch (...)
	{
		input.exceptions(oldexc);
		throw;
	}
	input.exceptions(oldexc);
}

/** Variant with filename support */
Load_npy::Load_npy(std::string filename)
{
	ifile.open(filename, std::ios::binary);
	init(static_cast<std::istream&>(ifile));
}


/** Pretty print information about the header */
std::string Load_npy::info()const
{
	std::stringstream out;
	out << "Load_npy format: dtype: " << dtype;
	out << ", ordering: " << Load_npy_Header::Order_to_string(ordering);
	out << ", dimensions: " << shape.size() << " --> (";
	for (int i=0; i<shape.size()-1; ++i) { out << shape[i] << ", "; }
	out << shape.back() << ")";
	return out.str();
}

/** Pass a python-style format code (e.g. float is "f4") to check if agrees */
bool Load_npy::check_format(std::string format)const
{
	return format == dtype.substr(1,2);
}

/** Load the data: buf needs to be the correct size. */
void Load_npy::read(std::istream &input, void *buf)const
{
	// Because we currently don't worry about big endian, and we assume this code is
	// running on a little endian machine, we can just read this verbatim
	std::size_t toread = bytes;
	for (auto x : shape) { toread *= x; }
	input.read(reinterpret_cast<char*>(buf), toread);
}

/** Read a single "row" of the data.
 *  Here row means a (dimensions - 1) slice.
 */
void Load_npy::readrow(std::istream &input, void *row)const
{
	std::size_t toread = bytes;
	for (int i = 1; i < shape.size(); ++i) { toread *= shape[i]; }
	input.read(reinterpret_cast<char*>(row), toread);
}
