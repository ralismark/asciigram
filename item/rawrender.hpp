#pragma once

#include "../canvas.hpp"
#include <map>

struct SimpleStringRenderer
	: public Canvas
{
	std::map<int, std::string> lines;
public:
	void extend_line(int lineno, size_t length)
	{
		auto& line = lines[lineno];
		if(line.size() <= length) {
			line.resize(length + 1, ' ');
		}
	}

	virtual void impl_set(char fill, int x, int y) override
	{
		this->extend_line(y, x);
		lines[y][x] = fill;
	}

	void print() const
	{
		int first_line = lines.begin()->first - 1;
		int last_line = lines.rbegin()->first + 1;

		for(int i = first_line; i < last_line; ++i) {
			if(lines.count(i)) {
				std::cout << lines.at(i);
			}
			std::cout << '\n';
		}
	}
};
