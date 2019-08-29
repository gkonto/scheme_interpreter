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
Object *lambda_symbol          = 0;
Object *begin_symbol           = 0;
Object *cond_symbol            = 0;
Object *else_symbol            = 0;


// TODO code full of memory leaks
//
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

Object *make_primitive_proc(Object *(*fn)(struct Object *arguments)) 
{
    return new Object(fn);
}


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

static Object *lookup_variable_value(Object *var, Object *env)
{
    while (!is_the_empty_list(env)) {
        Object *frame = first_frame(env);
        Object *vars = frame_variables(frame);
        Object *vals = frame_values(frame);

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

static void set_variable_value(Object *var, Object *val, Object *env) 
{
    while (!is_the_empty_list(env)) {
        Object *frame = first_frame(env);
        Object *vars = frame_variables(frame);
        Object *vals = frame_values(frame);

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

static void define_variable(Object *var, Object *val, Object *env) 
{
    Object *frame = first_frame(env);    
    Object *vars = frame_variables(frame);
    Object *vals = frame_values(frame);

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

static Object *setup_environment(void)
{
    Object *initial_env = extend_environment(
                      the_empty_list,
                      the_empty_list,
                      the_empty_environment);
    return initial_env;
}




Object *read_pair(std::istream &in)
{
	eat_whitespace(in);

	int c = in.get();
	if (c == ')') {
		return the_empty_list;
	}
	in.unget();

	Object *car = read(in);

	eat_whitespace(in);
	
	c = in.get();
	if (c == '.') {
		c = in.peek();
		if (!is_delimiter(c)) {
			std::cerr << "dot not followed by delimiter" << std::endl;
			exit(1);
		}
		Object *cdr = read(in);
		eat_whitespace(in);
		c = in.get();
		if (c != ')') {
			std::cerr << "where was the trailing right paren?" << std::endl;
			exit(1);
		}
		return new Object(car, cdr);
	} else {
		in.unget();

		Object *cdr = read_pair(in);
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

bool is_initial(int c)
{
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
}

bool is_primitive_proc(Object *obj)
{
	return obj->type_ == TT_PRIMITIVE_PROC;
}

Object *is_null_proc(Object *arguments)
{
    return is_the_empty_list(car(arguments)) ? true_obj : false_obj;
}

Object *is_boolean_proc(Object *arguments)
{
    return is_boolean(car(arguments)) ? true_obj : false_obj;
}

Object *is_symbol_proc(Object *arguments)
{
    return is_symbol(car(arguments)) ? true_obj : false_obj;
}

Object *is_integer_proc(Object *arguments)
{
    return is_fixnum(car(arguments)) ? true_obj : false_obj;
}

Object *is_char_proc(Object *arguments) 
{
    return is_character(car(arguments)) ? true_obj : false_obj;
}

Object *is_string_proc(Object *arguments) 
{
    return is_string(car(arguments)) ? true_obj : false_obj;
}

Object *is_pair_proc(Object *arguments) 
{
    return is_pair(car(arguments)) ? true_obj : false_obj;
}


bool is_compound_proc(Object *obj);

Object *is_procedure_proc(Object *arguments) 
{
    Object *obj = car(arguments);
    return (is_primitive_proc(obj) ||
            is_compound_proc(obj)) ?
                true_obj :
                false_obj;
}


Object *char_to_integer_proc(Object *arguments) 
{
    return new Object((car(arguments))->char_value_, TT_FIXNUM);
}

Object *integer_to_char_proc(Object *arguments) 
{
    return new Object((car(arguments))->long_value_, TT_CHARACTER);
}

Object *number_to_string_proc(Object *arguments) 
{
    std::string buf = std::to_string(car(arguments)->long_value_);
    return new Object(buf, TT_STRING);
}

Object *string_to_number_proc(Object *arguments) 
{
	long x = 0;
	std::stringstream ss(car(arguments)->str_value_);
	ss >> x;
    return new Object(x, TT_FIXNUM);
}

Object *symbol_to_string_proc(Object *arguments) 
{
    return new Object(car(arguments)->str_value_, TT_STRING);
}

Object *string_to_symbol_proc(Object *arguments) 
{
    return make_symbol((car(arguments))->str_value_);
}



static Object *add_proc(Object *arguments)
{
	long result = 0;
	while(!is_the_empty_list(arguments)) {
		result += (car(arguments))->long_value_;
		arguments = cdr(arguments);
	}
	return new Object(result, TT_FIXNUM);
}

Object *sub_proc(Object *arguments) 
{
    long result;
    
    result = car(arguments)->long_value_;
    while (!is_the_empty_list(arguments = cdr(arguments))) {
        result -= (car(arguments))->long_value_;
    }
    return new Object(result, TT_FIXNUM);
}

Object *mul_proc(Object *arguments) 
{
    long result = 1;
    
    while (!is_the_empty_list(arguments)) {
        result *= (car(arguments))->long_value_;
        arguments = cdr(arguments);
    }
    return new Object(result, TT_FIXNUM);
}

Object *quotient_proc(Object *arguments) 
{
    return new Object(
        ((car(arguments) )->long_value_)/
        ((cadr(arguments))->long_value_), TT_FIXNUM);
}

Object *remainder_proc(Object *arguments) 
{
    return new Object(
        ((car(arguments) )->long_value_)%
        ((cadr(arguments))->long_value_), TT_FIXNUM);
}

Object *is_number_equal_proc(Object *arguments) 
{
    long value;
    
    value = (car(arguments))->long_value_;
    while (!is_the_empty_list(arguments = cdr(arguments))) {
        if (value != ((car(arguments))->long_value_)) {
            return false_obj;
        }
    }
    return true_obj;
}

Object *is_less_than_proc(Object *arguments) 
{
    long previous;
    long next;
    
    previous = (car(arguments))->long_value_;
    while (!is_the_empty_list(arguments = cdr(arguments))) {
        next = (car(arguments))->long_value_;
        if (previous < next) {
            previous = next;
        }
        else {
            return false_obj;
        }
    }
    return true_obj;
}

Object *is_greater_than_proc(Object *arguments) 
{
    long previous;
    long next;
    
    previous = (car(arguments))->long_value_;
    while (!is_the_empty_list(arguments = cdr(arguments))) {
        next = (car(arguments))->long_value_;
        if (previous > next) {
            previous = next;
        }
        else {
            return false_obj;
        }
    }
    return true_obj;
}


Object *cons_proc(Object *arguments) {
    return new Object(car(arguments), cadr(arguments));
}

Object *car_proc(Object *arguments) {
    return caar(arguments);
}

Object *cdr_proc(Object *arguments) {
    return cdar(arguments);
}

Object *set_car_proc(Object *arguments) {
    set_car(car(arguments), cadr(arguments));
    return ok_symbol;
}

Object *set_cdr_proc(Object *arguments) {
    set_cdr(car(arguments), cadr(arguments));
    return ok_symbol;
}

Object *list_proc(Object *arguments) {
    return arguments;
}

Object *is_eq_proc(Object *arguments) {
    Object *obj1;
    Object *obj2;
    
    obj1 = car(arguments);
    obj2 = cadr(arguments);
    
    if (obj1->type_ != obj2->type_) {
        return false_obj;
    }
    switch (obj1->type_) {
        case TT_FIXNUM:
            return (obj1->long_value_ == 
                    obj2->long_value_) ?
                        true_obj : false_obj;
            break;
        case TT_CHARACTER:
            return (obj1->long_value_ == 
                    obj2->long_value_) ?
                        true_obj : false_obj;
            break;
        case TT_STRING:
            return (obj1->str_value_.compare(
                           obj2->str_value_) == 0) ?
                        true_obj : false_obj;
            break;
        default:
            return (obj1 == obj2) ? true_obj : false_obj;
    }
}

Object *make_compound_proc(Object *parameters, Object *body,
                           Object* env) 
{
    Object *obj = new Object(parameters, body, env);
    return obj;
}

bool is_compound_proc(Object *obj) {
    return obj->type_ == TT_COMPOUND_PROC;
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
	lambda_symbol = make_symbol("lambda");
	begin_symbol  = make_symbol("begin");
	cond_symbol   = make_symbol("cond");
	else_symbol   = make_symbol("else");

	define_variable(make_symbol("+"),
			new Object(add_proc),
			the_global_environment);

#define add_procedure(scheme_name, c_name)              \
    define_variable(make_symbol(scheme_name),           \
                    make_primitive_proc(c_name),        \
                    the_global_environment);

    add_procedure("null?"     , is_null_proc);
    add_procedure("boolean?"  , is_boolean_proc);
    add_procedure("symbol?"   , is_symbol_proc);
    add_procedure("integer?"  , is_integer_proc);
    add_procedure("char?"     , is_char_proc);
    add_procedure("string?"   , is_string_proc);
    add_procedure("pair?"     , is_pair_proc);
    add_procedure("procedure?", is_procedure_proc);
    
    add_procedure("char->integer" , char_to_integer_proc);
    add_procedure("integer->char" , integer_to_char_proc);
    add_procedure("number->string", number_to_string_proc);
    add_procedure("string->number", string_to_number_proc);
    add_procedure("symbol->string", symbol_to_string_proc);
    add_procedure("string->symbol", string_to_symbol_proc);
      
    add_procedure("+"        , add_proc);
    add_procedure("-"        , sub_proc);
    add_procedure("*"        , mul_proc);
    add_procedure("quotient" , quotient_proc);
    add_procedure("remainder", remainder_proc);
    add_procedure("="        , is_number_equal_proc);
    add_procedure("<"        , is_less_than_proc);
    add_procedure(">"        , is_greater_than_proc);

    add_procedure("cons"    , cons_proc);
    add_procedure("car"     , car_proc);
    add_procedure("cdr"     , cdr_proc);
    add_procedure("set-car!", set_car_proc);
    add_procedure("set-cdr!", set_cdr_proc);
    add_procedure("list"    , list_proc);

    add_procedure("eq?", is_eq_proc);

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

Object *make_lambda(Object *parameters, Object *body);

Object *definition_value(Object *exp) 
{
	if (is_symbol(cadr(exp))) {
		return caddr(exp);
	} else {
		return make_lambda(cdadr(exp), cddr(exp));
	}
}


static bool is_cond(Object *exp) {
    return is_tagged_list(exp, cond_symbol);
}

Object *cond_clauses(Object *exp) {
    return cdr(exp);
}

Object *cond_predicate(Object *clause) {
    return car(clause);
}

Object *cond_actions(Object *clause) {
    return cdr(clause);
}

char is_cond_else_clause(Object *clause) {
    return cond_predicate(clause) == else_symbol;
}


static bool is_last_exp(Object *seq) {
    return is_the_empty_list(cdr(seq));
}

static Object *first_exp(Object *seq) {
    return car(seq);
}

Object *sequence_to_exp(Object *seq) {
    if (is_the_empty_list(seq)) {
        return seq;
    }
    else if (is_last_exp(seq)) {
        return first_exp(seq);
    }
    else {
        return new Object(begin_symbol, seq);
    }
}

Object *make_if(Object *predicate, Object *consequent, Object *alternative) 
{
    return new Object(if_symbol,
                new Object(predicate,
                     new Object(consequent,
                          new Object(alternative, the_empty_list))));
}

Object *expand_clauses(Object *clauses) {
    Object *first;
    Object *rest;
    
    if (is_the_empty_list(clauses)) {
        return false_obj;
    }
    else {
        first = car(clauses);
        rest  = cdr(clauses);
        if (is_cond_else_clause(first)) {
            if (is_the_empty_list(rest)) {
                return sequence_to_exp(cond_actions(first));
            }
            else {
                fprintf(stderr, "else clause isn't last cond->if");
                exit(1);
            }
        }
        else {
            return make_if(cond_predicate(first),
                           sequence_to_exp(cond_actions(first)),
                           expand_clauses(rest));
        }
    }
}

Object *cond_to_if(Object *exp) {
    return expand_clauses(cond_clauses(exp));
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

Object *make_lambda(Object *parameters, Object *body)
{
    return new Object(lambda_symbol, new Object(parameters, body));
}


char is_lambda(Object *exp) {
    return is_tagged_list(exp, lambda_symbol);
}

Object *lambda_parameters(Object *exp) {
    return cadr(exp);
}

Object *lambda_body(Object *exp) {
    return cddr(exp);
}



static bool is_application(Object *exp)
{
	return is_pair(exp);
}

static Object *op(Object *exp)
{
	return car(exp);
}

static Object *operands(Object *exp)
{
	return cdr(exp);
}

static bool is_no_operands(Object *ops)
{
	return is_the_empty_list(ops);
}


Object *rest_exps(Object *seq) {
    return cdr(seq);
}


static Object *first_operand(Object *ops)
{
	return car(ops);
}

static Object *rest_operands(Object *ops)
{
	return cdr(ops);
}


Object *eval(Object *exp, Object *env);

Object *list_of_values(Object *exps, Object *env)
{
	if (is_no_operands(exps)) {
		return the_empty_list;
	} else {
		return new Object(eval(first_operand(exps), env),
				list_of_values(rest_operands(exps), env));
	}
}

static Object *eval_assignment(Object *exp, Object *env)
{
    set_variable_value(assignment_variable(exp),
                       eval(assignment_value(exp), env),
                       env);
    return ok_symbol;
}

Object *definition_variable(Object *exp) {
    if (is_symbol(cadr(exp))) {
        return cadr(exp);
    }
    else {
        return caadr(exp);
    }
}


static Object *eval_definition(Object *exp, Object *env)
{
    define_variable(definition_variable(exp),
                    eval(definition_value(exp), env),
                    env);
    return ok_symbol;
}



Object *begin_actions(Object *exp) {
    return cdr(exp);
}

char is_begin(Object *exp)
{
	return is_tagged_list(exp, begin_symbol);
}


/* until we have lists and symbols just echo */
Object *eval(Object *exp, Object *env)
{
	Object *procedure = 0;
	Object *arguments = 0;

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
	} else if (is_lambda(exp)) {
		return make_compound_proc(lambda_parameters(exp),
				lambda_body(exp),
				env);
	} else if (is_begin(exp)) {
		exp = begin_actions(exp);
		while (!is_last_exp(exp)) {
		    eval(first_exp(exp), env);
		    exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		goto tailcall;

	} else if (is_cond(exp)) {
		exp = cond_to_if(exp);
		goto tailcall;

	} else if (is_application(exp)) {

		procedure = eval(op(exp), env);
		arguments = list_of_values(operands(exp), env);
		if (is_primitive_proc(procedure)) {
		    return procedure->fun_(arguments);
		}
		else if (is_compound_proc(procedure)) {
		    env = extend_environment( 
			       procedure->compound_proc.parameters,
			       arguments,
			       procedure->compound_proc.env);
		    exp = new Object(begin_symbol, procedure->compound_proc.body);
		    goto tailcall;
		}
		else {
		    fprintf(stderr, "unknown procedure type\n");
		    exit(1);
		}

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
	Object *car_obj = car(pair);
	Object *cdr_obj = cdr(pair);

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
		case TT_PRIMITIVE_PROC:
		case TT_COMPOUND_PROC:
		      return "#<procedure>";
		default: {
			std::cerr << "cannot write unknown type" << std::endl;
			exit(1);
		}
	}
} /* write */

/***************************** REPL ******************************/


