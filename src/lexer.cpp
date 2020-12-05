#include"lexer.h"
 
string TextLexer::obrisiKomentare(string line) {

string pocetak="#";  

size_t pocetakKomentara=line.find_first_of(pocetak);
if(pocetakKomentara!=string::npos) {
    line.erase(pocetakKomentara);
}
return line;


}

vector<string> TextLexer::tokeni(string line) {


  vector<string> ret;
   
    string word="";
   
    for(auto x:line) {
  
     if(x!=' ' && x!='\t' && x!='\f' && x!='\n' 
      && x!='\r' && x!=',' && x!=':' && x!='+' && x!='-') {
        word=word+x;   
      
      }
      else  if(x=='+' || x=='-') {
      if(word!="") {
        
           ret.push_back(word); 
           word="";
         
           }
            word+=x; 
      
      }
      else if(x==':') {
         if(word!="") {
           word=word+x;
           ret.push_back(word);
          
         }  
          else { 
             
          if(ret.size()>0) ret[ret.size()-1]+=":";
        }
        word="";   
      }
      else {
     
        if(word=="+" || word=="-") continue;
        if(word!="")  ret.push_back(word); //ovde da proverimo ako word.size-1!=, da nam je onda greska, ali ne to zapravo ne moze jer bi onda prijavio gresku za instr
        
        word="";   
      }
    }
    if(word!="")
    ret.push_back(word);
   
  
   return ret;
}