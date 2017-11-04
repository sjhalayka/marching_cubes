// Source code by Shawn Halayka
// Source code is in the public domain

#include "string_utilities.h"

using std::vector;
using std::string;


string string_utilities::lower_string(const string &src_string)
{
	string temp = src_string;

	for(string::iterator i = temp.begin(); i != temp.end(); i++)
		*i = tolower(*i);

	return temp;
}

string string_utilities::upper_string(const string &src_string)
{
	string temp = src_string;

	for(string::iterator i = temp.begin(); i != temp.end(); i++)
		*i = toupper(*i);

	return temp;
}

string string_utilities::trim_whitespace_string(const string &src_string)
{
	string temp_string;

	for(size_t i = 0; i < src_string.length(); i++)
		if(' ' != src_string[i] && '\t' != src_string[i])
			temp_string += src_string[i];

	return temp_string;
}

vector<string> string_utilities::stl_str_tok(string token, const string &src_string)
{
	vector<string> dest_vector;

	size_t prev_pos = 0;

	// set the current position to wherever we find the first token at
	// 0 says start at the beginning of the string
	size_t curr_pos = src_string.find(token, 0);

	// if we've reached the end of the string, and no token was found
	// add the entire string as one giant chunk
	if(curr_pos == string::npos)
	{
		dest_vector.push_back(src_string);
		return dest_vector;
	}

	// else, if the string isn't blank, push the substring into the vector
	if("" != src_string.substr(prev_pos, curr_pos-prev_pos))
		dest_vector.push_back(src_string.substr(prev_pos, curr_pos-prev_pos));

	// keep doing it until we hit beyond the end of the source string
	while(curr_pos!=string::npos)
	{
		prev_pos = curr_pos+token.length();
		curr_pos = src_string.find(token, prev_pos);

		if("" != src_string.substr(prev_pos, curr_pos-prev_pos))
			dest_vector.push_back(src_string.substr(prev_pos, curr_pos-prev_pos));
	}

	return dest_vector;
}

bool string_utilities::is_short_signed_int(const string &src_string)
{
	if(src_string == "" || src_string.size() > 5)
		return false;

	string temp = lower_string(src_string);

	for(size_t i = 0; i < temp.size(); i++)
	{
		if(temp[i] == '-' && i != 0)
			return false;

		if(!isdigit(temp[i]) && temp[i] != '-')
			return false;
	}

	return true;
}

bool string_utilities::is_unsigned_int(const string &src_string)
{
	if(src_string == "" || src_string.length() > 10)
		return false;

	string temp = lower_string(src_string);

	for(size_t i = 0; i < temp.size(); i++)
	{
		if(!isdigit(temp[i]))
			return false;
	}

	double num = atof(temp.c_str());

	if(num > 4294967295.0)
		return false;

	return true;
}

bool string_utilities::is_real_number(const string &src_string)
{
	//ie: 
	//1
	//-23e4
	//1.E2
	//-2.717
	//.31415e1
	//-7.53e-9
	//7.53e+9

	if(src_string == "")
		return false;

	string temp = lower_string(src_string);

	bool found_dot = false;
	bool found_e = false;
	bool found_digit = false;

	for(size_t i = 0; i < temp.size(); i++)
	{
		if(isdigit(temp[i]))
		{
			if(found_digit == false)
				found_digit = true;	
		}
		else if(temp[i] == 'e')
		{
			if(found_e == true || found_digit == false || i == temp.size() - 1)
				return false;
			else
				found_e = true;
		}
		else if(temp[i] == '-' || temp[i] == '+')
		{
			if(!(i == 0 || temp[i-1] == 'e') || i == temp.size() - 1)
				return false;
		}
		else if(temp[i] == '.')
		{
			if(found_dot == true || (i != 0 && temp[i-1] == 'e'))
				return false;
			else
				found_dot = true;
		}
		else
		{
			return false;
		}
	}

	return true;
}
