#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "Constants.h"
#include "Hexagon.h"
#include <opencv2/xfeatures2d/nonfree.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "Helpers.h"
#include "Player.h"
#include "Piece.h"
#include "FindPieces.h"
#include "Tile.h"
#include "FindCards.h"
#include "FindAruco.h"
using namespace cv;
using namespace std;



void processNextTurn(VideoCapture& cap,vector<Player> gamePlayers,vector<Tile> hexagons); 

int main(int argc, char* argv[]){
	 VideoCapture cap(CAP_NUM);
	
	Mat backgroundImage;
	backgroundImage = imread("backgroundImage.png");
	if(backgroundImage.empty()){
		while(1){
			cap.read(backgroundImage);
			imshowresize("Press esc to capture backgroundImage",backgroundImage,false,false);
			char key = cv::waitKey(1);
			if (key == 27) {
				imwrite("backgroundImage.png",backgroundImage);
				destroyWindow("Press esc to capture backgroundImage");
				break;
			}
		}
	}
	
    vector<Tile> hexagons=findAllHexTiles(backgroundImage);
  	hexagons = assignTileNumbers(cap,hexagons);
   	while(1){
   		cap.read(backgroundImage);
   		imshowresize("Board",backgroundImage,false,false);
   		char key = cv::waitKey(1);
		if (key == 27) break;
   	}
  
  	destroyAllWindows();
  	Player player1(red);
  	Player player2(blue);
  	vector<Player> gamePlayers={player1,player2};
   	while(!((player1.get_score()>9))&&(!(player2.get_score()>9))){
		processNextTurn(cap, gamePlayers,hexagons); 
		

   	}

   	return EXIT_SUCCESS;
}

void processNextTurn(VideoCapture& cap,vector<Player> gamePlayers,vector<Tile> hexagons){

	vector<int> dice;
	Mat frame;
	cap.read(frame);
	imshowresize("frame",frame);
	while(1){
		cap.read(frame);
		dice= findArucoDiceTags(frame);
		if(dice.size()==2){
			break;
		}
	}
	destroyAllWindows();

	cap.read(frame);
	int diceroll=dice.at(0)+dice.at(1);
	cout<<"Dice roll: "<<diceroll<<endl;
	Point2f robber = findRobber(frame);
	cap.read(frame);
	vector<Piece> pieces = findPieces(frame);
	for(auto &player:gamePlayers){
		player.set_score(0);
	}
	for (auto& p: pieces){
		PieceType switchCase = p.get_type();
		int multiplicity=0;
		switch(switchCase){
			case road:
				break;
			case settlement:
				multiplicity=1;
				break;
			case city:
				multiplicity=2;
				break;
			default:
				break;
		}
		for(auto &player:gamePlayers){
			if(player.get_color()==p.get_color()){
				player.add_score(multiplicity);
			}
		}
		for(auto& h: hexagons){
			double distance=norm(p.get_loc()-h.get_loc());
			//check for robber
			double robberDistance=norm(h.get_loc()-robber);
		
			if(distance>60&&distance<110&&diceroll==h.get_number()&&robberDistance>50){
				if(multiplicity>0&&h.get_recource()!=desert){
					Recource recourceCase=h.get_recource();
					string resource="";
					switch(recourceCase){
						case grain:
							resource="grain";
							break;
						case sheep:
							resource="sheep";
							break;
						case brick:
							resource="brick";
							break;
						case stone:
							resource+"stone";
							break;
						case desert:
							break;
						default:
							break;
					}
					if(resource.length()>0){
						cout<<"Player "<<p.print_color()<<" receives "<<multiplicity<<" "<<resource<<endl;
					}
				}
			}
		}

	}
	cap.read(frame);
	Color largestArmy = findCardOwner(frame,"largestArmy.jpg");
	cap.read(frame);
	Color longestRoad = findCardOwner(frame,"longestRoad.jpg");
	for(auto& player:gamePlayers){
		if(player.get_color()==largestArmy){
			player.add_score(2);
		}
		if(player.get_color()==longestRoad){
			player.add_score(2);
		}
		cout<<"Player: "<<player.print_color()<<" score: "<<player.get_score()<<endl;
	}
}
