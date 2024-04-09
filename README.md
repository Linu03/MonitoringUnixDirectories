Subiectul proiectului: monitorizarea modificarilor aparute in directoare de a lungul timpului.


Saptamana 1: Utilizatorul poate specifica directorul pe care doreste sa il monitorizeze in linia de comanda, ca prim argument. Se vor urmari atat modificarile aparute la nivelul directorului, 
cat si la nivelul subarborelui. Se va parcurge directorul si intregul sau subrbore si se face un SNAPSHOT. Acest snapshot poate fi un fisier, care contine toate datele pe care le furnizeaza directorul
si subarborele si care sunt considerate necesare.

Saptamana 2: Se actualizeaza functionalitatea programului astfel incat sa primeasca oricate argumente, dar nu mai mult de 10 (argumentele sa fie diferite). Programul proceseaza doar directoarele , daca este fisier il ignoram.
Dupa ce facem un snapshot, il verificam cu snapshotul precedent. Programul primeste un director suplimentar (de iesire) in care sunt stocate toate snapshoturile (celor de la intrare).

./exe arg1 arg2 -o iesire  // deci 12 in total
