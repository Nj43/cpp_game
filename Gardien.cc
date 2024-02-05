#include "Gardien.h"
#include <stdio.h>
#include "Mover.h"
#include "Chasseur.h"
#include "Labyrinthe.h"
#include "Sound.h"
#include <cmath>
#include <ctime>
#include <chrono>

#include <iostream>
using namespace std;

Gardien::Gardien(Labyrinthe* l, const char* modele, int _LP) : Mover (120, 80, l, modele)
{
	_guard_fire = new Sound ("sons/hunter_fire.wav");
	_guard_hit = new Sound ("sons/hunter_hit.wav");
	_wall_hit = new Sound ("sons/hit_wall.wav");

    this->_LP = _LP;
    _lastFB = std::chrono::system_clock::now();
    attaque = false;
}


void Gardien::kill_gardien(){
    this->alive = false;
    this->rester_au_sol();
}

void Gardien::decrease_LP(){
    this->_LP -= 30;
	if (this->_LP <= 0){
        message ("I'm dead.");
        this->kill_gardien();
    }else{
        message ("Aïe ! I only have %d LP now.", (int) this->_LP);
        this->tomber();
    }
}

bool Gardien::isAlive(){
    return this->alive;
}

void Gardien::update(){
   this->fire(0);
}

void Gardien::fire(int angle_vertical){
    float theta = -std::atan2((_l->_guards[0]->_x - _x),(_l->_guards[0]->_y - _y));
	this->_angle = (180 * theta) / M_PI;
    
    //cout << see_chasseur() << "\n";
   
    //if the gardien sees the hunter
    //cout << (std::chrono::system_clock::now() -_lastFB).count() << "\n";
    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _lastFB;
    if ((elapsed_seconds).count() > 3.0)
    {
        if (((Chasseur*)(this->_l->_guards[0]))->isAlive())
        {
            _guard_fire -> play ();
            _lastFB = std::chrono::system_clock::now();
            _fb -> init (_x, _y, 10., angle_vertical, _angle);
        }
    }


}


bool Gardien::process_fireball (float dx, float dy)
{
	// calculer la distance entre le gardien et le lieu de l'explosion.
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
		// collision...
		
        float x_place = (this->_l->_guards[0]->_x - _fb -> get_x ())/ Environnement::scale;
        float y_place = (this->_l->_guards[0]->_y - _fb -> get_y ())/ Environnement::scale;
        
        //we get the gardien that is at (x,y) which means that it is hit by the fireball
        if ((abs(x_place) <= 1.5) && (abs(y_place) <= 1.5) &&  (((Chasseur*)(this->_l->_guards[0]))->isAlive() == true))
        {
            message("Chasseur hit");
            //((Gardien*)(this->_l->_guards[i]))->decrease_LP();
            
        }
		
	}
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	//message ("Booom...");
	return false;
}


bool Gardien::see_chasseur(){

	int x_gardien = this->_x / Environnement::scale;
	int y_gardien = this->_y / Environnement::scale;

	int x_chasseur = this->_l->_guards[0]->_x / Environnement::scale;
	int y_chasseur = this->_l->_guards[0]->_y / Environnement::scale;

	// Calcul de la distance entre le gardien et le chasseur
	int dst = sqrt ( pow ( this->_x - x_chasseur, 2 ) + pow ( this->_y - y_chasseur, 2));

	// Calcul de la portée de vision du gardien
	double view = 1 * this->_LP;
    
	if (dst<view) {
        
		//Calcul calcul de l'angle 
		float theta = std::atan((_l->_guards[0]->_x - _x)/ (_l->_guards[0]->_y - _y));
	    float angle = (180 * theta) / M_PI;
		float _x = -sin(angle);
		float _y = cos(angle);

		float x = x_gardien;
		float y = y_gardien;

		while (x_chasseur != round(x) || y_chasseur != round(y)) 
		{
			if (_l->data(round(x), round(y)) != EMPTY) return false;

			x += _x;
			y += _y;
		}

		return true;
	}

	return false;
}



