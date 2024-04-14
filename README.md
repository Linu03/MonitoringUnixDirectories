Project SO


Subiectul proiectului este monitorizarea modificarilor aparute in directoare de-a lungul timpului, prin realizarea de capturi (snapshots) la
cererea utilizatorului.
Cerintele programului:

Cerinta 1: 
·Se va dezvolta un program în limbajul C, prin care utilizatorul va putea specifica directorul de monitorizat ca argument în linia de
comandă, iar programul va urmări schimbările care apar în acesta și în subdirectoarele sale.
La fiecare rulare a programului, se va actualiza captura (snapshot-ul) directorului, stocând metadatele fiecărei intrări.
Pentru a reprezenta grafic un director și schimbările survenite înainte și după rularea programului de monitorizare a modificărilor, putem
utiliza un arbore de fișiere simplu.
Directorul înainte de rularea programului:
Directorul după rularea programului:
Nota: S-au prezentat două variante ca exemplu, însă organizarea directorului și a fișierelor de snapshot poate fi realizată în orice fel dorit,
fără a exista restricții impuse.
Explicatie versiunea 1: După rularea programului, în directorul principal este creat un nou fișier, denumit "Snapshot.txt" (sau alt format
specificat), care conține metadatele fiecărei intrări (fișiere și subdirectoare) din directorul principal și subdirectoarele sale. Acest snapshot
este actualizat la fiecare rulare a programului pentru a reflecta modificările survenite.
Nota: Selectarea metadatelor este complet la latitudinea utilizatorului. Este important să includem informații care să diferențieze fiecare
intrare din director într-un mod unic și să evidențieze modificările efectuate


Cerinta 2: 
S7
Funcționalitatea programului va fi actualizată pentru a permite primirea unui număr nespecificat de argumente (directoare) în linia de
comandă (dar nu mai mult de 10), iar fiecare argument este diferit. Logica pentru capturarea metadatelor va fi acum aplicată tuturor
argumentelor primite, ceea ce înseamnă că programul va actualiza snapshot-urile pentru toate directoarele specificate de utilizator.
De exemplu:
Pentru fiecare intrare din directoarele furnizat ca argumente, utilizatorul va putea compara snapshot-ul anterior al directorului specificat
cu cel actual. Dacă există diferențe între cele două snapshot-uri, captura veche va fi actualizată cu noile informații din snapshot-ul
curent.

1 ./program_exe dir1 dir2 dir3 dir4


1 // Fișierul instantaneu vechi pentru File1.txt

2 Marca temporală: 2023-03-15 10:30:00

3 Intrare: File1.txt

4 Dimensiune: 1024 octeți

5 Ultima modificare: 2023-03-14 08:45:00

6 Permisiuni: rw-r--r--

7 Număr Inode: 12345


1 // Fișierul instantaneu nou pentru File1.txt

2 Marca temporală: 2023-03-16 11:00:00

3 Intrare: File1.txt

4 Dimensiune: 1024 octeți

5 Ultima modificare: 2023-03-16 09:15:00

6 Permisiuni: rw-r--r-x

7 Număr Inode: 12345

În exemplele de mai sus, avem o reprezentare posibilă a unui snapshot vechi și curent (nou) pentru File1.txt. Având în vedere că noul
snapshot este diferit de cel anterior, cel vechi va fi suprascris.

Funcționalitatea codului va fi extinsă astfel încât programul să primească un argument suplimentar, reprezentând directorul de ieșire în
care vor fi stocate toate snapshot-urile intrărilor din directoarele specificate în linia de comandă. Acest director de ieșire va fi specificat
folosind opțiunea -o .
De exemplu, comanda pentru a rula programul va fi:

1 ./program_exe -o director_iesire dir1 dir2 dir3 dir4

