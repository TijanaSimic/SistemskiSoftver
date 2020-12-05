#ifndef SYMBOLDATA_H
#define SYMBOLDATA_H
#pragma once

#include "link.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

#define DUZINA_SEKCIJE 0 //ako jos nije izracunata

class Symbol
{

    static int RB;

protected:
    string ime;
    int brojSekcije;
    int vrednost;
    char flagLG;
    int redniBrojSimbola;
    bool defined;

public:
    vector<Link> flink;
    Symbol(string ime, int brojSekcije,
           char flagLG, int locationCounter, bool defined);
    void setFlink(Link bp);
    vector<Link> getFlink() { return flink; }
    Symbol(string ime);
    Symbol(){};
    Symbol(string ime, char flagLC, bool defined);

    string getNaziv() { return this->ime; }
    int getBrojSekcijeDefinisan() { return this->brojSekcije; } //broj sekcije u kojoj je definisan
    int getRedniBrojSimbola() { return this->redniBrojSimbola; }
    bool getDefined() { return defined; }
    void setBrojSekcije(int broj) { brojSekcije = broj; }
    void setVrednost(int broj) { vrednost = broj; }
    char getLocalGlobalFlag() { return flagLG; }
    void setDefined(bool defined) { this->defined = defined; }
    static Symbol getSymbolByName(vector<Symbol> tabelaSimbola, string name);
    void setFlag(char lc) { flagLG = lc; }
    int getTekucaVrednostSimbola() { return vrednost; }
    friend ostream &operator<<(ostream &it, const Symbol &s)
    {
        it << "ime: " << s.ime << " | brojSekcije: " << s.brojSekcije << " | vrednost: " << hex << s.vrednost << dec << " | flag: " << s.flagLG << " | rb: " << s.redniBrojSimbola<<" ";
        
        return it;
    }
};

#endif