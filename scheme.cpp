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

	init();
	while (1)
	{
		std::cout << "> ";
		//FIXME Note: std::cin reads from terminal.
		// I discards any whitespace character, and then reads until the first whitespace occured.
		// That is, if input is "          Hello World!          ",
		// in iostream will be stored: "Hello"
		// That is, no eat_whitespace is needed.
		std::cout << write(eval(read(std::cin), the_global_environment)) << std::endl;
	} /* while  */

	return 0;
}

/*****************************************************************/

