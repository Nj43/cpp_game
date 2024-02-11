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

	//After reading in all of the hoizontal lines and creating a map with only the verticals left, 
	//This means we need to iterate over the columns
	//std::cout<<lineCounter<<" "<<longestRow<<std::endl;
	std::vector<std::string> transposedMap=transposeMatrix(lines, lineCounter, longestRow);
	//printMatrix(transposedMap);
	//std::cout<<transposedMap[0]<<std::endl;

	for (size_t columnIndex=0; columnIndex < transposedMap.size(); columnIndex++){
		size_t columnPos = 0;
		int columnPlusIndex = 1;
		std::vector<std::vector<int>> columnPlusIndices;

		//std::cout<<"here"<<std::endl;
		while ((columnPos = transposedMap[columnIndex].find('+', columnPos)) != std::string::npos) {
			//std::cout<<"over here"<<std::endl;
			columnPlusIndices.push_back({static_cast<int>(columnIndex),static_cast<int>(columnPos)});
		    //std::cout<<line<<std::endl;
		    //std::cout<<plusIndex<<std::endl;
		    //std::cout<<pos<<std::endl;
		    //std::cout<<plusIndex<<std::endl;
		    //std::cout<<"twoPlusCounter: "<<twoPlusCounter<<std::endl;
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

	//for(i=0; i<longestRow; i++){

	//}
	//std::cout<< "First guard position x: " << guards[0][0][0] << "First guard position y: " << guards[0][0][1] << std::endl;
	//std::cout << "First wall: start x:" << Walls[4][0][0] <<  " start y: " << Walls[4][0][1] << "  end x: " << Walls[4][1][0] << "  end y: " << Walls[4][1][1] << std::endl; //this way we can access the lines
	//std::cout << horizontalWalls[0] << std::endl;

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

	// la deuxi�me � une texture diff�rente.
	_picts [1]._x1 = 8; _picts [1]._y1 = 0;
	_picts [1]._x2 = 10; _picts [1]._y2 = 0; 

	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	_picts [1]._ntex = wall_texture (tmp);

	// 4. placer les caisses.
	_boxes [0]._x = 70; _boxes [0]._y = 12; _boxes [0]._ntex = 0; 
	_boxes [1]._x = 10; _boxes [1]._y = 5; _boxes [1]._ntex = 0; 

	// mettre une autre texture � la troisi�me caisse.
	sprintf (tmp, "%s/%s", texture_dir, "boite.jpg");
	_boxes [2]._x = 65; _boxes [2]._y = 22; _boxes [2]._ntex = wall_texture (tmp);

	// 5. mettre deux marques au sol.
	_marks [0]._x = 50; _marks [0]._y = 14;
	sprintf (tmp, "%s/%s", texture_dir, "p1.gif");
	_marks [0]._ntex = wall_texture (tmp);

	_marks [1]._x = 20; _marks [1]._y = 15;
	sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
	_marks [1]._ntex = wall_texture (tmp);


	// 6. cr�er la matrice repr�sentant le sol du labyrinthe.

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

	// 8. le tr�sor.
	_treasor._x = 10;
	_treasor._y = 10;
	// 8.1 indiquer l'emplacement du tr�sor au sol.
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
	//std::cout<<"Total number of walls: "<<all_data[0].size()<<std::endl;
	//std::cout << "First wall: start x:" << all_data[0][4][0][0] <<  " start y: " << all_data[0][4][0][1] << "  end x: " << all_data[0][4][1][0] << "  end y: " << all_data[0][4][1][1] << std::endl;
	_nwall = all_data[0].size(); 
	_walls = new Wall [_nwall];

	// deux affiches.
	//_npicts = all_data[4].size(); 
	//_picts = new Wall [_npicts];

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
	//std::cout<<"All data 4 size (pictures x)"<<all_data[4].size()<<std::endl;
	//std::cout<<"All data 5 size (pictures y)"<<all_data[5].size()<<std::endl;
	//std::cout<<"All data 6 size (pictures x)"<<all_data[6].size()<<std::endl;
	//std::cout<<"All data 7 size (pictures y)"<<all_data[7].size()<<std::endl;
	
	for(int i=0; i<static_cast<int>(all_data[4].size()); i++){
		//char	tmp [128];
		//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		//std::cout<<"X coordinates: "<<all_data[4][i][0][0]<< " "<< all_data[4][i][0][0]+2<< "Y coordinate: "<<all_data[4][i][0][1]<<std::endl;
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
		//std::cout<<"J: "<<j<<std::endl;
		//char	tmp [128];
		//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
		_picts [j]._ntex = 0; 
		//_picts [i]._ntex = wall_texture (tmp);	
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
	//std::cout<<"Size of pics"
	//_picts [0]._x1 = 4; _picts [0]._y1 = 0;
	//_picts [0]._x2 = 6; _picts [0]._y2 = 0; 
	//_picts [0]._ntex = 0; 

	// la deuxi�me � une texture diff�rente.
	//_picts [0]._x1 = 8; _picts [0]._y1 = 0;
	//_picts [0]._x2 = 10; _picts [0]._y2 = 0; 

	//char	tmp [128];
	//sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	//_picts [0]._ntex = wall_texture (tmp);



	//_boxes [0]._x = 0; _boxes [0]._y = 12; _boxes [0]._ntex = 0; 
	//_boxes [1]._x = 10; _boxes [1]._y = 5; _boxes [1]._ntex = 0; 



	// mettre une autre texture � la troisi�me caisse.
	//sprintf (tmp, "%s/%s", texture_dir, "boite.jpg");
	//_boxes [2]._x = 40; _boxes [2]._y = 10; _boxes [2]._ntex = wall_texture (tmp);

	// 5. mettre deux marques au sol.
	//_marks [0]._x = 20; _marks [0]._y = 14;
	//sprintf (tmp, "%s/%s", texture_dir, "p1.gif");
	//_marks [0]._ntex = wall_texture (tmp);

	//_marks [1]._x = 20; _marks [1]._y = 15;
	//sprintf (tmp, "%s/%s", texture_dir, "p3.gif");
	//_marks [1]._ntex = wall_texture (tmp);

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

        //std::cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<" "<<std::endl;
        for (int y = y1; y <= y2; ++y) {
            for (int x = x1; x <= x2; ++x) {
                _data[x][y] = 1;
            }
        }
    }

	//printArray(_data, width () , height ());
	// 7.1 indiquer l'emplacement des caisses au sol.

	//_data [_boxes [0]._x][_boxes [0]._y] = 1;
	//_data [_boxes [1]._x][_boxes [1]._y] = 1;
	//_data [_boxes [2]._x][_boxes [2]._y] = 1;

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
		//std::cout<<static_cast<float>(all_data[1][g-1][0][0])<<std::endl;
		//std::cout<<static_cast<float>(all_data[1][g-1][0][1])<<std::endl;
		//std::cout<<"Scale: "<<scale<<std::endl;
		if (g%2==0){
			_guards[g] = new Gardien (this, "Serpent"); _guards[g] -> _x = scale*static_cast<float>(all_data[1][g-1][0][0]); _guards[g] -> _y = scale*static_cast<float>(all_data[1][g-1][0][1]);
		}
		else{
			_guards[g] = new Gardien (this, "Samourai"); _guards[g] -> _x = scale*static_cast<float>(all_data[1][g-1][0][0]); _guards[g] -> _y = scale*static_cast<float>(all_data[1][g-1][0][1]);
		}
		//std::cout<<(int)(_guards[g])->_x<<std::endl;
		//std::cout<<(int)(_guards[g])->_y<<std::endl;
		_data [(int)(_guards[g]->_x/scale)][(int)(_guards[g]->_y/scale)] = 1;
		//std::cout<<(int)(_guards [g] -> _x )<<std::endl;
		//std::cout<<(int)(_guards [g] -> _y )<<std::endl;
	}
	//printArray(_data, width () , height ());
	std::cout<<"Width: "<<width()<<std::endl;
	std::cout<<"Height: "<<height()<<std::endl;

	//std::cout<<(int)(_guards[1] -> _x) <<std::endl;
	//std::cout<<static_cast<float>(all_data[1][0][0][1])<<std::endl;
	//_guards [1] = new Gardien (this, "Lezard");_guards [1] -> _x = static_cast<float>(all_data[1][0][0][0]); _guards [1] -> _y = static_cast<float>(all_data[1][0][0][1]);
	
	
	//_guards [2] = new Gardien (this, "Blade"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	//_guards [3] = new Gardien (this, "Serpent"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	//_guards [4] = new Gardien (this, "Samourai"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;

	// 9.1 indiquer l'emplacement des gardiens au sol.
	//_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale )] = 1;
	//std::cout<<(int)(_guards [1] -> _x / scale)<<std::endl;
	//_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	//_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	//_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
	
}
void Labyrinthe::set_data (int i, int j, char value) {
	if (i >= 0 && i < this->width() && j >= 0 && j < this->height()) this->_data[i][j] = value;
}
