#include <string>
#include <sstream>
#include "node.hpp"
#include "parser.hpp"
#include "symboltable.hpp"

/*            DECLARATIONS           */
Node *make_environment();
/*            DECLARATIONS           */


namespace gb
{
	Boolean   *n_false_obj      = new Boolean(false);
	Boolean   *n_true_obj       = new Boolean(true);
	EmptyList *n_the_empty_list = new EmptyList();
	Symbol    *n_quote_symbol   = SymbolTable::make_symbol("quote");
	Symbol    *n_define_symbol  = SymbolTable::make_symbol("define");
	Symbol    *n_set_symbol     = SymbolTable::make_symbol("set!");
	Symbol    *n_ok_symbol      = SymbolTable::make_symbol("ok");
	Symbol    *n_if_symbol      = SymbolTable::make_symbol("if");
	Symbol    *n_lambda_symbol  = SymbolTable::make_symbol("lambda");
	Symbol    *n_begin_symbol   = SymbolTable::make_symbol("begin");
	Symbol    *n_cond_symbol    = SymbolTable::make_symbol("cond");
	Symbol    *n_else_symbol    = SymbolTable::make_symbol("else");
	Symbol    *n_let_symbol     = SymbolTable::make_symbol("let");
	Symbol    *n_and_symbol     = SymbolTable::make_symbol("and");
	Symbol    *n_or_symbol      = SymbolTable::make_symbol("or");
	Eof       *n_eof_object     = new Eof();
	Node      *n_the_global_environment = make_environment(); // TODO class Environment
	Node      *n_the_empty_environment = n_the_empty_list;
}


Node *enclosing_environment(Node *env)
{
	return env->cdr();
}


bool Fixnum::is_true()
{
	return true;
}

