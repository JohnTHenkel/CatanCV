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



void processNextTurn(vector<Player> gamePlayers,vector<Tile> hexagons); 

int main(int argc, char* argv[]){
	//Mat image = imread("TestPhotos/Dice6.png");
	//Mat image = imread("TestPhotos/2018-11-07-162752.jpg");
	//Mat base=imread("TestPhotos/2018-11-07-162701.jpg");
	//Mat image =imread("TestPhotos/2018-11-20-152025.jpg");
	//Mat backgroundImage = imread("TestPhotos/2018-11-07-162036.jpg");
	Mat backgroundImage = imread("TestPhotos/dice3.jpg");
	//Mat backgroundImage = imread("backgroundImage.png");
	Mat image = imread("TestPhotos/dice3.jpg");
	Mat imageGameState = imread("TestPhotos/gameState2.jpg");
	if(image.empty()){
		cerr << "Failed to read input image"<<endl;
		exit(EXIT_FAILURE);
	}
	if(imageGameState.empty()){
                cerr << "Failed to read input image"<<endl;
                exit(EXIT_FAILURE);
        }
    //VideoCapture cap(1);
    /*
    if(backgroundImage.empty()){

	    while(1){
	    	cap>>backgroundImage;
	    	imshowresize("Press escape to capture the background image.",backgroundImage,false,false);
	    	char key = cv::waitKey(1);
			if (key == 27) break; 

	    }
		imwrite("backgroundImage.png",backgroundImage);
	}
	*/

    vector<Tile> hexagons=findAllHexTiles(backgroundImage);
  	hexagons = assignTileNumbers(hexagons);
  	cout<<"hex"<<endl;
  	Player player1(red);
  	Player player2(blue);
  	vector<Player> gamePlayers={player1,player2};
   	while(!((player1.get_score()>9))&&(!(player2.get_score()>9))){
		cout<<"while"<<endl;
		processNextTurn(gamePlayers,hexagons); 
		

   	}

   	return EXIT_SUCCESS;
}

void processNextTurn(vector<Player> gamePlayers,vector<Tile> hexagons){
	VideoCapture cap(CAP_NUM);
	vector<int> dice;
	Mat frame;
	while(dice.size()<2){
		cap>>frame;
		dice= findArucoDiceTags(frame);
		cout<<dice.size();
	}
	int diceroll=dice.at(0)+dice.at(1);
	//robber
	vector<Piece> pieces = findPieces(frame);
	for (auto& p: pieces){
		for(auto& h: hexagons){
			double distance=norm(p.get_loc()-h.get_loc());
			if(distance>60&&distance<110&&diceroll==h.get_number()){

			}
		}
	}
	cap.release();
	//Color largestArmy = findCardOwner(imageGameState,"largestArmy.jpg");
}
