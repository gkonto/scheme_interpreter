#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "object.hpp"

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

