#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.

/*
 *	Pour que l'interface puisse appeler le constucteur de
 *	la classe Labyrinthe sans en avoir les sources...
 */

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

/*
 *	EXEMPLE de labyrinthe à taille fixe.
 */

#define	LAB_WIDTH	80
#define	LAB_HEIGHT	25

Labyrinthe::Labyrinthe (char* filename) : _width(LAB_WIDTH), _height(LAB_HEIGHT)
{
	// 1. allouer les tableaux pour les murs, les affiches et les caisses.

	// les 4 murs.
	_nwall = 4;
	_walls = new Wall [_nwall];
	// deux affiches.
	_npicts = 2;
	_picts = new Wall [_npicts];
	// 3 caisses.
	_nboxes = 3;
	_boxes = new Box [_nboxes];
	// 2 marques au sol.
	_nmarks = 2;
	_marks = new Box [_nmarks];

	// 2. placer les 4 murs.
	_walls [0]._x1 = 0; _walls [0]._y1 = 0;
	_walls [0]._x2 = width ()-1; _walls [0]._y2 = 0; 
	_walls [0]._ntex = 0; 

	_walls [1]._x1 = width ()-1; _walls [1]._y1 = 0;
	_walls [1]._x2 = width ()-1; _walls [1]._y2 = height ()-1; 
	_walls [1]._ntex = 0; 

	_walls [2]._x1 = width ()-1; _walls [2]._y1 = height ()-1;
	_walls [2]._x2 = 0; _walls [2]._y2 = height ()-1; 
	_walls [2]._ntex = 0; 

	_walls [3]._x1 = 0; _walls [3]._y1 = height ()-1;
	_walls [3]._x2 = 0; _walls [3]._y2 = 0; 
	_walls [3]._ntex = 0; 

	// 3. placer les affiches; Attention: pour des raisons de rapport d'aspect,
	// les affiches doivent faire 2 de long)
	_picts [0]._x1 = 4; _picts [0]._y1 = 0;
	_picts [0]._x2 = 6; _picts [0]._y2 = 0; 
	_picts [0]._ntex = 0; 

	// la deuxième à une texture différente.
	_picts [1]._x1 = 8; _picts [1]._y1 = 0;
	_picts [1]._x2 = 10; _picts [1]._y2 = 0; 

	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	_picts [1]._ntex = wall_texture (tmp);

	// 4. placer les caisses.
	_boxes [0]._x = 70; _boxes [0]._y = 12; _boxes [0]._ntex = 0; 
	_boxes [1]._x = 10; _boxes [1]._y = 5; _boxes [1]._ntex = 0; 

	// mettre une autre texture à la troisième caisse.
	sprintf (tmp, "%s/%s", texture_dir, "boite.jpg");
	_boxes [2]._x = 65; _boxes [2]._y = 22; _boxes [2]._ntex = wall_texture (tmp);

	// 5. mettre deux marques au sol.
	_marks [0]._x = 50; _marks [0]._y = 14;
	sprintf (tmp, "%s/%s", texture_dir, "p1.gif");
	_marks [0]._ntex = wall_texture (tmp);

	_marks [1]._x = 20; _marks [1]._y = 15;
	sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
	_marks [1]._ntex = wall_texture (tmp);


	// 6. créer la matrice représentant le sol du labyrinthe.

	// 6.1 allouer un tableau de pointeurs de lignes.
	_data = new char* [width ()];
	// 6.2 faire pointer chaque ligne sur son tableau de colonnes.
	for (int i = 0; i < width (); ++i)
		_data [i] = new char [height ()];

	// 7. indiquer l'emplacement des murs en marquant les cases du sol.
	for (int i = 0; i < width (); ++i)
		for (int j = 0; j < height (); ++j) {
			if (i == 0 || i == width ()-1 || j == 0 || j == height ()-1)
				_data [i][j] = 1;
			else
				_data [i][j] = EMPTY;
		}

	// 7.1 indiquer l'emplacement des caisses au sol.
	_data [_boxes [0]._x][_boxes [0]._y] = 1;
	_data [_boxes [1]._x][_boxes [1]._y] = 1;
	_data [_boxes [2]._x][_boxes [2]._y] = 1;

	// 8. le trésor.
	_treasor._x = 10;
	_treasor._y = 10;
	// 8.1 indiquer l'emplacement du trésor au sol.
	_data [_treasor._x][_treasor._y] = 1;

	// 9. allouer le chasseur et les 4 gardiens.
	_nguards = 5;
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this);
	_guards [1] = new Gardien (this, "Lezard");
	_guards [2] = new Gardien (this, "Blade"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	_guards [3] = new Gardien (this, "Serpent"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	_guards [4] = new Gardien (this, "Samourai"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;

	// 9.1 indiquer l'emplacement des gardiens au sol.
	_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale)] = 1;
	_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
}
