Na starcie bardzo przepraszam za brak polskich znakow, cos mi sie popsulo na
linuxie i nie moge tego naprawic na ten moment.

1. Instrukcja kompilacji
a) Na poczatku trzeba skompilowac plik konfiguracyjny 
przyklad:
gcc inf140766_configuration.c -o config
b) Pozniej nalezy skompilowac plik serwera i klienta
przuklad:
gcc inf140766_c.c -o client
gcc inf140766_s.c -o server

2. Instrukcja uruchomienia
a) Ponownie na poczatku trzeba uruchomic plik konfiguracyjny
przyklad:
./config
Opis:
Utworzy nam on pliki:
-inf140766_registeredUsersDatabase.txt
Z danymi 9 uzytkownikow wedlug wzorca:
uzytkownik;haslo;grupa1,grupa2,;
przyklad:
user1;pass1;20,21,;
-inf140766_groups.txt
Plik z nazwami 3 grup:
20,21,22,
-20.txt, 21.txt, 22.txt
Pliki w ktorych beda przechowywane konwersacje kazdej z grup

b)Najpierw w jednym terminalu uruchomic serwer
przyklad:
./server

c)W oddzielnym terminalu lub terminalach uruchomic klienta/klientow
przyklad:
./client

3. Zawartosc poszczegolnych plikow *.c
a) inf140766_c.c - plik klienta
b) inf140766_s.c - plik serwera
c) inf140766_configuration.c - plik konfiguracyjny
