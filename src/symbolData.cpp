 #include"symbolData.h"

int Symbol::RB=0; 

Symbol::Symbol(string ime, 
int brojSekcije, char flagLG,
int locationCounter, bool defined) {
    this->ime=ime;
    this->brojSekcije=brojSekcije;
    this->flagLG=flagLG;
    this->defined=defined;
    this->vrednost=locationCounter;
    this->redniBrojSimbola=RB++;
}


Symbol::Symbol(string ime) {
    this->ime=ime;
    this->vrednost=0;
    this->flagLG='l';
    this->redniBrojSimbola=RB++;
    this->brojSekcije=this->redniBrojSimbola;
    this->defined=true;
} //konstruktor za sekcije


void Symbol::setFlink(Link bp) {
    this->flink.push_back(bp);
}



Symbol Symbol::getSymbolByName(vector<Symbol> tabelaSimbola,string name){
    
    Symbol s1=Symbol();
    for(int i=0;i<tabelaSimbola.size();i++) 
        if(tabelaSimbola[i].getNaziv()==name) 
        return tabelaSimbola[i];
       
         return  s1;
       
              
    
}

  
