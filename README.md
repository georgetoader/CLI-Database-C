# basic-database-C
Programul simuleaza functionalitatea unei baze de date. Pentru a citi am folosit fgets si o variabila "input" pe 
care apoi am despartit-o in cuvinte folosind functia "strtok". Variabilele obtinute au fost transmise ca parametru diverselor
functii implementate.
* `INIT_DB` -> Initializeaza baza de date si ii atribuie numele dat.
* `DELETE_DB` -> Elibereaza toate memoria ocupata de baza de date.
* `INS_TABLE` -> Adauga in baza de date un tabel cu numele "name". Intai verific daca tipul de date transmis ca parametru este 
	valid si daca nu afisez un mesaj de eroare si ies. Apoi verific daca tabelul exista deja in baza de date prin parcurgerea
	tuturor tabelelor deja existente. Daca nu exista atunci il initializez si il adaug la sfarsit listei de tabele. Ii atribui
	numele, tipul de date si ii adaug coloanele.
* `SEARCH_TABLE` -> Verifica daca exista in baza de date un tabel cu numele "name" transmis ca parametru. Daca da atunci returneaza
	un pointer catre pozitia tabelului respectiv. Functia aceasta este folosita in majoritatea celorlalte functii la inceput. 
* `DELETE_TABLE` -> Sterge tabelul cu numele "name" transmis ca parametru. Verific daca exista tabelul in baza de date si daca da
	atunci prima data trebuie sa eliberez liniile. Verific tipul de date al elementelor liniilor. In functie de acesta parcurg 
	liniile si celulele acestora si le eliberez. Dupa ce am eliberat celulele parcurg iar liniile si le eliberez pe fiecare. 
	Parcurg coloanele si le eliberez iar la final eliberez tabelul si refac legaturile printr-o variabila "ant".
* `ADD` -> Adauga o linie in tabelul "name" transmis ca parametru. Verific daca exista tabelul in baza de date si daca da atunci 
	pot adauga linia. Verific tipul de date al elementelor tabelului si in functie de acesta incep sa parcurg lista liniilor.
	Initializez noua linie si o adaug la sfarsitul listei prin parcurgerea tuturor liniilor existente deja. Dupa ce am adaugat 
	linia trebuie sa adaug celulele acesteia. Daca este prima celula atunci aloc direct memorie, daca nu atunci folosesc o 
	variabila "ant" ce retine ultima celula alocata, aloc noua celula si apoi o leg de "ant". Copiez valoarea data in celula.
* `PRINT_TABLE` -> Functia afiseaza un tabel dat. Verific daca exista tabelul cautat in baza de date si daca da atunci afisez
	numele tabelului. Parcurg coloanele si le afisez folosing functia "padding_string" ce adauga numarul de spatii necesare
	dupa afisarea numelui coloanei. Dupa ce le-am afisat folosesc functia "separator" de "nr_col" ori pentru a afisa la
	fel de multe delimitatoare cate coloane sunt. Daca tabelul nu contine linii atunci ies, altfel in functie de tipul de
	date al elementelor parcurg liniile si pentru fiecare celula apelez functia "padding_string/int/float" in functie
	de tipul de date. In "padding_int"numar cate caractere are numarul respectiv iar in "padding_float" folosesc un cast la
	"int" pentru a afla numarul de caractere inainte de "." la care apoi adaug 7 caractere ("." si cele 6 zecimale).
* `CLEAR` -> Functia sterge toate liniile unui tabel dat. Verific daca exista tabelul cautat in baza de date si daca da atunci
	in functie de tipul de date parcurg liniile si celulele acestora si le eliberez.
* `PRINT_DB` -> Functia afiseaza baza de date. Afisez numele bazei de date si apoi pentru fiecare tabel apelez functia "PRINT_TABLE".
* `SEARCH` -> Functia cauta in tabelul "name" liniile care respecta relatia data prin parametri. Verific daca tabelul exista in
	baza de data si daca da atunci afisez numele tabelului. Parcurg coloanele pentru a verifica daca exista cea cautata si
	o numerotez. Parcurg liniile si elementele acestora pana la coloana gasita si apoi daca elementul respectiv verifica
	relatia atunci ma intorc la inceputul liniei si o afisez.
* `DELETE_LINE` -> Functia cauta in tabelul "name" liniile care respecta relatia data prin parametri si le sterge. Verific
	daca tabelul exista in baza de data si daca da atunci afisez numele tabelului. Parcurg coloanele pentru a verifica
	daca exista cea cautata si o numerotez. Parcurg liniile si elementele acestora pana la coloana gasita si apoi daca
	elementul respectiv verifica relatia atunci ma intorc la inceputul liniei si sterg celulele acesteia, apoi linia. Am
	o variabila "ant" ce retine linia anterioara pentru a forma noile legaturi.


