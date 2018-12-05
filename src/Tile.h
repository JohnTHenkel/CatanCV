#include "Enums.h"
#pragma once

class Tile{
private:
	enum Recource recourceType;
	Point2f location;
	int number;
public:
	Tile(Point2f newLoc){
		location = newLoc;
	}
	Tile(Recource newRecourceType, Point2f newLoc){
		recourceType = newRecourceType;
		location = newLoc;
	}
	Tile(Recource newRecourceType, Point2f newLoc, int newNumber){
                recourceType = newRecourceType;
                location = newLoc;
		number = newNumber;
        }
	void set_number(int newNumber){number = newNumber;}
	void set_recource(Recource newRecource){recourceType = newRecource;}
	int get_number() const {return number;}
	Point2f get_loc() const {return location;}
	Recource get_recource() const {return recourceType;}
};
