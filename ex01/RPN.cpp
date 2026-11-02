#include "RPN.hpp"
#include <sstream>
#include <stdexcept>
#include <cctype>

RPN::RPN() {}

RPN::RPN(const RPN &other) : _stack(other._stack) {}

RPN &RPN::operator=(const RPN &other)
{
	if (this != &other)
		_stack = other._stack;
	return *this;
}

RPN::~RPN() {}

bool RPN::isNumber(const std::string &token)
{
	if (token.length() != 1)
		return false;
	if (!std::isdigit(static_cast<unsigned char>(token[0])))
		return false;
	return true;
}

bool RPN::isOperator(const std::string &token)
{
	if (token.length() != 1)
		return false;
	if (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/')
		return true;
	return false;
}

void RPN::applyOperator(char op)
{
	if (_stack.size() < 2)
		throw std::runtime_error("Error");

	// The top of the stack is the RIGHT operand
	long right = _stack.top();
	_stack.pop();
	long left = _stack.top();
	_stack.pop();

	long result = 0;
	if (op == '+')
		result = left + right;
	else if (op == '-')
		result = left - right;
	else if (op == '*')
		result = left * right;
	else
	{
		if (right == 0)
			throw std::runtime_error("Error");
		result = left / right;
	}
	_stack.push(result);
}

long RPN::evaluate(const std::string &expression)
{
	// Start from an empty stack in case evaluate() is called more than once
	while (!_stack.empty())
		_stack.pop();

	std::istringstream iss(expression);
	std::string token;

	while (iss >> token)
	{
		if (isNumber(token))
			_stack.push(token[0] - '0');
		else if (isOperator(token))
			applyOperator(token[0]);
		else
			throw std::runtime_error("Error");
	}

	if (_stack.size() != 1)
		throw std::runtime_error("Error");
	return _stack.top();
}