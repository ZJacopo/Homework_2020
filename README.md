# Homework_2020
Alcune rapide informazioni su come compilare i file: 
1. E' presente il file che costruisce le liste di ingressi e uscite in formato .txt (questi due file dovranno essere contenuti nella stessa cartella che conterrà anche 
tutti i programmi successivi). 
Per eseguirlo è sufficiente:

g++ -Wall -o list_creator list_creator.cpp
./list_creator

2. Per il robot è necessario eseguire il file 'Rob_client.cpp'

g++ -Wall -o Rob_client Rob_client.cpp

3. Il file complessivo 'homework_2020' richiede due header: Shop.h e Rob_server.h:
g++ -c Shop.cpp -pthread
g++ -c Rob_server.cpp -pthread
g++ -c homework_2020.cpp -pthrerad
g++ -Wall -o main homework_2020.o Shop.o Rob_server.o -pthread

4. A questo punto per eseguire il tutto si possono aprire due terminali e digiare
nel primo:      ./main 4321
nel secondo:    ./Rob_client localhost 4321

Il programma principale dovrebbe comunicare al robot quando accendersi, quando spegnersi e quando spegnersi completamente alla chiusura del negozio; 
si otterranno due file.txt contenenti i clienti uscenti con la loro permanenza e le statischire ora per ora divisi per maschi e femmine. Per agevolare la lettura 
si è pensato di aumentare (in list_creator) il tempo di permanenza delle donne, così da poterlo vedere meglio nella statistica.
  
