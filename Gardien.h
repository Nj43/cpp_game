/**
 * The Gardien class inherits from the Mover class and is responsible for the logic of the guards. It 
 * includes special move function for guards and the logic to see the hunter. Additionally, specific firing 
 * functionalities (hit probability, processing a fireball etc) are implemented. The death logic of guards
 * is defined using the kill_gardien and the isAlive functions.  
 */

#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Sound.h"
#include <chrono>
#include <ctime>
#include <random>

class Labyrinthe;

class Gardien : public Mover {
private :
	int _LP; //the life point of the guard
	bool alive; //true if the guard is alive
	std::chrono::time_point<std::chrono::system_clock> _lastFB; //time since last fireball has been sent out
	std::chrono::time_point<std::chrono::system_clock> _lastHeal; //time since last increase of LP has been performed
	std::random_device rd; // obtain a random number from hardware
	int _mode;//0 is patrol mode and 1 is attack mode
	int update_counter; //to count and not update movement at every step
	int speed;  //speed parameter detemining what fraction of Environment::scale the guards can progress in one frame
	//float hitbox; // hitbox parameter with which we wanted to fix the aesthetic of the guards hitting objects 

public:
	
	Gardien (Labyrinthe* l, const char* modele, int LP);

	Sound*	_guard_fire;	// bruit de l'arme du gardien.
	Sound*	_guard_hit;		// cri du gardien touché.
	Sound*	_wall_hit;		// on a tapé un mur.

	// mon gardien pense tr�s mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy);
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);
	void kill_gardien(void);
	void decrease_LP(void);
	void increase_LP(void);
	bool isAlive(void);
	float see_chasseur(void);
	bool check_obstacles();
	double hit_probability();	


};

#endif
