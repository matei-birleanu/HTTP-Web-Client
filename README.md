# README - Tema 4: HTTP - Client REST

## Descriere generală
Această temă implementează un client REST utilizând funcționalități din laboratorul 9, în special funcțiile pentru crearea mesajelor POST, GET și DELETE. Deși checker-ul afișa diverse erori, toate comenzile au fost testate manual și funcționează corect.

Biblioteca folosită pentru parsarea răspunsurilor serverului este **parson**, utilizând fișierele `parson.c` și `parson.h`. Am ales această bibliotecă datorită ușurinței de utilizare și a documentației clare. 

## Autorizare
- **Token JWT**: Este stocat într-o variabilă globală și actualizat la fiecare acces în bibliotecă. Acesta este folosit pentru autorizarea mesajelor viitoare.
- **Cookie-ul de sesiune**: Este inițializat la fiecare logare pentru a asigura recunoașterea utilizatorului de către server.

## Structură generală
- Fiecare comandă este implementată printr-o funcție dedicată.
- **Main**: Citește comanda utilizatorului și apelează funcția corespunzătoare. Dacă comanda este invalidă, afișează mesajul „comandă necunoscută”.

---

## Funcționalități implementate

### **Register**
1. Se citesc `username` și `password` și se încorporează într-un obiect JSON (`JSON_Object`).
2. Mesajul este trimis către server folosind metoda POST.
3. Mesajul include:
   - URL-ul (definit ca macro).
   - JSON-ul serializat.
4. În cazul unui răspuns negativ de la server, se afișează un mesaj de eroare. În caz contrar, se confirmă succesul.

### **Login**
- Procedură similară cu funcția de register.
- Răspunsul serverului este procesat cu:
  - `basic_extract_json_response`.
  - `json_parse_string`.
- În caz de succes:
  - Se actualizează cookie-ul de sesiune.
  - Token-ul JWT anterior este eliberat.

### **Enter Library**
- Se trimite o cerere către server pentru accesarea bibliotecii.
- În caz de succes, se actualizează token-ul JWT pentru acces la resursele protejate.

### **Get Books**
- Se trimite o cerere GET către server.
- Token-ul JWT este inclus în mesaj.
- Dacă răspunsul este pozitiv:
  - Se caută structura JSON în răspuns.
  - Se afișează toate cărțile din bibliotecă.

### **Add Book**
- Se construiește un obiect JSON cu informațiile despre carte (`author`, `title` etc.).
- Mesajul este trimis către server.
- Se afișează mesaje în funcție de răspunsul primit.

### **Get Book**
- URL-ul este personalizat cu ID-ul cărții utilizând `sprintf`.
- Dacă răspunsul este pozitiv, informațiile despre carte sunt extrase cu:
  - `json_object_get_number`.
  - `json_object_get_string`.

### **Delete Book**
- URL-ul este personalizat similar funcției *Get Book*.
- Mesajul este construit cu `compute_delete_request`.

### **Logout**
- Se trimite o cerere către server.
- Se resetează cookie-ul de sesiune și token-ul JWT pentru a preveni accesul unui utilizator deconectat.

---

## Detalii tehnice
- Am folosit fișierele și funcțiile din laboratorul 9 pentru:
  - Crearea cererilor (`compute_message`, `send_to_server`, `receive_from_server`).
  - Parsarea răspunsurilor.
- Am definit macro-uri pentru:
  - URL-urile utilizate.
  - IP-ul și portul host-ului.

---

## Observații
- În funcțiile din `request.c`, am adăugat token-ul JWT în antetul mesajelor trimise către server.
- Am folosit **2 zile de întârziere**.

---

## Mulțumiri
Vă mulțumesc pentru atenție! Pentru întrebări sau sugestii, nu ezitați să mă contactați.
