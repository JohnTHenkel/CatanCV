#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <cmath>
#include "Constants.h"
#include "Dice.h"
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
	//Mat image = imread("TestPhotos/Dice6.png");
	//Mat image = imread("TestPhotos/2018-11-07-162752.jpg");
	//Mat base=imread("TestPhotos/2018-11-07-162701.jpg");
	//Mat image =imread("TestPhotos/2018-11-20-152025.jpg");
	//Mat backgroundImage = imread("TestPhotos/2018-11-07-162036.jpg");
	 VideoCapture cap(CAP_NUM);
	Mat backgroundImage = imread("TestPhotos/dice3.jpg");
	//Mat backgroundImage = imread("backgroundImage.png");
	Mat image = imread("TestPhotos/dice3.jpg");
	Mat imageGameState = imread("TestPhotos/gameState3.jpg");
	if(image.empty()){
		cerr << "Failed to read input image"<<endl;
		exit(EXIT_FAILURE);
	}
	
	
    vector<Tile> hexagons=findAllHexTiles(backgroundImage);
   
  	hexagons = assignTileNumbers(cap,hexagons);
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
	for(int i=0;i<120;i++){
		cap>>frame;
	}
	while(dice.size()<2){
		cap>>frame;
		dice= findArucoDiceTags(frame);
		cout<<dice.size();
	}
	cap>>frame;
	int diceroll=dice.at(0)+dice.at(1);
	cout<<"Dice roll: "<<diceroll<<endl;
	//robber
	//Point2f robber = findRobber(frame);
	Point2f robber =Point2f(0,0);
	cap>>frame;
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
						cout<<"Player "<<p.get_color()<<" receives "<<multiplicity<<" "<<resource<<endl;
					}
				}
			}
		}

	}
	cap>>frame;
	Color largestArmy = findCardOwner(frame,"largestArmy.jpg");
	for(auto& player:gamePlayers){
		cout<<"Player: "<<player.get_color()<<" score: "<<player.get_score()<<endl;
	}
}
