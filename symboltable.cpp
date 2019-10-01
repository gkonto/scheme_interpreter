#include <sstream>
#include <fstream>
#include "symboltable.hpp"
#include "node.hpp"
#include "parser.hpp"

Node *environment_proc(Node *arguments);

SymbolTable::SymbolTable() 
	: table_(gb::n_the_empty_list) 
{
}


SymbolTable &SymbolTable::get()
{
	static SymbolTable table;

	return table;
}


void SymbolTable::add_symbol(Node *p_symbol)
{
	table_ = new Pair(p_symbol, table_);
}


Symbol *SymbolTable::find_symbol(const std::string &symbol) const
{
	Node *element = get_element();

	while (!element->is_the_empty_list())
	{
		//TODO I dont like this
		Symbol *car_obj = static_cast<Symbol *>(element->car());
		if (car_obj->is_same_label(symbol)) {
			return car_obj;
		}
		element = element->cdr();
	}
	return 0;
}


Symbol *SymbolTable::make_symbol(const std::string &value)
{
	SymbolTable &s_table = get();
	Symbol *found_s = s_table.find_symbol(value);

	if (!found_s) {
		found_s = new Symbol(value);
		s_table.add_symbol(found_s);
	}

	return found_s;
}


static void add_binding_to_frame(Node *var, Node *val, Node *frame) 
{
    frame->set_car(new Pair(var, frame->car()));
    frame->set_cdr(new Pair(val, frame->cdr()));
}


Node *first_frame(Node *p_pair)
{
	return p_pair->car();
}

Node *variables(Node *frame)
{
	return frame->car();
}

Node *values(Node *frame)
{
	return frame->cdr();
}


void define_variable(Node *var, Node *val, Node *env) 
{

    Node *frame = first_frame(env);    
    Node *vars = variables(frame);
    Node *vals = values(frame);

    while (!vars->is_the_empty_list()) {
        if (var == vars->car()) {
            vals->set_car(val);
            return;
        }
        vars = vars->cdr();
        vals = vals->cdr();
    }
    add_binding_to_frame(var, val, frame);
}


Node *make_primitive_proc(Node *(*fn)(struct Node *arguments))
{
	return new PrimitiveProc(fn);
}


Node *is_null_proc(Node *arguments)
{
	Node *p_car = arguments->car();
	return p_car->is_the_empty_list() ? gb::n_true_obj : gb::n_false_obj;
}


Node *is_symbol_proc(Node *arguments)
{
	Node *p_car = arguments->car();
	return p_car->is_symbol() ? gb::n_true_obj : gb::n_false_obj;
}


Node *is_integer_proc(Node *arguments)
{
	Node *p_car = arguments->car();
	return p_car->is_fixnum() ? gb::n_true_obj : gb::n_false_obj;
}


Node *is_char_proc(Node *arguments) 
{
	Node *p_car = arguments->car();
	return p_car->is_character() ? gb::n_true_obj : gb::n_false_obj;
}


Node *is_string_proc(Node *arguments) 
{
	Node *p_car = arguments->car();
	return p_car->is_string() ? gb::n_true_obj : gb::n_false_obj;
}


Node *is_pair_proc(Node *arguments) 
{
	Node *p_car = arguments->car();
	return p_car->is_pair() ? gb::n_true_obj : gb::n_false_obj;
}


Node *is_procedure_proc(Node *arguments) 
{
    Node *obj = arguments->car();

    return (obj->is_primitive_proc() ||
            obj->is_compound_proc()) ? gb::n_true_obj :	gb::n_false_obj;
}


Node *char_to_integer_proc(Node *arguments) 
{
	Char *p_car = static_cast<Char *>(arguments->car());
	return new Fixnum(p_car->value());
}


Node *number_to_string_proc(Node *arguments) 
{
	Fixnum *p_car = static_cast<Fixnum *>(arguments->car());
	std::string buf = std::to_string(p_car->value());
	return new String(buf);
}


