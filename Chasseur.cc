#include "Chasseur.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> 
#include <string>
#include <cmath> 
#include <random>
class Labyrinthe;
/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	//get the new positions on x and y axis
	int new_x=(_x+dx)/Environnement::scale;
	int new_y=(_y+dy)/Environnement::scale;

	//see if we changed our place in the matrix
	bool changed_x= new_x != (int)(_x/Environnement::scale); 
	bool changed_y= new_y != (int)(_y/Environnement::scale);

	//check if index in matrix we want to move to is empty
	bool empty=_l -> data ((int)(new_x),(int)(new_y));

	//we change this function in order to update the matrix

	//if the slot is not empty, we need to find out if it is because of us
	//or if it is another object
	if (empty ==1){
		//If we change our position, we need to update the matrix and we are free to move
		if((changed_x or changed_y)){
			((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, 1); //update the new position
			((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
			_x += dx;
			_y += dy;
			return true;
		}
	}
	//if we are free to move we update the position in the matrix and move
	else{
		((Labyrinthe *) _l)->set_data ((int) new_x, (int) new_y, 1); //update the new position
		((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	// initialise les sons.
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
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
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
	//std::cout<<"                                           ("<<_x<<","<<_y<<")"<<std::endl;
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de vis�e */ angle_vertical, _angle);
}

/*
 *	Clic droit: par d�faut fait tomber le premier gardien.
 *
 *	Inutile dans le vrai jeu, mais c'est juste pour montrer
 *	une utilisation des fonctions � tomber � et � rester_au_sol �
 */

void Chasseur::right_click (bool shift, bool control)
{
	if (shift)
		_l -> _guards [1] -> rester_au_sol ();
	else
		_l -> _guards [1] -> tomber ();
}
