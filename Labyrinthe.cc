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

int Labyrinthe::getTresorX(){

	return _treasor._x;
}

int Labyrinthe::getTresorY(){

	return _treasor._y;
}


void printMatrix(const std::vector<std::string>& matrix) {
    for (const auto& row : matrix) {
        std::cout << row << std::endl;
    }
}
std::vector<std::string> transposeMatrix(const std::vector<std::string>& matrix, int height, int width) {
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



std::vector<std::vector<std::vector<std::vector<int>>>> createLabyrinth(char* filename){
	//First, we need to read in the file and store the necessary elements for the map
	std::vector<std::string> lines;
	std::ifstream file(filename);
	std::vector<std::vector<std::vector <int>>> horizontalWalls;
	std::vector<std::vector<std::vector<int>>> verticalWalls;
	std::vector<std::vector<std::vector<int>>> Walls;

	std::vector<std::vector<std::vector <int>>> guards; //this is a weird type but because it has to fit the function, I have to add an unnecessary dimension
	std::vector<std::vector<std::vector <int>>> boxes;
	std::vector<std::vector<std::vector <int>>> pictures_a_x;
	std::vector<std::vector<std::vector <int>>> pictures_a_y;
	std::vector<std::vector<std::vector <int>>> pictures_b_x;
	std::vector<std::vector<std::vector <int>>> pictures_b_y;
	std::vector<std::vector<std::vector <int>>> treasure;
	std::vector<std::vector<std::vector <int>>> chasseur_pos;
	//std::cout<<"Size of Pictures: "<< pictures_a.size()<<std::endl;
	std::vector<std::vector<int>> plusIndices;
	int longestRow=0;
	//int guard_counter=0;
	int lineCounter=0;
	int plusIndex = 0; //we need to start with 1 to make the % questions work
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
				lines.push_back(line);
   				size_t pos = 0;
   				if(static_cast<int>(line.size()) > longestRow){
   					longestRow=static_cast<int>(line.size());
   				}
				
				while ((pos = line.find('+', pos)) != std::string::npos) {
				    plusIndices.push_back({static_cast<int>(pos), lineCounter});
				    if(line[pos-1] == '-' || line[pos-1] == '+' ){
						Walls.push_back({plusIndices[plusIndex-1], plusIndices[plusIndex]});
				    }
				    pos++;
				    plusIndex++;
				}

				for (size_t i = 0; i < line.length(); ++i) {
                	char c = line[i];
					char before=line[i-1];
					char after=line[i-1];
					//std::cout<<c<<std::endl;
					//	std::cout<<c<<std::endl;
					if(c=='G'){
						
						guards.push_back({{static_cast<int>(i), lineCounter}}); //because we have to fit function signature, this we have an unnecessary dimension, maybe change at end
						//guard_counter++;
					}
					else if(c=='C'){
						
						chasseur_pos.push_back({{static_cast<int>(i), lineCounter}}); //because we have to fit function signature, this we have an unnecessary dimension, maybe change at end
						//guard_counter++;
					}
					
					else if(c=='x'){
						boxes.push_back({{static_cast<int>(i), lineCounter}});
					}
					else if(c=='a'&&(before=='-'|| after=='-')){
						pictures_a_x.push_back({{static_cast<int>(i), lineCounter}});
					}

					else if(c=='b'&&(before=='-'|| after=='-')){
						pictures_b_x.push_back({{static_cast<int>(i), lineCounter}});
					}
					else if(c=='T'){
						treasure.push_back({{static_cast<int>(i), lineCounter}});
					}
					
				}
				lineCounter++; 
				//std::cout<<"here"<<std::endl;
			}
			
			
		}
		file.close();
	}


	std::vector<std::string> transposedMap=transposeMatrix(lines, lineCounter, longestRow);


	for (size_t columnIndex=0; columnIndex < transposedMap.size(); columnIndex++){
		size_t columnPos = 0;
		int columnPlusIndex = 1;
		std::vector<std::vector<int>> columnPlusIndices;

		//std::cout<<"here"<<std::endl;
		while ((columnPos = transposedMap[columnIndex].find('+', columnPos)) != std::string::npos) {
			columnPlusIndices.push_back({static_cast<int>(columnIndex),static_cast<int>(columnPos)});

		    	//we need to check if the next character after a plus is a - symbol, if yes, then there is another wall coming
		    if(transposedMap[columnIndex][columnPos-1] == '|' || transposedMap[columnIndex][columnPos-1] == '+'){
				//std::cout<<"Stored indices: "<<columnPlusIndices[columnPlusIndex-2][0]<<", "<<columnPlusIndices[columnPlusIndex-1][0]<<std::endl;
				Walls.push_back({columnPlusIndices[columnPlusIndex-2], columnPlusIndices[columnPlusIndex-1]});
		    }
		    columnPos++;
		    columnPlusIndex++;
		}

		for (int i = 0; i < static_cast<int>(transposedMap[columnIndex].length()); ++i) {
            char c = transposedMap[columnIndex][i];
			char before=transposedMap[columnIndex][i-1];
			char after=transposedMap[columnIndex][i+1];
			//std::cout<<"Character c: "<<c<<std::endl;
			//std::cout<<"Column Index: "<<columnIndex<<std::endl;
			if (c=='a' && (before == '|' || after == '|')){
				//std::cout<<"Transposed X coordinate: "<< static_cast<int>(i) << " Transposed Y coordinate: " <<columnIndex<<std::endl;
				pictures_a_y.push_back({{static_cast<int>(columnIndex), static_cast<int>(i)}});
			}

			if (c=='b' && (before == '|' || after == '|')){
				//std::cout<<"Transposed X coordinate: "<< static_cast<int>(i) << " Transposed Y coordinate: " <<columnIndex<<std::endl;
				pictures_b_y.push_back({{static_cast<int>(columnIndex), static_cast<int>(i)}});
			}
		}

	}


	return {Walls, guards, chasseur_pos, boxes, pictures_a_x, pictures_a_y, pictures_b_x, pictures_b_y, treasure};//, treasure, pictures_a, pictures_b};
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

	std::vector<std::vector<std::vector<std::vector<int>>>> all_data=createLabyrinth(filename);

	//first, place the horizontal walls
	_nwall = all_data[0].size(); 
	_walls = new Wall [_nwall];

	// 3 caisses.
	_nboxes = all_data[3].size();
	//std::cout<<"number of boxes"<< _nboxes<<std::endl;
	_boxes = new Box [_nboxes];
	// 2 marques au sol.
	_nmarks = 1;
	_marks = new Box [_nmarks];

	// 3. placer les affiches; Attention: pour des raisons de rapport d'aspect,
	// les affiches doivent faire 2 de long)
	
	
	//std::cout<<"Number of Pictures: "<< all_data[4].size()<<std::endl;
	//_picts = new Wall [all_data[4].size()];
	_npicts = all_data[4].size() + all_data[5].size()+all_data[6].size() + all_data[7].size(); 
	_picts = new Wall [_npicts];
	
	for(int i=0; i<static_cast<int>(all_data[4].size()); i++){
		
		_picts [i]._x1 =  all_data[4][i][0][0]; _picts [i]._y1 =all_data[4][i][0][1];
		_picts [i]._x2 = (all_data[4][i][0][0]+2); _picts [i]._y2 = all_data[4][i][0][1];	

		//char	tmp [128];
		//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		_picts [i]._ntex = 0; 
		//_picts [i]._ntex = wall_texture (tmp);	
	}

	for(int j=all_data[4].size(); j<static_cast<int>((all_data[4].size()+all_data[5].size())); j++){
		//char	tmp [128];
		//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		int i = j-all_data[4].size();
		//std::cout<<"X coordinates: "<<all_data[5][i][0][0]<< " "<<  "Y coordinate: "<<all_data[5][i][0][1]<<std::endl;
		_picts [j]._x1 =  all_data[5][i][0][0]; _picts [j]._y1 =all_data[5][i][0][1];
		_picts [j]._x2 = all_data[5][i][0][0]; _picts [j]._y2 = (all_data[5][i][0][1]+2);	

		_picts [j]._ntex = 0; 

	}

	
	for(int j=(all_data[4].size()+all_data[5].size()); j<static_cast<int>((all_data[4].size()+all_data[5].size()+all_data[6].size())); j++){
		//char	tmp [128];
		//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		int i = j-(all_data[4].size()+all_data[5].size());
		//std::cout<<"X coordinates: "<<all_data[6][i][0][0]<< " "<<  "Y coordinate: "<<all_data[6][i][0][1]<<std::endl;
		_picts [j]._x1 =  all_data[6][i][0][0]; _picts [j]._y1 =all_data[6][i][0][1];
		_picts [j]._x2 = (all_data[6][i][0][0]+2); _picts [j]._y2 = all_data[6][i][0][1];	
		//std::cout<<"J: "<<j<<std::endl;
		char	tmp [128];
		sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		//_picts [j]._ntex = 0; 
		_picts [j]._ntex = wall_texture (tmp);	
	}
	
	for(int j=all_data[4].size()+all_data[5].size()+all_data[6].size(); j<static_cast<int>((all_data[4].size()+all_data[5].size()+all_data[6].size()+all_data[7].size())); j++){
		//char	tmp [128];
		//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		int i = j-(all_data[4].size()+all_data[5].size()+all_data[6].size());
		//std::cout<<"Car X coordinates: "<<all_data[7][i][0][0]<< " "<<  " Car Y coordinate: "<<all_data[7][i][0][1]<<std::endl;
		_picts [j]._x1 =  all_data[7][i][0][0]; _picts [j]._y1 =all_data[7][i][0][1];
		_picts [j]._x2 = all_data[7][i][0][0]; _picts [j]._y2 = (all_data[7][i][0][1]+2);	
		//std::cout<<"J: "<<j<<std::endl;
		char	tmp [128];
		sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		//_picts [j]._ntex = 0; 
		_picts [j]._ntex = wall_texture (tmp);	
	}
	

	int heightValue=0; //max y value
	int widthValue=0; //max x value

	for (int i=0; i< _nwall; i++){
		_walls [i]._x1 = all_data[0][i][0][0]; _walls [i]._y1 = all_data[0][i][0][1]; //start point
		_walls [i]._x2 = all_data[0][i][1][0]; _walls [i]._y2 = all_data[0][i][1][1]; //end point
		_walls [i]._ntex = 0;
		//if (i==4){
		//	std::cout << "First wall: start x:" << all_data[0][i][0][0] <<  " start y: " << all_data[0][i][0][1] << "  end x: " << all_data[0][i][1][0] << "  end y: " << all_data[0][i][1][1] << std::endl;
		//}

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


	// 6. cr�er la matrice repr�sentant le sol du labyrinthe.

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
	

	//draw horizontal lines
    for (int i=0; i < static_cast<int>(all_data[0].size()); i++){
		//std::cout<<"Wall nr "<< i<<std::endl;
    	int x1 = all_data[0][i][0][0];
        int y1 = all_data[0][i][0][1];
        int x2 = all_data[0][i][1][0];
        int y2 = all_data[0][i][1][1];

        for (int y = y1; y <= y2; ++y) {
            for (int x = x1; x <= x2; ++x) {
                _data[x][y] = 1;
            }
        }
    }


	for(int i=0; i<_nboxes; i++){
		//std::cout<<"Box coordiante: "<< all_data[3][i][0][0] <<" "<<all_data[3][i][0][1]<<std::endl;
		_boxes[i]._x=all_data[3][i][0][0]; _boxes[i]._y=all_data[3][i][0][1]; _boxes [i]._ntex = 0;
		_data [_boxes [i]._x][_boxes [i]._y] = 1 ;
	}

	// 8. le tr�sor.
	_treasor._x = all_data[8][0][0][0];
	_treasor._y = all_data[8][0][0][1];	
	// 8.1 indiquer l'emplacement du tr�sor au sol.
	_data [_treasor._x][_treasor._y] = 1;

	// 9. allouer le chasseur et les 4 gardiens.
	//std::cout<<"Number of Guards: "<<all_data[1].size()<<std::endl;
	_nguards = all_data[1].size() + 1; //get the number of guards
	_guards = new Mover* [_nguards];
	//std::cout<<"start pos chasseur: "<<all_data[2][0][0][0]<<" "<<all_data[2][0][0][1]<<std::endl;
	_guards [0] = new Chasseur (this); _guards [0] -> _x=scale*static_cast<float>(all_data[2][0][0][0]); _guards [0] ->_y=scale*static_cast<float>(all_data[2][0][0][1]);
	_data [(int)(_guards [0] -> _x/scale)][(int)(_guards [0] -> _y/scale)] = 1;
	
	for (int g = 1; g < _nguards; g++){
		std::cout<<g << monster[g-1]<<std::endl;
		_guards[g] = new Gardien (this, monster[g-1], 200); _guards[g] -> _x = scale*static_cast<float>(all_data[1][g-1][0][0]); _guards[g] -> _y = scale*static_cast<float>(all_data[1][g-1][0][1]);

		_data [(int)(_guards[g]->_x/scale)][(int)(_guards[g]->_y/scale)] = 1;

	}

}


void Labyrinthe::set_data (int i, int j, char value) {
		if (i >= 0 && i < this->width() && j >= 0 && j < this->height()) 
		{
			this->_data[i][j] = value;
		}
		
}