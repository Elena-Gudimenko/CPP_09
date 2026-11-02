#include "BitcoinExchange.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cctype>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) : _rates(other._rates) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
		_rates = other._rates;
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}

// Remove leading and trailing spaces/tabs
std::string BitcoinExchange::trim(const std::string &str)
{
	size_t start = str.find_first_not_of(" \t\r");
	if (start == std::string::npos)
		return "";
	size_t end = str.find_last_not_of(" \t\r");
	return str.substr(start, end - start + 1);
}

// Check format YYYY-MM-DD and that the date really exists
bool BitcoinExchange::isValidDate(const std::string &date)
{
	if (date.length() != 10 || date[4] != '-' || date[7] != '-')
		return false;
	for (size_t i = 0; i < date.length(); i++)
	{
		if (i == 4 || i == 7)
			continue;
		if (!std::isdigit(static_cast<unsigned char>(date[i])))
			return false;
	}

	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	if (month < 1 || month > 12)
		return false;

	int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int maxDay = daysInMonth[month - 1];

	bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
	if (month == 2 && isLeap)
		maxDay = 29;

	if (day < 1 || day > maxDay)
		return false;
	return true;
}

// Accept: optional sign, digits, optional '.' followed by digits
bool BitcoinExchange::isValidNumber(const std::string &str)
{
	size_t i = 0;
	bool hasDigit = false;

	if (str.empty())
		return false;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (i < str.length() && std::isdigit(static_cast<unsigned char>(str[i])))
	{
		hasDigit = true;
		i++;
	}
	if (i < str.length() && str[i] == '.')
	{
		i++;
		while (i < str.length() && std::isdigit(static_cast<unsigned char>(str[i])))
		{
			hasDigit = true;
			i++;
		}
	}
	if (i != str.length() || !hasDigit)
		return false;
	return true;
}

void BitcoinExchange::loadDatabase(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: could not open database.");

	std::string line;
	std::getline(file, line); // skip header "date,exchange_rate"

	while (std::getline(file, line))
	{
		if (trim(line).empty())
			continue;
		size_t comma = line.find(',');
		if (comma == std::string::npos)
			throw std::runtime_error("Error: invalid database line.");

		std::string date = trim(line.substr(0, comma));
		std::string rateStr = trim(line.substr(comma + 1));

		if (!isValidDate(date) || !isValidNumber(rateStr))
			throw std::runtime_error("Error: invalid database line.");

		_rates[date] = std::strtod(rateStr.c_str(), NULL);
	}
	if (_rates.empty())
		throw std::runtime_error("Error: empty database.");
}

void BitcoinExchange::processInput(const std::string &filename) const
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return;
	}

	std::string line;
	bool firstLine = true;

	while (std::getline(file, line))
	{
		// Skip the header only if it is the first line
		if (firstLine)
		{
			firstLine = false;
			if (trim(line) == "date | value")
				continue;
		}
		if (trim(line).empty())
			continue;

		size_t pipe = line.find('|');
		if (pipe == std::string::npos)
		{
			std::cerr << "Error: bad input => " << trim(line) << std::endl;
			continue;
		}

		std::string date = trim(line.substr(0, pipe));
		std::string valueStr = trim(line.substr(pipe + 1));

		if (!isValidDate(date) || !isValidNumber(valueStr))
		{
			std::cerr << "Error: bad input => " << trim(line) << std::endl;
			continue;
		}

		double value = std::strtod(valueStr.c_str(), NULL);
		if (value < 0)
		{
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}
		if (value > 1000)
		{
			std::cerr << "Error: too large a number." << std::endl;
			continue;
		}

		// lower_bound returns the first key >= date
		std::map<std::string, double>::const_iterator it = _rates.lower_bound(date);
		if (it == _rates.end() || it->first != date)
		{
			// No exact match: step back to the closest earlier date
			if (it == _rates.begin())
			{
				std::cerr << "Error: no data before => " << date << std::endl;
				continue;
			}
			--it;
		}
		std::cout << date << " => " << value << " = " << value * it->second << std::endl;
	}
}