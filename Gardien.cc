#include <stdio.h>
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstring> 
#include <string>
#include <fstream>
#include <vector>
#include <assert.h>

#include "Gardien.h"
#include "Mover.h"
#include "Chasseur.h"
#include "Labyrinthe.h"
#include "Sound.h"

#include <iostream>
using namespace std;

#define RECOVERY_TIME 12.0 //parameter that determines the time interval between the heal 
#define COEFFICIENT 0.02 //coefficient that relates the health of a guard to his ability to shoot accurately

/**
 * Call the constructor to initialize variables
 */
Gardien::Gardien(Labyrinthe* l, const char* modele, int _LP) : Mover (120, 80, l, modele)
{
	_guard_fire = new Sound ("sons/hunter_fire.wav");
	_guard_hit = new Sound ("sons/hunter_hit.wav");
	_wall_hit = new Sound ("sons/hit_wall.wav");

    this->_LP = _LP;
	this->alive = true;
    this->_lastFB = std::chrono::system_clock::now();
	this->_lastHeal = std::chrono::system_clock::now();
    this->update_counter=0; //to count and not update movement at every step
	this->speed=Environnement::scale/10; //Default setting for the speed value, can be adjusted to machine
	//this->hitbox=Environnement::scale/2; //hitbox value 
	this->_mode=0; //patrol mode by default
}



/**
 * Function that kills the guard. 
 * It makes the guard stay on the ground but empty the data case so that the hunter can move through.
 */