Node *string_to_number_proc(Node *arguments) 
{
	long x = 0;
	String *p_car = static_cast<String *>(arguments->car());
	std::stringstream ss(p_car->value());
	ss >> x;
	return new Fixnum(x);
}


Node *symbol_to_string_proc(Node *arguments) 
{
	Symbol *p_car = static_cast<Symbol *>(arguments->car());
	return new String(p_car->label());
}


Node *string_to_symbol_proc(Node *arguments) 
{
	String *p_car = static_cast<String *>(arguments->car());
	return SymbolTable::make_symbol(p_car->value());
}


Node *sub_proc(Node *arguments) 
{
    long result;
    
    Fixnum *p_car = static_cast<Fixnum *>(arguments->car());

    result = p_car->value();
    arguments = arguments->cdr();
    while (!arguments->is_the_empty_list()) {
	p_car = static_cast<Fixnum *>(arguments->car());
	result -= p_car->value();
	arguments = arguments->cdr();
    }
    return new Fixnum(result);
}


Node *mul_proc(Node *arguments) 
{
	long result = 1;

	while (!arguments->is_the_empty_list()) {
		Fixnum *p_car = static_cast<Fixnum *>(arguments->car());
		result *= p_car->value();
		arguments = arguments->cdr();
	}
	return new Fixnum(result);
}


Node *quotient_proc(Node *arguments) 
{
	Fixnum *p_car  = static_cast<Fixnum *>(arguments->car());
	Fixnum *p_cadr = static_cast<Fixnum *>(arguments->cdr()->car());

	return new Fixnum(p_car->value()/ (p_cadr->value()));
}


Node *remainder_proc(Node *arguments) 
{
	Fixnum *p_car = static_cast<Fixnum *>(arguments->car());
	Fixnum *p_cadr = static_cast<Fixnum *>(arguments->cdr()->car());

	return new Fixnum(p_car->value() % p_cadr->value());
}


Node *is_number_equal_proc(Node *arguments) 
{
	long value;
	Fixnum *p_car = static_cast<Fixnum *>(arguments->car());
	value = p_car->value();
	arguments = arguments->cdr();
	while (!arguments->is_the_empty_list()) {
		p_car = static_cast<Fixnum *>(arguments->car());
		if (value != p_car->value()) {
			return gb::n_false_obj;
		}
		arguments = arguments->cdr();
	}
	return gb::n_true_obj;
}


Node *is_less_than_proc(Node *arguments) 
{
	long previous;
	long next;

	Fixnum *p_car = static_cast<Fixnum *>(arguments->car());

	previous = p_car->value();
	arguments = arguments->cdr();

	while (!arguments->is_the_empty_list()) {
		p_car = static_cast<Fixnum *>(arguments->car());
		next = p_car->value();
		if (previous < next) {
			previous = next;
		} else {
			return gb::n_false_obj;
		}
		arguments = arguments->cdr();
	}
	return gb::n_true_obj;
}


Node *is_greater_than_proc(Node *arguments) 
{
	long previous;
	long next;

	Fixnum *p_car = static_cast<Fixnum *>(arguments->car());
	previous = p_car->value();
	arguments = arguments->cdr();

	while (!arguments->is_the_empty_list()) {
		p_car = static_cast<Fixnum *>(arguments->car());
		next = p_car->value();
		if (previous > next) {
			previous = next;
		} else {
			return gb::n_false_obj;
		}
		arguments = arguments->cdr();
	}
	return gb::n_true_obj;
}


Node *cons_proc(Node *arguments) 
{
	Node *p_car = arguments->car();
	Node *p_cadr = arguments->cdr()->car();

	return new Pair(p_car, p_cadr);
}


Node *car_proc(Node *arguments) 
{
    return arguments->car()->car();
}


Node *cdr_proc(Node *arguments) 
{
    return arguments->car()->cdr();
}


