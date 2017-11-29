#include "cursor.hpp"

point cur;
point region;

struct OwnerFinder
	: public Canvas
{
	int current_id;
	int target_id;
	int tx, ty;
public:
	OwnerFinder(int x, int y)
		: current_id(-1), target_id(-1), tx(x), ty(y)
	{
	}

	virtual void impl_set(char /* fill */, int x, int y) override
	{
		if(x == tx && y == ty) {
			target_id = current_id;
		}
	}
};

int idhere()
{
	OwnerFinder of(cur.x, cur.y);
	for(unsigned int i = 0; i < es.elements.size(); ++i) {
		// need to manually set id for each element
		// can't draw es directly
		of.current_id = i;
		of.draw(*es.elements[i]);
	}
	return of.target_id;
}

struct OwnerFinderRegion
	: public Canvas
{
	std::set<int> included;
	point min, max;
	int current_id;
public:
	OwnerFinderRegion(int minx, int miny, int maxx, int maxy)
		: min(minx, miny), max(maxx, maxy)
	{
	}

	virtual void impl_set(char /* fill */, int x, int y) override
	{
		if(min.x <= x && x <= max.x &&
			min.y <= y && y <= max.y) {
			included.insert(current_id);
		}
	}
};

std::set<int> id_in_region(int x1, int y1, int x2, int y2)
{
	OwnerFinderRegion ofr{std::min(x1, x2), std::min(y1, y2),
		std::max(x1, x2), std::max(y1, y2)};

	for(unsigned int i = 0; i < es.elements.size(); ++i) {
		ofr.current_id = i;
		ofr.draw(*es.elements[i]);
	}
	return std::move(ofr.included);
}
