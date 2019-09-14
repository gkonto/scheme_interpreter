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
};

class Fixnum : public Node
{
	public:
		explicit Fixnum(long val) : value_(val) {}
		bool is_fixnum() { return true; }

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

		std::string write(std::ostream &out);
	private:
		bool value_;
};

class Char : public Node
{
	public:
		explicit Char(char val) : value_(val) {}
		bool is_character() { return true; }

		std::string write(std::ostream &out);
	private:
		char value_;
};

class String : public Node
{
	public:
		explicit String(const std::string &val) : value_(val) {}
		bool is_string() { return true; }

		std::string write(std::ostream &out);
	private:
		std::string value_;
};

class EmptyList : public Node
{
	public:
		explicit EmptyList() {}
		bool is_the_empty_list() { return true; }

		std::string write(std::ostream &out);
	private:
};

class Symbol : public Node
{
	public:
		explicit Symbol(const std::string &label);
		bool is_symbol() { return true; }
		std::string label() { return label_; }

		bool is_same_label(const std::string &value) const
		{
			return !label_.compare(value);
		}

		std::string write(std::ostream &out);
	private:
		const std::string label_;

};

class Pair : public Node
{
	public:
		explicit Pair(Node *car, Node *cdr) : car_(car), cdr_(cdr) {}
		bool is_pair() { return true; }
		Node *car() { return car_; }
		Node *cdr() { return cdr_; }

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

		std::string write(std::ostream &out);
	private:
		FILE *stream_;//TODO replace FILE * with c++
};

class OutputPort : public Node
{
	public:
		explicit OutputPort(FILE *stream) : stream_(stream) {}
		bool is_output_port() { return true; }

		std::string write(std::ostream &out);
	private:
		FILE *stream_;
};

class Eof
{
	public:
		Eof() {}
		bool is_eof() { return true; }

		std::string write(std::ostream &out);
	private:
};


namespace gb
{
	extern Boolean   *n_false_obj;
	extern Boolean   *n_true_obj; 
	extern EmptyList *n_the_empty_list;
}



#endif 
