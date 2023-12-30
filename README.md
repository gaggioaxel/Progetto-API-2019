# Prova Finale di Algoritmi e Strutture Dati - a.a. 2018-2019

## Descrizione problema
### Introduzione
- **Obiettivo:** Implementare efficientemente un algoritmo corretto.

### Esecuzione del progetto
- Implementazione richiesta nel linguaggio C, utilizzando esclusivamente la libreria standard (libc).
- Non si devono utilizzare thread o tecniche di parallelizzazione.

### Descrizione astratta del problema
Il sistema monitora vari fenomeni relativi a entità e relazioni tra di esse:
- Inizio o fine del monitoraggio di un'entità.
- Stabilimento o cessazione di relazioni tra entità.
- Ogni entità è identificata da un nome (es. "Alice", "Bruno").
- Le relazioni tra entità sono identificate da nomi specifici (es. "amico_di", "segue").
- Ogni relazione ha un verso e non è necessariamente simmetrica.

### Descrizione formulazione del problema
Attraverso i seguenti comandi:

  -  addent: si creano nuove entità.
  -  addrel: si stabiliscono nuove relazioni tra entità esistenti.
  -  delent: si eliminano entità esistenti.
  -  delrel: si eliminano relazioni esistenti tra entità.
  -  report: serve per verificare la corretta gestione del sistema.

Il comando report stampa su standard output, in ordine alfabetico, tutte le relazioni gestite dal sistema. Per ogni relazione, viene mostrata l'entità che riceve il maggior numero di relazioni di quel tipo. Nel caso in cui più entità ricevano lo stesso numero di una relazione, esse vengono considerate in ordine alfabetico prima del numero di occorrenze.

### Requisito del sistema
- Restituire, su richiesta, per ogni relazione, l'entità che riceve il maggior numero di relazioni.
- In caso di più entità con lo stesso numero massimo di relazioni ricevute, stamparle in ordine crescente di identificativo.

## Implementazione effettuata
Ho implementato un metodo basato su Hashmap e ricerca binaria per risolvere il problema corrente, ottimizzando l'accesso e la gestione delle relazioni tra entità a discapito di un maggiore utilizzo di memoria, riallocata in maniera dinamica.
TODO: implementare una versione del'algoritmo che utilizzi un approccio basato su alberi binari per ottimizzare ulteriormente le prestazioni e ridurre l'uso di memoria.

## Test
### Casi di Test
L'implementazione è stata soggetta a diversi casi di test, sottoponendola a [Test Pubblici](https://github.com/gaggioaxel/Progetto-API-2019/tree/main/Public_Tests) e successivamente valutata attraverso [Test Privati](https://github.com/gaggioaxel/Progetto-API-2019/tree/main/Private_Tests) tramite una piattaforma specifica. Questi test sono stati progettati per verificare la corretta gestione delle relazioni tra entità, valutando la memoria occupata e il tempo di esecuzione del programma.


### Compilazione ed Esecuzione
Per eseguire il programma, è necessario compilare il file [fileinput.c](https://github.com/gaggioaxel/Progetto-API-2019/blob/main/Source/fileinput.c) dalla linea di comando utilizzando i seguenti flag:

`gcc -Wmaybe-uninitialized -Wuninitialized -Wall -pedantic -Werror -g3 main.c -o main`

Per testare il programma, è possibile eseguirlo utilizzando uno degli input pubblici disponibili e confrontare l'output prodotto con il relativo output atteso:

```
cat input.txt | ./main > output.txt
diff output.txt public_output.txt
```