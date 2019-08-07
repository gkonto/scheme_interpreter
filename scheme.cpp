#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**************************** MODEL ******************************/

enum ObjectType
{
	TT_FIXNUM,
	TT_BOOLEAN

};

/*****************************************************************/

struct Object 
{
	Object(long value, ObjectType type) : value_(value), type_(type) {}

	union
	{
		long value_;
	};
	ObjectType type_;
};

Object *false_obj;
Object *true_obj ;

/*****************************************************************/
bool is_boolean(Object *obj)
{
	return obj->type_ == TT_BOOLEAN;
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
void init(void)
{
	false_obj = new Object(0, TT_BOOLEAN);
	true_obj  = new Object(1, TT_BOOLEAN);
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

void write(Object *obj)
{
	switch (obj->type_)
	{
		case TT_FIXNUM: {
			std::cout << obj->value_;
			break;
		}
		case TT_BOOLEAN: {
			char val = is_false(obj) ? 'f' : 't';
			std::cout << val;
			break;
		}
		default: {
			std::cerr << "cannot write unknown type" << std::endl;
			exit(1);
		}
	}
} /* write */

/***************************** REPL ******************************/

int main(void) 
{
	printf("Welcome to Bootstrap Scheme. "
		"Use ctrl-c to exit.\n");

	while (1)
	{
		std::cout << "> ";
		//FIXME Note: std::cin reads from terminal.
		// I discards any whitespace character, and then reads until the first whitespace occured.
		// That is, if input is "          Hello World!          ",
		// in iostream will be stored: "Hello"
		// That is, no eat_whitespace is needed.
		write(eval(read(std::cin)));
		std::cout << std::endl;
	} /* while  */

	return 0;
}

/*****************************************************************/