bool Fixnum::is_false()
{
	return false;
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

	std::string ret;
	ret += car_obj->write(out);
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


//TODO class Environment?
Node *lookup_variable_value(Node *var, Node *env)
{
	while (!env->is_the_empty_list())
	{
		Node *frame = first_frame(env);
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
		env = enclosing_environment(env);
	}
	std::cerr << "unbound variable" << std::endl;
	exit(1);
}

Node *Symbol::eval(Node *env)
{
	return lookup_variable_value(this, env);
}

Node *text_of_quotation(Node *exp)
{
	return exp->cdr()->car();
}

bool is_tagged_list(Node *expr, Node *tag)
{
	return expr->car() == tag;
}

static void set_variable_value(Node *var, Node *val, Node *env) 
{
	//FIXME bad code
    while (!env->is_the_empty_list()) {
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
        env = enclosing_environment(env);
    }
    std::cerr << "unbound variable" << std::endl;
    exit(1);
}

static Node *assignment_variable(Node *exp) 
{
	//FIXME bad code
	return exp->cdr()->car();
}

static Node *assignment_value(Node *exp)
{
	return exp->cdr()->cdr()->car();
}

Node *eval_assignment(Node *exp, Node *env)
{
	Node *assignment_val = assignment_value(exp);
	Node *evaluated_assignment = assignment_val->eval(env);
	Node *assignment_var = assignment_variable(exp);

    set_variable_value(assignment_var, evaluated_assignment, env);
    return gb::n_ok_symbol;
}

Node *definition_variable(Node *exp)
{
	Node *p_s = exp->cdr()->car();
	if (p_s->is_symbol())
	{
		return p_s;
	} else {
		return exp->cdr()->car()->car();
	}
	return nullptr;
}

Node *make_lambda(Node *parameters, Node *body)
{
	return new Pair(gb::n_lambda_symbol, new Pair(parameters, body));
}

Node *definition_value(Node *exp)
{
	Node *p_node = exp->cdr()->car();
	if (p_node->is_symbol())
	{
		return exp->cdr()->cdr()->car();
	} else {
		return make_lambda(exp->cdr()->car()->cdr(), exp->cdr()->cdr());
	}
}

static Node *eval_definition(Pair *exp, Node *env)
{
	Node *def_variable = definition_variable(exp);
	Node *def_value    = definition_value(exp);
	define_variable(def_variable, def_value->eval(env), env);
	return gb::n_ok_symbol;
}

static bool is_quoted(Node *expression)
{
	return is_tagged_list(expression, gb::n_quote_symbol);
}

static bool is_definition(Node *expression)
{
	return is_tagged_list(expression, gb::n_define_symbol);
}

static bool is_assignment(Node *expression)
{
	return is_tagged_list(expression, gb::n_set_symbol);
}

static bool is_variable(Node *expression)
{
	return expression->is_symbol();
}

static bool is_if(Node *expression)
{
	return is_tagged_list(expression, gb::n_if_symbol);
}


Node *if_predicate(Node *p_node)
{
	return p_node->cdr()->car();
}

Node *if_consequent(Node *p_node)
{
	return p_node->cdr()->cdr()->car();
}

Node *if_alternative(Node *p_node)
{
	Node *p_exp = p_node->cdr()->cdr()->cdr();

	if (p_exp->is_the_empty_list()) {
		return gb::n_false_obj;
	} else {
		return p_exp->car();
	}
}



static Node *op(Node *exp)
{
	return exp->car();
}

static Node *operands(Node *exp)
{
	return exp->cdr();
}

static bool is_no_operands(Node *ops)
{
	return ops->is_the_empty_list();
}

static Node *first_operand(Node *ops)
{
	return ops->car();
}

static Node *rest_operands(Node *ops)
{
	return ops->cdr();
}

Node *list_of_values(Node *exp, Node *env)
{
	if (is_no_operands(exp)) {
		return gb::n_the_empty_list;
	} else {
		Node *first_op = first_operand(exp);

		return new Pair(first_op->eval(env),
				list_of_values(rest_operands(exp), env));
	}
}

Node *eval_expression(Node *arguments)
{
	return arguments->car();
}

Node *eval_environment(Node *arguments)
{
	return arguments->cdr()->car();
}

Node *apply_operator(Node *arguments)
{
	return arguments->car();
}




Node *prepare_apply_operands(Node *arguments)
{
	Node *p_cdr = arguments->cdr();

	if (p_cdr->is_the_empty_list()) {
		return arguments->car();
	} else {
		return new Pair(arguments->car(), prepare_apply_operands(p_cdr));

	}
}

Node *apply_operands(Node *arguments)
{
	return prepare_apply_operands(arguments->cdr());
}

bool is_lambda(Node *exp) {
    return is_tagged_list(exp, gb::n_lambda_symbol);
}

Node *lambda_parameters(Node *exp) {
    return exp->cdr()->car();
}

Node *lambda_body(Node *exp) {
    return exp->cdr()->cdr();
}


bool is_begin(Node *exp)
{
	return is_tagged_list(exp, gb::n_begin_symbol);
}

Node *begin_actions(Node *exp)
{
	return exp->cdr();
}

bool is_last_exp(Node *seq)
{
	return seq->cdr()->is_the_empty_list();
}

Node *first_exp(Node *seq)
{
	return seq->car();
}

Node *rest_exps(Node *seq) {
    return seq->cdr();
}

bool is_cond(Node *exp)
{
	return is_tagged_list(exp, gb::n_cond_symbol);
}

Node *cond_clauses(Node *exp) {
    return exp->cdr();
}

Node *cond_predicate(Node *clause)
{
	return clause->car();
}

char is_cond_else_clause(Node *clause) {
    return cond_predicate(clause) == gb::n_else_symbol;
}

Node *sequence_to_exp(Node *seq) {

    if (seq->is_the_empty_list()) {
        return seq;
    } else if (is_last_exp(seq)) {
        return first_exp(seq);
    } else {
        return new Pair(gb::n_begin_symbol, seq);
    }
}

Node *cond_actions(Node *clause)
{
	return clause->cdr();
}

Node *make_if(Node *predicate, Node *consequent, Node *alternative) 
{
    return new Pair(gb::n_if_symbol,
                new Pair(predicate,
                     new Pair(consequent,
                          new Pair(alternative, gb::n_the_empty_list))));
}


Node *expand_clauses(Node *clauses) {
    
    if (clauses->is_the_empty_list()) {
        return gb::n_false_obj;
    } else {
        Node *first = clauses->car();
        Node *rest  = clauses->cdr();


        if (is_cond_else_clause(first)) {
            if (rest->is_the_empty_list()) {
                return sequence_to_exp(cond_actions(first));
            } else {
		    std::cerr << "else clause isn't last cond->if" << std::endl;
                exit(1);
            }
        } else {
            return make_if(cond_predicate(first),
                           sequence_to_exp(cond_actions(first)),
                           expand_clauses(rest));
        }
    }
}
Node *cond_to_if(Node *exp)
{
	return expand_clauses(cond_clauses(exp));
}


bool is_let(Node *exp)
{
	return is_tagged_list(exp, gb::n_let_symbol);
}


Node *let_bindings(Node *exp) {
    return exp->cdr()->car();
}

Node *let_body(Node *exp) {
    return exp->cdr()->cdr();
}


Node *binding_parameter(Node *binding) 
{
    return binding->car();
}


Node *binding_argument(Node *binding) 
{
    return binding->cdr()->car();
}


Node *bindings_parameters(Node *bindings) 
{
    if (bindings->is_the_empty_list())
    {
         return      gb::n_the_empty_list;
    } else {
              return  new Pair(binding_parameter(bindings->car()),
                    bindings_parameters(bindings->cdr()));
    }
}


Node *bindings_arguments(Node *bindings) 
{
	if (bindings->is_the_empty_list())
	{
		return gb::n_the_empty_list ;
	} else {
		return new Pair(binding_argument(bindings->car()),
			bindings_arguments(bindings->cdr()));
	}
}


Node *let_parameters(Node *exp) 
{
    return bindings_parameters(let_bindings(exp));
}


Node *let_arguments(Node *exp) 
{
    return bindings_arguments(let_bindings(exp));
}



bool is_and(Node *p_exp)
{
	return is_tagged_list(p_exp, gb::n_and_symbol);
}


Node *let_to_application(Node *exp)
{
    return new Pair(make_lambda(let_parameters(exp), let_body(exp)), let_arguments(exp));
}


Node *and_tests(Node *exp)
{
	return exp->cdr();
}


bool is_or(Node *exp)
{
	return is_tagged_list(exp, gb::n_or_symbol);
}


Node *or_tests(Node *exp)
{
	return exp->cdr();
}


Node *Pair::eval(Node *env)
{
	//TODO fix that code
	if (is_quoted(this))
	{
		return text_of_quotation(this);
	} else if (is_definition(this))
	{
		return eval_definition(this, env);
	} else if (is_assignment(this))
	{
		return eval_assignment(this, env);
	} else if (is_variable(this))
	{
		return lookup_variable_value(this, env);
	} else if (is_if(this))
	{
		Node *if_pred = if_predicate(this);
		Node *eval_pred = if_pred->eval(env);
		Node *exp = eval_pred->is_true() ? if_consequent(this) : if_alternative(this);
		
		return exp->eval(env);
	} else if (is_lambda(this)) {
		return new CompoundProc(lambda_parameters(this),
				lambda_body(this),
				env);
	} else if (is_begin(this)) {
		Node *exp = begin_actions(this);
		while (!is_last_exp(exp)) {
			Node *f_exp = first_exp(exp);
			f_exp->eval(env);
			exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		return exp->eval(env);
	} else if (is_cond(this)) {
			Node *exp = cond_to_if(this);
			return exp->eval(env);
	} else if (is_let(this)) {
		Node *exp = let_to_application(this);
		return exp->eval(env);
	} else if (is_and(this)) {
		Node *exp = and_tests(this);

		if (exp->is_the_empty_list()) {
		    return gb::n_true_obj;
		}

		while (!is_last_exp(exp)) {
			Node *p_first = first_exp(exp);
		    Node *result = p_first->eval(env);
		    if (result->is_false()) {
			return result;
		    }
		    exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		return exp->eval(env);
	} else if (is_or(this)) {
		Node *exp = or_tests(this);
		if (exp->is_the_empty_list()) {
		    return gb::n_false_obj;
		}
		while (!is_last_exp(exp)) {
			Node *p_first = first_exp(exp);
		    Node *result = p_first->eval(env);
		    if (result->is_true()) {
			return result;
		    }
		    exp = rest_exps(exp);
		}
		exp = first_exp(exp);
		return exp->eval(env);

	} else {
		Node *procedure = op(this)->eval(env);
		Node *arguments = list_of_values(operands(this), env);

		if (procedure->is_primitive_proc())
		{
			PrimitiveProc *pp = static_cast<PrimitiveProc *>(procedure);
			if (pp->fun() == eval_proc) {
				Node *exp = eval_expression(arguments);
				Node *env = eval_environment(arguments);
				return exp->eval(env);
			} else if (pp->fun() == apply_proc) {
				procedure = apply_operator(arguments);
				arguments = apply_operands(arguments);
			}
		}


		if (procedure->is_primitive_proc()) {
			PrimitiveProc *pp = static_cast<PrimitiveProc *>(procedure);
			return pp->exec(arguments);
		} else if (procedure->is_compound_proc()) {
			CompoundProc *cp = static_cast<CompoundProc *>(procedure);
			env = extend_environment(
					cp->parameters(),
					arguments,
					cp->env()
					);

			Node *exp = new Pair(gb::n_begin_symbol, cp->body());
			return exp->eval(env);
		} else {
			std::cerr << "unkown procedure type" << std::endl;
			exit(1);
		}

	}

	std::cerr << "Pair::eval" << std::endl;
	exit(1);
	return nullptr;
}

