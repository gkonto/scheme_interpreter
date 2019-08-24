#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "object.hpp"

/**************************** MODEL ******************************/
Object *false_obj = 0;
Object *true_obj  = 0;
Object *the_empty_list = 0;

Object *car(Object *pair)
{
	return pair->pair.car;
}

void set_car(Object *pair, Object *value)
{
	pair->pair.car = value;
}

Object *cdr(Object *pair)
{
	return pair->pair.cdr;
}

void set_cdr(Object *pair, Object *value)
{
	pair->pair.cdr = value;
}

#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))


Object *read_pair(std::istream &in)
{
	Object *car;
	Object *cdr;

	eat_whitespace(in);

	int c = in.get();
	if (c == ')') {
		return the_empty_list;
	}
	in.unget();

	car = read(in);

	eat_whitespace(in);
	
	c = in.get();
	if (c == '.') {
		c = in.peek();
		if (!is_delimiter(c)) {
			std::cerr << "dot not followed by delimiter" << std::endl;
			exit(1);
		}
		cdr = read(in);
		eat_whitespace(in);
		c = in.get();
		if (c != ')') {
			std::cerr << "where was the trailing right paren?" << std::endl;
			exit(1);
		}
		return new Object(car, cdr);
	} else {
		in.unget();
		cdr = read_pair(in);
		return new Object(car, cdr);
	}
}


bool is_pair(Object *obj)
{
	return obj->type_ == TT_PAIR;
}

bool is_boolean(Object *obj)
{
	return obj->type_ == TT_BOOLEAN;
}

bool is_the_empty_list(Object *obj)
{
	return obj == the_empty_list;
}

/*****************************************************************/
bool  is_false(Object *obj)
{
	return obj == false_obj;
}

/*****************************************************************/
bool is_true(Object *obj)
{
	return !is_false(obj);
}

/*****************************************************************/
/* no GC so truely "unlimited extent" */
bool is_fixnum(Object *obj)
{
	return obj->type_ == TT_FIXNUM;
}

/*****************************************************************/
bool is_character(Object *obj)
{
	return obj->type_ == TT_CHARACTER;
}

bool is_string(Object *obj)
{
	return obj->type_ == TT_STRING;
}


/*****************************************************************/
void init(void)
{
	false_obj = new Object(false, TT_BOOLEAN);
	true_obj  = new Object(true, TT_BOOLEAN);
	the_empty_list = new Object(TT_THE_EMPTY_LIST);
}
/***************************** READ ******************************/

char is_delimiter(int c)
{
	return isspace(c) || c == EOF ||
		c == '('   || c == ')' ||
		c == '"'   || c == ';';
}

/*****************************************************************/

void eat_whitespace(std::istream &in)
{
	int c;

	while ((c = in.get()) != EOF)
	{
		if (isspace(c)) {
			continue;
		} else if (c == ';') { /* comments are whitespace also */
			while (((c = in.get()) != EOF) && (c != '\n'));
			continue;
		}

		in.unget();
		break;
	}
}

/*****************************************************************/
void eat_expected_string(std::istream &in, const std::string &to_compare)
{
	//std::string got = in.str();

	std::string got;
	in >> got;

	if (got.compare(to_compare)) {
		std::cerr << "unexpected character" << to_compare << std::endl;	
		exit(1);
	}
}

/*****************************************************************/
void peek_expected_delimiter(std::istream &in) 
{
	if (!is_delimiter(in.peek())) {
		std::cerr << "character not followed by delimiter" << std::endl;
		exit(1);
	}
}

/*****************************************************************/

Object *read_character(std::istream &in)
{
	char c = in.get();
	switch (c) {
		case EOF:
			std::cerr << "incomplete character literal" << std::endl;
			exit(1);
		case 's':
			if (in.peek() == 'p') {
				eat_expected_string(in, "pace");
				peek_expected_delimiter(in);
				return new Object(' ', TT_CHARACTER);
			}
			break;
		case 'n':
			if (in.peek() == 'e') {
				eat_expected_string(in, "ewline");
				peek_expected_delimiter(in);
				return new Object('\n', TT_CHARACTER);
			}
			break;
	}
	peek_expected_delimiter(in);

	return new Object(c, TT_CHARACTER);

}

