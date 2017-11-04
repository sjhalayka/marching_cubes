// Source code by Shawn Halayka
// Source code is in the public domain


#include "eqparse.h"


void quaternion_julia_set_equation_parser::cleanup(void)
{
	unique_formula_string = "";
	answers.clear();
	constant_scratch_heap.clear();
	instructions.clear();
	scratch_heap.clear();
	execution_stack.clear();
}

// don't need c, because it's passed in through setup
float quaternion_julia_set_equation_parser::iterate(const quaternion &src_Z, const short unsigned int &max_iterations, const float &threshold)
{
	Z = src_Z;

	float len_sq = Z.self_dot();
	const float threshold_sq = threshold*threshold;

	for(short unsigned int i = 0; i < max_iterations; i++)
	{
		for(size_t i = 0; i < execution_stack.size(); i++)
			(q_math.*execution_stack[i].f)(execution_stack[i].a, execution_stack[i].b, execution_stack[i].out);

		if((len_sq = Z.self_dot()) >= threshold_sq)
			break;
	}

	return sqrt(len_sq);
}


string quaternion_julia_set_equation_parser::get_unique_formula_string(void)
{
	return unique_formula_string;
}



void quaternion_julia_set_equation_parser::setup_constant_quaternion(const vector<string> &tokens, const size_t &ordered_term_index)
{
	for(size_t i = 0; i < 4; i++)
	{
		float *dest_double = 0;

		if(i == 0)
			dest_double = &answers[ordered_term_index].x;
		if(i == 1)
			dest_double = &answers[ordered_term_index].y;
		if(i == 2)
			dest_double = &answers[ordered_term_index].z;
		if(i == 3)
			dest_double = &answers[ordered_term_index].w;

		string token = lower_string(tokens[i]);

		if(token == "c.x")
			*dest_double = C.x;
		else if(token == "-c.x")
			*dest_double = -C.x;
		else if(token == "c.y")
			*dest_double = C.y;
		else if(token == "-c.y")
			*dest_double = -C.y;
		else if(token == "c.z")
			*dest_double = C.z;
		else if(token == "-c.z")
			*dest_double = -C.z;
		else if(token == "c.w")
			*dest_double = C.w;
		else if(token == "-c.w")
			*dest_double = -C.w;
		else
		{
			istringstream in(token);
			in >> *dest_double;
		}
	}
}


void quaternion_julia_set_equation_parser::setup_variable_quaternion(const vector<string> &tokens, const size_t &ordered_term_index)
{
	quaternion copymask; // defaults to zeroes

	for(size_t i = 0; i < 4; i++)
	{
		float *dest_double = 0;
		float *copy_mask_dest_double = 0;

		if(i == 0)
		{
			dest_double = &answers[ordered_term_index].x;
			copy_mask_dest_double = &copymask.x;
		}
		if(i == 1)
		{
			dest_double = &answers[ordered_term_index].y;
			copy_mask_dest_double = &copymask.y;
		}
		if(i == 2)
		{
			dest_double = &answers[ordered_term_index].z;
			copy_mask_dest_double = &copymask.z;
		}
		if(i == 3)
		{
			dest_double = &answers[ordered_term_index].w;
			copy_mask_dest_double = &copymask.w;
		}

		string token = lower_string(tokens[i]);

		if(token == "z.x")
			*copy_mask_dest_double = 1.0;
		else if(token == "-z.x")
			*copy_mask_dest_double = -1.0;
		else if(token == "z.y")
			*copy_mask_dest_double = 2.0;
		else if(token == "-z.y")
			*copy_mask_dest_double = -2.0;
		else if(token == "z.z")
			*copy_mask_dest_double = 3.0;
		else if(token == "-z.z")
			*copy_mask_dest_double = -3.0;
		else if(token == "z.w")
			*copy_mask_dest_double = 4.0;
		else if(token == "-z.w")
			*copy_mask_dest_double = -4.0;
		else if(token == "c.x")
			*dest_double = C.x;
		else if(token == "-c.x")
			*dest_double = -C.x;
		else if(token == "c.y")
			*dest_double = C.y;
		else if(token == "-c.y")
			*dest_double = -C.y;
		else if(token == "c.z")
			*dest_double = C.z;
		else if(token == "-c.z")
			*dest_double = -C.z;
		else if(token == "c.w")
			*dest_double = C.w;
		else if(token == "-c.w")
			*dest_double = -C.w;
		else
		{
			istringstream in(token);
			in >> *dest_double;
		}
	}

	// add copy mask to scratch heap
	scratch_heap[ordered_term_index].push_back(copymask);

	// setup copy_masked instruction to use Z as a, 0th scratch heap item as b, and this equation level's answer as out
	tokenized_instruction instruction;

	instruction.f = &quaternion_math::copy_masked;
	instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;
	instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
	instruction.b_index = 0;
	instruction.out_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
	instruction.out_index = ordered_term_index;

	instructions[ordered_term_index].push_back(instruction);
}


void quaternion_julia_set_equation_parser::setup_single_constant_equation(const string &src_token, const size_t &ordered_term_index)
{
	string token = lower_string(src_token);

	answers[ordered_term_index] = C;

	if(is_constant_swizzle(token)) // assumes that swizzle has been expanded
	{
		quaternion swizzlemask;

		for(size_t i = 0; i < 4; i++)
		{
			float *swizzle_mask_dest_double = 0;

			if(i == 0)
				swizzle_mask_dest_double = &swizzlemask.x;
			if(i == 1)
				swizzle_mask_dest_double = &swizzlemask.y;
			if(i == 2)
				swizzle_mask_dest_double = &swizzlemask.z;
			if(i == 3)
				swizzle_mask_dest_double = &swizzlemask.w;

			char char_token = token[i + 2];

			if(char_token == 'x')
				*swizzle_mask_dest_double = 1.0;
			else if(char_token == 'y')
				*swizzle_mask_dest_double = 2.0;
			else if(char_token == 'z')
				*swizzle_mask_dest_double = 3.0;
			else if(char_token == 'w')
				*swizzle_mask_dest_double = 4.0;
		}

		q_math.swizzle(&answers[ordered_term_index], &swizzlemask, &answers[ordered_term_index]);
	}
}


void quaternion_julia_set_equation_parser::setup_single_variable_equation(const string &src_token, const size_t &ordered_term_index)
{
	string token = lower_string(src_token);

	if(is_variable(token)) // 'z', so do copy
	{
		tokenized_instruction instruction;

		instruction.f = &quaternion_math::copy;
		instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;
		instruction.b_type = TOKENIZED_INSTRUCTION_DEST_NULL;
		instruction.out_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
		instruction.out_index = ordered_term_index;

		instructions[ordered_term_index].push_back(instruction);
	}
	else if(is_variable_swizzle(token)) // z.????, so do swizzle instruction
	{
		quaternion swizzlemask;

		for(size_t i = 0; i < 4; i++)
		{
			float *swizzle_mask_dest_double = 0;

			if(i == 0)
				swizzle_mask_dest_double = &swizzlemask.x;
			if(i == 1)
				swizzle_mask_dest_double = &swizzlemask.y;
			if(i == 2)
				swizzle_mask_dest_double = &swizzlemask.z;
			if(i == 3)
				swizzle_mask_dest_double = &swizzlemask.w;

			char char_token = token[i + 2];

			if(char_token == 'x')
				*swizzle_mask_dest_double = 1.0;
			else if(char_token == 'y')
				*swizzle_mask_dest_double = 2.0;
			else if(char_token == 'z')
				*swizzle_mask_dest_double = 3.0;
			else if(char_token == 'w')
				*swizzle_mask_dest_double = 4.0;
		}

		// add swizzle mask to scratch heap
		scratch_heap[ordered_term_index].push_back(swizzlemask);

		// setup swizzle instruction to use Z as a, 0th scratch heap item as b, and this equation level's answer as out
		tokenized_instruction instruction;

		instruction.f = &quaternion_math::swizzle;
		instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;
		instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
		instruction.b_index = 0;
		instruction.out_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
		instruction.out_index = ordered_term_index;

		instructions[ordered_term_index].push_back(instruction);
	}
}

