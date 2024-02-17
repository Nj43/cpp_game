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
	int _LP;
	bool alive;
	std::chrono::time_point<std::chrono::system_clock> _lastFB;
	std::random_device rd; // obtain a random number from hardware
	bool attaque;
	int _mode;//0 is patrol mode and 1 is attack mode
	int update_counter; //to count and not update movement at every step
	int fps_counter;
	int timer; //to count and not update movement at every step
	float moveRadius; //maximum range that the guards can move in one update
	int dangle; //maximum angle that the guardians can turn in one update
	int speed;
	float hitbox;

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
	bool isAlive(void);
	float see_chasseur(void);
	bool check_obstacles();
	double hit_probability(double coefficient);	
	void destruct_dead_gardien();

};

#endif
