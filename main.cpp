#include "item/box.hpp"
#include "item/rawrender.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>



int main()
{
	auto default_style = std::make_shared<BoxStyle>();

	default_style->tside = '-';
	default_style->bside = '-';
	default_style->rside = '|';
	default_style->lside = '|';

	default_style->tl_corner = '.';
	default_style->tr_corner = '.';
	default_style->bl_corner = '\'';
	default_style->br_corner = '\'';

	SimpleStringRenderer ssr;

	while(std::cin.get()) {
		Box box = {std::rand() % 100, std::rand() % 50, std::rand() % 100, std::rand() % 50, default_style};
		box.draw(ssr);
		ssr.print();
	}
}
