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

#include "save_npy.h"

#include <sstream>
#include <cstdint>
#include <fstream>

void Save_npy::write_row(void *buffer)
{
	out.write(reinterpret_cast<char*>(buffer), bytes * rowlength);
}

void Save_npy::write(void *buffer)
{
	out.write(reinterpret_cast<char*>(buffer), bytes * rowlength * height);
}

Save_npy::Save_npy(std::ostream& _out, const std::vector<int> shape, const char* format)
	: out(_out), rowlength{0}, height{0}
{
	// Validate format; only looks at first 2 bytes
	switch ( format[0] )
	{
		case 'i': intdata = true; break;
		case 'f': intdata = false; break;
		default:
			throw failure("Save_npy: Invalid format string.");
	}
	switch ( format[1] )
	{
		case '4': bytes = 4; break;
		case '8': bytes = 8; break;
		default:
			throw failure("Save_npy: Invalid format string.");
	}
	// Validate length and compute rowlength
	if ( shape.size() == 0 ) {
		throw failure("Save_npy: Invalid shape.");
	}
	rowlength = 1;
	for (int i=1; i<shape.size(); ++i) { rowlength *= shape[i]; }
	height = shape[0];

	// Set to throw exceptions, and let client handle
	auto oldexc = out.exceptions();
	out.exceptions(std::ios::badbit | std::ios::failbit);
	try {
		// magic string
		unsigned char magic[8] = { 0x93, 'N', 'U', 'M', 'P', 'Y', 1, 0 };
		out.write(reinterpret_cast<char*>(magic), 8);
		// header
		std::string head1("{'descr': '<"); // Assume little endian
		std::string head2("', 'fortran_order': False, 'shape': (");
		// header:: shape
		std::stringstream head3ss;
		head3ss << shape[0] << ", ";
		for (int i=1; i<shape.size(); ++i) {
			head3ss << shape[i];
			if ( i != shape.size()-1 ) { head3ss << ", "; }
		}
		head3ss << "), }";
		// Decide on padding, 1 for final 0x0A, and 10 for first bit of header
		int header_length = head1.length() + 2 + head2.length() + head3ss.str().length() + 11;
		int roundup = (header_length + 15) / 16;
		roundup = (roundup * 16) - header_length;
		header_length = header_length + roundup - 10;
		// Write header length and then data
		uint16_t length = header_length;
		out.write(reinterpret_cast<char*>(&length), 2);
		out.write(head1.data(), head1.length());
		out.write(format, 2);
		out.write(head2.data(), head2.length());
		out.write(head3ss.str().data(), head3ss.str().length());
		char padding[20];
		int i=0;
		while ( i < roundup ) { padding[i] = ' '; ++i; }
		padding[i] = 0x0a;
		out.write(padding, roundup+1);
	}
	catch (...)
	{
		out.exceptions(oldexc);
		throw;
	}
	out.exceptions(oldexc);
}

void save_npy(const std::string filename, const std::vector<int> shape, const char* format, void *buf)
{
	std::ofstream file(filename, std::ios_base::binary | std::ios_base::trunc);
	Save_npy snpy(file, shape, format);
	snpy.write(buf);
}
