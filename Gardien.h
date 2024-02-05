#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"
#include "Sound.h"
#include <chrono>
#include <ctime>

class Labyrinthe;

class Gardien : public Mover {
private :
	int _LP;
	bool alive;
	std::chrono::time_point<std::chrono::system_clock> _lastFB;
	bool attaque;

public:
	
	Gardien (Labyrinthe* l, const char* modele, int LP);

	Sound*	_guard_fire;	// bruit de l'arme du gardien.
	Sound*	_guard_hit;		// cri du gardien touché.
	Sound*	_wall_hit;		// on a tapé un mur.

	// mon gardien pense tr�s mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy){return false;}
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);
	void kill_gardien(void);
	void decrease_LP(void);
	bool isAlive(void);
	bool see_chasseur(void);
	

};

#endif
