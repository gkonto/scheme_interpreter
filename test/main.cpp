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
			
			// * Invalid tests after 'quote' support
			//empty list
			//_test("()", "()");

			//pairs
			//_test("(0 . 1)", "(0 . 1)");
			//_test("(0 1)", "(0 1)");
			//_test("(0 . (1 . ()))", "(0 1)");
			//_test("(0 . (1 . 2))", "(0 1 . 2)");
			
			//symbols
			//_test("asdf", "asdf");

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

			//primitive procedures
			_test("(+ 1 2 3)", "6");
			_test("(+ 3 -1)", "2");
			_test("+", "#<procedure>");
			_test("(boolean? #t)", "#t");
			_test("(integer->char 99)", "#\\c");
			_test("(< 1 2 3)", "#t");
			_test("(cons 'a 'b)", "(a . b)"); 
			//Lambda
			_test("(define (map proc items) (if (null? items) '() (cons (proc (car items)) (map proc (cdr items)))))", "ok");
			_test("(define (double x) (* 2 x))", "ok");
			_test("(map double '(0 1 2 3))", "(0 2 4 6)");
			_test("(define count ((lambda (total) (lambda (increment) (set! total (+ total increment)) total)) 0)) ; initial total", "ok");
			_test("(count 3)", "3");
			_test("(count 5)", "8");
			_test("(define (factorial n) (define (iter product counter max-count) (if (> counter max-count) product (iter (* counter product) (+ counter 1) max-count))) (iter 1 1 n))", "ok");
			_test("(factorial 4)", "24");
			_test("(define Y (lambda (f) ((lambda (x) (f (lambda (y) ((x x) y)))) (lambda (x) (f (lambda (y) ((x x) y)))))))", "ok");
			_test("(define factorial (Y (lambda (fact) (lambda (n) (if (= n 0) 1 (* n (fact (- n 1))))))))", "ok");
			_test("(factorial 5)", "120");

			_test("(begin 1 2 3)", "3");

			_test("(cond (#f   1) ((eq? 'a 'a) 2) (else         3))", "2");
			_test("(let ((x (+ 1 1)) (y (- 5 2))) (+ x y))", "5");


			_test("(and 1 2 3)", "3");
			_test("(and)", "#t");
			_test("(or #f 2 #t)", "2");
			_test("(or)", "#f");
			_test("(define a 1)", "ok");
			_test("(and #f (set! a 2))", "#f");
			_test("a", "1");
			_test("(and #t (set! a 2))", "ok");
			_test("a", "2");

			_test("(apply + '(1 2 3))", "6");
			_test("(apply + 1 2 '(3))", "6");
			_test("(apply + 1 2 3 '())", "6");

			_test("(define env (environment))", "ok");
			_test("(eval '(define z 25) env)", "ok");
			_test("(eval 'z env)", "25");

			_test("(define out (open-output-port \"asdf.txt\"))", "ok");
			_test("(write-char #\\c out)", "ok");
			_test("(close-output-port out)", "ok");
			_test("(load \"program.scm\")", "program-loaded"); // FIXME issue in this test: "program.scm" file is read, not program.scm as should.
			//FIXME is program.scm is empty file, there is an error..
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
			Object *exp = read(it);

			std::string got = write(std::cout, eval(exp, the_global_environment));

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
