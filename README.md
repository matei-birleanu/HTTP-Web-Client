Bîrleanu Teodor Matei 324 CA 


                                    README TEMA 4
                                   HTTP - Client REST

        Am folosit ca schelet laboratorul 9 , in special functiile de creeare a mesajelor pentru
    request urile de tip POST, GET, DELETE. Tin sa mentionez faptul ca checker ul dadea diferite 
    erori insa executand manual toate comenzile realizate ele functioneaza.
        Pentru a parsa raspunsurile de la server am folosit biblioteca utilizand fisierele "parson.c"
    si "parson.h". Am ales parson intrucat mi s a parut mai usor de folosit si de asemenea am urmarit
    cateva tutoriale pentru a intelege cum pot folosi functiile de baza pentru parsare.
        Pentru a retine informatiile de autorizare am folosit o variabila globala in care am retinut
    token ul JWT. La fiecare acces in biblioteca am actualizat acel token si l am prestat pentru a l
    folosi in viitoarele mesaje catre server. De asemenea, alaturi de token am retinut si un cookie pentru
    conectare care de asemenea era initializat la fiecare logare a utilizatorului pentru ca serverul sa stie
    in urmatoarele mesaje faptul ca user ul este conectat.
        Programul a fot elaborat in mare parte folosind cate o functie pentru fiecare comanda posibila. Prin
    urmare, in main am citit comanda si am apelat functia pentru executarea acesteia. Daca nu este o comanda
    valida voi printa un mesaj de "comanda necunoscuta".
        La comanda de register am citit username ul si parola dupa care le am incaspulat intr un obiect JSON
    (JSON_Object), aplcandu i functia "json_serialize_to_string" pentru a l incapsula in mesajul pe care l
    vom trimite catre server. Am definit cateva macro uri pentru fiecare url folosit , pentru ip ul de host si portul pe care realizez conexiunea. Am folosit de asemenea functiile din fisierul helpers.c de receive/send catre server sau compute_message. Am deschis conexiunea am trimis mesajul catre server am primit raspuns dupa care am inchis conexiunea. Daca in raspuns gasesc mesajul "error" atunci printez un mesaj de eraore. In caz contrar, actiunea s a indeplinit cu succes afisand un mesaj pozitiv. La sfarsit, eliberez resursele folosite pentru a reprezente mesajele in stilul JSON.
        Pentru functia de login am procedat asemanator functiei de register folosind metoda POST si url ul
    din enuntul temei. De asemenea, am manipulat raspunsul server ului cu ajutorul functiilor "basic_extract_json_response" si "json_parse_string". Am tratat cazurile de eroare sau succes identic insa in cazul de succes am actualizat cookie ul sesiunii create. De asemenea am eliberat orice alt token JWT folosit anterior pentru aceesarea resurselor. La final am eliberat toate resursele alocate.
        La functia "enter_library", am folosit acelasi sablon ca mai sus(compunere mesaj, deschidere
    conexiune ,trimitere mesaj ,primire mesaj,inchidere conexiune). Daca raspunsul de la server este unul pozitiv atunci actulizez token ul JWT pentru a avea acces la resurse.
        Pentru functia "get_books" am folosit acelasi sablon integran acel token in mesaj pentru ca server 
    ul sa cunoasca faptul ca avem acces la resurse. In caz de succes am cautat in string ul primit "{" fapt ce maracheaza inceputul textului de tip JSON afisand astfel toata biblioteca.
        La functia "add_book" am folosit aceeasi idee , am instantiat o variabila de tip JSON_Object careia
    i am atribuit toate campurile necesare (author, title etc). Dupa aceasta, am realizat acelasi procedeu afisand mesaje specifice in functie de mesajul primit de la server.
        Pentru functia "get_book", am personalizat url ul cu id ul cartii pe care vreau sa o afisez cu 
    ajutorul functiei sprintf. Dupa aceasta,am realizat acelasi procedeu . In cazul raspunsului pozitiv, am folosit functiile "json_object_get_number" si "json_object_get_string" pentru a extrage datele cartii si a le afisa.
        La functia "delete_book", am realizat ca la cea de "get_book" insa pentru compunerea mesajului 
    pentru server am folosit functia "compute_delete_request". 
        Pentru functia de logout am realizat acelasi sablon de compunere a mesajului si trimitere catre 
    server. De asemenea, am marcat cu NULL cookie ul de sesiune si token ul JWT astefel asigurandu ma ca nu voi da acces unui utilizator care este deconectat.
        In cadrul funciilor din request.c din cadrul laboratorului am adaguat in antetul lor si token ul
    JWT pentru a l adaugat la constructia mesajului pentru server.
        Am folosit 2 sleepdays!
        Va multumesc pentru atentie!
