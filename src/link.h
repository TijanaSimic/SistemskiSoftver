#ifndef LINK_H
#define LINK_H
#pragma once

#include <string>
#include<iostream>
using namespace std;

class Link
{ //link pa onda flink i rlink
private:
    int adresa;
    int brojSekcije;
    bool relocationFlag; //da li treba prepraviti ovaj simbol u tabeli relokacija
    //char znak; //+ ili - MISLIM DA NAM OVO SAD NE TREBA
    //imati i polje za tabelu relokacija, koja polja se menjaju
    int brojBajtova; //broj bajtova koje treba prepraviti
    bool pcRel=false;
    bool equ=false;

public:
    Link(int adresa, int brojSekcije, bool rel, int brojBajtova)
    {   this->pcRel=false;
        this->adresa = adresa;
        this->brojSekcije = brojSekcije;
        this->relocationFlag = rel;
        this->brojBajtova=brojBajtova;
    }
    bool getEqu() {return equ;}
    void setEqu(bool equ) {this->equ=equ;}
    bool getPcRel() { return pcRel;}
    void setPcRel(bool pcRel) {this->pcRel=pcRel;}
    int getAdresa() {return adresa;}
    int getBrojSekcije() {return brojSekcije;}
    int getBrojBajtova() {return brojBajtova;}
    bool getRelocationFlag() {return relocationFlag;}

    friend ostream &operator<<(ostream &it, const Link &s)
    {
        it <<hex<< "adresa: " << s.adresa <<dec<< " | brojSekcije: " << s.brojSekcije << " | relFlag: " << s.relocationFlag << " | brojBajtova "<<s.brojBajtova<<" |pcrel  "<<s.pcRel<<" | equ "<<s.equ<<endl;
        return it;
    }
};

#endif