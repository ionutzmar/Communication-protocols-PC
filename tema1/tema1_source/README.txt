Tema 1 PC
Mihalache Ionut-Marius, 324CC


In aceasta tema am implementat transferul unui fisier folosind fereastra glisanta.

Mi-am declarat o structura proprie, numita my_pkt cu 4 membri: ack, size, payload
si parity. Pe aceasta am scris-o cu memcpy() in structura librariei, de tip msg.
Campurile din structura proprie my_pkt:
1. ack - numarul de secventa al pachetului
2. payload - vector de charuri, zona efectiva unde stochez date
3. size - cati bytes din payload folosesc. Acest camp este egal cu valoarea de
return a functiei read()
4. parity - un int pentru verificarea coruperii pachetelor. Pentru verificare am
facut suma tuturor octetilor din structura my_pkt, iar daca un singur octet este
alterat, suma octetilor sigur va fi diferita.

La inceput am citit tot fisierul si l-am memorat in vectorul numit "file". In
acest vector pe pozitia 0 se afla numele fisierului iar pe pozitia 1 se afla
numarul total de pachete ce va fi trimis (util pentru a sti cand nu mai trebuie
receiverul sa astepte pachete). Incepand cu pozitia 2 este fisierul efectiv.

In implementarea ferestrei folosesc urmatoarele variabile importante:
1. int receivedAcks - int in care retin cate pachete mi-au fost confirmate ca au ajuns
cu succes.
2. int windowSize - marimea ferestrei
3. int messagesOnWire - cate mesaje am trimis si nu au fost inca confirmate
4. int lastIndex - numarul total de pachete

Pentru implementarea ferestrei am procedat in felul urmator:
Cat timp numarul de pachete care au fost confirmate ca au ajuns corect (receivedAcks)
este mai mic decat numarul total de pachete:
1. Daca numarul de mesaje trimise si neconfirmate (messagesOnWire) este egal
(sau mai mare, pentru orice caz) cu dimensiunea ferestrei, atunci astept ACK-urile.
2. Daca numarul de mesaje neconfirmate (lastIndex - receivedAcks) este mai mic decat
dimensiunea ferestrei (windowSize), atunci astept ACK-uri. Asta se intampla cand
mai este putin din fisier de transferat (la sfarsit).
3. Cat timp numarul de mesaje trimise si neconfirmate (messagesOnWire) este mai
mic decat dimensiunea ferestrei (windowSize), atunci trimit pachetele netrimise.
Cand un pachet este confirmat, in vectorul de pachete, ii fac campul ack = -1.

In receiver mi-am declarat un vector de mesaje de tip my_pkt. Astept pachete cat
timp numarul de pachete pe care trebuie sa le primesc este mai mare decat numarul
de pachete deja primite. Verific daca au fost corupte, si daca nu, trimit ACK
pentru ele. Apoi scriu fisierul.
