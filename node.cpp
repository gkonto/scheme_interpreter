#include <string>
#include <sstream>
#include "node.hpp"
#include "object.hpp"

static Node *make_frame(Node *vars, Node *vals)
{
	return new Pair(vars, vals);
}

static Node *extend_environment(Node *vars, Node *vals, Node *base_env)
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

void populate_environment(Node *env)
{
}

static Node *make_environment()
{
	Node *env = setup_environment();
	populate_environment(env);

	return env;
}

namespace gb
{
	Boolean *n_false_obj  = new Boolean(false);
	Boolean *n_true_obj   = new Boolean(true);
	EmptyList *n_the_empty_list = new EmptyList();

	Symbol *n_quote_symbol  = SymbolTable::make_symbol("quote");
	Symbol *n_define_symbol = SymbolTable::make_symbol("define");
	Symbol *n_set_symbol    = SymbolTable::make_symbol("set!");
	Symbol *n_ok_symbol     = SymbolTable::make_symbol("ok");

	Node *the_empty_environment = n_the_empty_list;
	Symbol *n_if_symbol = SymbolTable::make_symbol("if");
	Symbol *n_lambda_symbol = SymbolTable::make_symbol("lambda");
	Symbol *n_begin_symbol  = SymbolTable::make_symbol("begin");
	Symbol *n_cond_symbol   = SymbolTable::make_symbol("cond");
	Symbol *n_else_symbol   = SymbolTable::make_symbol("else");
	Symbol *n_let_symbol    = SymbolTable::make_symbol("let");
	Symbol *n_and_symbol    = SymbolTable::make_symbol("and");
	Symbol *n_or_symbol     = SymbolTable::make_symbol("or");

	Eof *n_eof_object = new Eof();

	Node *n_the_global_environment = make_environment(); // TODO class Environment
	Node *n_the_empty_environment = n_the_empty_list;
}


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


bool Boolean::is_false()
{
	return this == gb::n_false_obj;
}

bool Boolean::is_true()
{
	return !is_false();
}

std::string Fixnum::write(std::ostream &out)
{
	std::stringstream ss;
	ss << value_;
	out << ss.str();

	return ss.str();
}

std::string Boolean::write(std::ostream &out)
{
	std::stringstream ss;

	std::string val = is_false() ? "#f" : "#t";
	ss << val;
	out << val;
	return ss.str();
}

