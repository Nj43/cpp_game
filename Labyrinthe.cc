#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> 
#include <string>

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

#define	LAB_WIDTH	100
#define	LAB_HEIGHT	100


void printMatrix(const std::vector<std::string>& matrix) {
    for (const auto& row : matrix) {
        std::cout << row << std::endl;
    }
}
std::vector<std::string> transposeMatrix(const std::vector<std::string>& matrix, int height, int width) {
    std::vector<std::string> transposedMatrix(width, std::string(height, ' ')); // Swap width and height
    for (size_t row = 0; row < height; ++row) {
        // Iterate over each column of the original matrix
        for (size_t col = 0; col < width; ++col) {
            // Set the character at the corresponding position in the transposed matrix
            transposedMatrix[col][row] = matrix[row][col]; // Swap row and col
        }
    }
    return transposedMatrix;
}



std::vector<std::vector<std::vector<std::vector<int>>>> createLabyrinth(char* filename){
	//First, we need to read in the file and store the necessary elements for the map
	std::vector<std::string> lines;
	std::ifstream file(filename);
	std::vector<std::vector<std::vector <int>>> horizontalWalls;
	std::vector<std::vector<std::vector<int>>> verticalWalls;
	std::vector<std::vector<std::vector<int>>> Walls;
	std::vector<std::vector<int>> plusIndices;
	int longestRow=0;
	int lineCounter=0;
	int plusIndex = 1; //we need to start with 1 to make the % questions work
	//std::vector<std::vector<int>> plusIndices;
	//int lineCounter=-1;

	if (file.is_open()){
		std::string line;
		bool startSaving=false;
		while (std::getline(file, line)){
			if (line.find("+") != std::string::npos){
				startSaving = true;
			}
			if (startSaving){
				//for (int i=0; i<line.size(); i++){
				//	lines.push_back(line[i]);
				//}
				lines.push_back(line);
				//std::vector<int> plusIndices;
   				size_t pos = 0;
   				if(line.size() > longestRow){
   					longestRow=line.size();
   				}
				//since we are already going line by line, lets select and add the horizontal wall indices
				//A horizontal wall will be definded by two + symbols, so we need to find them in the line
				//Now we have all of the + symbol indices stored. A wall is defined by 2 + symbols.
				//That means, that if we iterate over the + symbols and create walls between every 2 plus symbols
				//they should reflect the horizontal walls of the labyrinth.
				//To cound every two + symbols, we instantiate a counter

				
				while ((pos = line.find('+', pos)) != std::string::npos) {
				    plusIndices.push_back({static_cast<int>(pos), lineCounter});
				    //std::cout<<plusIndex<<std::endl;
				    //std::cout<<line<<std::endl;
				    //std::cout<<pos<<std::endl;
				    //std::cout<<plusIndex<<std::endl;
				    //std::cout<<"twoPlusCounter: "<<twoPlusCounter<<std::endl;
				    	//we need to check if the next character after a plus is a - symbol, if yes, then there is another wall coming
				    if(line[pos-1] == '-'){
						//std::cout<<"Line counter: "<<lineCounter<<std::endl;
						//for(int i=0; i<plusIndices.size();i++){
						//	std::cout<<"Elements in vector: "<<plusIndices[i][1]<<std::endl;
						//}
						//std::cout<<"Stored indices of plusIndices vector: "<<plusIndices[lineCounter][0]<<std::endl;
						//std::cout<<"Stored indices: "<<plusIndices[plusIndex-2][1]<<", "<<plusIndices[plusIndex-1][1]<<std::endl;
						Walls.push_back({plusIndices[plusIndex-2], plusIndices[plusIndex-1]});
				    }
				    pos++;
				    plusIndex++;
				}
				lineCounter++; 
			}
			
			
		}
		//for (const std::string& line : lines){
			//std::cout << line << std::endl; 
		//}
		file.close();
	}

	//After reading in all of the hoizontal lines and creating a map with only the verticals left, 
	//This means we need to iterate over the columns
	std::cout<<lineCounter<<" "<<longestRow<<std::endl;
	std::vector<std::string> transposedMap=transposeMatrix(lines, lineCounter, longestRow);
	printMatrix(transposedMap);
	//std::cout<<transposedMap[0]<<std::endl;

	for (size_t columnIndex=0; columnIndex < transposedMap.size(); columnIndex++){
		size_t columnPos = 0;
		int columnPlusIndex = 1;
		std::vector<std::vector<int>> columnPlusIndices;
		//std::cout<<"here"<<std::endl;
		while ((columnPos = transposedMap[columnIndex].find('+', columnPos)) != std::string::npos) {
			//std::cout<<"over here"<<std::endl;
			columnPlusIndices.push_back({columnIndex,static_cast<int>(columnPos),});
		    //std::cout<<line<<std::endl;
		    //std::cout<<plusIndex<<std::endl;
		    //std::cout<<pos<<std::endl;
		    //std::cout<<plusIndex<<std::endl;
		    //std::cout<<"twoPlusCounter: "<<twoPlusCounter<<std::endl;
		    	//we need to check if the next character after a plus is a - symbol, if yes, then there is another wall coming
		    if(transposedMap[columnIndex][columnPos-1] == '|'){
				//std::cout<<"Stored indices: "<<columnPlusIndices[columnPlusIndex-2][0]<<", "<<columnPlusIndices[columnPlusIndex-1][0]<<std::endl;
				Walls.push_back({columnPlusIndices[columnPlusIndex-2], columnPlusIndices[columnPlusIndex-1]});
		    }
		    columnPos++;
		    columnPlusIndex++;
		}
	}

	//for(i=0; i<longestRow; i++){

	//}

	std::cout << "First wall: start x:" << Walls[4][0][0] <<  " start y: " << Walls[4][0][1] << "  end x: " << Walls[4][1][0] << "  end y: " << Walls[4][1][1] << std::endl; //this way we can access the lines
	//std::cout << horizontalWalls[0] << std::endl;
	return {Walls};
}
void printArray(char** array, int width, int height) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::cout << static_cast<int>(array[y][x]) << " "; // Print the value of each element
        }
        std::cout << std::endl; // Move to the next line after printing each row
    }
}



