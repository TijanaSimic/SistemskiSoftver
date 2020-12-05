#include "asembler.h"
#include <bits/stdc++.h>
using namespace std;

Asembler::Asembler(vector<Symbol> st, TextLexer t, string outputFile)
{
  this->tabelaSimbola = st;
  this->textLexer = t;
  this->outputFile.open(outputFile);
  Section nultaSekcija = Section("UND");
  sekcije.push_back(nultaSekcija);
  tabelaSimbola.push_back(nultaSekcija);
  this->locationCounter = 0x0;
  this->tekucaSekcija = nultaSekcija;

  instrukcije = unordered_map<string, int>();
  adresiranja = unordered_map<string, int>();

  adresiranja["immed"] = 0;
  adresiranja["immedL"] = 0;
  adresiranja["regDir"] = 1;
  adresiranja["regInd"] = 2;
  adresiranja["regIndPom"] = 3;
  adresiranja["regIndPomS"] = 3;
  adresiranja["regIndPomL"] = 3;
  adresiranja["mem"] = 4;
  adresiranja["memL"] = 4;

  instrukcije["halt"] = 0;
  instrukcije["iret"] = 1;
  instrukcije["ret"] = 2;
  instrukcije["int"] = 3;
  instrukcije["call"] = 4;
  instrukcije["jmp"] = 5;
  instrukcije["jeq"] = 6;
  instrukcije["jne"] = 7;
  instrukcije["jgt"] = 8;
  instrukcije["push"] = 9;
  instrukcije["pop"] = 10;
  instrukcije["xchg"] = 11;
  instrukcije["mov"] = 12;
  instrukcije["add"] = 13;
  instrukcije["sub"] = 14;
  instrukcije["mul"] = 15;
  instrukcije["div"] = 16;
  instrukcije["cmp"] = 17;
  instrukcije["not"] = 18;
  instrukcije["and"] = 19;
  instrukcije["or"] = 20;
  instrukcije["xor"] = 21;
  instrukcije["test"] = 22;
  instrukcije["shl"] = 23;
  instrukcije["shr"] = 24;
}

//OBRADITI SLUCAJ DA NAM SE U NULTOJ SEKCIJI NE SME NISTA BITI DEFINISANO

char Asembler::localGlobalExtern(string name)
{

  for (int i = 0; i < globalniSimboli.size(); i++)
  {
    if (globalniSimboli[i] == name)
      return 'g';
  }
  for (int i = 0; i < externiSimboli.size(); i++)
  {
    if (externiSimboli[i] == name)
      return 'e';
  }
  return 'l';
}

vector<string> Asembler::bajtoviIspis(int brBajtova, int vrednost, bool hexa)
{
  //negativan broj ide samo na 4 b a ostale u nule

  stringstream ss; //ako dobijemo skip 0xA da ga pretvorimo u int
  ss << brBajtova;
  string broj = ss.str();

  int brojBajtova = stoi(broj);

  stringstream ss1;
  ss1 << hex << vrednost;
  string resHexa = ss1.str();

  try
  {
    if (hexa)
    {
      if (brojBajtova == 1 && resHexa.size() > 2 && vrednost > 0)
        throw "Prekoracenje dozvoljenog opsega.";
      else if (brojBajtova == 2 && resHexa.size() > 4 && vrednost > 0)
        throw "Prekoracenje dozvoljenog opsega.";
    }
    else
    {
      if (brojBajtova == 1 && (vrednost > 127 || vrednost < -128))
        throw "Prekoracenje dozvoljenog opsega";
      else if (brojBajtova == 2 && (vrednost > 32767 || vrednost < -32768))
        throw "Prekoracenje dozvoljenog opsega";
    }
  }
  catch (const char *e)
  {
    cout << e;
    exit(1);
  }

  while (resHexa.size() < brojBajtova * 2) //ako mi je broj bajtova veci od 4 na koje moze hex da predstavi negativan broj ili od predstave pozitivnog broja possto pozitivan predstavi samo 10=A itd
  {
    string pom = resHexa;
    if (hexa)
    {
    }
    if (vrednost >= 0)
      resHexa = "0"; //dodajem 0 ili f
    else
      resHexa = "f";
    resHexa += pom;
  }

  vector<string> stringovi;
  if (resHexa.size() > 0)
    for (int i = 0, j = resHexa.size() - 1; i < resHexa.size() / 2; i = i + 2, j = j - 2)
    {
      swap(resHexa.at(i), resHexa.at(j - 1));
      swap(resHexa.at(i + 1), resHexa.at(j));
    }

  if (resHexa.size() > 1)
    for (int i = 0; i < resHexa.size(); i = i + 2)
    {
      string s = "";
      s += resHexa.at(i);
      s += resHexa.at(i + 1);

      stringovi.push_back(s);
    }

  if (stringovi.size() > brojBajtova)
    for (int i = stringovi.size() - brojBajtova; i > 0; i--)
      stringovi.pop_back();
 
  return stringovi;
}