void quaternion_julia_set_equation_parser::setup_constant_equation(vector<string> tokens, const size_t &ordered_term_index)
{
	// do non-raw swizzles
	for(size_t i = 0; i < tokens.size(); i++)
	{
		string token = lower_string(tokens[i]);

		if(!is_constant_swizzle(token))
			continue;

		// get next scratch heap address
		size_t next_scratch_heap_index = scratch_heap[ordered_term_index].size();
		scratch_heap[ordered_term_index].push_back(C);

		quaternion swizzlemask;

		for(size_t j = 0; j < 4; j++)
		{
			float *swizzle_mask_dest_double = 0;

			if(j == 0)
				swizzle_mask_dest_double = &swizzlemask.x;
			if(j == 1)
				swizzle_mask_dest_double = &swizzlemask.y;
			if(j == 2)
				swizzle_mask_dest_double = &swizzlemask.z;
			if(j == 3)
				swizzle_mask_dest_double = &swizzlemask.w;

			char char_token = token[j + 2];

			if(char_token == 'x')
				*swizzle_mask_dest_double = 1.0;
			else if(char_token == 'y')
				*swizzle_mask_dest_double = 2.0;
			else if(char_token == 'z')
				*swizzle_mask_dest_double = 3.0;
			else if(char_token == 'w')
				*swizzle_mask_dest_double = 4.0;
		}

		q_math.swizzle(&scratch_heap[ordered_term_index][next_scratch_heap_index], &swizzlemask, &scratch_heap[ordered_term_index][next_scratch_heap_index]);

		ostringstream out;
		out << static_cast<long unsigned int>(next_scratch_heap_index);
		tokens[i] = "SH#";
		tokens[i] += out.str();
	}

	// do functions
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if(is_function(tokens[i]))
		{
			// get function address
			tokenized_instruction instruction;
			instruction.f = get_function_instruction(tokens[i]);

			// trim C#?? to ??
			string source_address_token = tokens[i + 1].substr(2, tokens[i + 1].length() - 2);

			// setup address #
			istringstream in(source_address_token);
			size_t src_answer_address = 0;
			in >> src_answer_address;

			(q_math.*instruction.f)(&answers[src_answer_address], 0, &answers[src_answer_address]);

			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}

	// do raw swizzles
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if(is_raw_swizzle(tokens[i]))
		{
			string token = lower_string(tokens[i]);

			quaternion swizzlemask;

			for(size_t j = 0; j < 4; j++)
			{
				float *swizzle_mask_dest_double = 0;

				if(j == 0)
					swizzle_mask_dest_double = &swizzlemask.x;
				if(j == 1)
					swizzle_mask_dest_double = &swizzlemask.y;
				if(j == 2)
					swizzle_mask_dest_double = &swizzlemask.z;
				if(j == 3)
					swizzle_mask_dest_double = &swizzlemask.w;

				char char_token = token[j + 1];

				if(char_token == 'x')
					*swizzle_mask_dest_double = 1.0;
				else if(char_token == 'y')
					*swizzle_mask_dest_double = 2.0;
				else if(char_token == 'z')
					*swizzle_mask_dest_double = 3.0;
				else if(char_token == 'w')
					*swizzle_mask_dest_double = 4.0;
			}


			// trim C#?? to ??
			string source_address_token = tokens[i - 1].substr(2, tokens[i - 1].length() - 2);

			// setup address #
			istringstream in(source_address_token);
			size_t input_address = 0;
			in >>input_address;

			q_math.swizzle(&answers[input_address], &swizzlemask, &answers[input_address]);

			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}

	// do ^
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if("^" == tokens[i])
		{
			quaternion *a = 0;
			quaternion *pow_mask = 0;
			quaternion *output = 0;

			// get/set input/output
			if("c" == lower_string(tokens[i - 1]))
			{
				size_t next_scratch_heap_index = scratch_heap[ordered_term_index].size();
				scratch_heap[ordered_term_index].push_back(C);

				a = &scratch_heap[ordered_term_index][next_scratch_heap_index];
				output = &scratch_heap[ordered_term_index][next_scratch_heap_index];

				ostringstream out;
				out << static_cast<long unsigned int>(next_scratch_heap_index);
				tokens[i-1] = "SH#";
				tokens[i-1] += out.str();
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i - 1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("C" == src_tokens[0]) // constant equation token
				{
					a = &answers[address];
					output = &answers[address];
				}
				else
				{
					a = &scratch_heap[ordered_term_index][address];
					output = &scratch_heap[ordered_term_index][address];
				}
			}

			vector<string> dest_tokens = stl_str_tok("#", tokens[i + 1]);

			// trim C#?? to ??
			string source_address_token = tokens[i + 1].substr(2, tokens[i + 1].length() - 2);

			// setup address #
			istringstream in(source_address_token);
			size_t src_answer_address = 0;
			in >> src_answer_address;

			(q_math.pow)(a, &answers[src_answer_address], output);

			tokens.erase(tokens.begin() + i + 1);
			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}

	// do * /
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if("*" == tokens[i] || "/" == tokens[i])
		{
			quaternion *a = 0;
			quaternion *b = 0;
			quaternion *output = 0;

			// get/set a/output
			if("c" == lower_string(tokens[i - 1]))
			{
				size_t next_scratch_heap_index = scratch_heap[ordered_term_index].size();
				scratch_heap[ordered_term_index].push_back(C);

				a = &scratch_heap[ordered_term_index][next_scratch_heap_index];
				output = &scratch_heap[ordered_term_index][next_scratch_heap_index];

				ostringstream out;
				out << static_cast<long unsigned int>(next_scratch_heap_index);
				tokens[i-1] = "SH#";
				tokens[i-1] += out.str();
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i - 1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("C" == src_tokens[0]) // constant equation token
				{
					a = &answers[address];
					output = &answers[address];
				}
				else
				{
					a = &scratch_heap[ordered_term_index][address];
					output = &scratch_heap[ordered_term_index][address];
				}
			}

			// get/set b
			if("c" == lower_string(tokens[i + 1]))
			{
				b = &C;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i + 1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("C" == src_tokens[0]) // constant equation token
				{
					b = &answers[address];
				}
				else
				{
					b = &scratch_heap[ordered_term_index][address];
				}
			}

			if("*" == tokens[i])
				(q_math.mul)(a, b, output);
			else
				(q_math.div)(a, b, output);

			tokens.erase(tokens.begin() + i + 1);
			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}

	// do + -
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if("+" == tokens[i] || "-" == tokens[i])
		{
			quaternion *a = 0;
			quaternion *b = 0;
			quaternion *output = 0;


			// get/set a/output
			if("c" == lower_string(tokens[i - 1]))
			{
				size_t next_scratch_heap_index = scratch_heap[ordered_term_index].size();
				scratch_heap[ordered_term_index].push_back(C);

				a = &scratch_heap[ordered_term_index][next_scratch_heap_index];
				output = &scratch_heap[ordered_term_index][next_scratch_heap_index];

				ostringstream out;
				out << static_cast<long unsigned int>(next_scratch_heap_index);
				tokens[i-1] = "SH#";
				tokens[i-1] += out.str();
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i - 1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("C" == src_tokens[0]) // constant equation token
				{
					a = &answers[address];
					output = &answers[address];
				}
				else
				{
					a = &scratch_heap[ordered_term_index][address];
					output = &scratch_heap[ordered_term_index][address];
				}
			}

			// get/set b
			if("c" == lower_string(tokens[i + 1]))
			{
				b = &C;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i + 1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("C" == src_tokens[0]) // constant equation token
				{
					b = &answers[address];
				}
				else
				{
					b = &scratch_heap[ordered_term_index][address];
				}
			}

			if("+" == tokens[i])
				(q_math.add)(a, b, output);
			else
				(q_math.sub)(a, b, output);

			tokens.erase(tokens.begin() + i + 1);
			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}

	// copy to answer index
	quaternion *a;

	// get/set a
	if("c" == lower_string(tokens[0]))
	{
		a = &C;
	}
	else
	{
		vector<string> src_tokens = stl_str_tok("#", tokens[0]);

		istringstream in(src_tokens[1]);
		size_t address = 0;
		in >> address;

		if("C" == src_tokens[0]) // constant equation token
		{
			a = &answers[address];
		}
		else
		{
			a = &scratch_heap[ordered_term_index][address];
		}
	}

	(q_math.copy)(a, 0, &answers[ordered_term_index]);

	// don't need these anymore, this is a constant expression, no instructions will have been created
	scratch_heap[ordered_term_index].clear();
}

