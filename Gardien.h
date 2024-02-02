#ifndef GARDIEN_H
#define GARDIEN_H

#include "Mover.h"

class Labyrinthe;

class Gardien : public Mover {
private :
	int _LP;
	bool alive;

public:
	Gardien (Labyrinthe* l, const char* modele, int LP) : Mover (120, 80, l, modele)
	{
		_LP=LP;
		alive = true;
	}

	// mon gardien pense tr�s mal!
	void update (void) {};
	// et ne bouge pas!
	bool move (double dx, double dy) { return false; }
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical) {}
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy) { return false; }
	void kill_gardien(void);
	void decrease_LP(void);
	bool isAlive(void);
	

};

#endif
