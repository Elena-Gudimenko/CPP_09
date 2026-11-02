#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <list>
#include <string>

class RPN
{
private:
	std::stack<long, std::list<long> >	_stack;

	static bool	isNumber(const std::string &token);
	static bool	isOperator(const std::string &token);
	void		applyOperator(char op);

public:
	RPN();
	RPN(const RPN &other);
	RPN &operator=(const RPN &other);
	~RPN();

	long	evaluate(const std::string &expression);
};

#endif