// Source code by Shawn Halayka
// Source code is in the public domain

#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H


#include <vector>
#include <string>
#include <cstdlib>


namespace string_utilities
{
	std::string lower_string(const std::string &src_string);
	std::string upper_string(const std::string &src_string);
	std::string trim_whitespace_string(const std::string &src_string);

	std::vector< std::string > stl_str_tok(std::string token, const std::string &SrcString);

	bool is_short_signed_int(const std::string &src_string);
	bool is_unsigned_int(const std::string &src_string);
	bool is_real_number(const std::string &src_string);
};




#endif
