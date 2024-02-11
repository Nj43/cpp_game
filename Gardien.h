#ifndef GARDIEN_H
#define GARDIEN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> 
#include <string>
#include <cmath> 
#include <random>
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
	int speed=Environnement::scale/5;
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
			double dx=-sin((float)((float)_angle/180)*PI) * rad; //needs to be "-" because of our coordinate system 
			double dy=cos((float)((float)_angle/180)*PI) * rad;
			//if (EMPTY == _l -> data ((int)((_x+dx*speed)/Environnement::scale),(int)((_y+dy*speed)/Environnement::scale))){
				//std::cout<<"cannot move"<<std::endl;
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
	

		int new_x=(_x+dx*speed)/Environnement::scale;

		//however, this is going to be the field that I need to check
		int checkbox_x = (_x+dx*Environnement::scale)/Environnement::scale;
		
		//std::cout<<"old x: "<<_x/Environnement::scale<<std::endl;
		//std::cout<<"new x: "<<new_x<<std::endl;
        int new_y=(_y+dy*speed)/Environnement::scale;
		int checkbox_y = (_y+dy*Environnement::scale)/Environnement::scale;
		//std::cout<<"True new x and y: "<<
		std::cout<<"Rounded true movement: ("<<new_x<<","<<new_y<<")"<<std::endl;
		std::cout<<"Checkbox: ("<<checkbox_x<<","<<checkbox_y<<")"<<std::endl;

		
		//std::cout<<"new y: "<<new_y<<std::endl;
		bool empty=_l -> data ((int)(new_x),(int)(new_y));
		bool changed_x= new_x != (int)(_x/Environnement::scale);
		bool changed_y= new_y != (int)(_y/Environnement::scale);

		//std::cout<<"Changed x or y? "<<((empty!=0) and (changed_x or changed_y))<<std::endl;
		
		if ((new_x<0) || (new_y<0) || (new_y >= _l->height()) || (new_x >= _l->width())){
			//std::cout<<"new x: "<<new_x<<"new y: "<<new_y<<std::endl;

			return false;
		}
		//if something is in the way, check if it is us (no change in x or y, we are moving within a field)
		//If it is us in the way -> just move, if not  
		if((empty!=0)){
			//if(((empty != 0) and ((new_x != (int)(_x/Environnement::scale)) or (new_y != (int)(_y/Environnement::scale))))){
			//std::cout<<"Empty? "<<<<std::endl;

			if ((changed_x or changed_y)){
				
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

			else{
				//std::cout<<"move without something in the way"<<std::endl;
				//Update the matrix
				//std::cout<<"We have left a certain point in the matrix!"<<std::endl;
				//std::cout<<"Set 1 to: ("<<(int) new_x<<","<<(int) new_y<<")"<<std::endl;

				((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, '1'); //update the new position
				((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), '\0');
				//move the guard
				_x+=dx*speed; ///Environnement::scale;
				_y+=dy*speed; ///Environnement::scale;
				return true;
			}
			//printArray((Labyrinthe *)_l->_data, _l->width(), _l->height());
		}
		//if nothing is in the way, move
		else{
			if ((changed_x or changed_y)){
				//std::cout<<"We have left a certain point in the matrix!"<<std::endl;
				//std::cout<<"Set 1 to: ("<<(int) new_x<<","<<(int) new_y<<")"<<std::endl;
				((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, 1); //update the new position
				((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
			}
			_x+=dx*speed; ///Environnement::scale;
			_y+=dy*speed; ///Environnement::scale;
			return true;
		}
	return false;}
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
	bool see_guard(){return false;}
};

#endif
