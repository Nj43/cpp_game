/**
 * The Chassuer class inherits from the Mover class and is responsible for the logic of the hunter. It specifies firing 
 * functionalities (hit probability, processing a fireball etc). The death logic of the hunter is defined in the decrease_LP_chasseur
 * and increase_LP_chasseur functions. If the hunter touches the treasure the game is over. This functionality is implemented in the
 * win_game function.
 * 
 */

#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"
#include <ctime>
#include <chrono>

class Labyrinthe;

class Chasseur : public Mover {
private:
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);
	bool _alive; //true if the hunter is alive
	int _LP; //life point of the hunter
	std::chrono::time_point<std::chrono::system_clock> _lastHeal_C;
	bool _previousFireballExploded;
public:
	// les sons.
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touch�.
	static Sound*	_wall_hit;		// on a tap� un mur.

	// constructeur.
	Chasseur (Labyrinthe* l);
	// le chasseur ne pense pas!
	void update (void);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	
	// fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
	bool process_fireball (float dx, float dy);
	// tire sur un ennemi.
	void fire (int angle_vertical);
	// clic droit.
	void right_click (bool shift, bool control);
	bool isAlive();
	bool win_game();
	void decrease_LP_chasseur();
	void increase_LP_chasseur();
	double hit_probability();
};

#endif
