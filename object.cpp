#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "object.hpp"

/**************************** MODEL ******************************/
Object *false_obj              = 0;
Object *true_obj               = 0;
Object *symbol_table           = 0;
Object *the_empty_list         = 0;
Object *quote_symbol           = 0;
Object *define_symbol          = 0;
Object *set_symbol             = 0;
Object *ok_symbol              = 0;
Object *the_empty_environment  = 0;
Object *the_global_environment = 0;
Object *if_symbol              = 0;

Object *make_symbol(const std::string &value)
{
	Object *element = symbol_table;

	while (!is_the_empty_list(element)) {
		Object *car_obj = car(element);
		if (!car_obj->str_value_.compare(value)) {
			return car_obj;
		}
		element = cdr(element);
	};
	Object *obj = new Object(value, TT_SYMBOL);

	//TODO make public member that adds to symbol table
	symbol_table = new Object(obj, symbol_table);

	return obj;
}

bool is_symbol(Object *obj)
{
	return obj->type_ == TT_SYMBOL;
}

Object *car(Object *pair)
{
	return pair->pair.car;
}

void set_car(Object *pair, Object *value)
{
	pair->pair.car = value;
}

Object *cdr(Object *pair)
{
	return pair->pair.cdr;
}

void set_cdr(Object *pair, Object *value)
{
	pair->pair.cdr = value;
}

#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))

static Object *enclosing_environment(Object *env)
{
	return cdr(env);
}

static Object *first_frame(Object *env)
{
	return car(env);
}

static Object *make_frame(Object *variables, Object *values) {
    return new Object(variables, values);
}

static Object *frame_variables(Object *frame) {
    return car(frame);
}

static Object *frame_values(Object *frame) {
    return cdr(frame);
}

static void add_binding_to_frame(Object *var, Object *val, 
                          Object *frame) {
    set_car(frame, new Object(var, car(frame)));
    set_cdr(frame, new Object(val, cdr(frame)));
}

static Object *extend_environment(Object *vars, Object *vals,
                           Object *base_env) {
    return new Object(make_frame(vars, vals), base_env);
}

