#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> 
#include <string>

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touch�.
Sound*	Chasseur::_wall_hit;	// on a tap� un mur.

/*
 *	Pour que l'interface puisse appeler le constucteur de
 *	la classe Labyrinthe sans en avoir les sources...
 */

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}

/*
 *	EXEMPLE de labyrinthe � taille fixe.
 */

#define	LAB_WIDTH	100
#define	LAB_HEIGHT	100
#define HEALTH_G 200

/**
 * Function to output the x coordinate of the treasure. 
 *
 * @returns x coordinate of the treasure
 */
int Labyrinthe::get_tresorX(){

	return _treasor._x;
}

/**
 * Function to output the y coordinate of the treasure. 
 *
 * @returns y coordinate of the treasure
 */
int Labyrinthe::get_tresorY(){

	return _treasor._y;
}

/**
 * Function to transpose a matrix. Using the height and width values provided to the function, it will iterate over
 * the rows and columns and transpose the matrix element by element. It will store the resulting matrix in a new vector
 * and return it. 
 * 
 * @param matrix matrix that we want to transpose which contains vectors of strings 
 * @param height height value of the original matrix
 * @param width width value of the original matrix
 *
 * @returns transposed matrix 
 */

std::vector<std::string> transpose_matrix(const std::vector<std::string>& matrix, int height, int width) {
    std::vector<std::string> transposedMatrix(width, std::string(height, ' ')); // Swap width and height
    for (int row = 0; row < height; ++row) {
        // Iterate over each column of the original matrix
        for (int col = 0; col < width; ++col) {
            // Set the character at the corresponding position in the transposed matrix
            transposedMatrix[col][row] = matrix[row][col]; // Swap row and col
        }
    }
    return transposedMatrix;
}


/**
 * Function to read in the map from a given file. The functions starts by initializing a vector to store the coordinates 
 * for every object in the game. Then, the file is read in and horizontal walls and picutures, as well as all other objects 
 * stored in their respective vectors. Then, the lines are transposed using the transpose_matrix function and the transposed lines
 * are read in, storing vertical lines and posters. In the end, all objects are added to a vector and the vector returned.
 * 
 * @param filename name of the file where the labyrinth map is stored 
 *
 * @returns vector containing all the objects of the game and their respective positions (start and end positions for walls
 * and pictures)
 */

