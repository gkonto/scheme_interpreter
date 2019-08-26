#ifndef SCHEME_HPP
#define SCHEME_HPP

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

enum ObjectType
{
	TT_FIXNUM,
	TT_BOOLEAN,
	TT_CHARACTER,
	TT_STRING,
	TT_THE_EMPTY_LIST,
	TT_SYMBOL,
	TT_PAIR,
	TT_PRIMITIVE_PROC,

	TT_MAX
};

struct Pair
{
	struct Object *car;
	struct Object *cdr;
};


//TODO make it class
struct Object 
{
	//TODO convert to std::function
	typedef Object *(*PrimitiveProcFun)(struct Object *arguments);

	//TODO ObjectType can be defined implicitly
	explicit Object(const long value, ObjectType type) : long_value_(value), type_(type)
       	{
		//std::cout << "TT_FIXNUM" << std::endl;
		//printf("Object: %p\n", this);
	}
	explicit Object(const bool value, ObjectType type) : bool_value_(value), type_(type) 
	{
		//std::cout << "TT_BOOLEAN" << std::endl;
		//printf("Object: %p\n", this);
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



	//TODO make private
	union {
		long long_value_;
		bool bool_value_;
		char char_value_;
		std::string str_value_;
		Pair pair;
		PrimitiveProcFun fun_;
	};

	ObjectType type_;
};

// OBJECT DECLARATIONS 
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

std::string write(Object *obj);
//TODO make static
std::string write_pair(Object *obj);


//TODO make symbol table e separate class
Object *make_symbol(const std::string &value);


#endif
