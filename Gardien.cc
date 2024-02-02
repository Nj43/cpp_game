#include "Gardien.h"
#include <stdio.h>
#include "Mover.h"
#include "Chasseur.h"
#include "Labyrinthe.h"



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

bool Gardien::move(double dx, double dy){
    
}