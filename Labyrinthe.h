/**
 * The Labyrinthe class inherits form the Environment. In addition to the Environment functionalities, the Labyrinthe
 * function also includes a function to set elements in the data matrix via the set_data function. To get the x and y
 * coordinated of the treasure, the getTresorX and getTresorY functions are used, respectively.
 * 
 * 
 */


#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

class Labyrinthe : public Environnement {
private:
	int		_width;//max width value
	int		_height; //max height value 
	char**	_data; //matrix that contains the obstacles 
	const char* monster[12] = {"Lezard", "Lezard", "Samourai", "Marvin", "Potator", "Marvin","Marvin", "Potator", "Potator", "Samourai", "Marvin"}; //monster versions to choose from

public:
	Labyrinthe (char*);
	int width () { return _width;}	// retourne la largeur du labyrinthe.
	int height () { return _height;}	// retourne la longueur du labyrinthe.
	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j).
	void set_data (int i, int j, char value);
	int get_tresorX();
	int get_tresorY();
};

#endif