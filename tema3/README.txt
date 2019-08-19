Mihalache Ionut Marius
324CC

Tema 3 - PC

Tema este structurata in fisierele:
    client.c - trimiterea comenzilor catre server si parsarea raspunsului
    helpers.c - functiile de conectare la server, trimitere catre server
                a mesajelor, obtinerea adreseui IP in functie de domeniu
    requests.c - functii pentru compunerea mesajelor GET sau POST

Pentru fiecare task in parte (cu exceptia primului), pasii sunt asemanatori:
    - extrag cookie-urile din raspunsul serverului
    - folosind biblioteca Parson, imi extrag datele pentru urmatorul request
    - imi construiesc header si body-ul pentru urmatorul request
    - printez mesajul pe care vreau sa il trimit
    - il trimit catre server
    - astept raspuns de la server
    - printez ce am primit de la server

Requestul catre openweathermap dureaza in general mai mult timp, dar se termina
de fiecare data cu succes.
