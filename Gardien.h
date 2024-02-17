#ifndef GARDIEN_H
#define GARDIEN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> 
#include <string>
#include <cmath> 
#include <random>
#include <assert.h>
#include "Mover.h"

class Labyrinthe;
std::random_device rd; // obtain a random number from hardware
class Gardien : public Mover {
private: 
	int update_counter=0; //to count and not update movement at every step
	int fps_counter=0;
	int timer=0; //to count and not update movement at every step
	float moveRadius=1; //maximum range that the guards can move in one update
	int dangle=0; //maximum angle that the guardians can turn in one update
	int speed= Environnement::scale/10;
	float hitbox=Environnement::scale/2; 
	
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{}

	// mon gardien pense tr�s mal!
	void update (void) {
		timer+=1; 
		update_counter+=1;
		fps_counter+=1;

		if (update_counter==1){
			float rad= 1; //*Environnement::scale;

			double dx=-sin((float)((float)_angle/180)*M_PI) * rad; //needs to be "-" because of our coordinate system 
			double dy=cos((float)((float)_angle/180)*M_PI) * rad;

			float angle_difference=see_chasseur();

			if ((-30<=angle_difference) && (angle_difference <=30)){

				bool obstacles=check_obstacles();

				if (obstacles==1){
					_angle += angle_difference;
					
				}	
			}
			move(dx, dy);
			update_counter=0;	
		}
			
	
	};


	// et ne bouge pas!

	bool move (double dx, double dy){
		//TODO: if we set the speed parameter to scale/1 this works and the guards dont run into each other
		//in any other case (if we reduce the speed) we will run through other guards. Find out why and fix
		//if we have time
		int new_x=(_x+dx*speed)/Environnement::scale;
		//however, this is going to be the field that I need to check
		int checkbox_x_a = (_x+dx*speed+((dx>=0)-(dx<0))*hitbox)/Environnement::scale;
		//int checkbox_x_b = (_x+dx*speed+dx/std::abs(dx)*hitbox)/Environnement::scale;

        int new_y=(_y+dy*speed)/Environnement::scale;
		int checkbox_y_a = (_y+dy*speed+((dy>=0)-(dy<0))*hitbox)/Environnement::scale;

		bool empty=_l -> data ((int)(new_x),(int)(new_y));
		bool empty_checkbox=_l -> data ((int)(checkbox_x_a),(int)(checkbox_y_a));

		bool changed_x= new_x != (int)(_x/Environnement::scale); 
		bool changed_y= new_y != (int)(_y/Environnement::scale);

		bool changed_checkbox_x = checkbox_x_a !=(int)(_x/Environnement::scale); 
		bool changed_checkbox_y = checkbox_y_a !=(int)(_y/Environnement::scale); 

		if ((new_x<0) || (new_y<0) || (new_y >= _l->height()) || (new_x >= _l->width())){
			return false;
		}
		//if something is in the way, check if it is us (no change in x or y, we are moving within a field)
		//If it is us in the way -> just move, if not  
		if((empty_checkbox!=0)){
			if ((changed_checkbox_x or changed_checkbox_y)){
				if ((std::abs(dx)>0.01) or (std::abs(dy)>0.01)){
				//Change angle and dont move forward
				std::mt19937 gen(rd()); // seed the generator
				std::uniform_int_distribution<> wall_distr(-90, 90);
				int wall_delta=wall_distr(gen);
				if((_angle+wall_delta)<=0){
					_angle+=abs(wall_delta);
				}
				else if((_angle+wall_delta)>=360){
					_angle=0+(360-_angle);
				}
				else{
					_angle+=wall_delta;  //run into wall then change direction
				}
				return false;
				}
				
			}
		}
		//if nothing is in the way, move
		if ((changed_x or changed_y)){
			((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, 1); //update the new position
			((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
		}
		_x+=dx*speed; 
		_y+=dy*speed;
		return true;}

	float see_chasseur(){
		Mover* hunter = this->_l->_guards[0];
		int hunterX = hunter->_x / Environnement::scale;
		int hunterY = hunter->_y / Environnement::scale;
		float monsterX = this->_x / Environnement::scale;
		float monsterY = this->_y / Environnement::scale;
		// Calculate the angle between the monster and the hunter
		double dx = hunterX - monsterX;
		double dy = hunterY - monsterY;
		double angleToNormal = atan2(dy, dx) * 180 / M_PI;
		//std::cout<<"Angle to Hunter: "<<angleToHunter<<std::endl; 
		//double angle_diff = std::abs(angleToHunter-_angle-90.0);
		//angleToHunter -= 90.0;
		// std::cout<<"Angle to Hunter: "<<angleToHunter<<std::endl; 
		//std::cout<<"Angle: "<<_angle<<std::endl;
		double angleToNormal2 =  angleToNormal - 90 ;
		// Normalize angleToHunter to be between 0 and 360 degrees
		//if (angleToHunter < 0)
		//	angleToHunter += 360;
    if (angleToNormal2 <= -270) {
      angleToNormal2 +=360;
    }
		//std::cout<<"Diff: "<<angleToNormal2<<std::endl; 
    double angle_diff = angleToNormal2 - _angle;
    while (angle_diff < -180) {
      angle_diff += 360;
    }
    while (angle_diff > 180) {
      angle_diff -= 360;
    }
		//std::cout<<"Angle Diff: "<<angle_diff<<std::endl; 
		return angle_diff;
	}

	bool check_obstacles(){
		//we need to store the points that we cross in a vector
		std::vector<std::pair<int, int>> points;

		//To follow the chasseur, we first need to find his location and 
		//from our update function, we will be provided with the angle we are currently in
		Mover* hunter=this->_l->_guards[0];
		int hunterX = hunter->_x / Environnement::scale;
    	int hunterY = hunter->_y / Environnement::scale;

		int guardX=this->_x/ Environnement::scale;
		int guardY=this->_y/ Environnement::scale;

		//Now we want to find the difference between the hinter an the guard
		int dx=abs(guardX-hunterX);
		int dy=abs(guardY-hunterY);

		//Next, with our given relative positions of hunter and guard, we 
		//to figure out, in what direction we need to move. We therefore introduce
		//two sign variables for the x and y axis, determining, whether we have a 
		//positive or a negative direction between the two points

		int sx=(guardX<hunterX) ? 1: -1;
		int sy=(guardY<hunterY) ? 1: -1;

		//We also introduce a variable which will determine, whether we do a 
		//horizontal or a vertical step. If the error term is positive, that means
		// that the dx variable is bigger than the dy variable, which means
		// that we need to increment in vertical direction. Otherwise, we increment in 
		//horizontal direction

		int err=dx-dy;
		int counter=0;
		while (guardX!=hunterX || guardY!=hunterY){
			points.push_back({guardX, guardY});
			//If there is an obstacle in between guard and hunter, return false
			if (((_l -> data (guardX,guardY)) == 1) && (counter >=1)){
				return false;
			}
			int e2=2*err;
			if(e2>-dy){
				err-=dy;
				guardX+=sx;
			}
			if(e2<dx){
				err+=dx;
				guardY+=sy;
			}
			counter+=1;
		}
		points.push_back({hunterX, hunterY});
		//Now that we have collected all points, we need to check if the 
		return true;
	}
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
};
 

#endif
