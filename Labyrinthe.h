#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"

class Labyrinthe : public Environnement {
private:
	int		_width;
	int		_height;
	char**	_data;
	const char* monster[12] = {"Lezard", "Lezard", "Samourai", "Marvin", "Potator", "Marvin","Marvin", "Potator", "Potator", "Samourai", "Marvin"};

public:
	Labyrinthe (char*);
	int width () { return _width;}	// retourne la largeur du labyrinthe.
	int height () { return _height;}	// retourne la longueur du labyrinthe.
	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j).
	void set_data (int i, int j, char value);
	int getTresorX();
	int getTresorY();
	void get_data(int i, int j);
};

#endif