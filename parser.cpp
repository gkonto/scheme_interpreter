#include "parser.hpp"
#include "node.hpp"

Node *Parser::read_pair()
{
	eat_whitespace();

	int c = in_.get();
	if (c == ')') {
		return gb::n_the_empty_list;
	}
	in_.unget();

	Node *car = read();

	eat_whitespace();
	
	c = in_.get();
	if (c == '.') {
		c = in_.peek();
		if (!is_delimiter(c)) {
			std::cerr << "dot not followed by delimiter" << std::endl;
			exit(1);
		}
		Node *cdr = read();
		eat_whitespace();
		c = in_.get();
		if (c != ')') {
			std::cerr << "where was the trailing right paren?" << std::endl;
			exit(1);
		}
		return new Pair(car, cdr);
	} else {
		in_.unget();

		Node *cdr = read_pair();

		return new Pair(car, cdr);
	}

}

bool Parser::is_delimiter(int c)
{
	return isspace(c) || c == EOF ||
		c == '('   || c == ')' ||
		c == '"'   || c == ';';
}

bool Parser::is_initial(int c)
{
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
}

void Parser::eat_expected_string(const std::string &to_compare)
{
	std::string got;
	in_ >> got;

	if (got.compare(to_compare)) {
		std::cerr << "unexpected character" << to_compare << std::endl;	
		exit(1);
	}
}

Char *Parser::read_character()
{
	char c = in_.get();
	switch (c) {
		case EOF:
			std::cerr << "incomplete character literal" << std::endl;
			exit(1);
		case 's':
			if (in_.peek() == 'p') {
				eat_expected_string("pace");
				peek_expected_delimiter();
				return new Char(' ');
			}
			break;
		case 'n':
			if (in_.peek() == 'e') {
				eat_expected_string("ewline");
				peek_expected_delimiter();
				return new Char('\n');
			} break;
	}
	peek_expected_delimiter();

	return new Char(c);
}

void Parser::peek_expected_delimiter()
{
	if (!is_delimiter(in_.peek())) {
		std::cerr << "character not followed by delimiter" << std::endl;
		exit(1);
	}
}


void Parser::eat_whitespace()
{
	int c;

	while ((c = in_.get()) != EOF)
	{
		if (isspace(c)) {
			continue;
		} else if (c == ';') { /* comments are whitespace also */
			while (((c = in_.get()) != EOF) && (c != '\n'));
			continue;
		}

		in_.unget();
		break;
	}
}


Node *Parser::read()
{
	short sign = 1;
	long num = 0;
	std::string buffer;

	eat_whitespace();

	int c = in_.get();

	if (c == '#') {
		c = in_.get();
		switch(c) {
			case 't':
				return gb::n_true_obj;
			case 'f':
				return gb::n_false_obj;
			case '\\':
				return read_character();
			default:
				std::cerr << "uknown boolean literal" << std::endl;
				exit(1);
		}
	} else if (isdigit(c) || (c == '-' && (isdigit(in_.peek())))) {
		// read a fixnum
		if (c == '-') {
			sign = -1;
		} else {
			in_.unget();
		}

		while (isdigit(c = in_.get())) {
			num = (num * 10) + (c - '0');
		}
		num *= sign;

		if (is_delimiter(c)) {
			in_.unget();
			return new Fixnum(num);
		} else {
			std::cerr << "number not followed by delimiter" << std::endl;
			exit(1);
		}
	} else if (is_initial(c) || 
		((c == '+' || c == '-')
		 && is_delimiter(in_.peek())))
	{
		while (is_initial(c) || isdigit(c) || c == '+' || c == '-') {
			buffer += c;
			c = in_.get();
		}
		if (is_delimiter(c)) {
			in_.unget();
			return SymbolTable::make_symbol(buffer); } else {
			char i = c;
			std::cerr << "symbol not followed by delimiter. "
				"Found " << i << std::endl;
		}
	} else if (c == '"') { 
		std::string str;
		str += '"';

		while ((c = in_.get()) != '"') {
			if (c == '\\') {
				str += '\\';
				c = in_.get();
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
		return new String(str);

	} else if (c == '(') {
		return read_pair();
	} else if (c == '\'') {
		return new Pair(gb::n_quote_symbol, new Pair(read(), gb::n_the_empty_list));
	} else {
		char i = c;	
		std::cerr << "unexpected character " << i << ". Expecting ')' " << std::endl;
	}

	std::cerr <<  "read illegal state" << std::endl;
	exit(1);

}