void Asembler::obradaLinijeKoda(string line)
{

  line = textLexer.obrisiKomentare(line);
  vector<string> tokeni = textLexer.tokeni(line);

  for (int i = 0; i < tokeni.size(); i++)
  {
    string token = tokeni[i];

    if (token == ".global" || token == ".GLOBAL" || token == ".extern" || token == ".EXTERN")
    {

      try
      {
        if (tekucaSekcija.getRedniBrojSimbola() != 0)
          throw ".global ili .extern se moraju definisati u nultoj sekciji.";
        if (tokeni.size() > 1)
        {
          for (int j = 1; j < tokeni.size(); j++)
          {
            if (token == ".global" || token == ".GLOBAL")
            {
              char localGlobal = localGlobalExtern(tokeni[j]);
              if (localGlobal == 'e')
                throw "Simbol ne moze biti i globalni i eksterni.";
              if (regex_match(tokeni[j], regex("\\d+")))
                throw ".global simbol ne moze biti literal.";
              this->globalniSimboli.push_back(tokeni[j]);
            }
            else
            {
              char localGlobal = localGlobalExtern(tokeni[j]);
              if (localGlobal == 'g')
                throw "Simbol ne moze biti i globalni i eksterni.";
              if (regex_match(tokeni[j], regex("\\d+")))
                throw ".extern simbol ne moze biti literal.";
              Symbol noviSimbol = Symbol(tokeni[j], 0,
                                         'g', 0, true); //za eksterne stavljamo defined=1, brSekcije 0,flag g i vrednost 0
              tabelaSimbola.push_back(noviSimbol);
              this->externiSimboli.push_back(tokeni[j]);
            }
          }
          break;
        }
        else
          throw ".global direktiva zahteva bar jedan simbol.";
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
    }

    else if (token == ".section")
    {
      try
      {
        if (tokeni.size() -i !=2)
        {
          throw "Pogresan izgled tekstualnog fajla";
        }
        else
        {
          if (tekucaSekcija.getNaziv() != sekcije[0].getNaziv())
          {

            tekucaSekcija.setDuzinaSekcije(locationCounter);
            sekcije.push_back(tekucaSekcija);
          }
          if (regex_match(tokeni[1], regex("^\\S+")))
          {
            vector<Link>flinkovi;
            for(int p=0;p<tabelaSimbola.size();p++) {
              if(tabelaSimbola[p].getNaziv()==tokeni[i+1]) {
                
                for(int f=0;f<tabelaSimbola[p].flink.size();f++)
                  flinkovi.push_back(tabelaSimbola[p].flink[f]);
                tabelaSimbola.erase(tabelaSimbola.begin()+p--);
              }
            }
            Section s = Section(tokeni[i+1]);
            if(flinkovi.size()>0) 
            for(int f=0;f<flinkovi.size();f++)
            s.setFlink(flinkovi[f]);
            tekucaSekcija = s;
 
            locationCounter = 0x0;
            tabelaSimbola.push_back(s);
          }
          else
            throw "Pogresan naziv sekcije.";
        }
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
      break;
    }
    else if (token == ".end" || token == ".END")  
    {                                           
       
      if (tekucaSekcija.getNaziv() != sekcije[0].getNaziv())
      {

        tekucaSekcija.setDuzinaSekcije(locationCounter);
        sekcije.push_back(tekucaSekcija);
        razresavanjeSimbola(false);

        int brojNedefinisanih = 0;
        for (int j = 0; j < tabelaSimbola.size(); j++)
        {
          if (tabelaSimbola[j].getDefined() == false)
            brojNedefinisanih++;
        }

        racunanjeIndexaKlasifikacije();

        int brojNedefinisanih1 = 0;
        for (int k = 0; k < tabelaSimbola.size(); k++)
          if (tabelaSimbola[k].getDefined() == false)
            brojNedefinisanih1++;
        while (brojNedefinisanih1 < brojNedefinisanih)
        {
          brojNedefinisanih = brojNedefinisanih1;
          racunanjeIndexaKlasifikacije();
          brojNedefinisanih1 = 0;
          for (int o = 0; o < tabelaSimbola.size(); o++)
          {

            if (tabelaSimbola[o].getDefined() == false)
              brojNedefinisanih1++;
          };
        }
        try
        {
                    if (brojNedefinisanih > 0) { 
                  
           
           throw "Postoji kruzna zavisnost izmedju simbola.";
            } 
                
               }
        catch (const char *e)
        {
          cout << e;
          exit(1);
        }
        
      }
    }
    else if (regex_match(token, regex("\\S+:$")))  
    {                                             
      smatch m;
      regex_search(token, m, regex("\\S+:$"));
      for (auto x : m)
      {
        string s = x;

        s.erase(s.length() - 1, 1);
        try
        {
          char localGlobal = localGlobalExtern(s);
          if (localGlobal == 'e')
            throw "Ne moze se definisati eksterni simbol.";
          Symbol sim = Symbol::getSymbolByName(tabelaSimbola, s);
          if (sim.getNaziv() == "") //ako ga nema u tabeli simbola
          {
            Symbol noviSimbol = Symbol(s, tekucaSekcija.getRedniBrojSimbola(),
                                       localGlobal, locationCounter, true);
            tabelaSimbola.push_back(noviSimbol);
          }
          else if (sim.getDefined() == false)
          { 
            for (int i = 0; i < tabelaSimbola.size(); i++)
            {
              if (tabelaSimbola[i].getNaziv() == sim.getNaziv())
              {
                tabelaSimbola[i].setDefined(true);
                tabelaSimbola[i].setBrojSekcije(tekucaSekcija.getRedniBrojSimbola());
                tabelaSimbola[i].setVrednost(locationCounter);
               }
            }
          }
          else
          {
            char c = sim.getDefined() == true ? '1' : '0';
            throw "Labela se ne moze vise puta definisati.";
          }
        }
        catch (const char *e)
        {
          cout << e;
          exit(1);
        }

       }
    } 
    else if (token == ".skip")
    {
      try
      {
        if (tekucaSekcija.getRedniBrojSimbola() == 0)
          throw "Direktive se nalaze u pogresnim sekcijama.";
        if (tokeni.size() - i == 2)
        {
          vector<string> bajtovi;
          if (regex_match(tokeni[i + 1], regex("(\\d+)")))
          {
            bajtovi = bajtoviIspis(stoi(tokeni[i + 1]), 0, false);
          }
          else if (regex_match(tokeni[i + 1], regex("0x[0-9ABCDEFabcdef]+")))
            bajtovi = bajtoviIspis(stoi(tokeni[i + 1], 0, 16), 0, true);
          for (int i = 0; i < bajtovi.size(); i++)
          {
            tekucaSekcija.izlazniNiz.push_back(bajtovi[i]);
          }
          locationCounter += bajtovi.size();
        }
        else
          throw "Neregularna direktiva .skip.";
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
      break;
    }
    else if (token == ".byte" || token == ".word")
    {
      try
      {
        if (tekucaSekcija.getRedniBrojSimbola() == 0)
          throw "Direktive se nalaze u pogresnim sekcijama.";

        if (tokeni.size() - i > 1)
        {
          for (int j = i + 1; j < tokeni.size(); j++)
          {  
            if (regex_match(tokeni[j], regex("[+-]{0,1}\\d+")) || regex_match(tokeni[j], regex("[+-]{0,1}0x[0-9ABCDEFabcdef]+")))
            {
              obradaLiterala(tokeni[j], token == ".byte" ? 1 : 2);
              locationCounter += (token == ".byte") ? 1 : 2;
            }
            else if (regex_match(tokeni[j], regex("\\S+")))
            {
               obradaSimbola(tokeni[j], token == ".byte" ? 1 : 2, false, 0);
              locationCounter += (token == ".byte") ? 1 : 2;
            }
            else
              throw "Pogresan oblik direktive.";
          }
        }
        else
          throw "Pogresan oblik direktive.";
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }

      break;
    }
    else if (token == ".equ" || token == ".EQU")
    {
      try
      {
        if (tekucaSekcija.getRedniBrojSimbola() != 0)
          throw ".equ direktiva nije u odgovarajucoj sekciji.";
        if (tokeni.size() < 2)
          throw ".equ direktiva nije dobro definisana.";
        else
        {  string nazivSimbola = tokeni[1];
          IndexKlasifikacije equSimbol = IndexKlasifikacije(nazivSimbola);
          Symbol noviSimbol = Symbol(nazivSimbola, 0,
                                     'u', 0, false); //za eksterne stavljamo defined=1, brSekcije 0,flag g i vrednost 0
          tabelaSimbola.push_back(noviSimbol);
          int vrednostEquSimbola = 0;
          for (int j = 2; j < tokeni.size(); j++)
          {
            char znak = tokeni[j].at(0) == '-' ? '-' : '+';
            if (tokeni[j].at(0) == '+' || tokeni[j].at(0) == '-')
              tokeni[j].erase(0, 1);
            if (regex_match(tokeni[j], regex("\\d+")))
            {
              if (znak == '+')
                vrednostEquSimbola += stoi(tokeni[j]);
              else
                vrednostEquSimbola -= stoi(tokeni[j]);
              continue;
            }
            else if (regex_match(tokeni[j], regex("0x[0-9a-fA-f]+")))
            {
               if (znak == '+')
                vrednostEquSimbola += stoi(tokeni[j], 0, 16);
              else
                vrednostEquSimbola -= stoi(tokeni[j], 0, 16);
              continue;
            }

            equSimbol.dodajElement(IndexKlasifikacije::Element(tokeni[j], znak));
          }
          equSimbol.dodajVrednost(vrednostEquSimbola);
           equSimboli.push_back(equSimbol);
           break;
        }
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
    }
    else if (regex_match(token, regex("^(jmp)|(jne)|(jeq)|(jgt)[w]{0,1}$")))
    {

      try
      {
        if (tokeni.size() - i != 2)
          throw "Pogresno definisana istrukcija skoka.";
        vector<Operand *> operandi;
        int j = i + 1;
        Operand *op = tipAdresiranja(token, tokeni[j]);
        operandi.push_back(op);

        instrukcijskiDesktiptor(token, operandi);
        locationCounter++;
        deskriptorOperanda(operandi[0]);
        locationCounter++;
        if (op->tipAdresiranja == "immedL" || op->tipAdresiranja == "regIndPomL" || op->tipAdresiranja == "memL")
        {
          obradaLiterala(op->literal, 2); //skokovi su nam uvek na 2B jer je to adresa
          locationCounter += 2;
        }
        if (op->tipAdresiranja == "mem" || op->tipAdresiranja == "immed" || op->tipAdresiranja == "regIndPomS")
        {
          obradaSimbola(op->simbol, 2, false, 0);
          locationCounter += 2;
        }
        if (op->tipAdresiranja == "regIndPom")
        {
          obradaSimbola(op->simbol, 2, true, 2); //ZA POMERAJ SALJEMO POZITIVAN DVA PA GA ON DOLE ODUZIMA;
          locationCounter += 2;
        } // a ako dodje r7simbol to je pcrel
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
      break;
    }

    else if (regex_match(token, regex("^(halt)|(ret)|(iret)[w]{0,1}$")))
    {
      try
      {
        if (tokeni.size() - i != 1)
          throw "Pogresan broj operanada.";
        vector<Operand *> operandi;
        instrukcijskiDesktiptor(token, operandi);
        locationCounter++;
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
    }

    else if (regex_match(token, regex("^(int)|(push)|(pop)[w]{0,1}$")))
    { //stavila sam da int ne moze da bude B jer se u pc upisuje mem od 16
      try
      {
        if (tokeni.size() - i != 2)
          throw "Pogresno definisana istrukcija.";
        vector<Operand *> operandi;
        int j = i + 1;
        Operand *op = tipAdresiranja(token, tokeni[j]);
        operandi.push_back(op);

        instrukcijskiDesktiptor(token, operandi);
        locationCounter++;
        deskriptorOperanda(operandi[0]);
        locationCounter++;
        if (regex_match(token, regex("^pop[w]{0,1}$")) &&
            (op->tipAdresiranja == "immedL" || op->tipAdresiranja == "immed"))
          throw "Pogresno adresiranje uz instrukciju pop.";
        if (op->tipAdresiranja == "immedL" || op->tipAdresiranja == "regIndPomL" || op->tipAdresiranja == "memL")
        {
          obradaLiterala(op->literal, 2);  
          locationCounter += 2;
        }
        if (op->tipAdresiranja == "mem" || op->tipAdresiranja == "immed" || op->tipAdresiranja == "regIndPomS")
        {
          obradaSimbola(op->simbol, 2, false, 0);
          locationCounter += 2;
        }
        if (op->tipAdresiranja == "regIndPom")
        {
          obradaSimbola(op->simbol, 2, true, 2); //ZA POMERAJ SALJEMO POZITIVAN DVA PA GA ON DOLE ODUZIMA;
          locationCounter += 2;
        }
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
      break;
    }

    else if (regex_match(token, regex("^call[w]{0,1}$")))
    {

      try
      {
        if (tokeni.size() - i != 2)
          throw "Pogresno definisana istrukcija call.";
        vector<Operand *> operandi;
        int j = i + 1;
        Operand *op = tipAdresiranja(token, tokeni[j]);
        operandi.push_back(op);

        instrukcijskiDesktiptor(token, operandi);
        locationCounter++;
        deskriptorOperanda(operandi[0]);
        locationCounter++;
        if (op->tipAdresiranja == "mem")
        {
          obradaSimbola(op->simbol, 2, false, 0);
          locationCounter += 2;
        }
        else
          throw "Pogresan tip adresiranja uz instrukciju call.";
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
      break;
    }
    else if (regex_match(token, regex("^((xchg)|(mov)|(add)|(sub)|(mul)|(div)|(cmp)|(not)|(and)|(or)|(xor)|(test)|(shl)|(shr))([bw]){0,1}")))
    { //stavila sam da int ne moze da bude B jer se u pc upisuje mem od 16
      try
      {
        if (tokeni.size() - i != 3)
          throw "Pogresno definisana istrukcija.";
        vector<Operand *> operandi;
        for (int j = i + 1; j < tokeni.size(); j++)
        {
          Operand *op = tipAdresiranja(token, tokeni[j]);
          operandi.push_back(op);
        }
        bool b = false;
        bool memorijsko = false;
        bool pcRel = false;
        if (token.at(token.size() - 1) == 'b' && token != "sub")
        {
          b = true;
        }

        instrukcijskiDesktiptor(token, operandi);
        locationCounter++;
        for (int j = 0; j < operandi.size(); j++)
        {
          deskriptorOperanda(operandi[j]);
          locationCounter++;
          if (regex_match(token, regex("^xchg[bw]{0,1}$")) &&
              (operandi[j]->tipAdresiranja == "immedL" || operandi[j]->tipAdresiranja == "immed"))
            throw "Pogresno adresiranje uz instrukciju xchg.";
          if (regex_match(token, regex("^(mov)|(add)|(sub)|(mul)|(div)|(not)|(and)|(or)|(xor)|(test)|(shl)|(shr)[bw]{0,1}$")) &&
              (operandi[j]->tipAdresiranja == "immedL" || operandi[j]->tipAdresiranja == "immed") && j == 1)
            throw "Pogresno adresiranje uz aritmeticko-logicku instrukciju.";

          if (operandi[j]->tipAdresiranja == "immedL" ||
              operandi[j]->tipAdresiranja == "regIndPomL" ||
              operandi[j]->tipAdresiranja == "memL")
          {
            if (operandi[j]->tipAdresiranja == "memL")
            {
                if(b)  throw "Memorijsko adresiranje zahteva velicinu instrukcije na 2B.";
              
              if (memorijsko == false)
                memorijsko = true;
                else
                throw "U jednoj instrukciji ne mogu biti dva memorijska adresiranja.";
            }
            if (operandi[j]->tipAdresiranja == "regIndPomL")
            {
              if (b)
                throw "RegIndPom zahteva velicinu instrukcije na 2B.";
              obradaLiterala(operandi[j]->literal, 2);
              locationCounter += 2;
            }
            else
            {
                obradaLiterala(operandi[j]->literal, b == true ? 1 : 2);
              locationCounter += b == true ? 1 : 2;
            }

          }  
          if(operandi[j]->tipAdresiranja == "mem" || operandi[j]->tipAdresiranja == "immed" || operandi[j]->tipAdresiranja == "regIndPomS")
          {

            if (operandi[j]->tipAdresiranja == "mem")
            {
              if(b)  throw "Memorijsko adresiranje zahteva velicinu instrukcije na 2B.";
              
              if (memorijsko == false)
                memorijsko = true;
               
              else
                throw "U jednoj instrukciji ne mogu biti dva memorijska adresiranja.";
            }
            if (operandi[j]->tipAdresiranja == "regIndPomS")
            {
              if (b)
                throw "RegIndPom zahteva velicinu instrukcije na 2B.";
              obradaSimbola(operandi[j]->simbol, 2, false, 0);
              locationCounter += 2;
            }
         else   if (operandi[j]->tipAdresiranja == "mem")
            {
              if (b)
                throw "Memorijsko adresiranje zahteva velicinu instrukcije na 2B.";
              obradaSimbola(operandi[j]->simbol, 2, false, 0);
              locationCounter += 2;
            }
            else
            {
              obradaSimbola(operandi[j]->simbol, b ? 1 : 2, false, 0);
              locationCounter += b ? 1 : 2;
            }
          }

          if (operandi[j]->tipAdresiranja == "regIndPom")
          {
               
            if (b)
              "RegIndPom zahteva velicinu instrukcije na 2B.";
            if (j == 1)
              obradaSimbola(operandi[j]->simbol, 2, true, 2);
            else if (j == 0)
            {
              if (operandi[j + 1]->tipAdresiranja == "regDir" || operandi[j + 1]->tipAdresiranja == "regInd")
                obradaSimbola(operandi[j]->simbol, 2, true, 3);
              else
                obradaSimbola(operandi[j]->simbol, 2, true, 5);
            }
            locationCounter += 2;
          }
        }
      }
      catch (const char *e)
      {
        cout << e;
        exit(1);
      }
      break;
    }

    else {
      try {
        throw "Pogresan oblik tekstualnog fajla.";
      }
      catch(const char *e) {
        cout<<e;
        exit(1);
      }
    }
  }
}

void Asembler::ispis()
{

  outputFile << "#TABELASIMBOLA" << endl;

  for (int i = 0; i < tabelaSimbola.size(); i++)
  {
     for (int j = 0; j < sekcije.size(); j++)
    {
      if (sekcije[j].getNaziv() == tabelaSimbola[i].getNaziv() && i != 0)
      { outputFile << tabelaSimbola[i];
        outputFile << " | velicinaSekcije:" << sekcije[j].getDuzinaSekcije()<<endl;
        break;
      }
    }
   
  }
   for (int i = 0; i < tabelaSimbola.size(); i++)
  { bool pronadjen=false;
     for (int j = 0; j < sekcije.size(); j++)
    {
     if (sekcije[j].getNaziv() == tabelaSimbola[i].getNaziv() && i != 0)
      pronadjen=true;

    }
    if(pronadjen==false)  outputFile << tabelaSimbola[i]<<endl;
    else continue;
  }

  outputFile<<endl;

  for (int i = 1; i < sekcije.size(); i++)
  {
    outputFile << "#rel"<< sekcije[i].getNaziv()<< endl;
    for (int j = 0; j < sekcije[i].tabelaRelokacija.size(); j++)
      outputFile << sekcije[i].tabelaRelokacija[j];
    outputFile<<endl;
   }
   outputFile<<endl;

  for (int i = 1; i < sekcije.size(); i++)
  {
     outputFile <<"#"<<sekcije[i].getNaziv() << endl;
    for (int k = 0; k < sekcije[i].izlazniNiz.size(); k++)
      outputFile << sekcije[i].izlazniNiz[k] << " ";
    outputFile << endl
               << endl;
  }


   
}
void Asembler::obradaDeskriptora(int vrednost)
{

  stringstream ss1;
  ss1 << hex << vrednost;
  string resHexa = ss1.str();
   
  while (resHexa.size() < 2) //ako mi je broj bajtova veci od 4 na koje moze hex da predstavi negativan broj ili od predstave pozitivnog broja possto pozitivan predstavi samo 10=A itd
  {
    string pom = resHexa;

    resHexa = "0"; //dodajem 0 ili f
    resHexa += pom;
  }

  vector<string> stringovi;
  if (resHexa.size() > 0)
    for (int i = 0, j = resHexa.size() - 1; i < resHexa.size() / 2; i = i + 2, j = j - 2)
    {
      swap(resHexa.at(i), resHexa.at(j - 1));
      swap(resHexa.at(i + 1), resHexa.at(j));
    }
   if (resHexa.size() > 1)
    for (int i = 0; i < resHexa.size(); i = i + 2)
    {
      string s = "";
      s += resHexa.at(i);
       s += resHexa.at(i + 1);
     
      stringovi.push_back(s);
    }
  for (int i = 0; i < stringovi.size(); i++)
  {
    tekucaSekcija.izlazniNiz.push_back(stringovi[i]);
      }
}

void Asembler::obradaLiterala(string s, int brojBajtova)
{
   vector<string> bajtovi;
  if (regex_match(s, regex("[+-]{0,1}\\d+")))
    bajtovi = bajtoviIspis(brojBajtova, stoi(s), false);
  else if (regex_match(s, regex("[+-]{0,1}0x[0-9ABCDEFabcdef]+")))
    bajtovi = bajtoviIspis(brojBajtova, stoi(s, 0, 16), true);
  for (int i = 0; i < bajtovi.size(); i++)
  {
     tekucaSekcija.izlazniNiz.push_back(bajtovi[i]);
     
  }
}

void Asembler::obradaSimbola(string s, int brojBajtova, bool pcRel, int pomeraj) //ako saljem za .byte ne bi bilo lose da imam kao argument flag da li je byte ili ne
{
   Symbol sim = Symbol::getSymbolByName(tabelaSimbola, s);
  if (sim.getNaziv() == "") //ako ga nema u tabeli simbola
  {
    char localGlobal = localGlobalExtern(s);
    

    Symbol noviSimbol = Symbol(s, -1, localGlobal, -1, false); //za lokalne i glob nedefinisane stavljamo broj sekcije u kojoj su definisani na -1
                                                               //  if (localGlobal == 'l')
                                                               // {

    Link backpatchingData = Link(locationCounter, tekucaSekcija.getRedniBrojSimbola(),
                                 localGlobal == 'g' ? false : true, brojBajtova); //ako je lokalni mora da se prepravlja u rel tab a ako je lokalni ne
     if (localGlobal == 'l')
    {

      backpatchingData.setPcRel(pcRel);
    }
    for (int b = 0; b < equSimboli.size(); b++)
    {
      if (equSimboli[b].getImeSimbola() == s)
      {
        backpatchingData.setEqu(true);
        break;
      }
    }
    noviSimbol.setFlink(backpatchingData);

    /// }
    tabelaSimbola.push_back(noviSimbol);

    RelocationData relZapis;
    if (pcRel)
      relZapis = RelocationData(locationCounter, R_X86_64_PC32,
                                localGlobal == 'g' ? noviSimbol.getRedniBrojSimbola()
                                                   : -1);
    else
      relZapis = RelocationData(locationCounter, R_X86_64_32,
                                localGlobal == 'g' ? noviSimbol.getRedniBrojSimbola()
                                                   : -1);

    tekucaSekcija.setRelocationData(relZapis);
     

    vector<string> bajtovi = bajtoviIspis(brojBajtova, pcRel == false ? 0 : pomeraj * (-1), false);
    for (int i = 0; i < bajtovi.size(); i++)
      tekucaSekcija.izlazniNiz.push_back(bajtovi[i]);

    }
  else if (sim.getDefined() == false)
  {
    for (int i = 0; i < tabelaSimbola.size(); i++)
    {
      if (tabelaSimbola[i].getNaziv() == sim.getNaziv()) //ako vec postoji u tabeli simbola ali je nedef
      {
        char flagLG = tabelaSimbola[i].getLocalGlobalFlag();
         Link backpatchingData = Link(locationCounter, tekucaSekcija.getRedniBrojSimbola(),
                                     flagLG == 'g' ? false : true, brojBajtova); //ako je lokalni mora da se menja u reltab
        if (flagLG == 'l' || flagLG == 'u')
          backpatchingData.setPcRel(pcRel);
        for (int b = 0; b < equSimboli.size(); b++)
        {
          if (equSimboli[b].getImeSimbola() == s)
          {
            backpatchingData.setEqu(true);
            break;
          }
        }
        tabelaSimbola[i].setFlink(backpatchingData);
         RelocationData relZapis;
        if (pcRel)

          relZapis = RelocationData(locationCounter, R_X86_64_PC32,
                                    flagLG == 'g' ? tabelaSimbola[i].getRedniBrojSimbola()
                                                  : -1);
        else
          relZapis = RelocationData(locationCounter, R_X86_64_32,
                                    flagLG == 'g' ? tabelaSimbola[i].getRedniBrojSimbola()
                                                  : -1);
        tekucaSekcija.setRelocationData(relZapis);
      }
    }
    vector<string> bajtovi = bajtoviIspis(brojBajtova, pcRel == false ? 0 : pomeraj * (-1), false);
    for (int i = 0; i < bajtovi.size(); i++)
      tekucaSekcija.izlazniNiz.push_back(bajtovi[i]);
  }
  else if (sim.getDefined() == true) //ako postoji u tabeli simb i def je
  {
    char flagLG;
    for (int i = 0; i < tabelaSimbola.size(); i++)
    {
      if (tabelaSimbola[i].getNaziv() == sim.getNaziv())
      {
        flagLG = tabelaSimbola[i].getLocalGlobalFlag();

        RelocationData relZapis = RelocationData(locationCounter, R_X86_64_32,
                                                 flagLG == 'g' ? tabelaSimbola[i].getRedniBrojSimbola() : tabelaSimbola[i].getBrojSekcijeDefinisan());
        tekucaSekcija.setRelocationData(relZapis);
        if (pcRel == false)
        {
          vector<string> bajtovi;
          if (flagLG == 'g')
            bajtovi = bajtoviIspis(brojBajtova, 0, false);
          else
            bajtovi = bajtoviIspis(brojBajtova, tabelaSimbola[i].getTekucaVrednostSimbola(), false);
          for (int i = 0; i < bajtovi.size(); i++)
            tekucaSekcija.izlazniNiz.push_back(bajtovi[i]);
        }
        else
        {
          vector<string> bajtovi; //PROVERITI DA LI AKO JE GLOBALAN I U ISTOJ SU SEKCIJI TREBA DA SE RACUNA POMERAJ
          if (flagLG == 'g')
            bajtovi = bajtoviIspis(brojBajtova, (-1) * pomeraj, false);
          else if (flagLG == 'l' && tabelaSimbola[i].getBrojSekcijeDefinisan() == tekucaSekcija.getRedniBrojSimbola())
          {
            bajtovi = bajtoviIspis(brojBajtova, tabelaSimbola[i].getTekucaVrednostSimbola() - locationCounter - pomeraj, false);
            tekucaSekcija.tabelaRelokacija.pop_back();
          }
          else if (flagLG == 'l' && tabelaSimbola[i].getBrojSekcijeDefinisan() != tekucaSekcija.getRedniBrojSimbola())
           { 
              bajtovi = bajtoviIspis(brojBajtova, tabelaSimbola[i].getTekucaVrednostSimbola() - pomeraj, false);
           
           }for (int i = 0; i < bajtovi.size(); i++)
            tekucaSekcija.izlazniNiz.push_back(bajtovi[i]);
        }
      }
    }
  }
}

void Asembler::razresavanjeSimbola(bool brisanjeRelokacija)
{
  try
  {
    for (int i = 0; i < tabelaSimbola.size(); i++)
    {
      if (tabelaSimbola[i].getDefined())
      {
         for (int j = 0; j < tabelaSimbola[i].flink.size(); j++)
        {
          int adresa = tabelaSimbola[i].flink[j].getAdresa();
          int brojBajtova = tabelaSimbola[i].flink[j].getBrojBajtova();
          int brojSekcije = tabelaSimbola[i].flink[j].getBrojSekcije();
          bool relocationFlag = tabelaSimbola[i].flink[j].getRelocationFlag();
          bool pcRel = tabelaSimbola[i].flink[j].getPcRel();
           int lcSkoka = 0; //tamo gde pozivamo jmp
          bool equ = tabelaSimbola[i].flink[j].getEqu();

           for (int k = 0; k < sekcije.size(); k++)
          {
            if (sekcije[k].getRedniBrojSimbola() == brojSekcije && adresa + brojBajtova <= sekcije[k].izlazniNiz.size())
            {
              if (relocationFlag)
              {
                for (int q = 0; q < sekcije[k].tabelaRelokacija.size(); q++)
                {
                  if (sekcije[k].tabelaRelokacija[q].getAdresa() == adresa &&
                      sekcije[k].tabelaRelokacija[q].getVrednost() == -1)
                  {
                    if (tabelaSimbola[i].getLocalGlobalFlag() == 'l')
                        if (pcRel == true && tabelaSimbola[i].getBrojSekcijeDefinisan() == sekcije[k].getRedniBrojSimbola())
                      { //ako su u istoj sekciji definisan i koriscen simbol i u pitanju je skok
                         lcSkoka = sekcije[k].tabelaRelokacija[q].getAdresa();
                          sekcije[k].tabelaRelokacija.erase(sekcije[k].tabelaRelokacija.begin() + q--);
                      }
                      if (brisanjeRelokacija == false)
                      {
                          if ((tabelaSimbola[i].getLocalGlobalFlag()=='l' && pcRel==false) || (pcRel==true && tabelaSimbola[i].getBrojSekcijeDefinisan() != sekcije[k].getRedniBrojSimbola()))
                        
                          sekcije[k].tabelaRelokacija[q].setVrednost(tabelaSimbola[i].getBrojSekcijeDefinisan());
                                             
                      else
                        sekcije[k].tabelaRelokacija[q].setVrednost(tabelaSimbola[i].getRedniBrojSimbola());
                    }
                     else
                    {
                      
                      sekcije[k].tabelaRelokacija.erase(sekcije[k].tabelaRelokacija.begin() + q--);
                    }
                  }
                }
              }
               vector<string> stareVrednosti;
              for (int p = 0; p < brojBajtova; p++)
              {
                stareVrednosti.push_back(sekcije[k].izlazniNiz[adresa++]);
              }
        
              vector<string> noveVrednosti = bajtoviIspis(stareVrednosti.size(), tabelaSimbola[i].getTekucaVrednostSimbola(), true);
          
              vector<string> izlazniBajtovi;
              if (equ == true || tabelaSimbola[i].getLocalGlobalFlag() == 'l')
                izlazniBajtovi = saberi(stareVrednosti, noveVrednosti);
              if (pcRel)
              {
                vector<string> lcSkokaString = bajtoviIspis(izlazniBajtovi.size(), -lcSkoka, false);
          
                izlazniBajtovi = saberi(izlazniBajtovi, lcSkokaString);
                 
                
              }
               adresa = tabelaSimbola[i].flink[j].getAdresa();
     
              for (int r = 0; r < izlazniBajtovi.size(); r++)
              {
                 if (adresa + brojBajtova <= sekcije[k].izlazniNiz.size())
                {
                   swap(sekcije[k].izlazniNiz[adresa++], izlazniBajtovi[r]);
                  brojBajtova--;
                }
              }
            }
          }
         }
        tabelaSimbola[i].flink.erase(tabelaSimbola[i].flink.begin(), tabelaSimbola[i].flink.end());
      }
      else if (tabelaSimbola[i].getLocalGlobalFlag() == 'u')
      {
         continue;
      }
      else {
        bool sekcija=false;
        for(int o=0;o<sekcije.size();o++) {
          if(sekcije[o].getNaziv()==tabelaSimbola[i].getNaziv())
          sekcija=true;
         
        } if(sekcija==false) throw "Postoje simboli koji nisu definisani.";
      }
        
    }
  }
  catch (const char *e)
  {
    cout << e;
    exit(1);
  }
}

vector<string> Asembler::saberi(vector<string> a, vector<string> b)
{
  try
  {
    vector<string> bajtovi;
    if (a.size() != b.size())
      throw "Pogresan broj bajtova za izmenu.";
    int brojBajtova = a.size();
    for (int i = 0, j = a.size() - 1; i < a.size() / 2; j--, i++)
    {
      swap(a.at(i), a.at(j));
      swap(b.at(i), b.at(j));
    }
 string prviBroj = "0x";
    string drugiBroj = "0x";

    for (int i = 0; i < a.size(); i++)
    {
      prviBroj += a[i];
      drugiBroj += b[i];
    }
    int zbir = 0;
    if (brojBajtova == 1)
    {
      __int8_t prvi = stoi(prviBroj, 0, 16);
      __int8_t drugi = stoi(drugiBroj, 0, 16);
      zbir = prvi + drugi;
    }
    else if (brojBajtova == 2)
    {
      __int16_t prvi = stoi(prviBroj, 0, 16);
      __int16_t drugi = stoi(drugiBroj, 0, 16);
      zbir = prvi + drugi;
       }
     bajtovi = bajtoviIspis(brojBajtova, zbir, true);
      
    return bajtovi;
  }
  catch (const char *e)
  {
    cout << e;
    exit(1);
  }
}

void Asembler::racunanjeIndexaKlasifikacije()
{
  struct Index
  {
    int brojSekcije;
    int vrednost;
    vector<string> simboli;
    Index(int sekcija, int vrednostIndexa)
    {
      brojSekcije = sekcija;
      vrednost = vrednostIndexa;
    }
  };
  try
  {
    for (int i = 0; i < equSimboli.size(); i++)
    {
      int vrednostEquSimbola = equSimboli[i].vrednost;
      vector<Index> tabelaIndexa;
      for (int l = 0; l < equSimboli[i].izraz.size(); l++)
      {
         bool flagAkoSimbolNijePronadjenUTabeliSimb = true;

        for (int j = 0; j < tabelaSimbola.size(); j++)
        {
           if (tabelaSimbola[j].getNaziv() == equSimboli[i].izraz[l].simbol)
          {
            flagAkoSimbolNijePronadjenUTabeliSimb = false;
            if (tabelaSimbola[j].getDefined())
            {
              int brSekcijeDef = tabelaSimbola[j].getBrojSekcijeDefinisan();
               int vrednostTrenutnogSimbola = tabelaSimbola[j].getTekucaVrednostSimbola();
              string nazivSimbola = tabelaSimbola[j].getNaziv();
              char flag = localGlobalExtern(tabelaSimbola[j].getNaziv());
              char znak = equSimboli[i].izraz[l].znakSimbola;

              if (flag == 'e')
                znak = '+';
               if (znak == '+')
                vrednostEquSimbola += vrednostTrenutnogSimbola;
              else
                vrednostEquSimbola -= vrednostTrenutnogSimbola;
               bool flagZaPronalazenje = true;
              if (tabelaIndexa.size() > 0)
              {
                for (int k = 0; k < tabelaIndexa.size(); k++)
                {
             
                  if (tabelaIndexa[k].brojSekcije == brSekcijeDef)
                  {
                    flagZaPronalazenje = false;
                    if (znak == '+')
                    {
                      tabelaIndexa[k].vrednost++;
                       tabelaIndexa[k].simboli.push_back(nazivSimbola);
                    }
                    else
                    {
                      tabelaIndexa[k].vrednost--;
                      if (tabelaIndexa[k].vrednost < 0)
                        break;
                       tabelaIndexa[k].simboli.erase(tabelaIndexa[k].simboli.begin());
                    }
                  }
                }
              }
              if (flagZaPronalazenje)
              {
                 tabelaIndexa.push_back(Index(tabelaSimbola[j].getBrojSekcijeDefinisan(),
                                             equSimboli[i].izraz[l].znakSimbola == '+' ? 1 : -1));
                tabelaIndexa[tabelaIndexa.size() - 1].simboli.push_back(tabelaSimbola[j].getNaziv());
              }
             }

            else
            {
              goto KRAJ;
            }
          }
        }
        if (flagAkoSimbolNijePronadjenUTabeliSimb)
          throw "Ne postoje svi simboli iz izraza .equ direktive u tabeli simbola.";
      }
       
      for (int z = 0; z < tabelaIndexa.size(); z++)
      {
        if (tabelaIndexa[z].vrednost == 0 || tabelaIndexa[z].brojSekcije == -2)
          tabelaIndexa.erase(tabelaIndexa.begin() + z--);
      }
      
      if (tabelaIndexa.size() > 1)
        throw "Pogresno definisana direktiva .equ.";
      else if (tabelaIndexa.size() == 1)
      {
        Index poslednjiSimbol = tabelaIndexa[0];
        char localGlobalFlag;

        if ((poslednjiSimbol.vrednost != 0 && poslednjiSimbol.vrednost != 1) || poslednjiSimbol.simboli.size() > 1)
          throw ".Equ direktiva ne moze da se razresi.";
         for (int s = 0; s < tabelaSimbola.size(); s++)
        {
          if (tabelaSimbola[s].getNaziv() == poslednjiSimbol.simboli[0])
          {
             localGlobalFlag = localGlobalExtern(tabelaSimbola[s].getNaziv());
            if (localGlobalFlag == 'g')
            {
              int vrednostZaOduzimanje = tabelaSimbola[s].getTekucaVrednostSimbola(); //ako je globalni ili eksterni
              vrednostEquSimbola -= vrednostZaOduzimanje;
            }
          }
        }

        for (int h = 0; h < tabelaSimbola.size(); h++)
        {
          if (tabelaSimbola[h].getNaziv() == equSimboli[i].imeSimbola)
          {

            tabelaSimbola[h].setVrednost(vrednostEquSimbola);
             tabelaSimbola[h].setBrojSekcije(poslednjiSimbol.brojSekcije);
            tabelaSimbola[h].setDefined(1);
             tabelaSimbola[h].setFlag(localGlobalFlag == 'l' ? 'l' : 'g');
            if (localGlobalFlag == 'g')
            {
              int r = 0;
              for (; r < globalniSimboli.size(); r++)
                if (globalniSimboli[r] == tabelaSimbola[h].getNaziv())
                  break;
              if (r == globalniSimboli.size())
                globalniSimboli.push_back(tabelaSimbola[h].getNaziv());
            }
            razresavanjeSimbola(false);
            if (localGlobalFlag == 'e')
            {
              int r = 0;
              for (; r < externiSimboli.size(); r++)
                if (externiSimboli[r] == tabelaSimbola[h].getNaziv())
                  break;
              if (r == externiSimboli.size())
               externiSimboli.push_back(tabelaSimbola[h].getNaziv());
                  tabelaSimbola[h].setVrednost(0);
            }
          }
        }
      }
      else
      {
        for (int h = 0; h < tabelaSimbola.size(); h++)
        {
          if (tabelaSimbola[h].getNaziv() == equSimboli[i].imeSimbola)
          {
            tabelaSimbola[h].setVrednost(vrednostEquSimbola);
            tabelaSimbola[h].setBrojSekcije(-2);
            tabelaSimbola[h].setDefined(1);
            tabelaSimbola[h].setFlag('l');  
              razresavanjeSimbola(true);      
          }
        }
      }

    KRAJ:
      continue;
    }
  }
  catch (const char *e)
  {
    cout << e;
    exit(1);
  }
}

void Asembler::instrukcijskiDesktiptor(string instrukcija, vector<Operand *> operandi)
{
  int velicinaOperanada = 0;
   if (operandi.size() > 0)
    velicinaOperanada = velicinaOperanda(instrukcija, operandi);

  if (instrukcija.at(instrukcija.size() - 1) == 'b' && instrukcija!="sub")
  {
    instrukcija.erase(instrukcija.size() - 1);
  }
  else if (instrukcija.at(instrukcija.size() - 1) == 'w')
  {

    instrukcija.erase(instrukcija.size() - 1);
  }
  int oc = instrukcije[instrukcija];
  int operacioniKod = oc << 3;
  velicinaOperanada <<= 2;
  operacioniKod = operacioniKod | velicinaOperanada;
  obradaDeskriptora(operacioniKod);
}

int Asembler::velicinaOperanda(string instrukcija, vector<Operand *> operandi)
{
  try
  {
    int suma = 1;
    bool b = false;
    if (instrukcija.at(instrukcija.size() - 1) == 'b' && instrukcija != "sub")
    {
      b = true;
    }
    else
      suma = 2;

    int i = 0;

    while (i < operandi.size())
    {  Operand *operand = operandi[i];

      if (b == true)
      {
        if ((adresiranja[operand->tipAdresiranja] == 1) &&
            operand->lowHigh != 'l' && operand->lowHigh != 'h') //ako je b a nije regLH
          throw "Instrukcija je velicine 1B a koristi se vrednost registra velicine 2B.";
      }
      else
      {
        if ((adresiranja[operand->tipAdresiranja] == 1) &&
                operand->lowHigh == 'l' ||
            operand->lowHigh == 'h') //ako je w a  regLH
          throw "Instrukcija je velicine 2B a koristi se vrednost regista velicine 1B.";
      }

      i++;
    }
    if (suma == 1)
      return 0;
    else
      return 1;
  }
  catch (const char *e)
  {
    cout << e;
    exit(1);
  }
}

void Asembler::deskriptorOperanda(Operand *operand)
{
  int adresiranje = adresiranja[operand->tipAdresiranja];
  int lh = 0;
  int registar = 0;
  if (adresiranje == 1 || adresiranje == 2 || adresiranje == 3)
  {
    registar = operand->registar;
  }

  if (adresiranje == 1)
  {
    if (operand->lowHigh == 'h')
      lh = 1;
    else
      lh = 0;
  }
  adresiranje <<= 5;

  registar <<= 1;
  adresiranje = adresiranje | registar;
  adresiranje |= lh;

  obradaDeskriptora(adresiranje);
}
