#ifndef NODE_HPP
#define NODE_HPP

#include <iostream> 

//This is abstract
class Node
{
	public: 
		virtual bool is_symbol()         { return false; }
		virtual bool is_pair()           { return false; }
		virtual bool is_the_empty_list() { return false; }
		virtual bool is_fixnum()         { return false; }
		virtual bool is_character()      { return false; }
		virtual bool is_string()         { return false; }
		virtual bool is_primitive_proc() { return false; }
		virtual bool is_compound_proc()  { return false; }
		virtual bool is_input_port()     { return false; }
		virtual bool is_output_port()    { return false; }
		virtual bool is_eof()            { return false; } 
		virtual bool is_boolean()        { return false; }
		virtual bool is_true()  { return false; }
		virtual bool is_false() { return false; }

		virtual bool equals(Node *b)
		{
			std::cerr << "Node::equals" << std::endl;
			return false;
		}

		virtual void set_car(Node *p_car)
		{
			std::cerr << "Error: Node: set_car()" << std::endl;
			std::cerr << "Should never reach" << std::endl;
			exit(1);
		}

		virtual void set_cdr(Node *p_cdr)
		{
			std::cerr << "Error: Node: set_cdr()" << std::endl;
			std::cerr << "Should never reach" << std::endl;
			exit(1);
		}

		virtual Node *car()
		{
			//FIXME i dont like it
			std::cerr << "Error: Node: car()" << std::endl;
			std::cerr << "Should never reach" << std::endl;
			exit(1);
		}

		virtual Node *cdr()
		{
			//FIXME i dont like it
			std::cerr << "Error: Node: car()" << std::endl;
			std::cerr << "Should never reach" << std::endl;
			exit(1);
		}

		virtual std::string write(std::ostream &out) = 0;
		virtual Node *eval(Node *env) = 0;

		virtual bool compare(class Fixnum *p_node) { return false; }
};

