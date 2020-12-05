#ifndef INDEXKLASIHIKACIJE_H
#define INDEXKLASIFIKACIJE_H
#pragma once
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class IndexKlasifikacije
{

public:
  struct Element
  {

   string simbol;
    char znakSimbola;

     Element(string simb, char znak)
    {
        simbol = simb;
      znakSimbola = znak;
     }
  };
  string imeSimbola;
  int vrednost = 0;
  vector<Element> izraz;
  IndexKlasifikacije(string imeSimbola) { this->imeSimbola = imeSimbola; this->vrednost=0; }
  void dodajElement(Element e) { izraz.push_back(e); }
  void dodajVrednost(int broj) { vrednost += broj; }
  void racunanjeIndexaKlasifikacije();
  string getImeSimbola() { return imeSimbola; }
};

#endif