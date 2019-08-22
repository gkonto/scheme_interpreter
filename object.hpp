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

	TT_MAX
};

struct Object 
{
	explicit Object(long value, ObjectType type) : long_value_(value), type_(type) {}
	explicit Object(bool value, ObjectType type) : bool_value_(value), type_(type) {}
	explicit Object(char value, ObjectType type) : char_value_(value), type_(type) {}

	union
	{
		long long_value_;
		bool bool_value_;
		char char_value_;
	};
	ObjectType type_;
};

extern Object *false_obj;
extern Object *true_obj; 


// Object callbacks
bool is_boolean(Object *obj);
bool is_false(Object *obj);
bool is_true(Object *obj);
bool is_fixnum(Object *obj);
Object *read(std::istream &in);


void init(void);
char is_delimiter(int c);
void eat_whitespace(std::istream &in);

Object *eval(Object *exp);
std::string write(Object *obj);


#endif
