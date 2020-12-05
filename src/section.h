#ifndef SECTION_H
#define SECTION_H
#pragma once


#include<vector>
#include<string>
#include"relocationData.h"
#include"symbolData.h"
using namespace std;

class Section:public Symbol {

int duzinaSekcije;

public:

vector<string> izlazniNiz;
vector<RelocationData> tabelaRelokacija;
 
void setRelocationData(RelocationData relocationData) {
        this->tabelaRelokacija.push_back(relocationData);
};

void setDuzinaSekcije(int duzina) {this->duzinaSekcije=duzina;}
int getDuzinaSekcije() {return duzinaSekcije;}

Section(string imeSekcije):Symbol(imeSekcije) {}

Section(){};


 
};





#endif