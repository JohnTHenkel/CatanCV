#include "Enums.h"
#pragma once

class Tile{
private:
	enum Recource resourceType;
	Point2f location;
	int number;
public:
	Tile(Point2f newLoc){
		location = newLoc;
	}
	Tile(Recource newRecourceType, Point2f newLoc){
		resourceType = newRecourceType;
		location = newLoc;
	}
	Tile(Recource newRecourceType, Point2f newLoc, int newNumber){
                resourceType = newRecourceType;
                location = newLoc;
		number = newNumber;
    }
	void set_number(int newNumber){number = newNumber;}
	void set_recource(Recource newRecource){resourceType = newRecource;}
	int get_number() const {return number;}
	Point2f get_loc() const {return location;}
	Recource get_recource() const {return resourceType;}
	
};
