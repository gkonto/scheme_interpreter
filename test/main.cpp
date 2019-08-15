
#include <iostream>
#include <string>
#include <sstream>

#include "../object.hpp"


class Test_SchemeInterpreter
{
	public:
		Test_SchemeInterpreter()
		{
			_test("123", "123");
			_test("-123", "-123");
			_test("007", "7");
			_test("#t", "#t");
			_test("#f", "#f");
		}

		~Test_SchemeInterpreter()
		{
			std::cout << "All tests passed !" << std::endl;;
		}


	private:
		
		void _test(const std::string &input, const std::string &expecting)
		{
			std::istringstream it(input);

			std::string got = write(eval(read(it)));
			if (expecting.compare(got)) {
				std::cout << "Error in test!" << std::endl;
				std::cout << "Input    : " << input << std::endl;
				std::cout << "Expected : " << expecting << std::endl;
				std::cout << "Got      : " << got << std::endl;
				exit(1);
			}
		}
};



int main(void)
{
	Test_SchemeInterpreter();
	return 0;
}
