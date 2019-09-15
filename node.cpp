#include <string>
#include <sstream>
#include "node.hpp"
#include "object.hpp"

namespace
{
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
		if (!car_obj->is_same_label(symbol)) {
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
		Symbol *found_s = new Symbol(value);
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

Node *Symbol::eval(Node *env)
{
	return nullptr;
	//return lookup_variable_value(env);
}

Node *Pair::eval(Node *env)
{
	return nullptr;
}

