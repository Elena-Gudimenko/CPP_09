#include "PmergeMe.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <climits>
#include <cctype>
#include <ctime>

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe &other) : _vec(other._vec), _deq(other._deq) {}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
	if (this != &other)
	{
		_vec = other._vec;
		_deq = other._deq;
	}
	return *this;
}

PmergeMe::~PmergeMe() {}

// Convert a string to a positive int; throw on anything else
int PmergeMe::parseNumber(const std::string &str)
{
	if (str.empty())
		throw std::runtime_error("Error");

	long result = 0;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(str[i])))
			throw std::runtime_error("Error");
		result = result * 10 + (str[i] - '0');
		if (result > INT_MAX)
			throw std::runtime_error("Error");
	}
	if (result == 0)
		throw std::runtime_error("Error");
	return static_cast<int>(result);
}

/* ************************************************************************** */
/*                            Ford-Johnson: vector                            */
/* ************************************************************************** */

void PmergeMe::sortVector(std::vector<int> &arr)
{
	if (arr.size() <= 1)
		return;

	// 1. Make pairs: first = larger (winner), second = smaller (loser)
	bool hasStraggler = false;
	int straggler = 0;
	if (arr.size() % 2 != 0)
	{
		hasStraggler = true;
		straggler = arr.back();
	}

	std::vector<std::pair<int, int> > pairs;
	for (size_t i = 0; i + 1 < arr.size(); i += 2)
	{
		if (arr[i] > arr[i + 1])
			pairs.push_back(std::make_pair(arr[i], arr[i + 1]));
		else
			pairs.push_back(std::make_pair(arr[i + 1], arr[i]));
	}

	// 2. Recursively sort the winners
	std::vector<int> winners;
	for (size_t i = 0; i < pairs.size(); i++)
		winners.push_back(pairs[i].first);
	sortVector(winners);

	// 3. Build pend so that pend[i] is the partner of winners[i]
	std::vector<int> pend;
	std::vector<bool> used(pairs.size(), false);
	for (size_t i = 0; i < winners.size(); i++)
	{
		for (size_t j = 0; j < pairs.size(); j++)
		{
			if (!used[j] && pairs[j].first == winners[i])
			{
				pend.push_back(pairs[j].second);
				used[j] = true;
				break;
			}
		}
	}

	// 4. Main chain: sorted winners, with b1 in front (b1 <= a1, the smallest winner)
	std::vector<int> chain = winners;
	chain.insert(chain.begin(), pend[0]);

	// 5. Insert the rest of pend in Jacobsthal order: b3 b2, b5 b4, b11..b6, ...
	size_t prevJ = 1;
	size_t currJ = 3;
	while (prevJ < pend.size())
	{
		size_t top = currJ;
		if (top > pend.size())
			top = pend.size();

		for (size_t k = top; k > prevJ; k--)
		{
			// b_k is <= a_k, so search only in the part of the chain before a_k
			std::vector<int>::iterator bound =
				std::lower_bound(chain.begin(), chain.end(), winners[k - 1]);
			std::vector<int>::iterator pos =
				std::lower_bound(chain.begin(), bound, pend[k - 1]);
			chain.insert(pos, pend[k - 1]);
		}

		size_t nextJ = currJ + 2 * prevJ;
		prevJ = currJ;
		currJ = nextJ;
	}

	// 6. The odd element has no partner: search the whole chain
	if (hasStraggler)
	{
		std::vector<int>::iterator pos =
			std::lower_bound(chain.begin(), chain.end(), straggler);
		chain.insert(pos, straggler);
	}

	arr = chain;
}

/* ************************************************************************** */
/*                            Ford-Johnson: deque                             */
/* ************************************************************************** */

