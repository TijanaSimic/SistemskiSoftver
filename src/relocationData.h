#ifndef RELOCATIONDATA_H
#define RELOCATIONDATA_H
#pragma once

#include <string>
#include<iostream>
using namespace std;

class RelocationData
{

    int adresa;
    int vrednost;
    string tip;

public:
    RelocationData(){};
    RelocationData(int adresa, string tip, int vrednost) {
         this->adresa=adresa;
    this->vrednost=vrednost;
    this->tip=tip;
    }

    int getAdresa() {return adresa;}
    int  getVrednost() {return vrednost;}
    void setVrednost(int vrednost) {this->vrednost=vrednost;}

    friend ostream &operator<<(ostream &it, const RelocationData &s)
    {
        it << "adresa: " <<hex<< s.adresa <<dec<< " | tip: " << s.tip << " | vrednost: " << s.vrednost<<endl;
        return it;
    }
         
};

#endif