#ifndef NODE_HPP
#define NODE_HPP


//This is abstract
class Node
{

};

class Fixnum : public Node
{
	public:
		explicit Fixnum(long val) : value_(val) {}
	private:
		long value_;
};

class Boolean : public Node
{
	public:
		explicit Boolean(bool val) : value_(val) {}
	private:
		bool value_;
};

class Char : public Node
{
	public:
		explicit Char(char val) : value_(val) {}
	private:
		char value_;
};

class String : public Node
{
	public:
		explicit String(const std::string &val) : value_(val) {}
	private:
		std::string value_;
};

class EmptyList : public Node
{
	public:
		explicit EmptyList() {}
	private:
};

class Symbol : public Node
{
	public:
		explicit Symbol(const std::string &label) : label_(label) {} 
	private:
		const std::string label_;

};

class Pair : public Node
{
	public:
		explicit Pair(Node *car, Node *cdr) : car_(car), cdr_(cdr) {}
	private:
		Node *car_;
		Node *cdr_;
};

class PrimitiveProc : public Node
{
	public:
		using PrimitiveProcFun = Node *(*)(Node *args);

		explicit PrimitiveProc(PrimitiveProcFun fun) : fun_(fun) {}
	private:
		PrimitiveProcFun fun_;
};


class CompoundProc : public Node
{
	public:
		CompoundProc(Node *params, Node *body, Node *env) : 
       			parameters_(params), body_(body), env_(env)
			{}
	private:
		Node *parameters_;
		Node *body_;
		Node *env_;

};

class InputPort : public Node
{
	public:
		InputPort(FILE *stream) : stream_(stream) {}
	private:
		FILE *stream_;//TODO replace FILE * with c++
};

class OutputPort : public Node
{
	public:
		OutputPort(FILE *stream) : stream_(stream) {}
	private:
		FILE *stream_;
};

class Eof
{
	public:
		Eof() {}
	private:
}



#endif 
