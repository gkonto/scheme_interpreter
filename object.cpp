#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fstream>
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
Object *let_symbol             = 0;
Object *and_symbol             = 0;
Object *or_symbol              = 0;
Object *eof_object             = 0;

Object *make_environment(void);
Object *make_input_port(FILE *in);
static Object *setup_environment(void);
void populate_environment(Object *env);
bool is_compound_proc(Object *obj);
bool is_input_port(Object *obj);

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


Object *apply_proc(Object *arguments) 
{
	std::cerr << "illegal state: The body of the apply primitive procedure should not execute." << std::endl;
	exit(1);
}




Object *is_procedure_proc(Object *arguments) 
{
    Object *obj = car(arguments);
    return (is_primitive_proc(obj) ||
            is_compound_proc(obj)) ?
                true_obj :
                false_obj;
}


Object *interaction_environment_proc(Object *arguments) {
    return the_global_environment;
}


Object *null_environment_proc(Object *arguments) {
    return setup_environment();
}


Object *environment_proc(Object *arguments) 
{
    return make_environment();
}

Object *eval_proc(Object *arguments) 
{
	std::cerr << "illegal state: The body of the eval primitive procedure should not execute." << std::endl;
	exit(1);
}

Object *make_environment(void)
{
    Object *env = setup_environment();
    populate_environment(env);
    return env;
}

Object *load_proc(Object *arguments) {
    std::string filename(car(arguments)->str_value_);
    //in = fopen(filename, "r");
    std::ifstream ifs(filename, std::istream::in);
    if (ifs.fail()) {
	    std::cerr << "could not load file \"" << filename << "\"" << std::endl;
        exit(1);
    }

    Object *exp;
    Object *result;
    while ((exp = read(ifs)) != NULL) {
        result = eval(exp, the_global_environment);
    }

    ifs.close();
    return result;
}


Object *make_input_port(FILE *stream) {
    Object *obj;
    
    obj = new Object();
    obj->type_ = TT_INPUT_PORT;
    obj->in_ = stream;
    return obj;
}



Object *open_input_port_proc(Object *arguments) 
{
    std::string filename(car(arguments)->str_value_);

    FILE *in = fopen(filename.c_str(), "r");
    if (in == NULL) {
	    std::cerr << "could not open file \" " << filename << "\"" << std::endl;
        exit(1);
    }
    return make_input_port(in);
}

Object *close_input_port_proc(Object *arguments) {
    
    int result = fclose(car(arguments)->in_);
    if (result == EOF) {
	    std::cerr << "could not close input port" << std::endl;
        exit(1);
    }
    return ok_symbol;
}

bool is_input_port(Object *obj)
{
	return obj->type_ == TT_INPUT_PORT;
}

Object *is_input_port_proc(Object *arguments) {
    return is_input_port(car(arguments)) ? true_obj : false_obj;
}


Object *make_output_port(FILE *stream) 
{
    Object *obj = new Object();
    obj->type_ = TT_OUTPUT_PORT;
    obj->out_ = stream;
    return obj;
}


bool is_output_port(Object *obj) {
    return obj->type_ == TT_OUTPUT_PORT;
}