std::vector<std::vector<std::vector<std::vector<int>>>> createLabyrinth(char* filename){
	//First, we need to read in the file and store the necessary elements for the map
	std::vector<std::string> lines;
	std::ifstream file(filename);
	std::vector<std::vector<std::vector <int>>> horizontalWalls;
	std::vector<std::vector<std::vector<int>>> verticalWalls;
	std::vector<std::vector<std::vector<int>>> Walls;
	std::vector<std::vector<std::vector <int>>> guards; 
	std::vector<std::vector<std::vector <int>>> boxes;
	std::vector<std::vector<std::vector <int>>> pictures_b;
	std::vector<std::vector<std::vector <int>>> pictures_a;
	std::vector<std::vector<std::vector <int>>> treasure;
	std::vector<std::vector<std::vector <int>>> chasseur_pos;
	std::vector<std::vector<int>> plusIndices;
	int longestRow=0;
	int lineCounter=0;
	int plusIndex = 0; 


	if (file.is_open()){
		std::string line;
		bool startSaving=false;
		while (std::getline(file, line)){
			if (line.find("+") != std::string::npos){
				startSaving = true;
			}
			if (startSaving){
				lines.push_back(line);
   				size_t pos = 0;
   				if(static_cast<int>(line.size()) > longestRow){
   					longestRow=static_cast<int>(line.size());
   				}
				
				while ((pos = line.find('+', pos)) != std::string::npos) {
				    plusIndices.push_back({lineCounter, static_cast<int>(pos)});
				    if(line[pos-1] == '-' || line[pos-1] == '+' || line[pos-1] == 'a' || line[pos-1] == 'b'){
						Walls.push_back({plusIndices[plusIndex-1], plusIndices[plusIndex]});
				    }
				    pos++;
				    plusIndex++;
				}
				for (size_t i = 0; i < line.length(); ++i) {
                	char c = line[i];
					char before=line[i-1];
					char after=line[i+1];
					if(c=='G'){
						
						guards.push_back({{lineCounter, static_cast<int>(i)}}); 
					}
					else if(c=='C'){
						
						chasseur_pos.push_back({{lineCounter, static_cast<int>(i)}}); 
					}
					
					else if(c=='x'){
						boxes.push_back({{lineCounter, static_cast<int>(i)}});
					}
					else if(c=='a'&&(before=='-'|| after=='-')){
						pictures_a.push_back({{lineCounter, static_cast<int>(i)}, {lineCounter, static_cast<int>(i)-2}});
					}

					else if(c=='b'&&(before=='-'|| after=='-')){
						pictures_b.push_back({{lineCounter, static_cast<int>(i)}, {lineCounter, static_cast<int>(i)-2}});
					}
					else if(c=='T'){
						treasure.push_back({{lineCounter, static_cast<int>(i)}});
					}
					
				}
				lineCounter++; 
			}
			
			
		}
		file.close();
	}


	std::vector<std::string> transposedMap=transpose_matrix(lines, lineCounter, longestRow);
	for (size_t columnIndex=0; columnIndex < transposedMap.size(); columnIndex++){
		size_t columnPos = 0;
		int columnPlusIndex = 1;
		std::vector<std::vector<int>> columnPlusIndices;
		while ((columnPos = transposedMap[columnIndex].find('+', columnPos)) != std::string::npos) {
			columnPlusIndices.push_back({static_cast<int>(columnPos), static_cast<int>(columnIndex)});
		    //we need to check if the next character after a plus is a | symbol, if yes, then there is another wall coming
		    if(transposedMap[columnIndex][columnPos-1] == '|' || transposedMap[columnIndex][columnPos-1] == '+'||transposedMap[columnIndex][columnPos-1] == 'a'||transposedMap[columnIndex][columnPos-1] == 'b'){
				Walls.push_back({columnPlusIndices[columnPlusIndex-2], columnPlusIndices[columnPlusIndex-1]});
		    }
		    columnPos++;
		    columnPlusIndex++;
		}

		for (int i = 0; i < static_cast<int>(transposedMap[columnIndex].length()); ++i) {
            char c = transposedMap[columnIndex][i];
			char before=transposedMap[columnIndex][i-1];
			char after=transposedMap[columnIndex][i+1];
			if (c=='a' && (before == '|' || after == '|')){
				pictures_a.push_back({{static_cast<int>(i), static_cast<int>(columnIndex)}, {static_cast<int>(i)-2, static_cast<int>(columnIndex)}});
			}
			if (c=='b' && (before == '|' || after == '|')){
				pictures_b.push_back({{static_cast<int>(i) ,static_cast<int>(columnIndex)}, {static_cast<int>(i)-2, static_cast<int>(columnIndex)}});
			}
		}
	}
	return {Walls, guards, chasseur_pos, boxes, pictures_a, pictures_b, treasure}; 
}

/**
 * Constructor of the Labyrinthe class. It will call the create_labyrinthe function to read in the 
 *information from the map and store it in the given member variables of the Labyrinthe class. 
 * 
 * @param filename name of the file where the labyrinth map is stored 
 */

