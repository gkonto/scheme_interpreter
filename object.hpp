#ifndef SCHEME_HPP
#define SCHEME_HPP

#define NEW_FUNCTION

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "node.hpp"
enum ObjectType
{
	TT_FIXNUM,         // class Fixnum
	TT_BOOLEAN,        // class Boolean
	TT_CHARACTER,      // class Char
	TT_STRING,         // class String
	TT_THE_EMPTY_LIST, // class EmptyList
	TT_SYMBOL,         // class Symbol
	TT_PAIR,           // class Pair
	TT_PRIMITIVE_PROC, // class PrimitiveProc
	TT_COMPOUND_PROC,  // class CompoundProc
	TT_INPUT_PORT,     // class InputPort
	TT_OUTPUT_PORT,    // class OutputPort
	TT_EOF_OBJECT,     // class Eof

	TT_MAX
};

struct Pair_old
{
	struct Object *car;
	struct Object *cdr;
};

struct CompoundProc_old
{
	struct Object *parameters;
	struct Object *body;
	struct Object *env;
};


//TODO make it class
struct Object 
{
	//TODO convert to std::function
	typedef Object *(*PrimitiveProcFun)(struct Object *arguments);

	//TODO ObjectType can be defined implicitly
	explicit Object(const long value, ObjectType type) : long_value_(value), type_(type)
       	{
		if (type == TT_CHARACTER) {
			char_value_ = value;		}
		//std::cout << "TT_FIXNUM" << std::endl;
		//printf("Object: %p\n", this);
	}
	explicit Object(const bool value, ObjectType type) : bool_value_(value), type_(type) 
	{
		//std::cout << "TT_BOOLEAN" << std::endl;
		//printf("Object: %p\n", this);
	}

	explicit Object(Object *parameters, Object *body, Object *env)
	{
		type_ = TT_COMPOUND_PROC;
		compound_proc.parameters = parameters;
		compound_proc.body = body;
		compound_proc.env = env;
	}

	explicit Object(const char value, ObjectType type) : char_value_(value), type_(type)
	{
		//std::cout << "TT_CHARACTER" << std::endl;
		//printf("Object: %p\n", this);
	}

	explicit Object(const std::string &value, ObjectType type) : str_value_(value), type_(type)
	{
		//std::cout << "TT_STRING" << std::endl;
		//printf("Object: %p\n", this);
	}

	explicit Object(ObjectType type) : type_(type)
	{
		//std::cout << "TT_THE_EMPTY_LIST" << std::endl;
		//printf("Object: %p\n", this);
	}
	
	explicit Object(PrimitiveProcFun fun) : fun_(fun), type_(TT_PRIMITIVE_PROC)
	{
		//std::cout << "TT_PRIMITIVE_PROC" << std::endl;
		//printf("Object: %p\n", this);
	}

	explicit Object(Object *car, Object *cdr) : type_(TT_PAIR)
	{
		//std::cout << "TT_PAIR" << std::endl;
		//printf("Object: %p\n", this);
		pair.car = car;
		pair.cdr = cdr;
	}

	explicit Object() :
		long_value_(0),
		bool_value_(0),
		char_value_(' '),
		in_(0)
	{

	}



	//TODO make private
		long long_value_;
		bool bool_value_;
		char char_value_;
		std::string str_value_;
		Pair_old pair;
		PrimitiveProcFun fun_;
		CompoundProc_old compound_proc;
		FILE *in_;
		FILE *out_;

	// TODO type not needed.. 
	ObjectType type_;
};

// OBJECT DECLARATIONS 
//extern make global static
namespace global {
//	extern Boolean   *n_false_obj;
//	extern Boolean   *n_true_obj; 
//	extern EmptyList *n_the_empty_list;

	extern Object *false_obj;
	extern Object *true_obj; 
	extern Object *the_empty_list;
	extern Object *symbol_table;
	extern Object *quote_symbol;
	extern Object *set_symbol;
	extern Object *ok_symbol;
	extern Object *the_empty_environment;
	extern Object *the_global_environment;
	extern Object *if_symbol;
	extern Object *lambda_symbol;
	extern Object *let_symbol;
	extern Object *and_symbol;
	extern Object *or_symbol;
	extern Object *eof_object;
};




// Object callbacks
bool is_boolean(Object *obj);
bool is_false(Object *obj);
bool is_true(Object *obj);
bool is_fixnum(Object *obj);
bool is_char(Object *obj);
bool is_string(Object *obj);
bool is_symbol(Object *obj);
bool is_initial(int c);
bool is_the_empty_list(Object *obj);
bool is_primitive_proc(Object *obj);


//pair
bool is_pair(Object *obj);
Object *car(Object *pair);
void set_car(Object *pair, Object *value);
Object *cdr(Object *pair);
void set_cdr(Object *pair, Object *value);

Object *read(std::istream &in);
Object *read_pair(std::istream &in);


void init(void);
char is_delimiter(int c);
void eat_whitespace(std::istream &in);

Object *eval(Object *exp, Object *env);

std::string write(std::ostream &out, Object *obj);
//TODO make static
std::string write_pair(Object *obj);


//TODO make symbol table e separate class
Object *make_symbol(const std::string &value);


#endif
