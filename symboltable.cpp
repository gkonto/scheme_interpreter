#include "symboltable.hpp"
#include "node.hpp"


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