void Gardien::kill_gardien(){
    this->alive = false;
    this->rester_au_sol();
	//empty the data case otherwise, the chaseeur can get stuck 
	((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
}


/**
 * Function that decreases the life point of the guard. 
 * If its life point is less than 0 it means the guard is dead.
 */
void Gardien::decrease_LP(){
    this->_LP -= 50; //it decreases by 50 points
	if (this->_LP <= 0){ //if the guard is dead then kill it
		this->kill_gardien();
        message ("I'm dead.");
    }else{
        message ("Aïe ! Guard only has %d LP now.", (int) this->_LP);
        this->tomber();  //otherwise the guard fall down 
    }
}

/**
 * Function that increases the life point of the guard
 * when the guard goes a certain amount of time without suffering a wound
 */
void Gardien::increase_LP(){
	//seconds between last heal time and now
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _lastHeal;
	if ((elapsed_seconds).count() > RECOVERY_TIME) //if more than 8 seconds have elapsed
	{
		int aug = this->_LP + 30; 
		if(aug <= 200){ //check if it doesn't exceed the maximum life point of the guard
			this->_LP = aug;
			_lastHeal = std::chrono::system_clock::now(); //the most recent heal time is saved
			message ("Guard got healded.");
		}
	}
}


/**
 * Fuction that checks if the guard is alive
 */
bool Gardien::isAlive()
{
    return this->alive;
}

/**
 * Fuction that continually updates the state of the guard objects over time.
 * 
 */
void Gardien::update()
{	
	float angle_difference=see_chasseur(); 

	if (this->isAlive() == true){ //the guard can only move if he is alive
		this->increase_LP(); //increase the LP 
		float rad= 1;

		//calculate the dx and dy values for a given angle
		double dx=-sin((float)((float)_angle/180)*M_PI) * rad; //needs to be "-" because of our coordinate system 
		double dy=cos((float)((float)_angle/180)*M_PI) * rad;

		//Check if the hunter is in the cone of vision of the guard (-30, +30) form _angle 
		if ((-30<=angle_difference) && (angle_difference <=30)){
			bool obstacles=check_obstacles();
			//If the hunter is in the cone of vision, we need to check if there are any obstacles between the guard and the hunter
			if (obstacles==1){
				//If the vision is clear, follow the hunter by adapting the angle value, switch into attack mode and fire
				_angle += angle_difference;
				this->fire(0);
				this->_mode = 1;
			}
			else{
				//If there is something blocking the view, stay in patrol state
				this->_mode = 0;
			}	
		}
		
		move(dx, dy);
	}
}


/**
 * Fuction that lauchs the fireball
 * @param angle_vertical we can ignore this parameter for this project, always set to 0
 */
void Gardien::fire(int angle_vertical){
	
    float theta = std::atan2((_l->_guards[0]->_x - _x),(_l->_guards[0]->_y - _y)); //the angle in radian
	float angleF = (180 * theta) / M_PI; // the angle in degree
    
	//the guard can lauch the fireball every 3 seconds
	//calculate the time elapsed since the last fireball
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _lastFB;
	
    if ((elapsed_seconds).count() > 3.0) //it has been more than 3 secondes
    {
		//if the hunter is alive and the guard that lauches the fireball is alive
        if (((Chasseur*)(this->_l->_guards[0]))->isAlive()==true && this->isAlive() == true)
        {
            _guard_fire -> play (); //play the sound
            _lastFB = std::chrono::system_clock::now(); //redefine the last time a fireball was launched 

			//lauch the fireball by giving the coordinates of the position of the guard and the angle of the fireball.
            _fb -> init (_x, _y, 10., angle_vertical, angleF); 
        }
    }
}


/**
 * Fuction that calculates the hit probability of the guard given the life point
 * the less life points it has, the worse his shot
 */
double Gardien::hit_probability() {
	int rest_LP = this->_LP;
	// Calculate hit probability based on health and coefficient
	return COEFFICIENT * rest_LP;
}


/**
 * Fuction that process how the launched fireball work
 * 
 * @param dx changes of the fireball in the x axis
 * @param dy changes of the fireball in the y axis
 * 
 * @return true if the fireball can move to the direction dx and dy, false otherwise(hit something)
 */
bool Gardien::process_fireball (float dx, float dy)
{
	//calculer la distance entre le gardien et le lieu de l'explosion.
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

		//To give the hunter a chance, each guard has a probability of missing his target; 
		//this probability depends on the guard's health: the less health points he has, 
		//the worse his shot. The coefficient that relates the health of a guard to his ability to shoot 
		//accurately is also a parameter of the program.
		double probability = hit_probability();
        

		//how close the fireball exploded to the hunter
		float x_place = (this->_l->_guards[0]->_x - _fb -> get_x ())/ Environnement::scale;
		float y_place = (this->_l->_guards[0]->_y - _fb -> get_y ())/ Environnement::scale;
		
		//it the fireball exploded close enough to the hunter then hunter lost its life point
		//the "closeness" is caculated based on the precision of the guard's aim and its life point.
		if ((abs(x_place) <= 1*probability) && (abs(y_place) <= 1*probability) &&  (((Gardien*)(this->_l->_guards[0]))->isAlive() == true))
		{
			((Chasseur*)(this->_l->_guards[0]))->decrease_LP_chasseur(); 	
		}
	}
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	return false;
}


/**
 * Fuction that implements the movement of the guard with a given angle. The change in x and y direction
 * with respect to the angle has been computed in the update function. The move function checks, if there 
 * is something in the way (and possibly change the angle randomly if the guard hits a wall). If the guard
 * leaves a field in the matrix, it is updated
 * 
 * @param dx changes of the position of the guard in the x axis
 * @param dy changes of the position of the guard in the y axis
 * 
 * @return true if the it can move to the direction dx and dy, false otherwise(blocked by something)
 */
bool Gardien::move (double dx, double dy){
	
	//the new position in x axis
	int new_x=(_x+dx*speed)/Environnement::scale;
	//the new position in y axis
	int new_y=(_y+dy*speed)/Environnement::scale;

	//We tried to implement a collider that is bigger than the speed parameters, however this did not work in the ends
	//int checkbox_x_a = (_x+dx*speed+((dx>=0)-(dx<0))*hitbox)/Environnement::scale;
	//int checkbox_y_a = (_y+dy*speed+((dy>=0)-(dy<0))*hitbox)/Environnement::scale;

	bool empty=_l -> data ((int)(new_x),(int)(new_y));
	//bool empty_checkbox=_l -> data ((int)(checkbox_x_a),(int)(checkbox_y_a));

	bool changed_x= new_x != (int)(_x/Environnement::scale); //check if rounded _x and new_x positions are the same 
	bool changed_y= new_y != (int)(_y/Environnement::scale); //check if rounded _y and new_y positions are the same

	//bool changed_checkbox_x = checkbox_x_a !=(int)(_x/Environnement::scale); 
	//bool changed_checkbox_y = checkbox_y_a !=(int)(_y/Environnement::scale); 
	
	//If the guard wants to move outside of the playing field, return false and do not move the guard
	if ((new_x<0) || (new_y<0) || (new_y >= _l->height()) || (new_x >= _l->width())){
		return false;
	}

	//if something is in the way, check if it is us (no change in x or y, we are moving within a field)
	//If it is us in the way -> just move, if not  
	if((empty!=0)){
		//If we switch the field in our matrix, and the field is not empty, that means, there is an obstacle.
		//Run into change the angle at random and return.
		if ((changed_x or changed_y)){
			if ((std::abs(dx)>0.01) or (std::abs(dy)>0.01)){

			//Change angle and dont move forward
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> wall_distr(-90, 90);
			int wall_delta=wall_distr(gen);

			if((_angle+wall_delta)<=0){
				_angle+=abs(wall_delta);
			}
			else if((_angle+wall_delta)>=360){
				_angle=0+(360-_angle);
			}
			else{
				_angle+=wall_delta; //run into wall then change direction
			}
			return false;
			}
			
		}
	}
	//If we dont have any obstacle in the way, and we chang the field in the matrix, update the matrix
	if ((changed_x or changed_y)){
		((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, 1); //update the new position
		((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
	}
	//Displace the guard
	_x+=dx*speed; 
	_y+=dy*speed; 
	return true;
}


/**
 * Fuction that calculates the difference between the angle of the guard and the angle between 
 * the guard and the hunter. The returned value is evaluated in the update function.  
 * 
 * @return the angle difference between guards angle and the hunter in relation to guard
 */
float Gardien::see_chasseur(){
	//get the hunter's position
	Mover* hunter = this->_l->_guards[0];
	int hunterX = hunter->_x / Environnement::scale; //the x-axis position of the hunter
	int hunterY = hunter->_y / Environnement::scale; //the y-axis position of the hunter
	float gardianX = this->_x / Environnement::scale; //the x-axis position of the guard
	float gardianY = this->_y / Environnement::scale; //the x-axis position of the guard

	//distance between hunter and guard
	double dx = hunterX - gardianX;
	double dy = hunterY - gardianY;
	// Calculate the angle between the hunter and the hunter in degree
	double angleToNormal = atan2(dy, dx) * 180 / M_PI;
	
	//Turn the angle by 90 degrees to solve the 90 degree offset of the result in atan2
	double angleToNormal2 =  angleToNormal - 90 ;

	// Normalize angleToHunter to be between 0 and 360 degrees
    if (angleToNormal2 <= -270) {
      angleToNormal2 +=360;
    }
	
	//Normalize the angle_diff to be between -180 and 180
    double angle_diff = angleToNormal2 - _angle;
    while (angle_diff < -180) {
      angle_diff += 360;
    }
    while (angle_diff > 180) {
      angle_diff -= 360;
    }
	return angle_diff;
}



/**
 * Fuction that check if there are obstacles blocking the guards
 * 
 * @return true if there are obstacles, false otherwise
 */
bool Gardien::check_obstacles(){
	//we need to store the points that we cross in a vector
	std::vector<std::pair<int, int>> points;

	//To follow the hunter, we first need to find his location and 
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
	//that the dx variable is bigger than the dy variable, which means
	//that we need to increment in vertical direction. Otherwise, we increment in 
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
	//If we performed the iterations over all the fields in the matrix between us and the hunter
	//and we don't hit an obstacle, we can see the hunter and return true. 
	return true;
	}
