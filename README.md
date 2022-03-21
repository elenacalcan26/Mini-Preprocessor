# Mini-Preprocessor

<h5> Calcan Elena-Claudia <br/>
331CA</h5><br/>
 
  
 Programul reprezinta o implementare a unui mini preprocesor care analizeaza cod
 sursa C.  
  
  - programul poate primi argumente in linia de comanda avand urmatoarea semantura:
 
        so-cpp [-D <SYMBOL>[=<MAPPING>]] [-I <DIR>] [<INFILE>] [ [-o] <OUTFILE>]
  
  
  ### Argumente primite 
  ---------------------------------------------------------------------
   
   
  1. -D \<SYMBOL>[=\<MAPPING>] sau -D\<SYMBOL>[=\<MAPPING>] 
     - se salveaza simbolul cu valoarea asociata acestuia
     - se verifica daca simbolui ii este asociat o valoare 
     - in cazul in care nu se gasete o valoare, atunci simbolul se asciaza cu
     sirul vid 
      
  2. -I \<DIR> sau -I\<DIR> 
      - se extrage directorul precizat de modificatorul -I si se adauga intr-un array 
  de string-uri 
  
  3. \<INFILE> si \[ [-o] \<OUTFILE>]
      - programul primeste cel mult un fisier de input si de output
      - daca se primesc mai multe fisiere, atunci programul iese cu eroarea 12
      - daca lipseste fisierul, atunci citirea si afiseara se realizeaza la stdin,
      respectiv stdout 
  
  ------------------------------------------------------------------------- 
  
   - programul proceseaza codul citind linie cu linie input-ul si verificand 
   daca pe fiecare linie exista sau nu directive  
  
  
  
  ### Directive 
 -----------------------------------------------------------------  
    
  1.  #define  
      - se salveaza simbolul definit cu valoarea asociata acestuia  
      - valoarea asociata este parcursa caracter cu caracter pentru a verifica daca 
        nu contine un alt simbol care a fost definit inainte 
      - pentru define-urile de tip multilinie se verifica existena caracterului '\', atunci 
      se citesc si se proceseaza liniile pana cand se gaseste prima linie ce nu contine caracterul 
      - asocierile sunt stocate intr-un hashmap
    
  
  2. #undef  
      - se elimina asocierea dintre simbol si valoarea sa asociata, se sterg din hashmap  
   
  3. #if \<cond> \ #elif \<cond> \ #else \ #endif 
      - se verifica evaluarea conditiei sa fie diferita de 0, caz in care liniile de cod 
      din cadrul blocului respectiv sunt preprocesate 
      - pentru a stii ce bloc este preprocesat s-a folosit o variabila ce are urmatoarele valori: 
  
                -> 1 = blocul se poate scrie 
                -> -1 = blocul nu se poate scrie 
      - cand se gaseste #endif variabila se reseteaza la valoarea 1 
   
  4. #ifdef \<symbol> / #ifndef \<symbol> / #elif / #endif 
      - se verifica daca simbolul a fost definit anterior sau nu 
      - abordarea este la fel ca la directiva #if \<cond> si se face in functie de valoarea returnata
      de functia de verificare a simbolului in hashmap 
  
  5. #include  
      - realizeaza preprocesarea fisierelor header si preproceseaza liniile de cod ale acestuia 
      - fisierul este cautat in directorul curent, uramd sa se creeze path-uri cu directoarele 
      slavate din array pana la prima sa aparitie
      - preprocesarea se realizeaza la fel ca fisierul primit la intrare
      - in cazul in care nu se gaseste se iese din program cu -1  
  
  ----------------------------------------------------------------------------- 
  
  - pentru liniile de cod ce nu contin directive, se sparge linia dupa spatii 
  - string-urile rezultate sunt parcurse caracter cu caracter pentru a verifca aparitia simbolurilor 
  definite anterior si inlocuirea acestora cu valorile asociate 
  - string-urile rezultate sunt adaugate intr-un buffer ce este scris in fisierul de output 
  
  ### Hashmap 
  --------------------------------------------------------------------------- 
  
  - s-a implementat un hashmap pentru a retine simbolurie definite cu valorile acestora 
  - implementarea hashmap-ului trateaza coliziunile folosind linear probing 
  - cheia este reprezentata de simbolul definit 
  - hashmap-ul este reprezentat intern cu un array de bucket-uri
  - la inceput hashmap-ul este alocat cu o dimensiune maxima considerata, urmand sa isi 
  dubeleze capacitatea atunci cand se insereaza o pereche si nu mai sunt bucket-uri goale 
  - operatiile implementate sunt urmatoarele: 
    
          - inserare 
          - stergere
          - lookup  
   
   ## Bibliografie 
   ----------------------------------------------------------------------- 
   
   - https://www.journaldev.com/35238/hash-table-in-c-plus-plus
   - https://stackoverflow.com/questions/7666509/hash-function-for-string 
   - https://ocw.cs.pub.ro/courses/sd-ca/2020/laboratoare/lab-04
