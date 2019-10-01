#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <string>

class Node;
class Symbol;

class SymbolTable
{
	public:
		static SymbolTable &get();
		static class Symbol *make_symbol(const std::string &value);

		class Node   *get_element() const { return table_; }
		class Symbol *find_symbol(const std::string &symbol) const;
	private:
		SymbolTable();
		void add_symbol(Node *p_symbol);

		class Node *table_;
};

Node *first_frame(Node *p_pair);
Node *variables(Node *frame);
Node *values(Node *frame);
void define_variable(Node *var, Node *val, Node *env);
Node *eval_proc(Node *arguments);
Node *apply_proc(Node *arguments);
Node *extend_environment(Node *vars, Node *vals, Node *base_env);

#endif
