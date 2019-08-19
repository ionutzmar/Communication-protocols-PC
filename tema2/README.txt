Mihalache Ionut Marius - 324CC
Tema 2, PC

Am folosit limbajul c++ si am structurat codul in 2 fisiere: server.cpp si subscriber.cpp.

Server.cpp
La inceputul fisierului am declarat mai multe variabile, de care ma voi folosi
de-a lungul programului. Cele mai importante sunt:
map<int, string> clientIds; //corelatie intre file descriptor si clientId
map<string, clientStatus> clientIdsMap; //corelatie intre clientId si o structura cu care tinem evidenta clientilor conectati
map<string, map<string, bool>> topics; //<topic <cliendId, store&forward>>
multimap <string, string> missedNotification; // <clientId, notificare>
In structura serv_addr, de tip sockaddr_in am setat parametri corespunzatori pentru
server. Am creat un socket pentru conexiunile TCP, i-am dat bind cu serv_addr, iar
apoi am apelat functia listen, lar la numarul maxim de clienti am pus MAX_CLIENTS,
o valoare setata de mine la 128. Am ales acest numar, deoarece  am citit pe internet
ca asta ar fi numarul maxim de clienti acceptat. Am creat si socketul folosit pentru
UDP si am apelat functia bind. In continuare, intr-un while infinit, am folosit functia
select pentru a multiplexa intre conexiuni. In cazul inputului de la tastatura,
se verifica daca inputul este "exit" si se termina programul in caz afirmativ.
In cazul unui mesaj pe socketul udp, parsez mesajul conform cerintei. La final
printez intr-un string mesajul si il trimit catre toti clientii din topicul respectiv.
In cazul in care vine o conexiune pe socketul tcp, inseamna ca este un nou client
ce vrea sa se conecteze. Apelez functia accept, si retin informatiile despre client.
In cazul in care pentru acel cliendId am notificari netrimise, le trimit la conectare.
Pentru date disponibile pe o conexiune catre un client, citesc datele intr-o structura
de tip subscribeMessage, declarata de mine. Apoi abonez sau dezabonez clientul de la
un anumit topic.

Subscriber.cpp
Am declarat la inceput o structura de tipul subscribeMessage, folosita pentru
trimiterea de date catre server. Am creat un socket si m-am conectat la server.
Imediat dupa conectare am trimis ID-ul clientului. Intr-ul while infinit am multiplexat
cu ajutorul functiei select intre inputul de la tastatura si conexiunile de la server.
Daca inputul este "exit", inchid clientul. Apoi, in functie de comanda subscribe sau
unsubscribe, abonez sau dezabonez clientul de la topicul specificat, tinand cont la
abonare de store & forward. In cazul in care serverul se inchide, inchid si clientul.
