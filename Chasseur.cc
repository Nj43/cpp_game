#include "Chasseur.h"
#include "Gardien.h"
#include "Labyrinthe.h"
#include <iostream>
using namespace std;


#define HEALTH_C 100
#define RECOVERY_TIME_C 12.0
#define COEFFICIENT_C 0.03

/**
 * Call the constructor to initialize variables
 */
Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	// initialise les sons.
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");

	this->_LP = HEALTH_C;
	this->alive = true;
	this->_lastHeal_C = std::chrono::system_clock::now();
}


/**
 * Function that decreases the life point of the gardian. 
 * If its life point is less than 0 it means the guardian is dead.
 */
void Chasseur::decrease_LP_chasseur(){
    this->_LP -= 10; //it decreases by 10 points
	if (this->_LP <= 0){ //if the chasseur is dead then it looses the game
        message ("I'm dead.");
        partie_terminee(false);
    }else{
        message ("Aïe ! Chasseur only has %d LP now.", (int) this->_LP); 
    }
}


/**
 * Function that increases the life point of the chasseur
 * when the chasseur goes a certain amount of time without suffering a wound
 */
void Chasseur::increase_LP_chasseur(){
	//seconds between last heal time and now
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _lastHeal_C;
	if ((elapsed_seconds).count() > RECOVERY_TIME_C)
	{
		int aug = this->_LP + 10;
		if(aug <= 100){ //check if it doesn't exceed the maximum life point of the chasseur
			this->_LP = aug;
			_lastHeal_C = std::chrono::system_clock::now(); //the most recent heal time is saved
			message ("Chasseur got healed.");
		}
	}
	
}

/**
 * Fuction that process how the lauched fireball work
 * 
 * @param dx changes of the position of the chasseur in the x axis
 * @param dy changes of the position of the chasseur in the y axis
 * 
 * @return true if the it can move to the direction dx and dy, false otherwise(blocked by something)
 */
bool Chasseur::move_aux (double dx, double dy)
{
	update(); //each time the chasseur move update its state

	//can move only if it is empty
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale), (int)((_y + dy) / Environnement::scale)) or 2 == _l -> data ((int)((_x + dx) / Environnement::scale), (int)((_y + dy) / Environnement::scale)))
	{
		//change the state of the position data
    	((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 0);
		//change positions in x and y axis
		_x += dx;
		_y += dy;
		//change the state of the position data
    	((Labyrinthe *) _l)->set_data ((int) (_x/Environnement::scale), (int) (_y/Environnement::scale), 2);
		return true;
	}
	return false;
}


/**
 * Fuction that make the chasseur win the game
 * 
 * @return true if chasseur won the game, false otherwise
 */
bool Chasseur::win_game()
{
	//positions of the tresor box on the labyrinthe
	int x_tresor = ((Labyrinthe*)(this->_l))->getTresorX();
	int y_tresor = ((Labyrinthe*)(this->_l))->getTresorY();

	//position of the chasseur
	int x = this->_x/Environnement::scale;
	int y = this->_y/Environnement::scale;
	
	//if chasseur has reached the tresor box
	return (abs(x_tresor - x) <= 1) && (abs(y_tresor - y) <= 1);
}


/**
 * Fuction that continually updates the state of the Gardian objects over time.
 * 
 */
void Chasseur::update (void){ 
	if(this->isAlive() == true){ //check if we can increse the LP
		increase_LP_chasseur(); //increase the LP
	}

	
	if (win_game()){ //check if the chasseur won the game
		partie_terminee(true); //finish the game
	}
}


/**
 * Fuction that calculates the hit probability of the chasseur given the life point
 * the less life points it has, the worse its shot
 */
double Chasseur::hit_probability() {
	int rest_LP = this->_LP;
	// Calculate hit probability based on health and coefficient
	return COEFFICIENT_C * rest_LP;	
}

/**
 * Fuction that process how the lauched fireball work
 * 
 * @param dx changes of the fireball in the x axis
 * @param dy changes of the fireball in the y axis
 * 
 * @return true if the fireball can move to the direction dx and dy, false otherwise(hit something)
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
	}else{
		// collision... (if a gardien is touched then we have to decrease its LP(or kill it))
		for (int i = 1; i < this->_l->_nguards; i++)
		{
			//how close the fireball exploded to the gardian
			float x_place = (this->_l->_guards[i]->_x - _fb -> get_x ())/ Environnement::scale;
			float y_place = (this->_l->_guards[i]->_y - _fb -> get_y ())/ Environnement::scale;

			//To give the hunter a chance, each guard has a probability of missing his target; 
			//this probability depends on the guard's health: the less health points he has, 
			//the worse his shot. The coefficient that relates the health of a guard to his ability to shoot 
			//accurately is also a parameter of the program.
			double probability = hit_probability();
			
			//it the fireball exploded close enough to the gardian then gardian lost its life point
			//the "closeness" is caculated based on the precision of the chasseur's aim and its life point.
			if ((abs(x_place) <= 1*probability) && (abs(y_place) <= 1*probability) &&  (((Gardien*)(this->_l->_guards[i]))->isAlive() == true))
			{
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

/**
 * Fuction that lauchs the fireball
 * @param angle_vertical we can ignore this parameter for this project, always set to 0
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

/**
 * Fuction that checks if the gardian is alive
 */
bool Chasseur::isAlive(){
	return this->alive;
}