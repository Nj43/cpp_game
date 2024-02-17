#include "Gardien.h"
#include <stdio.h>
#include "Mover.h"
#include "Chasseur.h"
#include "Labyrinthe.h"
#include "Sound.h"
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstring> 
#include <string>
#include <fstream>
#include <vector>
#include <assert.h>

#include <iostream>
using namespace std;

Gardien::Gardien(Labyrinthe* l, const char* modele, int _LP) : Mover (120, 80, l, modele)
{
	_guard_fire = new Sound ("sons/hunter_fire.wav");
	_guard_hit = new Sound ("sons/hunter_hit.wav");
	_wall_hit = new Sound ("sons/hit_wall.wav");

    this->_LP = _LP;
    _lastFB = std::chrono::system_clock::now();
    attaque = false;
    this->update_counter=0; //to count and not update movement at every step
	this->fps_counter=0;
	this->timer=0; //to count and not update movement at every step
	this->moveRadius=1; //maximum range that the guards can move in one update
	this->dangle=0; //maximum angle that the guardians can turn in one update
	this->speed=Environnement::scale/10;
}


void Gardien::kill_gardien(){
    this->alive = false;
    this->rester_au_sol();
}

void Gardien::decrease_LP(){
    this->_LP -= 30;
	if (this->_LP <= 0){
        message ("I'm dead.");
        this->kill_gardien();
    }else{
        message ("Aïe ! I only have %d LP now.", (int) this->_LP);
        this->tomber();
    }
}

bool Gardien::isAlive(){
    return this->alive;
}

void Gardien::update(){
    if(see_chasseur() and isAlive() == true){
		cout << "see" << "\n" ;
        this->fire(0);
    }
    timer+=1; 
	update_counter+=1;
	fps_counter+=1;
	
	// Generate random movement
	if (update_counter==1 && isAlive() == true){
		float rad= 1; //*Environnement::scale;

		double dx=-sin((float)((float)_angle/180)*M_PI) * rad; //needs to be "-" because of our coordinate system 
		double dy=cos((float)((float)_angle/180)*M_PI) * rad;
		bool see=see_chasseur();
		if (see == 1){
			//std::cout<<"Peakaboo! "<<std::endl;
			bool obstacles=check_obstacles();
			//std::cout<<"Obstacles? "<< obstacles <<std::endl;
			if (obstacles==1){
				//std::cout<<"No Obstacles! "<<std::endl;
				std::cout<<"Peakaboo! "<<std::endl;
				this->fire(0);
				
			}	
		}

		else if (see == 0){
			//std::cout<<"Not visible!"<<std::endl;
		}
		cout << "Angle : " << _angle << "\n";
		assert ((_angle<=360) and (_angle>=0));
		move(dx, dy);
		update_counter=0;	
	}
}

void Gardien::fire(int angle_vertical){
    float theta = std::atan2((_l->_guards[0]->_x - _x),(_l->_guards[0]->_y - _y));
	float angleF = (180 * theta) / M_PI;
    
    //if the gardien sees the hunter
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _lastFB;
    if ((elapsed_seconds).count() > 3.0)
    {
        if (((Chasseur*)(this->_l->_guards[0]))->isAlive())
        {
            _guard_fire -> play ();
            _lastFB = std::chrono::system_clock::now();
            _fb -> init (_x, _y, 10., angle_vertical, angleF);
        }
    }
}


bool Gardien::process_fireball (float dx, float dy)
{
	// calculer la distance entre le gardien et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}else{
		// collision...
		
        float x_place = (this->_l->_guards[0]->_x - _fb -> get_x ())/ Environnement::scale;
        float y_place = (this->_l->_guards[0]->_y - _fb -> get_y ())/ Environnement::scale;
        
        //we get the gardien that is at (x,y) which means that it is hit by the fireball
        if ((abs(x_place) <= 1.5) && (abs(y_place) <= 1.5) &&  (((Chasseur*)(this->_l->_guards[0]))->isAlive() == true))
        {
            message("Chasseur hit");
        }
		
	}
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	return false;
}




bool Gardien::see_chasseur()
{
	Mover* hunter = this->_l->_guards[0];
	int hunterX = hunter->_x / Environnement::scale;
	int hunterY = hunter->_y / Environnement::scale;
	float monsterX = this->_x / Environnement::scale;
	float monsterY = this->_y / Environnement::scale;
	//print(angle)
	// Calculate the angle between the monster and the hunter
	double dx = hunterX - monsterX;
	double dy = hunterY - monsterY;
	double angleToHunter = atan2(dy, dx) * 180 / M_PI;
	//double angle_diff = std::abs(angleToHunter-_angle-90.0);
	
	// Normalize angleToHunter to be between 0 and 360 degrees
	if (angleToHunter < 0)
		angleToHunter += 360;
	
	// Calculate the absolute difference between the angle
	// Ensure the smallest angle difference is considered
	double angle_diff = std::abs(angleToHunter-_angle-90.0);
	
	if (angle_diff > 180)
		angle_diff = 360 - angle_diff;

	// Check if the hunter is within the vision angle of the monster
	return  ((-3<=angle_diff) && (angle_diff <=3)); //we introduce some slack values
}



bool Gardien::move (double dx, double dy){
	
	int new_x=(_x+dx*speed)/Environnement::scale;
	int new_y=(_y+dy*speed)/Environnement::scale;
	int checkbox_x_a = (_x+dx*speed+((dx>=0)-(dx<0))*hitbox)/Environnement::scale;
	int checkbox_y_a = (_y+dy*speed+((dy>=0)-(dy<0))*hitbox)/Environnement::scale;

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
	if((empty_checkbox!=0)){
		
		if ((changed_checkbox_x or changed_checkbox_y)){

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
			return false;
			}	
		}
	}
		
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
	return false;
}





bool Gardien::check_obstacles(){
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