Labyrinthe::Labyrinthe (char* filename) : _width(LAB_WIDTH), _height(LAB_HEIGHT)
{
	/*
	//createLabyrinth(filename);
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

	printArray(_data, width (), height ());
	//std::cout<<width () << std::endl;
}
	
	//printArray(_data, width (), height ());
	*/




	
	std::vector<std::vector<std::vector<std::vector<int>>>> all_data=createLabyrinth(filename);

	//first, place the horizontal walls
	std::cout<<"Total number of walls: "<<all_data[0].size()<<std::endl;
	std::cout << "First wall: start x:" << all_data[0][4][0][0] <<  " start y: " << all_data[0][4][0][1] << "  end x: " << all_data[0][4][1][0] << "  end y: " << all_data[0][4][1][1] << std::endl;
	_nwall = static_cast<int>(all_data[0].size()); 
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

	int heightValue=0; //max y value
	int widthValue=0; //max x value

	for (int i=0; i< _nwall; i++){
		_walls [i]._x1 = all_data[0][i][0][0]; _walls [i]._y1 = all_data[0][i][0][1]; //start point
		_walls [i]._x2 = all_data[0][i][1][0]; _walls [i]._y2 = all_data[0][i][1][1]; //end point
		_walls [i]._ntex = 0;
		if (i==4){
			std::cout << "First wall: start x:" << all_data[0][i][0][0] <<  " start y: " << all_data[0][i][0][1] << "  end x: " << all_data[0][i][1][0] << "  end y: " << all_data[0][i][1][1] << std::endl;
		}

		if(heightValue < all_data[0][i][0][1] || heightValue < all_data[0][i][1][1]){
			if(all_data[0][i][1][1] > all_data[0][i][0][1]){
				heightValue=all_data[0][i][1][1];
			} else{
				heightValue=all_data[0][i][0][1];
			}
		}

		if(widthValue < all_data[0][i][0][0] || widthValue < all_data[0][i][1][0]){
			if(all_data[0][i][1][0] > all_data[0][i][0][0]){
				widthValue=all_data[0][i][1][0];
			} else{
				widthValue=all_data[0][i][0][0];
			}
		}

	}






	// 6. créer la matrice représentant le sol du labyrinthe.

	// 6.1 allouer un tableau de pointeurs de lignes.
	_data = new char* [height ()];
	// 6.2 faire pointer chaque ligne sur son tableau de colonnes.
    for (int i = 0; i < height (); ++i) {
        _data[i] = new char[width ()];
    }

    // Initialize the array with 0s
    for (int y = 0; y < height (); ++y) {
        std::memset(_data[y], EMPTY, width () * sizeof(char));
    }

    for (int i=0; i < static_cast<int>(all_data[0].size()); i++){
    	int x1 = all_data[0][i][0][0];
        int y1 = all_data[0][i][0][1];
        int x2 = all_data[0][i][1][0];
        int y2 = all_data[0][i][1][1];

        std::cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<" "<<std::endl;
        for (int y = y1; y <= y2; ++y) {
            for (int x = x1; x <= x2; ++x) {
                _data[y][x] = 1;
            }
        }
    }
	//printArray(_data, width () , height ());
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
