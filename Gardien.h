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
	int update_counter; //to count and not update movement at every step
	float moveRadius=1; //maximum range that the guards can move in one update
	float maxAngle=0.2; //maximum angle that the guardians can turn in one update
public:
	Gardien (Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
	{}

	// mon gardien pense tr�s mal!
	void update (void) {
		// Generate random movement

		

		
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(1, 3);

		//float dx=Environnement::scale*(distr(gen)/10.f);
		//float dy=Environnement::scale*(distr(gen)/10.f);

		//std::cout<<"Movement: x_axis: "<<dx<<"  Movement: y_axis: "<<dy<<std::endl;

		double dx = Environnement::scale*(rand() % 3 - 1)*0.2; // Random change in x-coordinate (-5, 0, 5)
		double dy = Environnement::scale*(rand() % 3 - 1)*0.2; // Random change in y-coordinate (-5, 0, 5)
		
		//randomly pick angle
		//_angle+=distr(gen);;

		/*
		double desired_angle = atan2(dy, dx);

		double angle_diff = desired_angle -_angle;
		if (angle_diff > M_PI) {
			angle_diff -= 2 * M_PI;
		} else if (angle_diff <= -M_PI) {
			angle_diff += 2 * M_PI;
		}

		// Rotate the guard to face the desired direction
		_angle += angle_diff*100;
		std::cout<<angle_diff<<std::endl;
		
		// Check if the new position is within the boundaries of the Labyrinthe
		*/
		//double dx=(Environnement::scale*1)/5;
		//double dy=(Environnement::scale*1)/5;
		//std::cout<<"Movement: x_axis: "<<dx<<"  Movement: y_axis: "<<dy<<std::endl;
		//std::cout<< _l -> data ((int)((_x + dx) / Environnement::scale),(int)((_y + dy) / Environnement::scale))<<std::endl;
		std::cout<<(int)((_x + dx) / Environnement::scale)<<"  "<<(int)((_y + dy) / Environnement::scale)<<std::endl;
		if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),(int)((_y + dy) / Environnement::scale))){
			std::cout<<"Here"<<std::endl;
			move(dx, dy);
		
		}
		else{
			_angle+=distr(gen);;

		}
		
	
	};
	// et ne bouge pas!
	bool move (double dx, double dy) { 
		_x += dx;
        _y += dy;

        // Print new position for testing
        //std::cout << "New position: (" << _x << ", " << _y << ")\n";

        return true; // Movement successful
	}//return false; }
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
};

#endif