Node *set_car_proc(Node *arguments) 
{
	Node *p_car = arguments->car();
	Node *p_cadr = arguments->cdr()->car();
	p_car->set_car(p_cadr);
	return gb::n_ok_symbol;
}


Node *set_cdr_proc(Node *arguments) 
{
	Node *p_car = arguments->car();
	Node *p_cadr = arguments->cdr()->car();

	p_car->set_cdr(p_cadr);
	return gb::n_ok_symbol;
}


Node *list_proc(Node *arguments) 
{
    return arguments;
}


Node *is_eq_proc(Node *arguments) 
{
	Node *obj1 = arguments->car();
	Node *obj2 = arguments->cdr()->car();

	return obj1->equals(obj2) ? gb::n_true_obj : gb::n_false_obj;
}


Node *apply_proc(Node *arguments)
{
	std::cerr << "illegal state: The body of the apply primitive procedure should not execute." << std::endl;
	exit(1);
}


Node *eval_proc(Node *arguments)
{
	std::cerr << "illegal state: The body of the eval primitive procedure should not execute." << std::endl;
	exit(1);
}


Node *interaction_environment_proc(Node *arguments)
{
	return gb::n_the_global_environment;
}

static Node *make_frame(Node *vars, Node *vals)
{
	return new Pair(vars, vals);
}



Node *extend_environment(Node *vars, Node *vals, Node *base_env)
{
	return new Pair(make_frame(vars, vals), base_env);
}


static Node *setup_environment()
{
	Node *initial_env = extend_environment(
			gb::n_the_empty_list,
			gb::n_the_empty_list,
			gb::n_the_empty_environment);

	return initial_env;
}


Node *null_environment_proc(Node *arguments)
{
	return setup_environment();
}


Node *is_boolean_proc(Node *arguments)
{
	Node *p_car = arguments->car();
	return p_car->is_boolean() ? gb::n_true_obj : gb::n_false_obj;
}


Node *integer_to_char_proc(Node *arguments) 
{
	Fixnum *p_car = static_cast<Fixnum *>(arguments->car());
	return new Char(p_car->value());
}

Node *add_proc(Node *arguments)
{
	long result = 0;
    
	while (!arguments->is_the_empty_list()) {
		Fixnum *p_num = static_cast<Fixnum *>(arguments->car());
		result += p_num->value(); 
		arguments = arguments->cdr();
	}
	return new Fixnum(result);
}

Node *load_proc(Node *arguments)
{
	String *p_str = static_cast<String *>(arguments->car());
	std::string filename(p_str->value());

	std::ifstream ifs(filename, std::istream::in);

	if (ifs.fail()) {
		std::cerr << "could not load file \"" << filename << "\"" << std::endl;
		exit(1);
	}

	Node *exp = nullptr;
	Node *result = nullptr;

	Parser parser(ifs);
	while ((exp = parser.read()) != NULL) {
		result = exp->eval(gb::n_the_global_environment);
	}

	ifs.close();
	return result;
}


Node *open_input_port_proc(Node *arguments)
{
	String *p_str = static_cast<String *>(arguments->car());
	std::string filename(p_str->value());
	FILE *in = fopen(filename.c_str(), "r");
	if (in == nullptr) {
		std::cerr << "could not open file \" " << filename << "\"" << std::endl;
		exit(1);
	}
	return new InputPort(in);
}


Node *close_input_port_proc(Node *arguments)
{
	InputPort *p_in = static_cast<InputPort *>(arguments->car());
	int result = fclose(p_in->stream());
	if (result == EOF)
	{
		std::cerr << "could not close input port" << std::endl;
		exit(1);
	}
	return gb::n_ok_symbol;
}


Node *is_input_port_proc(Node *arguments)
{
	Node *p_car = arguments->car();
	return p_car->is_input_port() ? gb::n_true_obj : gb::n_false_obj;
}