void PmergeMe::sortDeque(std::deque<int> &arr)
{
	if (arr.size() <= 1)
		return;

	// 1. Make pairs: first = larger (winner), second = smaller (loser)
	bool hasStraggler = false;
	int straggler = 0;
	if (arr.size() % 2 != 0)
	{
		hasStraggler = true;
		straggler = arr.back();
	}

	std::deque<std::pair<int, int> > pairs;
	for (size_t i = 0; i + 1 < arr.size(); i += 2)
	{
		if (arr[i] > arr[i + 1])
			pairs.push_back(std::make_pair(arr[i], arr[i + 1]));
		else
			pairs.push_back(std::make_pair(arr[i + 1], arr[i]));
	}

	// 2. Recursively sort the winners
	std::deque<int> winners;
	for (size_t i = 0; i < pairs.size(); i++)
		winners.push_back(pairs[i].first);
	sortDeque(winners);

	// 3. Build pend so that pend[i] is the partner of winners[i]
	std::deque<int> pend;
	std::deque<bool> used(pairs.size(), false);
	for (size_t i = 0; i < winners.size(); i++)
	{
		for (size_t j = 0; j < pairs.size(); j++)
		{
			if (!used[j] && pairs[j].first == winners[i])
			{
				pend.push_back(pairs[j].second);
				used[j] = true;
				break;
			}
		}
	}

	// 4. Main chain: sorted winners, with b1 in front
	std::deque<int> chain = winners;
	chain.push_front(pend[0]);

	// 5. Insert the rest of pend in Jacobsthal order
	size_t prevJ = 1;
	size_t currJ = 3;
	while (prevJ < pend.size())
	{
		size_t top = currJ;
		if (top > pend.size())
			top = pend.size();

		for (size_t k = top; k > prevJ; k--)
		{
			std::deque<int>::iterator bound =
				std::lower_bound(chain.begin(), chain.end(), winners[k - 1]);
			std::deque<int>::iterator pos =
				std::lower_bound(chain.begin(), bound, pend[k - 1]);
			chain.insert(pos, pend[k - 1]);
		}

		size_t nextJ = currJ + 2 * prevJ;
		prevJ = currJ;
		currJ = nextJ;
	}

	// 6. The odd element
	if (hasStraggler)
	{
		std::deque<int>::iterator pos =
			std::lower_bound(chain.begin(), chain.end(), straggler);
		chain.insert(pos, straggler);
	}

	arr = chain;
}

/* ************************************************************************** */
/*                                    run                                     */
/* ************************************************************************** */

void PmergeMe::run(int argc, char **argv)
{
	if (argc < 2)
		throw std::runtime_error("Error");

	// Validate all arguments before doing anything
	for (int i = 1; i < argc; i++)
		parseNumber(argv[i]);

	std::cout << "Before:";
	for (int i = 1; i < argc; i++)
		std::cout << " " << parseNumber(argv[i]);
	std::cout << std::endl;

	// vector: fill + sort, both timed
	std::clock_t start = std::clock();
	_vec.clear();
	for (int i = 1; i < argc; i++)
		_vec.push_back(parseNumber(argv[i]));
	sortVector(_vec);
	std::clock_t end = std::clock();
	double vecTime = static_cast<double>(end - start) * 1000000.0 / CLOCKS_PER_SEC;

	// deque: fill + sort, both timed
	start = std::clock();
	_deq.clear();
	for (int i = 1; i < argc; i++)
		_deq.push_back(parseNumber(argv[i]));
	sortDeque(_deq);
	end = std::clock();
	double deqTime = static_cast<double>(end - start) * 1000000.0 / CLOCKS_PER_SEC;

	std::cout << "After:";
	for (size_t i = 0; i < _vec.size(); i++)
		std::cout << " " << _vec[i];
	std::cout << std::endl;

	std::cout << std::fixed << std::setprecision(5);
	std::cout << "Time to process a range of " << _vec.size()
			  << " elements with std::vector : " << vecTime << " us" << std::endl;
	std::cout << "Time to process a range of " << _deq.size()
			  << " elements with std::deque  : " << deqTime << " us" << std::endl;
}