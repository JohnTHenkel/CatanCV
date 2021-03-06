// enum Color is defined in Enums.h
//enum Color{red,white,blue,orange};
#include "Enums.h"
class Player{
private:
	int score;
	enum Color color;
public:
	Player(Color newColor){score = 0; color = newColor;}
	int get_score() const {return score;}
	Color get_color() const {return color;}
	void set_score(int newScore){score = newScore;}
	void add_score(int scoreToAdd){score = score + scoreToAdd;}
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
