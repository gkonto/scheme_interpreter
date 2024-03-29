#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.hpp"
#include "node.hpp"

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
		Parser parser(std::cin);
		Node *exp = parser.read();
		if (exp == nullptr)
		{
			break;
		}
		Node *evaluated = exp->eval(gb::n_the_global_environment);
		evaluated->write(std::cout);
		std::cout << std::endl;
	
	} /* while  */

	return 0;

}
/*****************************************************************/

