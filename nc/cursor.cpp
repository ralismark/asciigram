#include "cursor.hpp"

point cur;

int idhere()
{
	OwnerFinder of(cur.x, cur.y);
	for(int i = 0; i < es.elements.size(); ++i) {
		// need to manually set id for each element
		// can't draw es directly
		of.current_id = i;
		of.draw(*es.elements[i]);
	}
	return of.target_id;
}
