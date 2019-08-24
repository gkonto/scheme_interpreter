
#include <iostream>
#include <string>
#include <sstream>

#include "../object.hpp"


class Test_SchemeInterpreter
{
	public:
		Test_SchemeInterpreter()
		{
			init();
			_test("123", "123");
			_test("-123", "-123");
			_test("007", "7");
			
			// booleans
			_test("#t", "#t");
			_test("#f", "#f");
	

			// chars
			_test("#\\a", "#\\a");
			_test("#\\newline", "#\\newline");
			_test("#\\\n", "#\\newline");
			_test("#\\space", "#\\space");
			_test("#\\ ", "#\\space");

			//string
			_test("\"asdf\"", "\"asdf\"");
			_test("\"asdf\\\"asdf\"", "\"asdf\\\"asdf\"");
			_test("\"asdf\\n\"", "\"asdf\\n\"");
			_test("\"asdf\n\"", "\"asdf\\n\"");

			//empty list
			_test("()", "()");

			//pairs
			_test("(0 . 1)", "(0 . 1)");
			_test("(0 1)", "(0 1)");
			_test("(0 . (1 . ()))", "(0 1)");
			_test("(0 . (1 . 2))", "(0 1 . 2)");
			
			//symbols
			_test("asdf", "asdf");

		}

		~Test_SchemeInterpreter()
		{
			std::cout << "All tests passed !" << std::endl;;
		}


	private:
		
		void _test(const std::string &input, const std::string &expecting)
		{
			std::istringstream it(input);
			std::cout << "[+] Test: " << input << std::endl;
			std::string got = write(eval(read(it)));
			if (expecting.compare(got)) {
				std::cout << "Error in test!" << std::endl;
				std::cout << "Input    : " << input << std::endl;
				std::cout << "Expected : " << expecting << std::endl;
				std::cout << "Got      : " << got << std::endl;
				exit(1);
			}
			std::cout << "[-]       " << input << " --- Success" << std::endl;
		}
};



int main(void)
{
	Test_SchemeInterpreter();
	return 0;
}
