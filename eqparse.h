// Source code by Shawn Halayka
// Source code is in the public domain


#ifndef EQPARSE_H
#define EQPARSE_H


#include "quaternion_math.h"
#include "string_utilities.h"
using string_utilities::lower_string;
using string_utilities::stl_str_tok;
using string_utilities::is_real_number;

#include <vector>
using std::vector;

#include <string>
using std::string;

#include <sstream>
using std::istringstream;
using std::ostringstream;

// for console debug purposes
#include <iostream>
using std::cout;
using std::endl;


typedef void (quaternion_math::*qmath_func_ptr)(const quaternion *const, const quaternion *const, quaternion *const);

#define TOKENIZED_INSTRUCTION_DEST_ANSWER 0
#define TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP 1
#define TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP 2
#define TOKENIZED_INSTRUCTION_DEST_Z 3
#define TOKENIZED_INSTRUCTION_DEST_C 4
#define TOKENIZED_INSTRUCTION_DEST_NULL 5


class term
{
public:
	size_t level;
	vector<string> text;
	bool constant;
	bool quaternion;
};

class tokenized_instruction
{
public:

	tokenized_instruction()
	{
		a_type = TOKENIZED_INSTRUCTION_DEST_NULL;
		b_type = TOKENIZED_INSTRUCTION_DEST_NULL;
		out_type = TOKENIZED_INSTRUCTION_DEST_NULL;
		f = 0;
	}

	qmath_func_ptr f;
	size_t a_type, b_type, out_type;
	size_t a_index, b_index, out_index;
};

class assembled_instruction
{
public:
	qmath_func_ptr f;
	quaternion *a, *b, *out;
};

class function_mapping
{
public:
	function_mapping(const string &src_function_name, qmath_func_ptr src_f)
	{
		function_name = lower_string(src_function_name);
		f = src_f;
	}

	string function_name;
	qmath_func_ptr f;
};

class quaternion_julia_set_equation_parser
{
public:
	quaternion_julia_set_equation_parser() { setup_function_map(); }
	~quaternion_julia_set_equation_parser() { cleanup(); }
	bool setup(const string &src_formula, string &error_output, const quaternion &src_C);
	float iterate(const quaternion &src_Z, const short unsigned int &max_iterations, const float &threshold);
	string get_unique_formula_string(void);

protected:
	void setup_function_map(void);
	void cleanup(void);
	qmath_func_ptr get_function_instruction(const string &src_token);
	void setup_constant_quaternion(const vector<string> &tokens, const size_t &ordered_term_index);
	void setup_variable_quaternion(const vector<string> &tokens, const size_t &ordered_term_index);
	void setup_single_constant_equation(const string &src_token, const size_t &ordered_term_index);
	void setup_single_variable_equation(const string &src_token, const size_t &ordered_term_index);
	void setup_constant_equation(vector<string> tokens, const size_t &ordered_term_index);
	void setup_variable_equation(vector<string> tokens, const size_t &ordered_term_index);
	void setup_final_Z_copy(const string &src_destination);

	bool is_valid_destination(const string &src_string);
	bool is_variable_swizzle(const string &src_string);
	bool is_constant_swizzle(const string &src_string);
	bool is_raw_swizzle(const string &src_string);
	bool is_function(const string &src_string);
	bool is_math_operator(const string &src_string);
	bool is_constant(const string &src_string);
	bool is_variable(const string &src_string);
	bool is_constant_quaternion(const string &src_string);
	bool is_variable_quaternion(const string &src_string);
	bool cleanup_equation_tokens(vector<string> &equation);
	void get_terms(vector<string> src_equation, vector<term> &ordered_terms);
	bool compile_ordered_terms(const vector<term> &ordered_terms);
	bool assemble_compiled_instructions(void);

	quaternion Z, C;
	quaternion_math q_math;
	string unique_formula_string;

	vector< quaternion > answers;
	vector< quaternion > constant_scratch_heap;
	vector<	vector<tokenized_instruction> > instructions;
	vector< vector< quaternion > > scratch_heap;
	vector< assembled_instruction > execution_stack;
	vector< function_mapping > function_map;
};

#endif
