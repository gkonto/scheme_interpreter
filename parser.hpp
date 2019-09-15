#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>

class Node;
class Pair;
class Char;
class List;

class Parser
{
	public:
		Parser(std::istream &in) : in_(in) {}
		class Node *read();
	private:
		bool is_delimiter(int c);	
		bool is_initial(int c);
		void eat_whitespace();
		class Char *read_character();
		void eat_expected_string(const std::string &to_compare);
		void peek_expected_delimiter();

		class Node *read_pair();
		std::istream &in_;
};


#endif
