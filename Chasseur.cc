#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"
#include <iostream>
using namespace std;


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

	this->_LP = 200;
	this->alive = true;
}

/*
 *	Tente un deplacement.
 */

bool Chasseur::move_aux (double dx, double dy)
{
	update();
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

bool Chasseur::win_game()
{
	int x_tresor = ((Labyrinthe*)(this->_l))->getTresorX();
	int y_tresor = ((Labyrinthe*)(this->_l))->getTresorY();
	int x = this->_x/Environnement::scale;
	int y = this->_y/Environnement::scale;
	
	return (abs(x_tresor - x) <= 1) && (abs(y_tresor - y) <= 1);
}


void Chasseur::update (void){ 
	if (win_game()){
		partie_terminee(true);
	}
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
	//cout << x << "\n";
	//cout << y << "\n";

	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.
		return true;
	}else{
		// collision... (if a gardien is touched then we have to decrease its LP(or kill it))
		for (int i = 1; i < this->_l->_nguards; i++)
		{
			float x_place = (this->_l->_guards[i]->_x - _fb -> get_x ())/ Environnement::scale;
			float y_place = (this->_l->_guards[i]->_y - _fb -> get_y ())/ Environnement::scale;
			
			//we get the gardien that is at (x,y) which means that it is hit by the fireball
			if ((abs(x_place) <= 1.5) && (abs(y_place) <= 1.5) &&  (((Gardien*)(this->_l->_guards[i]))->isAlive() == true))
			{
				//message("hit %d", i);
				((Gardien*)(this->_l->_guards[i]))->decrease_LP();
				
			}
			
		}
		
	}
	
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	//message ("Booom...");
	return false;


}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
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


bool Chasseur::isAlive(){
	return this->alive;
}