Node *read_proc(Node *arguments) 
{
	InputPort *p_car = static_cast<InputPort *>(arguments->car());
	FILE *in = arguments->is_the_empty_list() ? stdin : p_car->stream();

	//TODO not sure if correct
	std::fstream ini;
	ini << in;
	Parser parser(ini);
	Node *result = parser.read();

	return !result ? gb::n_eof_object : result;
}


Node *read_char_proc(Node *arguments)
{
	OutputPort *p_out = static_cast<OutputPort *>(arguments->car());
	FILE *in = arguments->is_the_empty_list() ? stdin : p_out->stream();

	std::fstream ini;
	ini << in;
	char result = ini.get();
	if (result == EOF) {
		return gb::n_eof_object;
	} else {
		return new Char(result);
	}
}


Node *peek_char_proc(Node *arguments) 
{
	InputPort *p_in = static_cast<InputPort *>(arguments->car());
	FILE *in = arguments->is_the_empty_list() ? stdin : p_in->stream();

	std::fstream ini;
	ini << in;
	char result = ini.peek();

	if (result == EOF) {
		return gb::n_eof_object;
	} else {
		return new Char(result);
	}
}


Node *is_eof_object_proc(Node *arguments)
{
	Node *p_car = arguments->car();
	return p_car->is_eof() ? gb::n_true_obj : gb::n_false_obj;
}


Node *open_output_port_proc(Node *arguments)
{
	String *p_string = static_cast<String *>(arguments->car());
	std::string filename(p_string->value());
	FILE *out = fopen(filename.c_str(), "w");
	if (!out)
	{
		std::cerr << "could not open file \"" << filename << "\"";
		exit(1);
	}
	return new OutputPort(out);
}


Node *close_output_port_proc(Node *arguments)
{
	OutputPort *p_out = static_cast<OutputPort *>(arguments->car());
	int result = fclose(p_out->stream());
	if (result == EOF) {
		std::cerr << "could not close output port" << std::endl;
		exit(1);
	}
	return gb::n_ok_symbol;
}


Node *is_output_port_proc(Node *arguments)
{
	Node *p_car = arguments->car();
	return p_car->is_output_port() ? gb::n_true_obj : gb::n_false_obj;
}


Node *write_char_proc(Node *arguments) 
{
    Node *character = arguments->car();
    arguments = arguments->cdr();

    OutputPort *p_port = static_cast<OutputPort *>(arguments->car());
    FILE *out = arguments->is_the_empty_list() ? stdout : p_port->stream();

    Char *p_char = static_cast<Char *>(character);
    putc(p_char->value(), out);    
    fflush(out);

    return gb::n_ok_symbol;
}


Node *write_proc(Node *arguments) 
{
    OutputPort *exp = static_cast<OutputPort *>(arguments->car());
    arguments = arguments->cdr();

    OutputPort *p_out = static_cast<OutputPort *>(arguments->car());
    FILE *out = arguments->is_the_empty_list() ? stdout : p_out->stream();

    std::fstream ini;
    ini << out;
    exp->write(ini);
    ini.flush();

    return gb::n_ok_symbol;
}


Node *error_proc(Node *arguments)
{
	Node *p_car = arguments->car();

	while (!arguments->is_the_empty_list()) {
		p_car->write(std::cerr);
		std::cerr << " ";
		arguments = arguments->cdr();
	};
	std::cout << std::endl << "exiting" << std::endl;

	exit(1);
}




void populate_environment(Node *env)
{

#define add_procedure(scheme_name, c_name)              \
	define_variable(SymbolTable::make_symbol(scheme_name),           \
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
    add_procedure("eq?"                    , is_eq_proc);
    add_procedure("apply"                  , apply_proc);
    add_procedure("interaction-environment", interaction_environment_proc);
    add_procedure("null-environment"       , null_environment_proc);
    add_procedure("environment"            , environment_proc);
    add_procedure("eval"                   , eval_proc);
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


Node *make_environment()
{
	Node *env = setup_environment();
	populate_environment(env);

	return env;
}



Node *environment_proc(Node *arguments)
{
	return make_environment();
}