Labyrinthe::Labyrinthe (char* filename) : _width(LAB_WIDTH), _height(LAB_HEIGHT)
{
	std::vector<std::vector<std::vector<std::vector<int>>>> all_data=createLabyrinth(filename);
	//Initiate the member variables arrays
	_nwall = all_data[0].size(); 
	_walls = new Wall [_nwall];

	_nboxes = all_data[3].size();
	_boxes = new Box [_nboxes];
	_nmarks = 1;
	_marks = new Box [_nmarks];

	_npicts = all_data[4].size() + all_data[5].size();
	_picts = new Wall [_npicts];
	
	//Store the picture objects of type a (peace signs) and add their x and y values
	for(int i=0; i<static_cast<int>(all_data[4].size()); i++){
		_picts [i]._x1 =  all_data[4][i][0][0]; _picts [i]._y1 =all_data[4][i][0][1];
		_picts [i]._x2 = all_data[4][i][1][0]; _picts [i]._y2 = all_data[4][i][1][1];	
		_picts [i]._ntex = 0; 	 //pictures of type a will be peace signs
	}
	//Store the picture objects of type b (cars) and add their x and y values
	for(int j=all_data[4].size(); j<static_cast<int>((all_data[4].size()+all_data[5].size())); j++){
		int i = j-all_data[4].size();
		_picts [j]._x1 =  all_data[5][i][0][0]; _picts [j]._y1 =all_data[5][i][0][1];
		_picts [j]._x2 = all_data[5][i][1][0]; _picts [j]._y2 = all_data[5][i][1][1];	
		char	tmp [128];
		sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");  //pictures of type b will be cars
		_picts [j]._ntex = wall_texture (tmp);	

	}
	
	//Create the _data array to store guards, walls, boxes and the hunter
	_data = new char* [height ()];
    for (int i = 0; i < height (); ++i) {
        _data[i] = new char[width ()];
    }

    // Initialize the array with 0s
    for (int y = 0; y < height (); ++y) {
        std::memset(_data[y], EMPTY, width () * sizeof(char));
    }

	//Read in the walls' starting and end points and draw the lines between the two points in the matrix
	for (int i=0; i< _nwall; i++){
		_walls [i]._x1 = all_data[0][i][0][0]; _walls [i]._y1 = all_data[0][i][0][1]; //start point
		_walls [i]._x2 = all_data[0][i][1][0]; _walls [i]._y2 = all_data[0][i][1][1]; //end point
		_walls [i]._ntex = 0;

		for (int y = _walls [i]._y1; y <= _walls [i]._y2; ++y) {
            for (int x = _walls [i]._x1; x <= _walls [i]._x2; ++x) {
                _data[x][y] = 1;
            }
        }
	}
	
	//Place the boxes by setting the _data array to 1 at their positions
	for(int i=0; i<_nboxes; i++){
		_boxes[i]._x=all_data[3][i][0][0]; _boxes[i]._y=all_data[3][i][0][1]; _boxes [i]._ntex = 0;
		_data [_boxes [i]._x][_boxes [i]._y] = 1 ;
	}
	
	//Place the treasure
	_treasor._x = all_data[6][0][0][0];
	_treasor._y = all_data[6][0][0][1];	
	_data [_treasor._x][_treasor._y] = 1;

	//Place the hunter (_guards[0]) and guards. Render the guards appearances. 
	_nguards = all_data[1].size() + 1; //get the number of guards
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this); _guards [0] -> _x=scale*static_cast<float>(all_data[2][0][0][0]); _guards [0] ->_y=scale*static_cast<float>(all_data[2][0][0][1]);
	_data [(int)(_guards [0] -> _x/scale)][(int)(_guards [0] -> _y/scale)] = 1;
	
	for (int g = 1; g < _nguards; g++){
		std::cout<<g << monster[g-1]<<std::endl;
		_guards[g] = new Gardien (this, monster[g-1], HEALTH_G); _guards[g] -> _x = scale*static_cast<float>(all_data[1][g-1][0][0]); _guards[g] -> _y = scale*static_cast<float>(all_data[1][g-1][0][1]);
		_data [(int)(_guards[g]->_x/scale)][(int)(_guards[g]->_y/scale)] = 1;

	}
}


void Labyrinthe::set_data (int i, int j, char value) {
		if (i >= 0 && i < this->width() && j >= 0 && j < this->height()) 
		{
			this->_data[i][j] = value;
		}
		
}