std::string Char::write(std::ostream &out)
{
	char c = value_;
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

std::string String::write(std::ostream &out)
{
	std::string str = value_;
	out << str;

	return str;
}

std::string EmptyList::write(std::ostream &out)
{
	out << "()";	
	return "()";
}

Node *EmptyList::eval(Node *env)
{
	std::cout << "Entering EmptyList::eval()" << std::endl;
	return nullptr;
}

Symbol::Symbol(const std::string &value)
	: label_(value) 
{
}

std::string Symbol::write(std::ostream &out)
{
	out << label_;
	return label_;
}

std::string Pair::write(std::ostream &out)
{
	std::string ret = "(";
	ret += write_pair(out);
	ret += ")";
	out << ret;

	return ret;
}

std::string Pair::write_pair(std::ostream &out)
{
	Node *car_obj = car();
	Node *cdr_obj = cdr();

	std::string ret(car_obj->write(out));

	if (cdr_obj->is_pair()) {
		ret += " ";
		//FIXME: I dont like this cast
		Pair *cdr = static_cast<Pair *>(cdr_obj);
		ret += cdr->write_pair(out);
	} else if (cdr_obj->is_the_empty_list()) {
		return ret;
	} else {
		ret += " . ";
		ret += cdr_obj->write(out);
	}

	return ret;

}

std::string PrimitiveProc::write(std::ostream &out)
{
	out << "#<primitive-procedure>";
	return "#<primitive-procedure>";
}

std::string CompoundProc::write(std::ostream &out)
{
	out << "#<compound-procedure>";
	return "#<compound-procedure>";
}

std::string InputPort::write(std::ostream &out)
{
	out << "#<input-port>";
	return "#<input-port>";
}

std::string OutputPort::write(std::ostream &out)
{
	out << "#<output-port>";
	return "#<output-port>";
}

std::string Eof::write(std::ostream &out)
{
	out << "#<eof>";
	return "#<eof>";
}


Node *Fixnum::eval(Node *env)
{
	return this;
}

Node *Boolean::eval(Node *env)
{
	return this;
}

Node *Char::eval(Node *env)
{
	return this;
}

Node *String::eval(Node *env)
{
	return this;
}

Pair *first_frame(Pair *p_pair)
{
	return static_cast<Pair *>(p_pair->car());
}

Node *variables(Pair *frame)
{
	return frame->car();
}

Node *values(Pair *frame)
{
	return frame->cdr();
}

Node *enclosing_environment(Pair *env)
{
	return env->cdr();
}

//TODO class Environment?
Node *lookup_variable_value(Symbol *var, Node *env)
{
	while (!env->is_the_empty_list())
	{
		Pair *pair_env = static_cast<Pair *>(env);

		Pair *frame = first_frame(pair_env);
		Node *vars  = variables(frame);
		Node *vals  = values(frame);

		while (!vars->is_the_empty_list()) 
		{
			Pair *temp_vars = static_cast<Pair *>(vars);
			Pair *temp_vals = static_cast<Pair *>(vals);
			if (var == temp_vars->car()) {
				return temp_vals->car();
			}
			vars = temp_vars->cdr();
			vals = temp_vals->cdr();
		}
		env = enclosing_environment(pair_env);
	}
	std::cerr << "unbound variable" << std::endl;
	exit(1);
}

Node *Symbol::eval(Node *env)
{
	std::cout << "Symbol::eval" << std::endl;
	return lookup_variable_value(this, env);
}

Node *text_of_quotation(Pair *exp, Node *env)
{
	//FIXME bad code
	Pair *p_cdr = static_cast<Pair *>(exp->cdr());
	return p_cdr->car();
}

bool is_tagged_list(Pair *expr, Node *tag)
{
	//FIXME bad code
	return expr->car() == tag;
}

static void set_variable_value(Node *var, Node *val, Node *env) 
{
	//FIXME bad code
    while (!env->is_the_empty_list()) {
	    Pair *pair_env = static_cast<Pair *>(env);
        Pair *frame = first_frame(pair_env);
        Node *vars = variables(frame);
        Node *vals = values(frame);

        while (!vars->is_the_empty_list()) {
		Pair *temp_vars = static_cast<Pair *>(vars);
		Pair *temp_vals = static_cast<Pair *>(vals);
            if (var == temp_vars->car()) {
                temp_vals->set_car(val);
                return;
            }


            vars = temp_vars->cdr();
            vals = temp_vals->cdr();
        }
        env = enclosing_environment(pair_env);
    }
    std::cerr << "unbound variable" << std::endl;
    exit(1);
}

static Node *assignment_variable(Pair *exp) 
{
	//FIXME bad code
	Pair *p_cdr = static_cast<Pair *>(exp->cdr());
	return p_cdr->car();
}

static Node *assignment_value(Pair *exp)
{
	//FIXME bad code
	Pair *temp1 = static_cast<Pair *>(exp->cdr());
	Pair *temp2 = static_cast<Pair *>(temp1->cdr());
	return temp2->car();
}

Node *eval_assignment(Pair *exp, Node *env)
{
	//TODO really bad code
	Node *assignment_val = assignment_value(exp);
	Node *evaluated_assignment = assignment_val->eval(env);

	Node *assignment_var = assignment_variable(exp);

    set_variable_value(assignment_var, evaluated_assignment, env);
    return gb::n_ok_symbol;
}

Node *Pair::eval(Node *env)
{
	//TODO fix that code
	if (is_tagged_list(this, gb::n_quote_symbol)) {
		return text_of_quotation(this, env);
	} else if (is_tagged_list(this, gb::n_set_symbol)) {
		return eval_assignment(this, env);
	}

	std::cerr << "Pair::eval" << std::endl;
	exit(1);
	return nullptr;
}

