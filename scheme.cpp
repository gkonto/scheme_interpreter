#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**************************** MODEL ******************************/
enum ObjectType
{
	TT_FIXNUM
};

/*****************************************************************/

struct Object 
{
	Object(long value) : value_(value), type_(TT_FIXNUM) {}

	union
	{
		long value_;
	};
	ObjectType type_;
};

/*****************************************************************/

/* no GC so truely "unlimited extent" */
char is_fixnum(Object *obj)
{
	return obj->type_ == TT_FIXNUM;
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

	if (isdigit(c) || (c == '-' && (isdigit(in.peek())))) 
	{

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
			return new Object(num);
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
		case TT_FIXNUM:
			std::cout << obj->value_;
			break;
		default:
			std::cerr << "cannot write unknown type" << std::endl;
			exit(1);
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
		write(eval(read(std::cin)));
		std::cout << std::endl;
	} /* while  */

	return 0;
}

/*****************************************************************/