/*****************************************************************/

Object *read(std::istream &in) 
{
	short sign = 1;
	long num = 0;

	eat_whitespace(in);

	int c = in.get();

	if (c == '#') {
		c = in.get();
		switch(c) {
			case 't':
				return true_obj;
			case 'f':
				return false_obj;
			case '\\':
				return read_character(in);
			default:
				std::cerr << "uknown boolean literal" << std::endl;
				exit(1);
		}
	} else if (isdigit(c) || (c == '-' && (isdigit(in.peek())))) {
		// read a fixnum
		if (c == '-') {
			sign = -1;
		} else {
			in.unget();
		}

		while (isdigit(c = in.get())) {
			num = (num * 10) + (c - '0');
		}
		num *= sign;

		if (is_delimiter(c)) {
			in.unget();
			return new Object(num, TT_FIXNUM);
		} else {
			std::cerr << "number not followed by delimiter" << std::endl;
			exit(1);
		}
	} else if (c == '"') { 
		std::string str;
		str += '"';

		while ((c = in.get()) != '"') {
			if (c == '\\') {
				str += '\\';
				c = in.get();
				/*
				if (c == 'n') {
					c = '\n';
				}
				*/
				if (c == '"') {
					c = '"';
				}

			}
			if (c == '\n') {
				str += "\\n";
				continue;
			}
			if (c == EOF) {
				std::cerr << "non-terminated string liteal" << std::endl;
				exit(1);
			}
			str += c;
		}
		str += '"';
		return new Object(str, TT_STRING);

	} else if (c == '(') {
		return read_pair(in);
	} else {
		char i = c;	
		std::cerr << "unexpected character " << i << ". Expecting ')' " << std::endl;
	}

	std::cerr <<  "read illegal state" << std::endl;
	exit(1);
}

/*************************** EVALUATE ****************************/

/* until we have lists and symbols just echo */
Object *eval(Object *exp)
{
	return exp;
}

/**************************** PRINT ******************************/
std::string write_pair(Object *pair)
{
	Object *car_obj;
	Object *cdr_obj;
	car_obj = car(pair);
	cdr_obj = cdr(pair);

	std::string ret;

	ret += write(car_obj);
	if (cdr_obj->type_ == TT_PAIR) {
		ret += " ";
		ret += write_pair(cdr_obj);
	} else if (cdr_obj->type_ == TT_THE_EMPTY_LIST) {
		return ret;
	} else {
		ret += " . ";
		ret += write(cdr_obj);
	}
	return ret;
}

std::string write(Object *obj)
{
	std::stringstream ss;

	switch (obj->type_)
	{
		case TT_THE_EMPTY_LIST: {
			return "()";
		}
		case TT_FIXNUM: {
			ss << obj->long_value_;
			return ss.str();
		}
		case TT_BOOLEAN: {
			std::string val = is_false(obj) ? "#f" : "#t";
			ss << val;
			return ss.str();
		}
		case TT_CHARACTER: {
			char c = obj->char_value_;
			std::string ret("#\\");
			switch (c) {
				case '\n':
					ret.append("newline");
					return ret;
				case ' ':
					ret.append("space");
					return ret;
				default:
					ret += c;
					return ret;

			}
		}
		case TT_STRING: {
					std::string str = obj->str_value_;
					return str;
				}
		case TT_PAIR: {
				      std::string ret = "(";
				      ret += write_pair(obj);
				      ret += ")";
				      return ret;
				      }
		default: {
			std::cout << obj->type_ << std::endl;
			std::cerr << "cannot write unknown type" << std::endl;
			exit(1);
		}
	}
} /* write */

/***************************** REPL ******************************/