bool is_eof_object(Object *obj) {
    return obj == eof_object;
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

Object *read_proc(Object *arguments) {
    FILE *in = is_the_empty_list(arguments) ?
             stdin :
             car(arguments)->in_;

    //TODO not sure if correct
    std::fstream ini;
	ini << in;
    Object *result = read(ini);

    return (result == NULL) ? eof_object : result;
}


Object *read_char_proc(Object *arguments) {
    FILE *in = is_the_empty_list(arguments) ?
             stdin :
             car(arguments)->out_;

    std::fstream ini;
    ini << in;
    char result = ini.get();

    return (result == EOF) ? eof_object : new Object(result, TT_CHARACTER);
}

Object *peek_char_proc(Object *arguments) {
    
    FILE *in = is_the_empty_list(arguments) ?
             stdin :
             car(arguments)->in_;
    std::fstream ini;
    ini << in;
    char result = ini.peek();
   

    return (result == EOF) ? eof_object : new Object(result, TT_CHARACTER);
}


Object *is_eof_object_proc(Object *arguments) {
    return is_eof_object(car(arguments)) ? true_obj : false_obj;
}


Object *open_output_port_proc(Object *arguments) {

    std::string filename = car(arguments)->str_value_;
    FILE *out = fopen(filename.c_str(), "w");
    if (out == NULL) {
	    std::cerr << "could not open file \""<< filename << "\"";
        exit(1);
    }
    return make_output_port(out);
}


Object *close_output_port_proc(Object *arguments) {
    int result = fclose(car(arguments)->out_);
    if (result == EOF) {
	    std::cerr << "could not close output port" << std::endl;
        exit(1);
    }
    return ok_symbol;
}




Object *is_output_port_proc(Object *arguments) {
    return is_output_port(car(arguments)) ? true_obj : false_obj;
}



Object *write_char_proc(Object *arguments) {
    Object *character = car(arguments);
    arguments = cdr(arguments);
    FILE *out = is_the_empty_list(arguments) ?
             stdout :
             car(arguments)->out_;
    putc(character->char_value_, out);    
    fflush(out);
    return ok_symbol;
}

Object *write_proc(Object *arguments) {
    
    Object *exp = car(arguments);
    arguments = cdr(arguments);
    FILE *out = is_the_empty_list(arguments) ?
             stdout :
             car(arguments)->out_;
    std::fstream ini;
    ini << out;
    write(ini, exp);
    ini.flush();
    return ok_symbol;
}

Object *error_proc(Object *arguments) {

    while (!is_the_empty_list(arguments)) {
        write(std::cerr, car(arguments));
	std::cerr << " ";
        arguments = cdr(arguments);
    };
    std::cout << std::endl << "exiting" << std::endl;
    exit(1);
}







/*****************************************************************/

void populate_environment(Object *env) 
{

#define add_procedure(scheme_name, c_name)              \
    define_variable(make_symbol(scheme_name),           \
                    make_primitive_proc(c_name),        \
                    env);

    add_procedure("null?"      , is_null_proc);
    add_procedure("boolean?"   , is_boolean_proc);
    add_procedure("symbol?"    , is_symbol_proc);
    add_procedure("integer?"   , is_integer_proc);
    add_procedure("char?"      , is_char_proc);
    add_procedure("string?"    , is_string_proc);
    add_procedure("pair?"      , is_pair_proc);
    add_procedure("procedure?" , is_procedure_proc);
    
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

    add_procedure("apply", apply_proc);
    
    add_procedure("interaction-environment", 
                                     interaction_environment_proc);
    add_procedure("null-environment", null_environment_proc);
    add_procedure("environment"     , environment_proc);
    add_procedure("eval"            , eval_proc);


    add_procedure("load", load_proc);
    add_procedure("open-input-port"  , open_input_port_proc);
    add_procedure("close-input-port" , close_input_port_proc);
    add_procedure("input-port?"      , is_input_port_proc);
    add_procedure("read"             , read_proc);
    add_procedure("read-char"        , read_char_proc);
    add_procedure("peek-char"        , peek_char_proc);
    add_procedure("eof-object?"      , is_eof_object_proc);
    add_procedure("open-output-port" , open_output_port_proc);
    add_procedure("close-output-port", close_output_port_proc);
    add_procedure("output-port?"     , is_output_port_proc);
    add_procedure("write-char"       , write_char_proc);
    add_procedure("write"            , write_proc);

    add_procedure("error", error_proc);
}


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
	let_symbol    = make_symbol("let");
	and_symbol    = make_symbol("and");
	or_symbol     = make_symbol("or");

	the_empty_environment = the_empty_list;
	the_global_environment = make_environment();
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

char is_let(Object *exp) {
    return is_tagged_list(exp, let_symbol);
}

Object *let_bindings(Object *exp) {
    return cadr(exp);
}

Object *let_body(Object *exp) {
    return cddr(exp);
}


Object *binding_parameter(Object *binding) 
{
    return car(binding);
}

Object *binding_argument(Object *binding) 
{
    return cadr(binding);
}

Object *bindings_parameters(Object *bindings) 
{
    return is_the_empty_list(bindings) ?
               the_empty_list :
               new Object(binding_parameter(car(bindings)),
                    bindings_parameters(cdr(bindings)));
}

Object *bindings_arguments(Object *bindings) 
{
    return is_the_empty_list(bindings) ?
               the_empty_list :
               new Object(binding_argument(car(bindings)),
                    bindings_arguments(cdr(bindings)));
}

Object *let_parameters(Object *exp) 
{
    return bindings_parameters(let_bindings(exp));
}

Object *let_arguments(Object *exp) 
{
    return bindings_arguments(let_bindings(exp));
}

Object *let_to_application(Object *exp) 
{
    return new Object(make_lambda(let_parameters(exp), let_body(exp)), let_arguments(exp));
}

char is_and(Object *exp) {
    return is_tagged_list(exp, and_symbol);
}

Object *and_tests(Object *exp) {
    return cdr(exp);
}

char is_or(Object *exp) {
    return is_tagged_list(exp, or_symbol);
}

Object *or_tests(Object *exp) {
    return cdr(exp);
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


Object *apply_operator(Object *arguments) {
    return car(arguments);
}

Object *prepare_apply_operands(Object *arguments) {
    if (is_the_empty_list(cdr(arguments))) {
        return car(arguments);
    }
    else {
        return new Object(car(arguments),
                    prepare_apply_operands(cdr(arguments)));
    }
}


Object *apply_operands(Object *arguments) {
    return prepare_apply_operands(cdr(arguments));
}


Object *eval_expression(Object *arguments) {
    return car(arguments);
}

Object *eval_environment(Object *arguments) {
    return cadr(arguments);
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

	} else if (is_let(exp)) {
		exp = let_to_application(exp);
		goto tailcall;
	} else if (is_and(exp)) {
		exp = and_tests(exp);
		if (is_the_empty_list(exp)) {
		    return true_obj;
		}
		while (!is_last_exp(exp)) {
		    Object *result = eval(first_exp(exp), env);
		    if (is_false(result)) {
			return result;
		    }
		    exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		goto tailcall;
		}
		else if (is_or(exp)) {
		exp = or_tests(exp);
		if (is_the_empty_list(exp)) {
		    return false_obj;
		}
		while (!is_last_exp(exp)) {
		    Object *result = eval(first_exp(exp), env);
		    if (is_true(result)) {
			return result;
		    }
		    exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		goto tailcall;
	} else if (is_application(exp)) {

		procedure = eval(op(exp), env);
		arguments = list_of_values(operands(exp), env);

		/* handle apply specially for tailcall requirement */
		if (is_primitive_proc(procedure) && procedure->fun_ == eval_proc) {
			exp = eval_expression(arguments);
			env = eval_environment(arguments);
			goto tailcall;
		}

		/* handle apply specially for tail call requirement */
		if (is_primitive_proc(procedure) && procedure->fun_ == apply_proc) {
			procedure = apply_operator(arguments);
			arguments = apply_operands(arguments);
		}


		if (is_primitive_proc(procedure)) {
		    return procedure->fun_(arguments);
		} else if (is_compound_proc(procedure)) {
		    env = extend_environment( 
			       procedure->compound_proc.parameters,
			       arguments,
			       procedure->compound_proc.env);
		    exp = new Object(begin_symbol, procedure->compound_proc.body);
		    goto tailcall;
		} else {
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
std::string write_pair(std::ostream &out, Object *pair)
{
	Object *car_obj = car(pair);
	Object *cdr_obj = cdr(pair);

	std::string ret;

	ret += write(out, car_obj);
	if (cdr_obj->type_ == TT_PAIR) {
		ret += " ";
		ret += write_pair(out, cdr_obj);
	} else if (cdr_obj->type_ == TT_THE_EMPTY_LIST) {
		return ret;
	} else {
		ret += " . ";
		ret += write(out, cdr_obj);
	}
	return ret;
}

std::string write(std::ostream &out, Object *obj)
{
	std::stringstream ss;

	switch (obj->type_)
	{
		case TT_THE_EMPTY_LIST: {
			out << "()";
			return "()";
		}
		case TT_FIXNUM: {
			ss << obj->long_value_;
			out << ss.str();
			return ss.str();
		}
		case TT_SYMBOL: {
					out << obj->str_value_;
			return obj->str_value_;
		}
		case TT_BOOLEAN: {
			std::string val = is_false(obj) ? "#f" : "#t";
			ss << val;
			out << val;
			return ss.str();
		}
		case TT_CHARACTER: {
			char c = obj->char_value_;
			std::string ret("#\\");
			switch (c) {
				case '\n':
					ret.append("newline");
					out << ret;
					return ret;
				case ' ':
					ret.append("space");
					out << ret;
					return ret;
				default:
					ret += c;
					out << ret;
					return ret;

			}
		}
		case TT_STRING: {
					std::string str = obj->str_value_;
					out << str;
					return str;
				}
		case TT_PAIR: {
			      std::string ret = "(";
			      ret += write_pair(out, obj);
			      ret += ")";
			      out << ret;
			      return ret;
		      }
		case TT_PRIMITIVE_PROC:
		case TT_COMPOUND_PROC:
		      out << "#<procedure>";
		      return "#<procedure>";
		default: {
			std::cerr << "cannot write unknown type" << std::endl;
			exit(1);
		}
	}
} /* write */

/***************************** REPL ******************************/


