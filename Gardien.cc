#include <stdio.h>
#include "Gardien.h"
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
	this->alive = true;
    _lastFB = std::chrono::system_clock::now();
    attaque = false;
    this->update_counter=0; //to count and not update movement at every step
	this->fps_counter=0;
	this->timer=0; //to count and not update movement at every step
	this->moveRadius=1; //maximum range that the guards can move in one update
	this->dangle=0; //maximum angle that the guardians can turn in one update
	this->speed=Environnement::scale/10;
	this->hitbox=Environnement::scale/2;
	this->_mode=0; //patrol mode by default
}

void Gardien::destruct_dead_gardien()
{
	if(this->alive == false)
	{
		this->rester_au_sol();
	}
}


void Gardien::kill_gardien(){
    this->alive = false;
    this->rester_au_sol();
	
}

void Gardien::decrease_LP(){
    this->_LP -= 50;
	if (this->_LP <= 0){
		this->kill_gardien();
        message ("I'm dead.");
    }else{
        message ("Aïe ! I only have %d LP now.", (int) this->_LP);
        this->tomber();
    }
}

bool Gardien::isAlive(){
    return this->alive;
}

void Gardien::update(){

	float angle_difference=see_chasseur();
	if ((-30<=angle_difference) && (angle_difference <=30)) 
	{
		this->_mode = 1; //this is the attack mode
	}else{
		this->_mode = 0;
	}

    timer+=1; 
	update_counter+=1;
	fps_counter+=1;
	if (update_counter==1 && this->isAlive() == true){
		float rad= 1;

		double dx=-sin((float)((float)_angle/180)*M_PI) * rad; //needs to be "-" because of our coordinate system 
		double dy=cos((float)((float)_angle/180)*M_PI) * rad;

		if (this->_mode == 1){
			bool obstacles=check_obstacles();
			if (obstacles==1){
				_angle += angle_difference;
				this->fire(0);
			}	
		}
		move(dx, dy);

		update_counter=0;	
	}
}

void Gardien::fire(int angle_vertical){
    float theta = std::atan2((_l->_guards[0]->_x - _x),(_l->_guards[0]->_y - _y));
	float angleF = (180 * theta) / M_PI;
    
    //if the gardien sees the hunter
    //cout << (std::chrono::system_clock::now() -_lastFB).count() << "\n";
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _lastFB;
    if ((elapsed_seconds).count() > 3.0)
    {
        if (((Chasseur*)(this->_l->_guards[0]))->isAlive()==true && isAlive() == true)
        {
            _guard_fire -> play ();
            _lastFB = std::chrono::system_clock::now();
            _fb -> init (_x, _y, 10., angle_vertical, angleF);
        }
    }
}

double Gardien::hit_probability(double coefficient) {
	int rest_LP = this->_LP;
	// Calculate hit probability based on health and coefficient
	//cout<<"rest_LP : " << rest_LP << "\n";
	//cout<<"calcul : " << coefficient * 100/rest_LP << "\n";
	return max(0.1, 1.0 - (coefficient * 100/rest_LP));
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

		//To give the hunter a chance, each guard has a probability of missing his target; 
		//this probability depends on the guard's health: the less health points he has, 
		//the worse his shot. The coefficient that relates the health of a guard to his ability to shoot 
		//accurately is also a parameter of the program.

		double coefficient = 0.4;
		double probability = hit_probability(coefficient);
        double random = ((double) rand() / (RAND_MAX));
		//cout<<"before Probability : " << probability << "\n";
		//we get the gardien that is at (x,y) which means that it is hit by the fireball
		//cout<<"Random : " << random << "\n";
		//cout<<"Probability : " << probability << "\n";
        if ((random < probability) &&  (((Chasseur*)(this->_l->_guards[0]))->isAlive() == true))
        {
			//cout<<"Random : " << random << "\n";
			//cout<<"Probability : " << probability << "\n";
            //message("Chasseur hit");
            ((Chasseur*)(this->_l->_guards[0]))->decrease_LP_chasseur();
            
        }

        /*
        //we get the gardien that is at (x,y) which means that it is hit by the fireball
        if ((abs(x_place) <= 5) && (abs(y_place) <= 5) &&  (((Chasseur*)(this->_l->_guards[0]))->isAlive() == true))
        {
            //message("Chasseur hit");
            ((Chasseur*)(this->_l->_guards[0]))->decrease_LP_chasseur();
            
        }
		*/
		
	}
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	//message ("Booom...");
	return false;
}


bool Gardien::move (double dx, double dy){
	//TODO: if we set the speed parameter to scale/1 this works and the guards dont run into each other
	//in any other case (if we reduce the speed) we will run through other guards. Find out why and fix
	//if we have time

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
	if((empty!=0)){
		
		if ((changed_x or changed_y)){
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



float Gardien::see_chasseur(){
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
	 
    double angle_diff = angleToNormal2 - _angle;
    while (angle_diff < -180) {
      angle_diff += 360;
    }
    while (angle_diff > 180) {
      angle_diff -= 360;
    }
	return angle_diff;
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
