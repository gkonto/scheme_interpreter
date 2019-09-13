#ifndef NODE_HPP
#define NODE_HPP


//This is abstract
class Node
{
	public: 
		virtual bool is_symbol()         { return false; }
		virtual bool is_pair()           { return false; }
		virtual bool is_boolean()        { return false; }
		virtual bool is_the_empty_list() { return false; }
		virtual bool is_fixnum()         { return false; }
		virtual bool is_character()      { return false; }
		virtual bool is_string()         { return false; }
		virtual bool is_primitive_proc() { return false; }
		virtual bool is_compound_proc()  { return false; }
		virtual bool is_input_port()     { return false; }
		virtual bool is_output_port()    { return false; }
		virtual bool is_eof()            { return false; } 
};

class Fixnum : public Node
{
	public:
		explicit Fixnum(long val) : value_(val) {}
		virtual bool is_fixnum() { return true; }
	private:
		long value_;
};

class Boolean : public Node
{
	public:
		explicit Boolean(bool val) : value_(val) {}
		bool is_boolean() { return true; }
	private:
		bool value_;
};

class Char : public Node
{
	public:
		explicit Char(char val) : value_(val) {}
		bool is_character() { return true; }
	private:
		char value_;
};

class String : public Node
{
	public:
		explicit String(const std::string &val) : value_(val) {}
		bool is_string() { return true; }
	private:
		std::string value_;
};

class EmptyList : public Node
{
	public:
		explicit EmptyList() {}
		bool is_the_empty_list() { return true; }
	private:
};

class Symbol : public Node
{
	public:
		explicit Symbol(const std::string &label) : label_(label) {} 
		bool is_symbol() { return true; }
	private:
		const std::string label_;

};

class Pair : public Node
{
	public:
		explicit Pair(Node *car, Node *cdr) : car_(car), cdr_(cdr) {}
		bool is_pair() { return true; }
	private:
		Node *car_;
		Node *cdr_;
};

class PrimitiveProc : public Node
{
	public:
		using PrimitiveProcFun = Node *(*)(Node *args);
		explicit PrimitiveProc(PrimitiveProcFun fun) : fun_(fun) {}
		bool is_primitive_proc() { return true; }
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
	private:
		FILE *stream_;//TODO replace FILE * with c++
};

class OutputPort : public Node
{
	public:
		explicit OutputPort(FILE *stream) : stream_(stream) {}
		bool is_output_port() { return true; }
	private:
		FILE *stream_;
};

class Eof
{
	public:
		Eof() {}
		bool is_eof() { return true; }
	private:
};



#endif 
