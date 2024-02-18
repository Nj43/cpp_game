/**
 * The Mover class is the parent class of the Gardien and the Chasseur classes. It includes an Environment
 * object that can be used to check the data matrix that they are set in. The FireBall object allows guards 
 * and hunter to shoot fireballs (each with their own logic implemented in their respective subclasses). 
 * Additionally, the angle, in which they face and their texture are stored as public variables. Functions
 * for processing the fireball (process_fireball, fire, right click), moving (move, tomber, rester_au_sol),
 * and updating are overridden in their respective subclasses.
 * 
 * 
 */

#ifndef MOVER_H
#define MOVER_H

class Labyrinthe;	// la (future) v�tre

#include "FireBall.h"
#include "Environnement.h"

class Mover {
private:
	static void*	init (const char*);	// initialise le mod�le md2.
public:
	Environnement*	_l;		// le labyrinthe dans lequel il �volue.
	FireBall*	_fb;		// sa boule de feu.
	float		_x, _y;		// position.
	int			_angle;		// angle de d�placement/tir.
	void*		_model;		// le mod�le graphique.

	Mover (int x, int y, Labyrinthe* l, const char* modele) :
		_l ((Environnement*)l), _fb (0), _x ((float)x), _y ((float)y),
		_angle (0)
	{ _model = init (modele); }
	virtual ~Mover () {}
	void tomber ();			// fait tomber un personnage (gardien) et se relever
	void rester_au_sol ();	// fait tomber un personnage (gardien) et le laisse au sol.
	virtual void update (void) =0;	// fait 'penser' le personnage (gardien).
	// fait bouger la boule de feu du personnage.
	virtual bool process_fireball (float dx, float dy) =0;
	// tente de d�placer le personnage de <dx,dy>.
	virtual bool move (double dx, double dy) =0;
	// fait tirer le personnage sur un ennemi (vous pouvez ignorer l'angle vertical).
	virtual void fire (int angle_vertical) =0;
	// appel�e pour le gardien 0 (chasseur) quand l'utilisateur fait un clic droit;
	// shift (control) est vrai si la touche shift (control) est appuy�e.
	virtual void right_click (bool shift, bool control) {}
};

#endif