void quaternion_julia_set_equation_parser::setup_variable_equation(vector<string> tokens, const size_t &ordered_term_index)
{
	// do constant non-raw swizzles
	for(size_t i = 0; i < tokens.size(); i++)
	{
		string token = lower_string(tokens[i]);

		if(!is_constant_swizzle(token))
			continue;

		// get next constant scratch heap address
		size_t next_scratch_heap_index = constant_scratch_heap.size();
		constant_scratch_heap.push_back(C);

		quaternion swizzlemask;

		for(size_t j = 0; j < 4; j++)
		{
			float *swizzle_mask_dest_double = 0;

			if(j == 0)
				swizzle_mask_dest_double = &swizzlemask.x;
			if(j == 1)
				swizzle_mask_dest_double = &swizzlemask.y;
			if(j == 2)
				swizzle_mask_dest_double = &swizzlemask.z;
			if(j == 3)
				swizzle_mask_dest_double = &swizzlemask.w;

			char char_token = token[j + 2];

			if(char_token == 'x')
				*swizzle_mask_dest_double = 1.0;
			else if(char_token == 'y')
				*swizzle_mask_dest_double = 2.0;
			else if(char_token == 'z')
				*swizzle_mask_dest_double = 3.0;
			else if(char_token == 'w')
				*swizzle_mask_dest_double = 4.0;
		}

		q_math.swizzle(&constant_scratch_heap[next_scratch_heap_index], &swizzlemask, &constant_scratch_heap[next_scratch_heap_index]);

		ostringstream out;
		out << static_cast<long unsigned int>(next_scratch_heap_index);
		tokens[i] = "CSH#";
		tokens[i] += out.str();
	}



	// do variable non-raw swizzles
	for(size_t i = 0; i < tokens.size(); i++)
	{
		string token = lower_string(tokens[i]);

		if(!is_variable_swizzle(token))
			continue;

		quaternion swizzlemask;

		for(size_t j = 0; j < 4; j++)
		{
			float *swizzle_mask_dest_double = 0;

			if(j == 0)
				swizzle_mask_dest_double = &swizzlemask.x;
			if(j == 1)
				swizzle_mask_dest_double = &swizzlemask.y;
			if(j == 2)
				swizzle_mask_dest_double = &swizzlemask.z;
			if(j == 3)
				swizzle_mask_dest_double = &swizzlemask.w;

			char char_token = token[j + 2];

			if(char_token == 'x')
				*swizzle_mask_dest_double = 1.0;
			else if(char_token == 'y')
				*swizzle_mask_dest_double = 2.0;
			else if(char_token == 'z')
				*swizzle_mask_dest_double = 3.0;
			else if(char_token == 'w')
				*swizzle_mask_dest_double = 4.0;
		}

		tokenized_instruction instruction;
		instruction.f = &quaternion_math::swizzle;

		instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;

		instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
		scratch_heap[ordered_term_index].push_back(swizzlemask);
		instruction.b_index = scratch_heap[ordered_term_index].size() - 1;

		instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
		scratch_heap[ordered_term_index].push_back(quaternion());
		instruction.out_index = scratch_heap[ordered_term_index].size() - 1;

		instructions[ordered_term_index].push_back(instruction);

		ostringstream out;
		out << static_cast<long unsigned int>(scratch_heap[ordered_term_index].size() - 1);
		tokens[i] = "SH#";
		tokens[i] += out.str();
	}


	// do functions
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if(is_function(tokens[i]))
		{
			// get function address
			tokenized_instruction instruction;
			instruction.f = get_function_instruction(tokens[i]);

			// can either be C# or V#
			string source_address_token = tokens[i + 1].substr(2, tokens[i + 1].length() - 2);

			// setup address #
			istringstream in(source_address_token);
			size_t src_answer_address = 0;
			in >> src_answer_address;

			if('C' == tokens[i+1][0])
			{
				(q_math.*instruction.f)(&answers[src_answer_address], 0, &answers[src_answer_address]);
			}
			else
			{
				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
				instruction.a_index = src_answer_address;
				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_NULL;
				instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				scratch_heap[ordered_term_index].push_back(quaternion());
				instruction.out_index = scratch_heap[ordered_term_index].size() - 1;

				instructions[ordered_term_index].push_back(instruction);

				ostringstream out;
				out << static_cast<long unsigned int>(scratch_heap[ordered_term_index].size() - 1);
				tokens[i+1] = "SH#";
				tokens[i+1] += out.str();
			}

			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}


	// do raw swizzles
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if(is_raw_swizzle(tokens[i]))
		{
			string token = lower_string(tokens[i]);

			quaternion swizzlemask;

			for(size_t j = 0; j < 4; j++)
			{
				float *swizzle_mask_dest_double = 0;

				if(j == 0)
					swizzle_mask_dest_double = &swizzlemask.x;
				if(j == 1)
					swizzle_mask_dest_double = &swizzlemask.y;
				if(j == 2)
					swizzle_mask_dest_double = &swizzlemask.z;
				if(j == 3)
					swizzle_mask_dest_double = &swizzlemask.w;

				char char_token = token[j + 1];

				if(char_token == 'x')
					*swizzle_mask_dest_double = 1.0;
				else if(char_token == 'y')
					*swizzle_mask_dest_double = 2.0;
				else if(char_token == 'z')
					*swizzle_mask_dest_double = 3.0;
				else if(char_token == 'w')
					*swizzle_mask_dest_double = 4.0;
			}


			// can either be C# or V# or SH#
			vector<string> src_tokens = stl_str_tok("#", tokens[i - 1]);

			istringstream in(src_tokens[1]);
			size_t address = 0;
			in >> address;

			if("C" == src_tokens[0])
			{
				q_math.swizzle(&answers[address], &swizzlemask, &answers[address]);
			}
			else if("V" == src_tokens[0])
			{
				tokenized_instruction instruction;
				instruction.f = &quaternion_math::swizzle;

				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
				instruction.a_index = address;

				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				scratch_heap[ordered_term_index].push_back(swizzlemask);
				instruction.b_index = scratch_heap[ordered_term_index].size() - 1;

				instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				scratch_heap[ordered_term_index].push_back(quaternion());
				instruction.out_index = scratch_heap[ordered_term_index].size() - 1;

				instructions[ordered_term_index].push_back(instruction);

				ostringstream out;
				out << static_cast<long unsigned int>(scratch_heap[ordered_term_index].size() - 1);
				tokens[i-1] = "SH#";
				tokens[i-1] += out.str();
			}
			else
			{
				tokenized_instruction instruction;
				instruction.f = &quaternion_math::swizzle;

				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				instruction.a_index = address;

				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				scratch_heap[ordered_term_index].push_back(swizzlemask);
				instruction.b_index = scratch_heap[ordered_term_index].size() - 1;

				instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				instruction.out_index = address;

				instructions[ordered_term_index].push_back(instruction);
			}

			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}


	// do ^
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if("^" == tokens[i])
		{
			tokenized_instruction instruction;
			instruction.f = &quaternion_math::pow;

			bool answer_requires_scratch_heap = true;

			if("c" == lower_string(tokens[i-1]))
			{
				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_C;
			}
			else if("z" == lower_string(tokens[i-1]))
			{
				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i-1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("SH" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.a_index = address;
					instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.out_index = address;

					answer_requires_scratch_heap = false;
				}
				else if("CSH" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP;
					instruction.a_index = address;
				}
				else if("C" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.a_index = address;
				}
				else if("V" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.a_index = address;
				}
			}

			if(true == answer_requires_scratch_heap)
			{
				instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				scratch_heap[ordered_term_index].push_back(quaternion());
				instruction.out_index = scratch_heap[ordered_term_index].size() - 1;

				ostringstream out;
				out << static_cast<long unsigned int>(scratch_heap[ordered_term_index].size() - 1);
				tokens[i-1] = "SH#";
				tokens[i-1] += out.str();
			}

			// setup b
			if("c" == lower_string(tokens[i+1]))
			{
				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_C;
			}
			else if("z" == lower_string(tokens[i+1]))
			{
				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_Z;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i+1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("SH" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.b_index = address;
				}
				else if("CSH" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP;
					instruction.b_index = address;
				}
				else if("C" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.b_index = address;
				}
				else if("V" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.b_index = address;
				}
			}

			instructions[ordered_term_index].push_back(instruction);

			tokens.erase(tokens.begin() + i + 1);
			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}



	// do * /
	for(size_t i = 0; i < tokens.size(); i++)
	{
		if("*" == tokens[i] || "/" == tokens[i])
		{
			tokenized_instruction instruction;

			if("*" == tokens[i])
				instruction.f = &quaternion_math::mul;
			else
				instruction.f = &quaternion_math::div;

			bool answer_requires_scratch_heap = true;

			if("c" == lower_string(tokens[i-1]))
			{
				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_C;
			}
			else if("z" == lower_string(tokens[i-1]))
			{
				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i-1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("SH" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.a_index = address;
					instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.out_index = address;

					answer_requires_scratch_heap = false;
				}
				else if("CSH" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP;
					instruction.a_index = address;
				}
				else if("C" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.a_index = address;
				}
				else if("V" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.a_index = address;
				}
			}

			if(true == answer_requires_scratch_heap)
			{
				instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				scratch_heap[ordered_term_index].push_back(quaternion());
				instruction.out_index = scratch_heap[ordered_term_index].size() - 1;

				ostringstream out;
				out << static_cast<long unsigned int>(scratch_heap[ordered_term_index].size() - 1);
				tokens[i-1] = "SH#";
				tokens[i-1] += out.str();
			}

			// setup b
			if("c" == lower_string(tokens[i+1]))
			{
				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_C;
			}
			else if("z" == lower_string(tokens[i+1]))
			{
				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_Z;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i+1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("SH" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.b_index = address;
				}
				else if("CSH" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP;
					instruction.b_index = address;
				}
				else if("C" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.b_index = address;
				}
				else if("V" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.b_index = address;
				}
			}

			instructions[ordered_term_index].push_back(instruction);

			tokens.erase(tokens.begin() + i + 1);
			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}

	for(size_t i = 0; i < tokens.size(); i++)
	{
		if("+" == tokens[i] || "-" == tokens[i])
		{
			tokenized_instruction instruction;

			if("+" == tokens[i])
				instruction.f = &quaternion_math::add;
			else
				instruction.f = &quaternion_math::sub;

			bool answer_requires_scratch_heap = true;

			if("c" == lower_string(tokens[i-1]))
			{
				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_C;
			}
			else if("z" == lower_string(tokens[i-1]))
			{
				instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i-1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("SH" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.a_index = address;
					instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.out_index = address;

					answer_requires_scratch_heap = false;
				}
				else if("CSH" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP;
					instruction.a_index = address;
				}
				else if("C" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.a_index = address;
				}
				else if("V" == src_tokens[0])
				{
					instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.a_index = address;
				}
			}

			if(true == answer_requires_scratch_heap)
			{
				instruction.out_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
				scratch_heap[ordered_term_index].push_back(quaternion());
				instruction.out_index = scratch_heap[ordered_term_index].size() - 1;

				ostringstream out;
				out << static_cast<long unsigned int>(scratch_heap[ordered_term_index].size() - 1);
				tokens[i-1] = "SH#";
				tokens[i-1] += out.str();
			}

			// setup b
			if("c" == lower_string(tokens[i+1]))
			{
				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_C;
			}
			else if("z" == lower_string(tokens[i+1]))
			{
				instruction.b_type = TOKENIZED_INSTRUCTION_DEST_Z;
			}
			else
			{
				vector<string> src_tokens = stl_str_tok("#", tokens[i+1]);

				istringstream in(src_tokens[1]);
				size_t address = 0;
				in >> address;

				if("SH" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
					instruction.b_index = address;
				}
				else if("CSH" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP;
					instruction.b_index = address;
				}
				else if("C" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.b_index = address;
				}
				else if("V" == src_tokens[0])
				{
					instruction.b_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
					instruction.b_index = address;
				}
			}

			instructions[ordered_term_index].push_back(instruction);

			tokens.erase(tokens.begin() + i + 1);
			tokens.erase(tokens.begin() + i);

			i = 0;
		}
	}

	/*
	for(size_t i = 0; i < tokens.size(); i++)
	cout << tokens[i] << " ";

	cout << endl << endl;
	*/

	tokenized_instruction instruction;
	instruction.f = &quaternion_math::copy;

	// setup a
	if("c" == lower_string(tokens[0]))
	{
		instruction.a_type = TOKENIZED_INSTRUCTION_DEST_C;
	}
	else if("z" == lower_string(tokens[0]))
	{
		instruction.a_type = TOKENIZED_INSTRUCTION_DEST_Z;
	}
	else
	{
		vector<string> src_tokens = stl_str_tok("#", tokens[0]);

		istringstream in(src_tokens[1]);
		size_t address = 0;
		in >> address;

		if("SH" == src_tokens[0])
		{
			instruction.a_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
			instruction.a_index = address;
		}
		else if("CSH" == src_tokens[0])
		{
			instruction.a_type = TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP;
			instruction.a_index = address;
		}
		else if("C" == src_tokens[0])
		{
			instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
			instruction.a_index = address;
		}
		else if("V" == src_tokens[0])
		{
			instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
			instruction.a_index = address;
		}
	}


	instruction.out_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
	instruction.out_index = ordered_term_index;

	instructions[ordered_term_index].push_back(instruction);
}

void quaternion_julia_set_equation_parser::setup_final_Z_copy(const string &src_destination)
{
	string destination = lower_string(src_destination);

	if("z" == destination)
	{
		tokenized_instruction instruction;

		instruction.f = &quaternion_math::copy;
		instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
		instruction.a_index = answers.size() - 1;
		instruction.b_type = TOKENIZED_INSTRUCTION_DEST_NULL;
		instruction.out_type = TOKENIZED_INSTRUCTION_DEST_Z;

		instructions[answers.size() - 1].push_back(instruction);
		//cout << "z push" << endl;
	}
	else
	{
		quaternion copy_mask;

		for(size_t j = 2; j < destination.length(); j++)
		{
			if('x' == destination[j])
				copy_mask.x = 1.0;
			else if('y' == destination[j])
				copy_mask.y = 2.0;
			else if('z' == destination[j])
				copy_mask.z = 3.0;
			else
				copy_mask.w = 4.0;
		}

		scratch_heap[answers.size() - 1].push_back(copy_mask);

		tokenized_instruction instruction;

		instruction.f = &quaternion_math::copy_masked;
		instruction.a_type = TOKENIZED_INSTRUCTION_DEST_ANSWER;
		instruction.a_index = answers.size() - 1;
		instruction.b_type = TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP;
		instruction.b_index = scratch_heap[answers.size() - 1].size() - 1;
		instruction.out_type = TOKENIZED_INSTRUCTION_DEST_Z;

		instructions[answers.size() - 1].push_back(instruction);
	}
}


bool quaternion_julia_set_equation_parser::assemble_compiled_instructions(void)
{
	size_t instruction_count = 0;

	for(size_t i = 0; i < instructions.size(); i++)
		instruction_count += instructions[i].size();

	// lala debug
	//		cout << instruction_count << endl;

	execution_stack.resize(instruction_count);

	size_t execution_stack_index = 0;

	for(size_t i = 0; i < instructions.size(); i++)
	{
		for(size_t j = 0; j < instructions[i].size(); j++)
		{
			execution_stack[execution_stack_index].f = instructions[i][j].f;

			// do a
			switch(instructions[i][j].a_type)
			{
			case TOKENIZED_INSTRUCTION_DEST_ANSWER:
				{
					execution_stack[execution_stack_index].a = &answers[instructions[i][j].a_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP:
				{
					execution_stack[execution_stack_index].a = &scratch_heap[i][instructions[i][j].a_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP:
				{
					execution_stack[execution_stack_index].a = &constant_scratch_heap[instructions[i][j].a_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_Z:
				{
					execution_stack[execution_stack_index].a = &Z;
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_C:
				{
					execution_stack[execution_stack_index].a = &C;
					break;
				}
			default: // no nulls allowed in A
				{
					execution_stack.clear();

					return false;
				}
			}

			// do b
			switch(instructions[i][j].b_type)
			{
			case TOKENIZED_INSTRUCTION_DEST_ANSWER:
				{
					execution_stack[execution_stack_index].b = &answers[instructions[i][j].b_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP:
				{
					execution_stack[execution_stack_index].b = &scratch_heap[i][instructions[i][j].b_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP:
				{
					execution_stack[execution_stack_index].b = &constant_scratch_heap[instructions[i][j].b_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_Z:
				{
					execution_stack[execution_stack_index].b = &Z;
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_C:
				{
					execution_stack[execution_stack_index].b = &C;
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_NULL:
				{
					execution_stack[execution_stack_index].b = 0;
					break;
				}
			default: // invalid address
				{
					execution_stack.clear();

					return false;
				}
			}




			// do out
			switch(instructions[i][j].out_type)
			{
			case TOKENIZED_INSTRUCTION_DEST_ANSWER:
				{
					execution_stack[execution_stack_index].out = &answers[instructions[i][j].out_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_TERM_SCRATCH_HEAP:
				{
					execution_stack[execution_stack_index].out = &scratch_heap[i][instructions[i][j].out_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_CONSTANTS_SCRATCH_HEAP:
				{
					execution_stack[execution_stack_index].out = &constant_scratch_heap[instructions[i][j].out_index];
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_Z:
				{
					execution_stack[execution_stack_index].out = &Z;
					break;
				}
			case TOKENIZED_INSTRUCTION_DEST_C: // not allowed in OUT
				{
					execution_stack.clear();

					return false;
				}
			default: // no nulls allowed in OUT
				{
					execution_stack.clear();

					return false;
				}
			}

			execution_stack_index++;
		}
	}

	return true;
}


qmath_func_ptr quaternion_julia_set_equation_parser::get_function_instruction(const string &src_token)
{
	string temp = lower_string(src_token);

	for(size_t i = 0; i < function_map.size(); i++)
		if(function_map[i].function_name == temp)
			return function_map[i].f;

	return 0;
}



void quaternion_julia_set_equation_parser::setup_function_map(void)
{
	function_map.push_back(function_mapping("sin", &quaternion_math::sin));
	function_map.push_back(function_mapping("sinh", &quaternion_math::sinh));
	function_map.push_back(function_mapping("exsin", &quaternion_math::exsin));
	function_map.push_back(function_mapping("exsinh", &quaternion_math::exsinh));
	function_map.push_back(function_mapping("coversin", &quaternion_math::coversin));
	function_map.push_back(function_mapping("coversinh", &quaternion_math::coversinh));
	function_map.push_back(function_mapping("cos", &quaternion_math::cos));
	function_map.push_back(function_mapping("cosh", &quaternion_math::cosh));
	function_map.push_back(function_mapping("excos", &quaternion_math::excos));
	function_map.push_back(function_mapping("excosh", &quaternion_math::excosh));
	function_map.push_back(function_mapping("versin", &quaternion_math::versin));
	function_map.push_back(function_mapping("versinh", &quaternion_math::versinh));
	function_map.push_back(function_mapping("ln", &quaternion_math::ln));
	function_map.push_back(function_mapping("exp", &quaternion_math::exp));
	function_map.push_back(function_mapping("sqrt", &quaternion_math::sqrt));
	function_map.push_back(function_mapping("inverse", &quaternion_math::inverse));
	function_map.push_back(function_mapping("conjugate", &quaternion_math::conjugate));
	function_map.push_back(function_mapping("sinc", &quaternion_math::sinc));
	function_map.push_back(function_mapping("sinhc", &quaternion_math::sinhc));
	function_map.push_back(function_mapping("csc", &quaternion_math::csc));
	function_map.push_back(function_mapping("csch", &quaternion_math::csch));
	function_map.push_back(function_mapping("excsc", &quaternion_math::excsc));
	function_map.push_back(function_mapping("excsch", &quaternion_math::excsch));
	function_map.push_back(function_mapping("covercsc", &quaternion_math::covercsc));
	function_map.push_back(function_mapping("covercsch", &quaternion_math::covercsch));
	function_map.push_back(function_mapping("cscc", &quaternion_math::cscc));
	function_map.push_back(function_mapping("cschc", &quaternion_math::cschc));
	function_map.push_back(function_mapping("cosc", &quaternion_math::cosc));
	function_map.push_back(function_mapping("coshc", &quaternion_math::coshc));
	function_map.push_back(function_mapping("sec", &quaternion_math::sec));
	function_map.push_back(function_mapping("sech", &quaternion_math::sech));
	function_map.push_back(function_mapping("exsec", &quaternion_math::exsec));
	function_map.push_back(function_mapping("exsech", &quaternion_math::exsech));
	function_map.push_back(function_mapping("vercsc", &quaternion_math::vercsc));
	function_map.push_back(function_mapping("vercsch", &quaternion_math::vercsch));
	function_map.push_back(function_mapping("secc", &quaternion_math::secc));
	function_map.push_back(function_mapping("sechc", &quaternion_math::sechc));
	function_map.push_back(function_mapping("tan", &quaternion_math::tan));
	function_map.push_back(function_mapping("tanh", &quaternion_math::tanh));
	function_map.push_back(function_mapping("extan", &quaternion_math::extan));
	function_map.push_back(function_mapping("extanh", &quaternion_math::extanh));
	function_map.push_back(function_mapping("covertan", &quaternion_math::covertan));
	function_map.push_back(function_mapping("covertanh", &quaternion_math::covertanh));
	function_map.push_back(function_mapping("tanc", &quaternion_math::tanc));
	function_map.push_back(function_mapping("tanhc", &quaternion_math::tanhc));
	function_map.push_back(function_mapping("cot", &quaternion_math::cot));
	function_map.push_back(function_mapping("coth", &quaternion_math::coth));
	function_map.push_back(function_mapping("excot", &quaternion_math::excot));
	function_map.push_back(function_mapping("excoth", &quaternion_math::excoth));
	function_map.push_back(function_mapping("covercot", &quaternion_math::covercot));
	function_map.push_back(function_mapping("covercoth", &quaternion_math::covercoth));
	function_map.push_back(function_mapping("cotc", &quaternion_math::cotc));
	function_map.push_back(function_mapping("cothc", &quaternion_math::cothc));
}





bool quaternion_julia_set_equation_parser::is_valid_destination(const string &src_string)
{
	string temp = lower_string(src_string);

	if(temp.size() == 1)
	{
		if(temp == "z")
			return true;
		else
			return false;
	}
	else if(temp.size() > 2 && temp.size() < 7)
	{
		if(temp.substr(0, 2) != "z.")
			return false;

		temp.erase(0, 2);

		for(size_t i = 0; i < temp.size(); i++)
		{
			if(temp[i] != 'x' && temp[i] != 'y' && temp[i] != 'z' && temp[i] != 'w')
			{
				return false;
			}
			else
			{
				// catch stuff like .yx and .yy and (incorrect order and dupes)... correct order is: xyzw (not wxyz)
				for(size_t j = 0; j < i; j++)
				{
					int temp_i_val = 0;
					int temp_j_val = 0;

					if(temp[i] == 'x')
						temp_i_val = 0;
					else if(temp[i] == 'y')
						temp_i_val = 1;
					else if(temp[i] == 'z')
						temp_i_val = 2;
					else if(temp[i] == 'w')
						temp_i_val = 3;

					if(temp[j] == 'x')
						temp_j_val = 0;
					else if(temp[j] == 'y')
						temp_j_val = 1;
					else if(temp[j] == 'z')
						temp_j_val = 2;
					else if(temp[j] == 'w')
						temp_j_val = 3;

					if(temp_i_val <= temp_j_val)
						return false;
				}
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool quaternion_julia_set_equation_parser::is_variable_swizzle(const string &src_string)
{
	string temp = lower_string(src_string);

	if(temp.size() > 2 && temp.size() < 7)
	{
		string temp_prefix = temp.substr(0, 2);

		if(temp_prefix != "z.")
			return false;

		temp.erase(0, 2);

		for(size_t i = 0; i < temp.size(); i++)
			if( temp[i] != 'x' && temp[i] != 'y' && temp[i] != 'z' && temp[i] != 'w')
				return false;
	}
	else
	{
		return false;
	}

	return true;
}

bool quaternion_julia_set_equation_parser::is_constant_swizzle(const string &src_string)
{
	string temp = lower_string(src_string);

	if(temp.size() > 2 && temp.size() < 7)
	{
		string temp_prefix = temp.substr(0, 2);

		if(temp_prefix != "c.")
			return false;

		temp.erase(0, 2);

		for(size_t i = 0; i < temp.size(); i++)
			if( temp[i] != 'x' && temp[i] != 'y' && temp[i] != 'z' && temp[i] != 'w')
				return false;
	}
	else
	{
		return false;
	}

	return true;
}

bool quaternion_julia_set_equation_parser::is_raw_swizzle(const string &src_string)
{
	string temp = lower_string(src_string);

	if(temp.size() > 1 && temp.size() < 6)
	{
		if(temp[0] != '.')
			return false;

		temp.erase(0, 1);

		for(size_t i = 0; i < temp.size(); i++)
			if( temp[i] != 'x' && temp[i] != 'y' && temp[i] != 'z' && temp[i] != 'w')
				return false;
	}
	else
	{
		return false;
	}

	return true;
}


bool quaternion_julia_set_equation_parser::is_function(const string &src_string)
{
	string temp = lower_string(src_string);

	if("quat" == temp)
		return true;

	for(size_t i = 0; i < function_map.size(); i++)
		if(temp == function_map[i].function_name)
			return true;


	return false;
}

bool quaternion_julia_set_equation_parser::is_math_operator(const string &src_string)
{
	if( src_string == "^" ||
		src_string == "*" ||
		src_string == "/" ||
		src_string == "+" ||
		src_string == "-" )
	{
		return true;
	}

	return false;
}

bool quaternion_julia_set_equation_parser::is_constant(const string &src_string)
{
	string temp = lower_string(src_string);

	if( temp == "c" )
	{
		return true;
	}
	//	else if(is_constant_swizzle(temp))
	//	{
	//		return true;
	//	}

	return false;
}

bool quaternion_julia_set_equation_parser::is_variable(const string &src_string)
{
	string temp = lower_string(src_string);

	if(temp == "z")
	{
		return true;
	}
	//	else if(is_variable_swizzle(temp))
	//	{
	//		return true;
	//	}

	return false;
}

bool quaternion_julia_set_equation_parser::is_constant_quaternion(const string &src_string)
{
	if(src_string == "")
		return false;

	string temp = lower_string(src_string);

	// look for commas, split into multiple parts, scan each part to see if it's a real number
	vector<string> reals;
	reals.push_back("");

	// split into seperate reals, assuming ',' is used to differentiate between the quaternion's parts
	for(size_t i = 0; i < temp.size(); i++)
	{
		if(temp[i] != ',')
		{
			reals[reals.size() - 1] += temp[i];
		}
		else
		{
			reals.push_back("");
		}
	}

	if(reals.size() != 4)
		return false;
	/*
	// quaternion must have min 1 part, max 4
	if(reals.size() < 1 || reals.size() > 4)
	return false;

	// if less than 4, add additional parts to make it 4 parts
	for(size_t i = reals.size(); i < 4; i++)
	reals.push_back("");
	*/

	// test that each number is a real number literal
	for(size_t i = 0; i < reals.size(); i++)
	{
		if(reals[i] == "")
			reals[i] = "0.0";

		if(false == is_real_number(reals[i]))
		{
			if( lower_string(reals[i]) != "c.x" &&
				lower_string(reals[i]) != "c.y" &&
				lower_string(reals[i]) != "c.z" &&
				lower_string(reals[i]) != "c.w" &&
				lower_string(reals[i]) != "-c.x" &&
				lower_string(reals[i]) != "-c.y" &&
				lower_string(reals[i]) != "-c.z" &&
				lower_string(reals[i]) != "-c.w")
			{
				return false;
			}
		}
	}

	return true;
}

bool quaternion_julia_set_equation_parser::is_variable_quaternion(const string &src_string)
{
	if(src_string == "")
		return false;

	string temp = lower_string(src_string);

	// look for commas, split into multiple parts, scan each part to see if it's a real number
	vector<string> reals;
	reals.push_back("");

	// split into seperate reals, assuming ',' is used to differentiate between the quaternion's parts
	for(size_t i = 0; i < temp.size(); i++)
	{
		if(temp[i] != ',')
		{
			reals[reals.size() - 1] += temp[i];
		}
		else
		{
			reals.push_back("");
		}
	}

	if(reals.size() != 4)
		return false;
	/*
	// quaternion must have min 1 part, max 4
	if(reals.size() < 1 || reals.size() > 4)
	return false;

	// if less than 4, add additional parts to make it 4 parts
	for(size_t i = reals.size(); i < 4; i++)
	reals.push_back("");
	*/

	// if less than 4, add additional parts to make it 4 parts
	for(size_t i = reals.size(); i < 4; i++)
		reals.push_back("");

	bool found_var = false;

	// test that each number is a real number literal
	for(size_t i = 0; i < reals.size(); i++)
	{
		if(reals[i] == "")
			reals[i] = "0.0";

		if(false == is_real_number(reals[i]))
		{
			if( lower_string(reals[i]) != "c.x" &&
				lower_string(reals[i]) != "c.y" &&
				lower_string(reals[i]) != "c.z" &&
				lower_string(reals[i]) != "c.w" &&
				lower_string(reals[i]) != "z.x" &&
				lower_string(reals[i]) != "z.y" &&
				lower_string(reals[i]) != "z.z" &&
				lower_string(reals[i]) != "z.w" &&
				lower_string(reals[i]) != "-c.x" &&
				lower_string(reals[i]) != "-c.y" &&
				lower_string(reals[i]) != "-c.z" &&
				lower_string(reals[i]) != "-c.w" &&
				lower_string(reals[i]) != "-z.x" &&
				lower_string(reals[i]) != "-z.y" &&
				lower_string(reals[i]) != "-z.z" &&
				lower_string(reals[i]) != "-z.w")
			{
				return false;
			}
			else
			{
				if(	lower_string(reals[i]) == "z.x" ||
					lower_string(reals[i]) == "z.y" ||
					lower_string(reals[i]) == "z.z" ||
					lower_string(reals[i]) == "z.w" ||
					lower_string(reals[i]) == "-z.x" ||
					lower_string(reals[i]) == "-z.y" ||
					lower_string(reals[i]) == "-z.z" ||
					lower_string(reals[i]) == "-z.w" )
				{
					found_var = true;
				}
			}
		}
	}

	return found_var;
}


bool quaternion_julia_set_equation_parser::cleanup_equation_tokens(vector<string> &equation)
{
	bool trimmed_something = false;

	bool trim = true; 

	// trim ( ) at begin/end
	while(equation.size() > 1 && true == trim)
	{
		if(equation[0] != "(" || equation[equation.size()-1] != ")")
		{
			trim = false;
			continue;
		}
		else
		{
			size_t paren_depth = 1;

			for(size_t i = 1; i < equation.size(); i++)
			{
				if(equation[i] == "(")
					paren_depth++;
				if(equation[i] == ")")
					paren_depth--;

				if(paren_depth == 0)
				{
					if(i != equation.size() - 1)
					{
						trim = false;
						break;
					}
					else
					{
						equation.erase(equation.end() - 1);
						equation.erase(equation.begin());
						trimmed_something = true;

						//for(size_t i = 0; i < equation.size(); i++)
						//cout << equation[i] << ' ';
						//cout << endl << endl;

					}
				}
			}
		}
	}

	// trim ( ) where there's no math operators inside and it's not following a function.
	trim = true;
	while(equation.size() > 2 && true == trim)
	{
		trim = false;

		for(size_t i = 0; i < equation.size() - 2 && trim == false; i++)
		{
			// if (C) or (Z)
			if( equation[i] == "(" && 
				(lower_string(equation[i+1]) == "c" || lower_string(equation[i+1]) == "z") &&
				equation[i+2] == ")" )
			{
				// if sine(C), etc, don't do anything
				if(i != 0 && is_function(equation[i-1]))
					continue;

				equation.erase(equation.begin() + i+2);
				equation.erase(equation.begin() + i);

				trim = true;
				trimmed_something = true;

				//for(size_t i = 0; i < equation.size(); i++)
				//cout << equation[i] << ' ';
				//cout << endl << endl;

				break;
			}
		}
	}

	// check if any C / Z .swizzles are next to each other, if so join them
	trim = true;
	while(equation.size() > 2 && true == trim)
	{
		trim = false;

		for(size_t i = 0; i < equation.size() - 1 && trim == false; i++)
		{
			if( (lower_string(equation[i]) == "c" || lower_string(equation[i]) == "z") &&
				is_raw_swizzle(equation[i+1]) )
			{
				equation[i] += equation[i+1];
				equation.erase(equation.begin() + i + 1);

				trim = true;
				trimmed_something = true;

				//for(size_t i = 0; i < equation.size(); i++)
				//cout << equation[i] << ' ';
				//cout << endl << endl;

				break;
			}
		}
	}

	// trim ( ) where there's no math operators inside and it's not following a function and not followed by a raw swizzle
	trim = true;
	while(equation.size() > 2 && true == trim)
	{
		trim = false;

		for(size_t i = 0; i < equation.size() - 2 && trim == false; i++)
		{
			// if (C) or (Z)
			if( equation[i] == "(" && 
				(is_variable_swizzle(equation[i+1]) || is_constant_swizzle(equation[i+1])) &&
				equation[i+2] == ")" )
			{
				// if sine(C), etc, don't do anything
				if(i != 0 && is_function(equation[i-1]))
					continue;

				if(i != equation.size() - 3 && is_raw_swizzle(equation[i+3]))
					continue;

				equation.erase(equation.begin() + i+2);
				equation.erase(equation.begin() + i);

				trim = true;
				trimmed_something = true;

				//for(size_t i = 0; i < equation.size(); i++)
				//cout << equation[i] << ' ';
				//cout << endl << endl;

				break;
			}
		}
	}




	//trim (( )) in equation
	trim = true;
	while(equation.size() > 4 && true == trim)
	{
		trim = false;

		//		0 1 2 3 4
		//		( ( C ) )

		for(size_t i = 0; i < equation.size() - 4 && trim == false; i++)
		{
			if(equation[i] == "(" && equation[i+1] == "(")
			{
				for(size_t j = i + 4; j < equation.size() && trim == false; j++)
				{
					if(equation[j-1] == ")" && equation[j] == ")")
					{
						size_t opening_parenthesis_count = 0;
						size_t closing_parenthesis_count = 0;

						bool found_closing_too_soon = false;

						// check for mismatched parentheses
						for(size_t k = i+2; k < j-1; k++)
						{
							if(equation[k] == "(")
								opening_parenthesis_count++;
							else if(equation[k] == ")")
								closing_parenthesis_count++;

							if(closing_parenthesis_count > opening_parenthesis_count)
							{
								found_closing_too_soon = true;
								break;
							}
						}

						if(false == found_closing_too_soon && opening_parenthesis_count == closing_parenthesis_count)
						{
							equation.erase(equation.begin() + j);
							equation.erase(equation.begin() + i);

							trim = true;
							trimmed_something = true;

							//for(size_t i = 0; i < equation.size(); i++)
							//cout << equation[i] << ' ';
							//cout << endl << endl;
						}
					}
				}
			}
		}
	}


	// trim unnecessary parentheses around functions
	trim = true;
	while(equation.size() > 4 && true == trim)
	{
		trim = false;

		//		0 1 2 3 4
		//		( ( C ) )
		//	    ( sin [ X ] )
		//		0  1  2 3 4 5
		for(size_t i = 0; i < equation.size() - 5 && trim == false; i++)
		{
			if(equation[i] == "(" && is_function(equation[i+1]))
			{
				for(size_t j = i + 4; j < equation.size() - 1 && trim == false; j++)
				{
					if(equation[j] == "]")
					{
						if(equation[j+1] == ")")
						{
							equation.erase(equation.begin() + j+1);
							equation.erase(equation.begin() + i);

							trim = true;
							trimmed_something = true;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}






	// expand swizzles
	for(size_t i = 0; i < equation.size(); i++)
	{
		if(is_raw_swizzle(equation[i]))
		{
			if(equation[i].size() < 5)
			{
				char last = equation[i][equation[i].size() - 1];

				for(size_t j = 0; j < 5 - equation[i].size(); j++)
				{
					equation[i] += last;
				}
			}
		}
		else if(is_variable_swizzle(equation[i]) || is_constant_swizzle(equation[i]))
		{
			if(equation[i].size() < 6)
			{
				char last = equation[i][equation[i].size() - 1];

				for(size_t j = 0; j < 6 - equation[i].size(); j++)
				{
					equation[i] += last;
				}
			}
		}
	}



	// merge adjacent swizzles..
	// ie: (C.xyyz).yzwx == C.yyzx
	trim = true;
	while(equation.size() > 4 && true == trim)
	{
		trim = false;

		//	    ( C.xyzw ) .xyz
		//		0    1   2   3
		for(size_t i = 0; i < equation.size() - 3 && trim == false; i++)
		{
			if( equation[i] == "(" &&
				(is_variable_swizzle(equation[i+1]) || is_constant_swizzle(equation[i+1]) ) &&
				equation[i+2] == ")" &&
				is_raw_swizzle(equation[i+3]) )
			{
				string new_swizzle = ".";

				for(size_t j = 0; j < 4; j++)
				{
					char temp_char = tolower(equation[i+3][j+1]);

					if(temp_char == 'x')
						new_swizzle += equation[i+1][2];
					else if(temp_char == 'y')
						new_swizzle += equation[i+1][3];
					else if(temp_char == 'z')
						new_swizzle += equation[i+1][4];
					else if(temp_char == 'w')
						new_swizzle += equation[i+1][5];
				}

				equation[i+1] = equation[i+1][0] + new_swizzle;

				equation.erase(equation.begin() + i+3);
				equation.erase(equation.begin() + i+2);
				equation.erase(equation.begin() + i);

				trim = true;
				trimmed_something = true;
			}
		}
	}


	return trimmed_something;
}




void quaternion_julia_set_equation_parser::get_terms(vector<string> src_equation, vector<term> &ordered_terms)
{
	vector<term> temp_terms;

	size_t level = 0;
	size_t max_level = 0;

	term temp_term;
	temp_term.level = level;



	// split into different levels
	for(vector<string>::const_iterator i = src_equation.begin(); i != src_equation.end(); i++)
	{
		if(*i == "(")
		{
			temp_term.text.push_back("NEXT");
			temp_terms.push_back(temp_term);

			temp_term.level = ++level;

			if(level > max_level)
				max_level = level;

			temp_term.text.resize(0);
			temp_term.text.push_back("BEGIN");
		}
		else if(*i == ")")
		{
			temp_term.text.push_back("END");
			temp_terms.push_back(temp_term);

			temp_term.level = --level;
			temp_term.text.resize(0);
		}
		else
		{
			if(lower_string(*i) != "quat")
				temp_term.text.push_back(*i);
		}
	}

	if(temp_term.text.size() != 0)
		temp_terms.push_back(temp_term);


	vector<term> final_terms;
	final_terms.resize(max_level + 1);

	for(size_t i = 0; i < temp_terms.size(); i++)
	{
		for(size_t j = 0; j < temp_terms[i].text.size(); j++)
		{
			final_terms[temp_terms[i].level].level = temp_terms[i].level;
			final_terms[temp_terms[i].level].text.push_back(temp_terms[i].text[j]);
		}
	}

	final_terms[0].text.insert(final_terms[0].text.begin(), "BEGIN");
	final_terms[0].text.push_back("END");


	vector< vector <term> > term_tree;
	term_tree.resize(max_level + 1);

	for(vector<term>::const_iterator i = final_terms.begin(); i != final_terms.end(); i++)
	{
		term temp_term;

		for(size_t j = 0; j < i->text.size(); j++)
		{
			if(i->text[j] == "END")
			{
				temp_term.level = i->level;
				term_tree[i->level].push_back(temp_term);
				temp_term.text.clear();
			}
			else
			{
				if(i->text[j] != "BEGIN")
				{
					temp_term.text.push_back(i->text[j]);
				}
			}
		}
	}

	for(size_t i = 0; i < term_tree.size(); i++)
	{
		size_t index = term_tree.size() - 1 - i;

		for(size_t j = 0; j < term_tree[index].size(); j++)
		{
			bool variable = false;

			for(size_t k = 0; k < term_tree[index][j].text.size(); k++)
			{
				if(is_variable(term_tree[index][j].text[k]) ||
					is_variable_swizzle(term_tree[index][j].text[k]) ||
					is_variable_quaternion(term_tree[index][j].text[k]) ||
					(term_tree[index][j].text[k].length() != 0 && term_tree[index][j].text[k][0] == 'V') )
				{
					variable = true;
					break;
				}
			}

			ordered_terms.push_back(term_tree[index][j]);
			ordered_terms[ordered_terms.size() - 1].constant = !variable;

			if(ordered_terms[ordered_terms.size() - 1].text.size() == 1 && (is_variable_quaternion(ordered_terms[ordered_terms.size() - 1].text[0]) || is_constant_quaternion(ordered_terms[ordered_terms.size() - 1].text[0])))
				ordered_terms[ordered_terms.size() - 1].quaternion = true;

			if(index != 0)
			{
				bool done = false;

				for(size_t k = 0; k < term_tree[index-1].size(); k++)
				{
					for(size_t l = 0; l < term_tree[index-1][k].text.size(); l++)
					{
						if(term_tree[index-1][k].text[l] == "NEXT")
						{
							ostringstream out;
							out << static_cast<long unsigned int>(ordered_terms.size() - 1);

							if(variable)
								term_tree[index-1][k].text[l] = "V#";
							else
								term_tree[index-1][k].text[l] = "C#";

							term_tree[index-1][k].text[l] += out.str();

							done = true;
						}

						if(done)
							break;
					}

					if(done)
						break;
				}
			}
		}
	}
}







bool quaternion_julia_set_equation_parser::setup(const string &src_formula, string &error_output, const quaternion &src_C)
{
	cleanup(); // reset everything just in case this isn't the first time through Setup
	error_output = "";

	C = src_C;


	string formula = lower_string(src_formula);


	// get rid of any white space in the formula
	for(string::iterator i = formula.begin(); i != formula.end();)
	{
		if(isspace(*i))
			formula.erase(i);
		else
			i++;
	}


	// find colon, delete up to and including it, this allows the user to put a comment at the beginning of the formula
	size_t pos = formula.find_first_of(':');

	if(pos != string::npos)
		formula.erase(0, pos + 1);


	// replace parentheses characters
	for(string::iterator i = formula.begin(); i != formula.end(); i++)
	{
		if(*i == '[' || *i == '{')
			*i = '(';
		else if(*i == ']' || *i == '}')
			*i = ')';
	}


	// trim unnecessary parentheses at beginning and end of formula (ie: "(Z = Z*Z + C)" -> "Z = Z*Z + C")
	while(formula.length() >= 2)
	{
		if(formula[0] == '(' && formula[formula.length() - 1] == ')')
			formula = formula.substr(1, formula.length() - 2);
		else
			break;
	}

	// find empty ()'s
	for(size_t i = 0; i < formula.length() - 1; i++)
	{
		if(formula[i] == '(' && formula[i+1] == ')')
		{
			error_output = "Unrecognized symbol: '()'";
			return false;
		}
	}


	size_t opening_parenthesis_count = 0;
	size_t closing_parenthesis_count = 0;

	// check for mismatched parentheses
	for(string::iterator i = formula.begin(); i != formula.end(); i++)
	{
		if(*i == '(')
			opening_parenthesis_count++;
		else if(*i == ')')
			closing_parenthesis_count++;

		if(closing_parenthesis_count > opening_parenthesis_count)
		{
			error_output = "Unexpected symbol: ')'";
			return false;
		}
	}


	if(opening_parenthesis_count != closing_parenthesis_count)
	{
		if(opening_parenthesis_count > closing_parenthesis_count)
			error_output = "Unexpected symbol: '('";
		else
			error_output = "Unexpected symbol: ')'";

		return false;
	}


	string destination;

	pos = formula.find_first_of('=');

	if(pos != string::npos)
	{
		destination = formula.substr(0, pos);
		formula.erase(0, pos + 1);
	}

	destination = lower_string(destination);

	if(destination == "")
	{
		destination = "z";
	}
	else if(!is_valid_destination(destination))
	{
		error_output = "Unrecognized destination symbol: " + destination;
		return false;
	}


	// if the formula is (now) blank, abort
	if(formula == "")
	{
		error_output = "Custom formula text is blank.";
		return false;
	}


	// break up string
	vector<string> equation;
	equation.push_back("");


	for(size_t i = 0; i < formula.size(); i++)
	{
		if( formula[i] == '^' ||
			formula[i] == '*' ||
			formula[i] == '/' ||
			formula[i] == '+' ||
			formula[i] == '-' ||
			formula[i] == '(' ||
			formula[i] == ')'  )
		{
			if(equation.size() > 1 && equation[equation.size() - 2] == "^")
			{
				// before this if statement was added, things like Z ^ a - quat(2) would fail
				if(formula[i] == '-' || formula[i] == '.' || tolower(formula[i]) == 'e' || isdigit(formula[i]))
				{
					if(formula[i] == '-' && i != formula.size() - 1 && (formula[i+1] == '.' || tolower(formula[i+1]) == 'e' || isdigit(formula[i+1])))
					{
						// no need to perform any operations on the equation vector yet
						equation[equation.size() - 1] += formula[i];

						// it took me way too long to figure out where to put this statement :)
						continue;
					}
				}
			}

			if(formula[i] != ')')
			{

				// if it's possible that we may be still processing a quaternion constant
				if(equation.size() > 2 && lower_string(equation[equation.size() - 3]) == "quat" && equation[equation.size() - 2] == "(")
				{
					// no need to perform any operations on the equation vector yet
					equation[equation.size() - 1] += formula[i];

					// it took me way too long to figure out where to put this statement :)
					continue;
				}
			}

			if(equation[equation.size() - 1] != "")// && !processing_quat)
				equation.push_back("");

			equation[equation.size() - 1] += formula[i];

			if(i < formula.size() - 1)// && !processing_quat)
				equation.push_back("");
		}
		else
		{
			equation[equation.size() - 1] += formula[i];
		}

	}



	// check to see that only real numbers and a follow the ^ operator
	for(size_t i = 0; i < equation.size(); i++)
	{
		if(equation[i] == "^" && i != equation.size() - 1)
		{
			if(!is_real_number(equation[i+1]) && lower_string(equation[i+1]) != "quat")
			{
				error_output = "Unexpected symbol '" + equation[i+1] + "' following symbol: " + equation[i];
				return false;
			}
		}
	}

	// make pow exponents into quats
	for(size_t i = 0; i < equation.size(); i++)
	{
		if(equation[i] == "^" && i != equation.size() - 1 && lower_string(equation[i+1]) != "quat")
		{
			equation.insert(equation.begin() + i + 2, ")");
			equation.insert(equation.begin() + i + 1, "(");
			equation.insert(equation.begin() + i + 1, "quat");
			i = 0;
			continue;
		}
	}



	// expand quaternion parameters so that quat(Z.x) does not get interpreted as a plain variable later
	for(size_t i = 2; i < equation.size(); i++)
	{
		if(lower_string(equation[i-2]) == "quat")
		{
			size_t comma_count = 0;

			for(size_t j = 0; j < equation[i].size(); j++)
			{
				if(equation[i][j] == ',')
					comma_count++;

				if(comma_count == 3)
					break;
			}

			comma_count = 3 - comma_count;

			for(size_t j = 0; j < comma_count; j++)
				equation[i] += ",0";
		}
	}


	for(size_t i = 0; i < equation.size(); i++)
	{
		if(i > 1 && equation[i-1] == "^" && is_real_number(equation[i]))
		{
			continue;
		}
		else if(is_math_operator(equation[i]))
		{
			// do tests to see that ( - Z + C + ) fails

			if(i == 0)
			{
				error_output = "Unexpected symbol at beginning of formula: " + equation[i];
				return false;
			}
			else if(is_math_operator(equation[i-1]))
			{
				error_output = "Unexpected symbol '" + equation[i] + "' following symbol: " + equation[i-1];
				return false;
			}
			else if(equation[i-1] == "(")
			{
				error_output = "Unexpected symbol '" + equation[i] + "' following symbol: " + equation[i-1];
				return false;
			}
			else if(i == equation.size() - 1)
			{
				error_output = "Unexpected symbol at end of formula: " + equation[i];
				return false;
			}

			continue;
		}
		else if(is_variable(equation[i]) || is_variable_swizzle(equation[i]) || is_constant(equation[i]) || is_constant_swizzle(equation[i]))
		{
			continue;
		}
		else if(equation[i] == "(" || equation[i] == ")")
		{
			if(equation[i] == "(" && i != 0 && !is_math_operator(equation[i-1]) && !is_function(equation[i-1]) && equation[i-1] != "(")
			{
				error_output  = "Unexpected symbol '" + equation[i] + "' following symbol: " + equation[i-1];
				return false;
			}
			else if(equation[i] == ")" && i != 0 && is_math_operator(equation[i-1]))
			{
				error_output = "Unexpected symbol '" + equation[i] + "' following symbol: " + equation[i-1];
				return false;
			}

			continue;
		}
		else if(is_constant_quaternion(equation[i]))
		{
			string last_function_name = "";

			for(size_t j = 0; j < i; j++)
			{
				if(is_function(equation[j]))
					last_function_name = equation[j];
			}

			if(lower_string(last_function_name) == "quat")
				continue;
			else
			{
				error_output = "Unexpected symbol '" + equation[i] + "'";
				return false;
			}
		}
		else if(is_variable_quaternion(equation[i]))
		{
			string last_function_name = "";

			for(size_t j = 0; j < i; j++)
			{
				if(is_function(equation[j]))
					last_function_name = equation[j];
			}

			if(lower_string(last_function_name) == "quat")
				continue;
			else
			{
				error_output = "Unexpected symbol '" + equation[i] + "'";
				return false;
			}
		}
		else if(is_raw_swizzle(equation[i]) && i != 0 && equation[i-1] == ")")
		{
			continue;
		}
		else if(is_function(equation[i]))
		{
			if(i == equation.size() - 1)
			{
				error_output = "Unexpected function symbol: " + equation[i];
				return false;
			}
			else if(equation[i+1] != "(")
			{
				error_output = "Function symbol not followed by '(': " + equation[i];
				return false;
			}
			else if(i != 0)
			{
				if( equation[i-1] != "(" &&
					!is_math_operator(equation[i-1]) )
				{
					error_output = "Unexpected symbol '" + equation[i] + "' following symbol: " + equation[i-1];
					return false;
				}
			}

			continue;
		}
		else
		{
			if(equation[i][equation[i].size()-1] == '=')
				error_output = "Unexpected symbol: " + equation[i];
			else
				error_output = "Unrecognized symbol: " + equation[i];

			return false;
		}
	}




	// convert function ()'s to []'s
	if(equation.size() > 1)
		for(size_t i = 1; i < equation.size() - 2; i++)
		{
			if(equation[i] == "(" && is_function(equation[i-1]))
			{
				equation[i] = "[";

				size_t paren_depth = 0;

				for(size_t j = i+1; j < equation.size(); j++)
				{
					if(equation[j] == "(")
					{
						paren_depth++;
					}
					else if(equation[j] == ")")
					{
						if(paren_depth == 0)
						{
							equation[j] = "]";
							break;
						}
						else
						{
							paren_depth--;
						}
					}
				}
			}
		}



		// keep trimming, though i have never seen any equation that needs more than one trim
		while(cleanup_equation_tokens(equation));

		if(equation.size() == 0)
		{
			error_output = "Processed formula contains no tokens. This is a bug, although I'm sure you deliberately tried to break the tokenizer. Bad user, bad bad bad. :)";
			return false;
		}

		// convert function ()'s to []'s
		for(size_t i = 0; i < equation.size(); i++)
		{
			if(equation[i] == "[")
				equation[i] = "(";
			else if(equation[i] == "]")
				equation[i] = ")";
		}





		unique_formula_string = destination;
		unique_formula_string += "=";



		for(size_t i = 0; i < equation.size(); i++)
		{


			unique_formula_string += equation[i];

		}
		//	cout << endl;



		vector<term> ordered_terms;
		get_terms(equation, ordered_terms);




		//for(size_t i = 0; i < ordered_terms.size(); i++)
		//{
		//	if(ordered_terms[i].constant == false)
		//		cout << "V";
		//	else
		//		cout << "C";

		//	if(ordered_terms[i].quaternion == false)
		//		cout << "E";
		//	else
		//		cout << "Q";

		//	cout << static_cast<long unsigned int>(i) << ": ";

		//	for(size_t j = 0; j < ordered_terms[i].text.size(); j++)
		//		cout << ordered_terms[i].text[j] << " ";

		//	cout << endl;
		//}

		if(false == compile_ordered_terms(ordered_terms))
		{
			error_output = "Internal compile error.";
			return false;
		}

		setup_final_Z_copy(destination);


		if(false == assemble_compiled_instructions())
		{
			error_output = "Internal assembly error.";
			return false;
		}

		return true;
}

bool quaternion_julia_set_equation_parser::compile_ordered_terms(const vector<term> &ordered_terms)
{
	try
	{
		answers.reserve(ordered_terms.size());
		answers.resize(ordered_terms.size());

		instructions.reserve(ordered_terms.size());
		instructions.resize(ordered_terms.size());

		scratch_heap.reserve(ordered_terms.size());
		scratch_heap.resize(ordered_terms.size());
	}
	catch(...)
	{
		return false;
	}

	// set up quaternions and single term equations
	for(size_t i = 0; i < ordered_terms.size(); i++)
	{
		if(ordered_terms[i].text.size() != 1)
			continue;

		bool is_last_term = false;

		if(i == ordered_terms.size() - 1)
			is_last_term = true;

		if(ordered_terms[i].quaternion == true)
		{
			vector<string> tokens = stl_str_tok(",", ordered_terms[i].text[0]);

			if(tokens.size() != 4)
				return false;

			if(ordered_terms[i].constant == true)
			{
				setup_constant_quaternion(tokens, i);
				//				cout << ordered_terms[i].text[0] << endl;
				//				cout << answers[i].x << " " << answers[i].y << " " << answers[i].z << " " << answers[i].w << endl;
				//				cout << endl;
			}
			else
			{
				setup_variable_quaternion(tokens, i);
				//				cout << ordered_terms[i].text[0] << endl;
				//				cout << answers[i].x << " " << answers[i].y << " " << answers[i].z << " " << answers[i].w << endl;
				//				cout << scratch_heap[i][0].x << " " << scratch_heap[i][0].y << " " << scratch_heap[i][0].z << " " << scratch_heap[i][0].w << endl;
				//				cout << endl;
			}
		}
		else
		{
			if(ordered_terms[i].constant == true)
			{
				setup_single_constant_equation(ordered_terms[i].text[0], i);
				//				cout << answers[i].x << " " << answers[i].y << " " << answers[i].z << " " << answers[i].w << endl;
				//				cout << endl;
			}
			else
			{
				setup_single_variable_equation(ordered_terms[i].text[0], i);
				//				if(instructions[i][0].f == &quaternion_math::swizzle)
				//					cout << scratch_heap[i][0].x << " " << scratch_heap[i][0].y << " " << scratch_heap[i][0].z << " " << scratch_heap[i][0].w << endl;
				//				else
				//					cout << "copy" << endl;
				//
				//				cout << endl;
			}
		}
	}




	// setup multipart terms (quaternions and single part terms are done in the loop above)
	for(size_t i = 0; i < ordered_terms.size(); i++)
	{
		if(ordered_terms[i].text.size() == 1)
			continue;

		if(ordered_terms[i].constant == true)
		{
			setup_constant_equation(ordered_terms[i].text, i);
			//			cout << ordered_terms[i].text[0] << endl;
			//			cout << answers[i].x << " " << answers[i].y << " " << answers[i].z << " " << answers[i].w << endl;
			//			cout << endl;

		}
		else
		{
			setup_variable_equation(ordered_terms[i].text, i);
			//			cout << ordered_terms[i].text[0] << endl;
			//			cout << answers[i].x << " " << answers[i].y << " " << answers[i].z << " " << answers[i].w << endl;
			//			cout << scratch_heap[i][0].x << " " << scratch_heap[i][0].y << " " << scratch_heap[i][0].z << " " << scratch_heap[i][0].w << endl;
			//			cout << endl;
		}
	}





	return true;
}