static Object *lookup_variable_value(Object *var, Object *env) {
    Object *frame;
    Object *vars;
    Object *vals;
    while (!is_the_empty_list(env)) {
        frame = first_frame(env);
        vars = frame_variables(frame);
        vals = frame_values(frame);
        while (!is_the_empty_list(vars)) {
            if (var == car(vars)) {
                return car(vals);
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = enclosing_environment(env);
    }
    std::cerr << "unbound variable" << std::endl;
    exit(1);
}

static void set_variable_value(Object *var, Object *val, Object *env) {
    Object *frame;
    Object *vars;
    Object *vals;

    while (!is_the_empty_list(env)) {
        frame = first_frame(env);
        vars = frame_variables(frame);
        vals = frame_values(frame);
        while (!is_the_empty_list(vars)) {
            if (var == car(vars)) {
                set_car(vals, val);
                return;
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = enclosing_environment(env);
    }
    std::cerr << "unbound variable" << std::endl;
    exit(1);
}

static void define_variable(Object *var, Object *val, Object *env) {
    Object *frame;
    Object *vars;
    Object *vals;
    
    frame = first_frame(env);    
    vars = frame_variables(frame);
    vals = frame_values(frame);

    while (!is_the_empty_list(vars)) {
        if (var == car(vars)) {
            set_car(vals, val);
            return;
        }
        vars = cdr(vars);
        vals = cdr(vals);
    }
    add_binding_to_frame(var, val, frame);
}

static Object *setup_environment(void) {
    Object *initial_env;
    
    initial_env = extend_environment(
                      the_empty_list,
                      the_empty_list,
                      the_empty_environment);
    return initial_env;
}




Object *read_pair(std::istream &in)
{
	Object *car;
	Object *cdr;

	eat_whitespace(in);

	int c = in.get();
	if (c == ')') {
		return the_empty_list;
	}
	in.unget();

	car = read(in);

	eat_whitespace(in);
	
	c = in.get();
	if (c == '.') {
		c = in.peek();
		if (!is_delimiter(c)) {
			std::cerr << "dot not followed by delimiter" << std::endl;
			exit(1);
		}
		cdr = read(in);
		eat_whitespace(in);
		c = in.get();
		if (c != ')') {
			std::cerr << "where was the trailing right paren?" << std::endl;
			exit(1);
		}
		return new Object(car, cdr);
	} else {
		in.unget();
		cdr = read_pair(in);
		return new Object(car, cdr);
	}
}


bool is_pair(Object *obj)
{
	return obj->type_ == TT_PAIR;
}

bool is_boolean(Object *obj)
{
	return obj->type_ == TT_BOOLEAN;
}

bool is_the_empty_list(Object *obj)
{
	return obj == the_empty_list;
}

/*****************************************************************/
bool  is_false(Object *obj)
{
	return obj == false_obj;
}

/*****************************************************************/
bool is_true(Object *obj)
{
	return !is_false(obj);
}

/*****************************************************************/
/* no GC so truely "unlimited extent" */
bool is_fixnum(Object *obj)
{
	return obj->type_ == TT_FIXNUM;
}

/*****************************************************************/
bool is_character(Object *obj)
{
	return obj->type_ == TT_CHARACTER;
}

bool is_string(Object *obj)
{
	return obj->type_ == TT_STRING;
}

char is_initial(int c)
{
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
}


/*****************************************************************/
void init(void)
{
	false_obj = new Object(false, TT_BOOLEAN);
	true_obj  = new Object(true, TT_BOOLEAN);
	the_empty_list = new Object(TT_THE_EMPTY_LIST);
	symbol_table = the_empty_list;
	quote_symbol = make_symbol("quote");
	define_symbol = make_symbol("define");
	set_symbol   = make_symbol("set!");
	ok_symbol    = make_symbol("ok");
	the_empty_environment = the_empty_list;
	the_global_environment = setup_environment();
	if_symbol = make_symbol("if");
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
void eat_expected_string(std::istream &in, const std::string &to_compare)
{
	//std::string got = in.str();

	std::string got;
	in >> got;

	if (got.compare(to_compare)) {
		std::cerr << "unexpected character" << to_compare << std::endl;	
		exit(1);
	}
}

/*****************************************************************/
void peek_expected_delimiter(std::istream &in) 
{
	if (!is_delimiter(in.peek())) {
		std::cerr << "character not followed by delimiter" << std::endl;
		exit(1);
	}
}

/*****************************************************************/

Object *read_character(std::istream &in)
{
	char c = in.get();
	switch (c) {
		case EOF:
			std::cerr << "incomplete character literal" << std::endl;
			exit(1);
		case 's':
			if (in.peek() == 'p') {
				eat_expected_string(in, "pace");
				peek_expected_delimiter(in);
				return new Object(' ', TT_CHARACTER);
			}
			break;
		case 'n':
			if (in.peek() == 'e') {
				eat_expected_string(in, "ewline");
				peek_expected_delimiter(in);
				return new Object('\n', TT_CHARACTER);
			} break;
	}
	peek_expected_delimiter(in);

	return new Object(c, TT_CHARACTER);

}

/*****************************************************************/

Object *read(std::istream &in) 
{
	short sign = 1;
	long num = 0;
	std::string buffer;

	eat_whitespace(in);

	int c = in.get();

	if (c == '#') {
		c = in.get();
		switch(c) {
			case 't':
				return true_obj;
			case 'f':
				return false_obj;
			case '\\':
				return read_character(in);
			default:
				std::cerr << "uknown boolean literal" << std::endl;
				exit(1);
		}
	} else if (isdigit(c) || (c == '-' && (isdigit(in.peek())))) {
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
			return new Object(num, TT_FIXNUM);
		} else {
			std::cerr << "number not followed by delimiter" << std::endl;
			exit(1);
		}
	} else if (is_initial(c) || 
		((c == '+' || c == '-')
		 && is_delimiter(in.peek())))
	{
		while (is_initial(c) || isdigit(c) || c == '+' || c == '-') {
			buffer += c;
			c = in.get();
		}
		if (is_delimiter(c)) {
			in.unget();
			return make_symbol(buffer);
		} else {
			char i = c;
			std::cerr << "symbol not followed by delimiter. "
				"Found " << i << std::endl;
		}
	} else if (c == '"') { 
		std::string str;
		str += '"';

		while ((c = in.get()) != '"') {
			if (c == '\\') {
				str += '\\';
				c = in.get();
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
		return new Object(str, TT_STRING);

	} else if (c == '(') {
		return read_pair(in);
	} else if (c == '\'') {
		return new Object(quote_symbol, new Object(read(in), the_empty_list));
	} else {
		char i = c;	
		std::cerr << "unexpected character " << i << ". Expecting ')' " << std::endl;
	}

	std::cerr <<  "read illegal state" << std::endl;
	exit(1);
}

/*************************** EVALUATE ****************************/

static bool is_self_evaluating(Object *exp)
{
	return is_boolean(exp) ||
		is_fixnum(exp) ||
		is_character(exp) ||
		is_string(exp);
}

static bool is_variable(Object *expression)
{
	return is_symbol(expression);
}

static bool is_tagged_list(Object *expression, Object *tag)
{
	if (is_pair(expression)) {
		Object *the_car = car(expression);
		return is_symbol(the_car) && (the_car == tag);
	}
	return 0;
}

static bool is_quoted(Object *expression)
{
	return is_tagged_list(expression, quote_symbol);
}



static Object *text_of_quotation(Object *exp)
{
	return cadr(exp);
}

static bool  is_assignment(Object *exp)
{
    return is_tagged_list(exp, set_symbol);
}

static Object *assignment_variable(Object *exp) 
{
    return car(cdr(exp));
}

static Object *assignment_value(Object *exp)
{
    return car(cdr(cdr(exp)));
}

static char is_definition(Object *exp)
{
    return is_tagged_list(exp, define_symbol);
}

static Object *definition_variable(Object *exp)
{
    return cadr(exp);
}

static Object *definition_value(Object *exp) 
{
    return caddr(exp);
}

static bool is_if(Object *expression)
{
	return is_tagged_list(expression, if_symbol);
}

//'if' symbol specific
static Object *if_predicate(Object *exp)
{
	return cadr(exp);
}

//'if' symbol specific
static Object *if_consequent(Object *exp)
{
	return caddr(exp);
}

//'if' symbol specific
static Object *if_alternative(Object *exp)
{
	if (is_the_empty_list(cdddr(exp))) {
		return false_obj;
	} else {
		return cadddr(exp);
	}
}


Object *eval(Object *exp, Object *env);

static Object *eval_assignment(Object *exp, Object *env)
{
    set_variable_value(assignment_variable(exp),
                       eval(assignment_value(exp), env),
                       env);
    return ok_symbol;
}

static Object *eval_definition(Object *exp, Object *env)
{
    define_variable(definition_variable(exp),
                    eval(definition_value(exp), env),
                    env);
    return ok_symbol;
}



/* until we have lists and symbols just echo */
Object *eval(Object *exp, Object *env)
{
tailcall:
	if (is_self_evaluating(exp)) {
		return exp;
	} else if (is_variable(exp)) {
		return lookup_variable_value(exp, env);
	} else if (is_quoted(exp)) {
		return text_of_quotation(exp);
	} else if (is_assignment(exp)) {
		return eval_assignment(exp, env);
	} else if (is_definition(exp)) {
		return eval_definition(exp, env);
	} else if (is_if(exp)) {
		exp = is_true(eval(if_predicate(exp), env)) ? if_consequent(exp) : if_alternative(exp);
		//TODO return eval(exp, env) ?
		goto tailcall;

	} else {
		std::cerr << "cannot eval unknown expression type" << std::endl;
		exit(1);
	}
	std::cerr << "eval illegal state" << std::endl;
	exit(1);
}

/**************************** PRINT ******************************/
std::string write_pair(Object *pair)
{
	Object *car_obj;
	Object *cdr_obj;
	car_obj = car(pair);
	cdr_obj = cdr(pair);

	std::string ret;

	ret += write(car_obj);
	if (cdr_obj->type_ == TT_PAIR) {
		ret += " ";
		ret += write_pair(cdr_obj);
	} else if (cdr_obj->type_ == TT_THE_EMPTY_LIST) {
		return ret;
	} else {
		ret += " . ";
		ret += write(cdr_obj);
	}
	return ret;
}

std::string write(Object *obj)
{
	std::stringstream ss;

	switch (obj->type_)
	{
		case TT_THE_EMPTY_LIST: {
			return "()";
		}
		case TT_FIXNUM: {
			ss << obj->long_value_;
			return ss.str();
		}
		case TT_SYMBOL: {
			return obj->str_value_;
		}
		case TT_BOOLEAN: {
			std::string val = is_false(obj) ? "#f" : "#t";
			ss << val;
			return ss.str();
		}
		case TT_CHARACTER: {
			char c = obj->char_value_;
			std::string ret("#\\");
			switch (c) {
				case '\n':
					ret.append("newline");
					return ret;
				case ' ':
					ret.append("space");
					return ret;
				default:
					ret += c;
					return ret;

			}
		}
		case TT_STRING: {
					std::string str = obj->str_value_;
					return str;
				}
		case TT_PAIR: {
				      std::string ret = "(";
				      ret += write_pair(obj);
				      ret += ")";
				      return ret;
				      }
		default: {
			std::cerr << "cannot write unknown type" << std::endl;
			exit(1);
		}
	}
} /* write */

/***************************** REPL ******************************/


