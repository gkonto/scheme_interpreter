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
		eat_whitespace(in);
		c = in.get();
		if (c == ')') {
			return the_empty_list;
		} else {
			char i = c;	
			std::cerr << "unexpected character " << i << ". Expecting ')' " << std::endl;
		}

	} else {
		std::cerr << "bab input. Unexpected" << (char)c << std::endl;
		exit(1);
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
		default: {
			std::cout << obj->type_ << std::endl;
			std::cerr << "cannot write unknown type" << std::endl;
			exit(1);
		}
	}
} /* write */

/***************************** REPL ******************************/


