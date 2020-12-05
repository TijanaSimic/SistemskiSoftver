#ifndef ASEMBLER_H
#define ASEMBLER_H
#pragma once

#include "indexKlasifikacije.h"
#include "symbolData.h"
#include <string>
#include <vector>
#include "relocationData.h"
#include "lexer.h"
#include <fstream>
#include <unordered_map>
#include "instructionParser.h"
#include "section.h"
#include "link.h"
#define R_X86_64_32 "R_X86_64_32"
#define R_X86_64_PC32 "R_X86_64_PC32"

using namespace std;

class Asembler
{

    vector<Symbol> tabelaSimbola;
    TextLexer textLexer;
    vector<Section> sekcije;
    Section tekucaSekcija;

public:
    ofstream outputFile;
    int locationCounter;
    unordered_map<string, int> instrukcije;
    unordered_map<string, int> adresiranja;

    vector<string> globalniSimboli;
    vector<string> externiSimboli;
    //    vector<string>equSimboli;
    vector<IndexKlasifikacije> equSimboli;

    Asembler(vector<Symbol> st, TextLexer t, string outputFile);
    void obradaLinijeKoda(string line);
    void ispisiTabeluSimbola();

    void setTekucaSekcija(Section sekcija) { this->tekucaSekcija = sekcija; }

    Section getTekucaSekcija() { return this->tekucaSekcija; }

    char localGlobalExtern(string name);
    void ispis();
 
    vector<string> bajtoviIspis(int brojBajtova, int vrednost, bool hexa);

    void racunanjeIndexaKlasifikacije();

    void obradaSimbola(string s, int brojBajtova, bool pcRel,int pomeraj);
    void obradaLiterala(string s, int brojBajtova);
    void obradaDeskriptora(int vrednost);

    void razresavanjeSimbola(bool brisanjeRelokacija);  
    vector<string> saberi(vector<string> a, vector<string> b);

    void instrukcijskiDesktiptor(string instrukcija, vector<Operand *> operandi);
    int velicinaOperanda(string instrukcija, vector<Operand *> operandi);

    void deskriptorOperanda(Operand *operand);
};

#endif