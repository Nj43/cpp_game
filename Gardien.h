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
	int speed= 0;//Environnement::scale/6;
	float hitbox=Environnement::scale/2; 
	
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{}

	// mon gardien pense tr�s mal!
	void update (void) {
		timer+=1; 
		update_counter+=1;
		fps_counter+=1;
		//std::cout<<"FPS: "<<fps_counter<<std::endl;
		//if (fps_counter%8 == 0){
		//	std::cout<<"No update"<<std::endl;
		//	update_counter=0;
		//	return;
		//}
		// Generate random movement
		if (update_counter==1){
			

			/*This part was used for random angle calculation for the movement of the guards, sine we move in straight lined, we dont need it
			std::mt19937 gen(rd()); // seed the generator
			std::uniform_int_distribution<> distr(0, 0);
			if ((timer%50) == 0){
				int delta=distr(gen);
				
				//std::cout<<"Angle: "<<_angle<<std::endl;
				if ((_angle+delta)==360){
					_angle=0;
				}
				else{ 
					_angle+=delta;
				}
			}
			*/
			//_angle+=3.1415; -> the _angle variable is given in degrees
			float rad= 1; //*Environnement::scale;

			double dx=-sin((float)((float)_angle/180)*M_PI) * rad; //needs to be "-" because of our coordinate system 
			double dy=cos((float)((float)_angle/180)*M_PI) * rad;
			//if (EMPTY == _l -> data ((int)((_x+dx*speed)/Environnement::scale),(int)((_y+dy*speed)/Environnement::scale))){
				//std::cout<<"cannot move"<<std::endl;
			bool seeChasseur=see_chasseur();
			
			if (seeChasseur == 1){
				std::cout<<"Peekaboo"<<std::endl;
			}
			//std::cout<<_angle<<std::endl;
			assert ((_angle<=360) and (_angle>=0));
			move(dx, dy);
		
			//}

			/*
			else{
				//int wall_delta=45;
				std::mt19937 gen(rd()); // seed the generator
				std::uniform_int_distribution<> wall_distr(-90, 90);
				int wall_delta=wall_distr(gen);


				if((_angle-wall_delta)<=0){
					_angle+=abs(wall_delta);
				}
				else if((_angle+wall_delta)>=360){
					_angle=0+(360-_angle);
				}
				else{
					_angle+=wall_delta; //_angle+90; //run into wall then change direction
				}
			
			}
			*/
			update_counter=0;	
		}
			
	
	};


	// et ne bouge pas!

	bool move (double dx, double dy){
		//TODO: if we set the speed parameter to scale/1 this works and the guards dont run into each other
		//in any other case (if we reduce the speed) we will run through other guards. Find out why and fix
		//if we have time


		
		/*
		std::cout<<"Hitbox: "<<hitbox<<std::endl;
		std::cout<<"Speed: "<<speed<<std::endl;
		std::cout<<"Delta x: "<<dx<<"  Delta y: "<<dy<<std::endl;
		std::cout<<"Original _x: "<<_x<<"  Original _y: "<<_y<<std::endl;
		std::cout<<"dx*speed="<<dx*speed<<"  dy*speed="<<dy*speed<<std::endl;
		std::cout<<"((dy>0)-(dy<0))"<<((dy>=0)-(dy<0))<<std::endl;
		*/
		int new_x=(_x+dx*speed)/Environnement::scale;

		//however, this is going to be the field that I need to check
		int checkbox_x_a = (_x+dx*speed+((dx>=0)-(dx<0))*hitbox)/Environnement::scale;
		//int checkbox_x_b = (_x+dx*speed+dx/std::abs(dx)*hitbox)/Environnement::scale;
		
		//std::cout<<"old x: "<<_x/Environnement::scale<<std::endl;
		//std::cout<<"new x: "<<new_x<<std::endl;
        int new_y=(_y+dy*speed)/Environnement::scale;
		//int checkbox_y = (_y+dy*Environnement::scale)/Environnement::scale;
		int checkbox_y_a = (_y+dy*speed+((dy>=0)-(dy<0))*hitbox)/Environnement::scale;
		//std::cout<<"True new x and y: "<<
		//std::cout<<"Rounded true movement: ("<<new_x<<","<<new_y<<")"<<std::endl;
		//std::cout<<"Checkbox: ("<<checkbox_x<<","<<checkbox_y<<")"<<std::endl;

		
		//std::cout<<"new y: "<<new_y<<std::endl;
		bool empty=_l -> data ((int)(new_x),(int)(new_y));
		bool empty_checkbox=_l -> data ((int)(checkbox_x_a),(int)(checkbox_y_a));

		//std::cout<<"Element in front? "<<empty<<"  Element in Checkbox? "<<empty_checkbox<<std::endl;
		bool changed_x= new_x != (int)(_x/Environnement::scale); 
		bool changed_y= new_y != (int)(_y/Environnement::scale);

		bool changed_checkbox_x = checkbox_x_a !=(int)(_x/Environnement::scale); 
		bool changed_checkbox_y = checkbox_y_a !=(int)(_y/Environnement::scale); 
		//std::cout<<"Changed x or y? "<<((empty!=0) and (changed_x or changed_y))<<std::endl;
		//std::cout<<"Changed x"
		if ((new_x<0) || (new_y<0) || (new_y >= _l->height()) || (new_x >= _l->width())){
			//std::cout<<"new x: "<<new_x<<"new y: "<<new_y<<std::endl;

			return false;
		}
		//if something is in the way, check if it is us (no change in x or y, we are moving within a field)
		//If it is us in the way -> just move, if not  
		if((empty_checkbox==1)){
			
			if ((changed_checkbox_x or changed_checkbox_y)){
				//std::cout<<"Checkox Alert!"<<std::endl;
				//std::cout<<checkbox_x_a<<std::endl;
				if ((std::abs(dx)>0.01) or (std::abs(dy)>0.01)){
					//move(dx/2, dy/2);
					//std::cout<<"Somethings in the way at: ("<<new_x<<","<<new_y<<")"<<std::endl;

				
				//Change angle and dont move forward
				std::mt19937 gen(rd()); // seed the generator
				std::uniform_int_distribution<> wall_distr(-90, 90);
				int wall_delta=wall_distr(gen);


				if((_angle+wall_delta)<=0){
					//std::cout<<"Here: original angle: "<<_angle<<"  new angle: "<<wall_delta<<std::endl;
					_angle+=abs(wall_delta);
				}
				else if((_angle+wall_delta)>=360){
					_angle=0+(360-_angle);
				}
				else{
					_angle+=wall_delta; //_angle+90; //run into wall then change direction
				}

	
				

				//_angle=-180;
				return false;
				}
				
			}
		}
		/*
		if((empty==1)){
			//std::cout<<"In here"<<std::endl;
			//if(((empty != 0) and ((new_x != (int)(_x/Environnement::scale)) or (new_y != (int)(_y/Environnement::scale))))){
			//std::cout<<"Empty? "<<<<std::endl;
			//std::cout<<"_X and _Y: ("<<_x<<","<<_y<<") With indices: ("<<(int)(_x/Environnement::scale)<<","<<(int)(_y/Environnement::scale)<<")"<<std::endl;
			
			if ((changed_x or changed_y) or (changed_checkbox_x or changed_checkbox_y)){
				
				
				std::cout<<"Somethings in the way at: ("<<new_x<<","<<new_y<<")"<<std::endl;

				
				//Change angle and dont move forward
				std::mt19937 gen(rd()); // seed the generator
				std::uniform_int_distribution<> wall_distr(-90, 90);
				int wall_delta=wall_distr(gen);


				if((_angle-wall_delta)<=0){
					_angle+=abs(wall_delta);
				}
				else if((_angle+wall_delta)>=360){
					_angle=0+(360-_angle);
				}
				else{
					_angle+=wall_delta; //_angle+90; //run into wall then change direction
				}
				

				//_angle=-180;
				return false;
			}
			

			//else{
				//std::cout<<"move without something in the way"<<std::endl;
				//Update the matrix
				//std::cout<<"We have left a certain point in the matrix!"<<std::endl;
				//std::cout<<"Set 1 to: ("<<(int) new_x<<","<<(int) new_y<<")"<<std::endl;

			((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, 1); //update the new position
			((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
			
			//move the guard
			_x+=dx*speed; ///Environnement::scale;
			_y+=dy*speed; ///Environnement::scale;
			return true;
			//}
			//printArray((Labyrinthe *)_l->_data, _l->width(), _l->height());
		}*/
		

		//if nothing is in the way, move
		//else{
		if ((changed_x or changed_y)){
			//std::cout<<"We have left a certain point in the matrix!"<<std::endl;
			//std::cout<<"Set 1 to: ("<<(int) new_x<<","<<(int) new_y<<")"<<std::endl;
			((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, 1); //update the new position
			((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
		}
		_x+=dx*speed; ///Environnement::scale;
		_y+=dy*speed; ///Environnement::scale;
		return true;
		//}
	return false;}

	bool see_chasseur(){
    Mover* hunter = this->_l->_guards[0];
    int hunterX = hunter->_x / Environnement::scale;
    int hunterY = hunter->_y / Environnement::scale;
    float monsterX = this->_x / Environnement::scale;
    float monsterY = this->_y / Environnement::scale;

	// Calculate the angle between the monster and the hunter
    double dx = hunterX - monsterX;
    double dy = hunterY - monsterY;
    double angleToHunter = atan2(dy, dx) * 180 / M_PI;
	std::cout<<angleToHunter<<std::endl;
    // Normalize angleToHunter to be between 0 and 360 degrees
    if (angleToHunter < 0)
        angleToHunter += 360;

    // Calculate the absolute difference between the angles
    double angleDifference = std::abs(angleToHunter - this->_angle);
	//std::cout<<"Angle diff: "<<angleDifference<<std::endl;
    // Ensure the smallest angle difference is considered
    if (angleDifference > 180)
        angleDifference = 360 - angleDifference;

    // Check if the hunter is within the vision angle of the monster
    return angleDifference <= this->_angle;
	}
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
	bool see_guard(){return false;}
	bool move_follow(){
		//if we see the chasser (separate function), we want to be able to follow
		//the chasseur. For that to work, we need to access the 
		return false;
	}
};
 

#endif
