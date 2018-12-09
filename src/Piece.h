//enum PieceType defined in Enums.h
//enum Color defined in Enums.h
#include "Enums.h"
#pragma once

class Piece{
private:
        enum Color color;
        enum PieceType type;
        Point2f location;
public:
	Piece(Color newColor, PieceType newType, Point2f newLoc){//constructor
		color = newColor;
		type = newType;
		location = newLoc;
	}

	Piece(const Piece& c){//copy constructor
		color = c.get_color();
		type = c.get_type();
		location = c.get_loc();
	}

	Color get_color() const {return color;}
	PieceType get_type() const {return type;}
	Point2f get_loc() const {return location;}
		string print_color(){
		Color switchCase = color;
		switch(switchCase){
			case red:
				return "red";
				break;
			case blue:
				return "blue";
				break;
			case orange:
				return "orange";
				break;
			case white:
				return "white";
				break;
			default:
				break;
		}
		return "";
	}
};

