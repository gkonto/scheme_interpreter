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

			
			/*
			 * Invalid tests after 'quote' support
			//empty list
			_test("()", "()");

			//pairs
			_test("(0 . 1)", "(0 . 1)");
			_test("(0 1)", "(0 1)");
			_test("(0 . (1 . ()))", "(0 1)");
			_test("(0 . (1 . 2))", "(0 1 . 2)");
			
			//symbols
			_test("asdf", "asdf");
			*/

			//quotes
			_test("(quote a)", "a");
			_test("'a", "a");
			_test("(quote (0 1 2))", "(0 1 2)");
			_test("'(0 1 2)", "(0 1 2)");
			_test("'()", "()");
			_test("'#t", "#t");
			_test("'123", "123");
			_test("123", "123");
			_test("'#\\c", "#\\c");
			_test("#\\c", "#\\c");
			_test("'\"asdf\"", "\"asdf\"");
			_test("\"asdf\"", "\"asdf\"");


			//environments
			_test("(define a 0)", "ok");
			_test("a", "0");
			_test("(set! a 1)", "ok");
			_test("a", "1");
			_test("(define a 2)", "ok");
			_test("a", "2");
			//_test("(set! b 3)", "unbound variable");
			
			//if
			_test("(if #t 1 2)", "1");
			_test("(if #t 'a 'b)", "a");
			_test("(if #f 1 2)", "2");
			_test("(if #t 1)", "1");
			_test("(if #f 1)", "#f");
			_test("(if 0 1 2)", "1");
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
			std::string got = write(eval(read(it), the_global_environment));
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