class Fixnum : public Node
{
	public:
		explicit Fixnum(long val) : value_(val) {}
		bool is_fixnum() { return true; }
		Node *eval(Node *env);
		bool is_true(); 
		bool is_false();
		long value() { return value_; }
		bool compare(Fixnum *p_node) { return true; }
		bool equals(Node *b)
		{
			Fixnum *a = dynamic_cast<Fixnum *>(b);
			if (a && a->value() == value_)
			{
				return true;
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		long value_;
};

class Boolean : public Node
{
	public:
		explicit Boolean(bool val) : value_(val) {}
		bool is_boolean() { return true; }
		bool is_true();
		bool is_false();
		Node *eval(Node *env);
		bool value() { return value_; }

		bool equals(Node *b)
		{
			Boolean *a = dynamic_cast<Boolean *>(b);
			if (a && a->value() == value_)
			{
				return true;
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		bool value_;
};

class Char : public Node
{
	public:
		explicit Char(char val) : value_(val) {}
		bool is_character() { return true; }
		Node *eval(Node *env);
		char value() { return value_; }

		bool equals(Node *b)
		{
			Char *a = dynamic_cast<Char *>(b);
			if (a && a->value() == value_)
			{
				return true;
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		char value_;
};

class String : public Node
{
	public:
		explicit String(const std::string &val) : value_(val) {}
		bool is_string() { return true; }
		Node *eval(Node *env);
		std::string value() { return value_; }

		bool equals(Node *b)
		{
			String *a = dynamic_cast<String *>(b);
			if (a && a->value() == value_)
			{
				return true;
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		std::string value_;
};

class List : public Node
{
	public:
	private:
};

class EmptyList : public List
{
	public:
		explicit EmptyList() {}
		bool is_the_empty_list() { return true; }

		bool equals(Node *b)
		{
			EmptyList *a = dynamic_cast<EmptyList *>(b);
			return a ? true : false;
		}

		std::string write(std::ostream &out);
		Node *eval(Node *env);
	private:
};

class Symbol : public Node
{
	public:
		explicit Symbol(const std::string &label);
		bool is_symbol() { return true; }
		std::string label() { return label_; }
		Node *eval(Node *env);

		bool equals(Node *b)
		{
			Symbol *a = dynamic_cast<Symbol *>(b);
			if (a && a->label() == label_)
			{
				return true;
			}
			return false;
		}


		bool is_same_label(const std::string &value) const
		{
			return !label_.compare(value);
		}

		std::string write(std::ostream &out);
	private:
		const std::string label_;

};


class Pair : public List
{
	public:
		explicit Pair(Node *car, Node *cdr) : car_(car), cdr_(cdr) {}
		bool is_pair() { return true; }
		Node *car() { return car_; }
		Node *cdr() { return cdr_; }
		void set_car(Node *p_node) { car_ = p_node; }
		void set_cdr(Node *p_node) { cdr_ = p_node; }
		Node *eval(Node *env);

		bool equals(Node *b)
		{
			Pair *a = dynamic_cast<Pair *>(b);
			if (a)
			{
				if (a->car()->equals(car_) && a->cdr()->equals(cdr_))
				{
					return true;
				}
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		std::string write_pair(std::ostream &out);

		Node *car_; // Μπορει να ειναι οτιδηποτε
		Node *cdr_; // auto einai pair ? 
};

class PrimitiveProc : public Node
{
	public:
		using PrimitiveProcFun = Node *(*)(Node *args);
		explicit PrimitiveProc(PrimitiveProcFun fun) : fun_(fun) {}
		bool is_primitive_proc() { return true; }
		Node *exec(Node *arguments) { return fun_(arguments); }
		PrimitiveProcFun fun() { return fun_; }
		Node *eval(Node *env) 
		{ 
			std::cerr << "PrimitiveProc: Should not reach" << std::endl;
			return nullptr;
		}

		bool equals(Node *b)
		{
			PrimitiveProc *a = dynamic_cast<PrimitiveProc *>(b);
			if (a && a->fun() == fun_)
			{
				return true;
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		PrimitiveProcFun fun_;
};


class CompoundProc : public Node
{
	public:
		explicit CompoundProc(Node *params, Node *body, Node *env) : 
       			parameters_(params), body_(body), env_(env)
			{}
		bool is_compound_proc() { return true; }
		Node *parameters() { return parameters_; }
		Node *body() { return body_; }
		Node *env() { return env_; }

		bool equals(Node *b)
		{
			CompoundProc *a = dynamic_cast<CompoundProc *>(b);
			if (a && parameters_ == a->parameters()
					&& body_ == a->body()
					&& body_ == a->env())
			{
				return true;
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		Node *parameters_;
		Node *body_;
		Node *env_;

};

class InputPort : public Node
{
	public:
		explicit InputPort(FILE *stream) : stream_(stream) {}
		bool is_input_port() { return true; }
		FILE *stream() { return stream_; }

		Node *eval(Node *env) {
			std::cerr << "InputPort::eval --> error" << std::endl;
			exit(1);
		}
		

		bool equals(Node *b)
		{
			InputPort *a = dynamic_cast<InputPort *>(b);
			if (a && stream_ == a->stream())
			{
				return true;
			}
			return false;
		}


		std::string write(std::ostream &out);
	private:
		FILE *stream_;//TODO replace FILE * with c++
};

class OutputPort : public Node
{
	public:
		explicit OutputPort(FILE *stream) : stream_(stream) {}
		bool is_output_port() { return true; }
		FILE *stream() { return stream_; }
		Node *eval(Node *env)
		{
			std::cerr << "OutputPort::eval --> error" << std::endl;
			exit(1);
		}

		bool equals(Node *b)
		{
			OutputPort *a = dynamic_cast<OutputPort *>(b);
			if (a && stream_ == a->stream())
			{
				return true;
			}
			return false;
		}

		std::string write(std::ostream &out);
	private:
		FILE *stream_;
};

class Eof : public Node
{
	public:
		Eof() {}
		bool is_eof() { return true; }
		Node *eval(Node *exp)
		{
			std::cerr << "Eof::eval --> Error" << std::endl;
			exit(1);
		}

		bool equals(Node *b)
		{
			Eof *a = dynamic_cast<Eof *>(b);
			return a ? true : false;
		}

		std::string write(std::ostream &out);
	private:
};


namespace gb
{
	extern Boolean   *n_false_obj;
	extern Boolean   *n_true_obj; 
	extern EmptyList *n_the_empty_list;
	extern Symbol    *n_quote_symbol;
	extern Node      *n_the_global_environment;
	extern Node      *n_the_empty_environment;
	extern Symbol    *n_ok_symbol;
	extern Eof       *n_eof_object;
}

class SymbolTable
{
	public:
		static SymbolTable &get();

		Node *get_element() const { return table_; }
		Symbol *find_symbol(const std::string &symbol) const;

		static Symbol *make_symbol(const std::string &value);
	private:
		SymbolTable();
		void add_symbol(Node *p_symbol);

		List *table_;
};


#endif 
