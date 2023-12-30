# Prova Finale di Algoritmi e Strutture Dati

## Descrizione problema
### Introduzione
- **Obiettivo:** Implementare efficientemente un algoritmo corretto.
- **Logistica:**
  - Il codice sorgente sarà caricato su un server, compilato e eseguito automaticamente.
  - Scadenze strettamente vincolanti: 10 luglio per i laureandi di luglio, 12 settembre per gli altri.
  - È essenziale gestire il tempo in modo adeguato per il successo del progetto.

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

### Requisito del sistema
- Restituire, su richiesta, per ogni relazione, l'entità che riceve il maggior numero di relazioni.
- In caso di più entità con lo stesso numero massimo di relazioni ricevute, stamparle in ordine crescente di identificativo.

### Valutazione
- Il programma deve compilare e girare correttamente.
- Saranno forniti casi di test per verificare il funzionamento del programma.
- Valutazione basata sulla correttezza dei risultati e sull'efficienza (tempi di risposta e uso della memoria).
- Il voto sarà determinato in base ai risultati sui casi di test.

---

## Implementazione effettuata
Ho implementato un metodo basato su HashMap per risolvere il problema corrente, ottimizzando l'accesso e la gestione delle relazioni tra entità.
TODO: implementare una versione del'algoritmo utilizzando un approccio basato su ricerca a tree per ottimizzare ulteriormente le prestazioni e l'efficienza del programma.