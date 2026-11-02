#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>

class PmergeMe
{
private:
	std::vector<int>	_vec;
	std::deque<int>		_deq;

	static int	parseNumber(const std::string &str);
	static void	sortVector(std::vector<int> &arr);
	static void	sortDeque(std::deque<int> &arr);

public:
	PmergeMe();
	PmergeMe(const PmergeMe &other);
	PmergeMe &operator=(const PmergeMe &other);
	~PmergeMe();

	void	run(int argc, char **argv);
};

#endif