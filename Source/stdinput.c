#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define true 1
#define false 0
#define bool _Bool
#define LOWDEFSP 64
#define HIGHDEFSPACE 524288
#define BUFFERINTDIM 12
#define INPUTSIZE 200

//definizione variabili complesse
typedef char* string;

//pre-dichiarazione strutture per evitare errori
typedef struct ar arc;
typedef arc* arrayArc;

//dichiarazione strutture
typedef struct {
    string entName;
    arrayArc* arcsIn;
    int maxIn;
    int occIn;
    arrayArc* arcsOut;
    int maxOut;
    int occOut;
} node;
typedef node* nodesArray;

typedef struct {
    node* ent;
    int score;
} player;
typedef player* leadboard;

typedef struct {
    string relName;
    leadboard* l1;
    int toPlayerScore;
    int dim_lboard;
    int occpied_lboard;
} relat;
typedef relat* relationsArray;

typedef struct ar {
    node* entTo;
    relat* rel;
} arc;

//variabili globali
nodesArray* entsArray;
relationsArray* relsArray;
int maxEntsArray;
int maxRelsArray;
int occRelsArray;

//FUNZIONI

//FUNZIONE DI HASH
int hashFunction(string x) {
    int i = 0;
    int y = 0;
    while (x[i] != '\0') {
        y += ((x[i]-44)*32);
        i++;
    }
    return y;
}
//FINE FUNZIONE DI HASH


//FINE SEZIONE ADDENT
bool entYetExistingInsert(int* indxPointer, string entName)
{
    int indx = hashFunction(entName);
    if(indx > maxEntsArray-1)
    {
        *indxPointer = indx;
        return false;
    }
    while(entsArray[indx] != NULL && strcmp(entsArray[indx]->entName, entName) != 0)
        indx++;
    if(entsArray[indx] == NULL)
    {
        *indxPointer = indx;
        return false;
    }
    return true;
}

void addentFunction(string entName)
{
    int indx;
    if(entsArray == NULL)
    {
        indx = hashFunction(entName);
        if(indx < 0)
            exit(1);
        maxEntsArray = HIGHDEFSPACE;
        entsArray = calloc(maxEntsArray, sizeof(node*));
    } else {
        if(entYetExistingInsert(&indx, entName))
        {
            free(entName);
            return;
        }
        if(indx < 0)
            exit(1);
    }

    node* ent = calloc(1, sizeof(node));
    ent->entName = entName;
    if(maxEntsArray <= indx+1)
    {
        maxEntsArray = indx+2;
        entsArray = realloc(entsArray, maxEntsArray*sizeof(node*));
    }
    entsArray[indx] = ent;
}
//SEZIONE ADDENT (DONE)


//FINE SEZIONE DELENT
int binEntSearch(string enTarget, node* ent1, string targRel, bool in) {
    int right, left = 0, med, strComp, end;
    arrayArc *arr;
    if (in) {
        right = ent1->occIn - 1;
        arr = ent1->arcsIn;
        end = ent1->occIn;
    } else {
        right = ent1->occOut - 1;
        arr = ent1->arcsOut;
        end = ent1->occOut;
    }
    while (left <= right) {
        med = (left + right) / 2;
        if ((strComp = strcmp(arr[med]->entTo->entName, enTarget)) < 0)
            left = med + 1;
        if (strComp > 0)
            right = med - 1;
        if (strComp == 0) {   //devo controllare in ampiezza gli archi
            left = med;     //a sinistra
            do {
                if (strcmp(arr[left]->rel->relName, targRel) == 0)  //se trovo anche la rel allora ritorno left
                    return left;
                left--;
            } while (left >= 0 && strcmp(arr[left]->entTo->entName, enTarget) == 0);
            right = med + 1;        //non trovata a sinistra, vado a destra
            while (right < end && strcmp(arr[right]->entTo->entName, enTarget) == 0) {
                if (strcmp(arr[right]->rel->relName, targRel) == 0)   //lo stesso
                    return right;
                right++;
            }
            return -1;   //combinazione entName && relName non presente
        }
    }
    return -1;      //non trovata la ent di destinazione negli archi uscenti da ent1
}

bool binPlayerSearch(string target, leadboard* l1, int right, int* indxPointer)
{
    int left=0, med, strcompare;
    while(left<=right)
    {
        med = (left+right)/2;
        if((strcompare = strcmp(l1[med]->ent->entName, target)) < 0)
            left = med + 1;
        else if(strcompare > 0)
            right = med - 1;
        else if(strcompare == 0)
        {
            *indxPointer = med;
            return true;        //found
        }
    }
    *indxPointer = left;
    return false;  //not found
}

int binRelSearch(string target)
{
    int right=occRelsArray-1, left=0, med, strComp;
    while(left <= right)
    {
        med = (left+right)/2;
        if((strComp = strcmp(relsArray[med]->relName, target)) < 0)
            left = med+1;
        else if(strComp > 0)
            right = med-1;
        else if(strComp == 0)
            return med;
    }
    return left;
}

void removeArcOut(string targEnt, arc* a1)
{ //(parte entTo) rimuovo collegamento a targEnt da archi entranti nell'entità di arrivo
    node* entFrom = a1->entTo;
    relat* rel = a1->rel;
    string enToScale = entFrom->entName;
    int indx = binEntSearch(targEnt, entFrom, rel->relName, true);
    if(indx < 0 || indx > entFrom->occIn-1)
        exit(1);
    arrayArc* arr = entFrom->arcsIn;
    free(arr[indx]);
    for (int max = entFrom->occIn; indx<max; indx++)
        arr[indx] = arr[indx+1];
    entFrom->arcsIn = arr;
    if(entFrom->occIn == 1)
    {
        free(entFrom->arcsIn);
        entFrom->arcsIn = NULL;
        entFrom->maxIn = 0;
        entFrom->occIn = 0;
    } else {
        if (entFrom->occIn == entFrom->maxIn - LOWDEFSP)
        {
            entFrom->maxIn -= LOWDEFSP;
            entFrom->arcsIn = realloc(entFrom->arcsIn, (entFrom->maxIn) * sizeof(arc *));
        }
        entFrom->occIn--;
    }


    //(parte rel) scalo lo score in rel dell'ent salvata in playerToScale o se 0 elimino player
    int indxPlayer;
    binPlayerSearch(enToScale, rel->l1, (rel->occpied_lboard)-1, &indxPlayer);
    if(indxPlayer < 0 || indxPlayer > rel->occpied_lboard-1)
        exit(1);
    leadboard* l1 = rel->l1;
    player* p1 = l1[indxPlayer];
    if(p1->score == 1)
    { //se score==1 elimino
        free(p1);
        if(rel->occpied_lboard == 1)
        {
            int indexRel = binRelSearch(rel->relName);
            if(indexRel < 0 || indexRel > occRelsArray-1)
                exit(1);
            for (int right = occRelsArray; indexRel < right; indexRel++)
                relsArray[indexRel] = relsArray[indexRel+1];
            free(rel->relName);
            free(rel);
            if(occRelsArray == maxRelsArray-LOWDEFSP)
            {
                maxRelsArray -= LOWDEFSP;
                relsArray = realloc(relsArray, maxRelsArray*sizeof(relat*));
            } else
                if(occRelsArray == 1)
                {
                    free(relsArray);
                    relsArray = NULL;
                    maxRelsArray = 0;
                }
            occRelsArray--;
            return;
        } else {
            for(int right = rel->occpied_lboard; indxPlayer < right; indxPlayer++)
                l1[indxPlayer] = l1[indxPlayer+1];
            rel->l1 = l1;
            if(rel->occpied_lboard == rel->dim_lboard-LOWDEFSP)
            {
                rel->dim_lboard -= LOWDEFSP;
                rel->l1 = realloc(rel->l1, (rel->dim_lboard)*sizeof(player*));
            }
            rel->occpied_lboard--;
            return;
        }
    } else { //altrimenti riduco score e aggiorno toPlayerScore
        p1->score--;
        int targScore = p1->score;
        for (int right = rel->occpied_lboard-1; right >= 0; right--) //se trovo un player con score maggiore del player che sto valutando, allora questo non è più topPlayer
            if (l1[right]->score > targScore)
            {
                targScore = -1;
                right = -1;
            }
        if (targScore != -1)
            rel->toPlayerScore--;
    }
}

void removeArcIn(string targEnt, arc* a1)
{   //(entTo) rimuovo collegamento a targEnt da archi uscenti dall'entità di partenza
    node* entFrom = a1->entTo;
    relat* rel = a1->rel;
    int indx = binEntSearch(targEnt, entFrom, rel->relName, false);
    if(indx < 0 || indx > entFrom->occOut-1)
        exit(1);
    arrayArc* arr = entFrom->arcsOut;
    free(arr[indx]);
    for (int max = entFrom->occOut; indx<max; indx++)
        arr[indx] = arr[indx+1];
    entFrom->arcsOut = arr;
    if(entFrom->occOut == 1)
    {
        free(entFrom->arcsOut);
        entFrom->arcsOut = NULL;
        entFrom->maxOut = 0;
        entFrom->occOut = 0;
    } else {
        if(entFrom->occOut == entFrom->maxOut-LOWDEFSP)
        {
            entFrom->maxOut -= LOWDEFSP;
            entFrom->arcsOut = realloc(entFrom->arcsOut, (entFrom->maxOut)*sizeof(arc*));
        }
        entFrom->occOut--;
    }

    //(rel) scalo score di player->entName o lo elimino
    binPlayerSearch(targEnt, rel->l1, rel->occpied_lboard-1, &indx);
    if(indx < 0 || indx > rel->occpied_lboard-1)
        exit(1);
    leadboard* l1 = rel->l1;
    player* p1 = l1[indx];
    //se score != 1 lo abbasso e aggiorno topPlayer eventualmente
    if(p1->score > 1)
    {
        p1->score--;
        int targScore = p1->score;
        for(int right = rel->occpied_lboard-1; right>=0; right--) //se trovo un player con score maggiore del player che sto valutando, allora questo non è più topPlayer
            if(l1[right]->score > targScore)
            {
                targScore = -1;
                right = -1;
            }
        if(targScore != -1)
            rel->toPlayerScore--;
        return;
        //se score == 1 allora libero player e se è l'ultimo rimasto, libero la relazione
    }                   //se score == 1, elimino player
    free(l1[indx]);
    for (int max = rel->occpied_lboard; indx < max; indx++)
        l1[indx] = l1[indx+1];
    rel->l1 = l1;
    if(rel->occpied_lboard == 1)
    {
        int indexRel = binRelSearch(rel->relName);
        if(indexRel < 0 || indexRel > occRelsArray-1)
            exit(1);
        free(rel->relName);
        free(rel);
        for(int right=occRelsArray; indexRel < right; indexRel++)
            relsArray[indexRel] = relsArray[indexRel+1];
        if(occRelsArray == maxRelsArray-LOWDEFSP)
        {
            maxRelsArray -= LOWDEFSP;
            relsArray = realloc(relsArray, maxRelsArray*sizeof(relat*));
        } else
            if(occRelsArray == 1)
            {
                free(relsArray);
                relsArray = NULL;
                maxRelsArray = 0;
            }
        occRelsArray--;
    } else {
        if(rel->occpied_lboard == rel->dim_lboard-LOWDEFSP)
        {
            rel->dim_lboard-=LOWDEFSP;
            rel->l1 = realloc(rel->l1, (rel->dim_lboard)*sizeof(player*));
        }
        rel->occpied_lboard--;

    }
}

bool entYetExistingSearch(int* indxPointer, string entName)
{
    int indx = hashFunction(entName);
    if(indx > maxEntsArray-1)
        return false;
    while(entsArray[indx] != NULL && strcmp(entsArray[indx]->entName, entName) != 0)
        indx++;
    if(entsArray[indx] == NULL)
        return false;
    *indxPointer = indx;
    return true;
}

void delentFunction(string entName) {
    if (entsArray == NULL) //comando non valido
    {
        free(entName);
        return;
    }
    int indxEnt;
    if (!entYetExistingSearch(&indxEnt, entName)) //se non esiste comando non valido
    {
        free(entName);
        return;
    }
    node* ent1 = entsArray[indxEnt];

    //rimuovo archi entranti e quindi player da ogni rel
    for(int i=0; i < ent1->occIn; i++)
    {
        removeArcIn(entName, ent1->arcsIn[i]);
        free(ent1->arcsIn[i]);
    }

    //rimuovo archi uscenti quindi scalo le posizioni dei players collegati a questa entità
    for(int i=0; i < ent1->occOut; i++)
    {
        removeArcOut(entName, ent1->arcsOut[i]);
        free(ent1->arcsOut[i]);
    }
    free(entName);      //libero l'argomento passato da input

    //libero la entName dell'ent nell'array e l'elemento entità dall'array
    free(ent1->entName);
    free(ent1);
    while(entsArray[indxEnt] != NULL)  //scorro eventuali collisioni
    {
        entsArray[indxEnt] = entsArray[indxEnt+1];
        indxEnt++;
    }
    if(indxEnt == maxEntsArray-1) {
        while (indxEnt >= 0 && entsArray[indxEnt] == NULL)
            indxEnt--;
        if (indxEnt < 0)
        {
            free(entsArray);
            entsArray = NULL;
            maxEntsArray = 0;
        } else
            if(indxEnt < maxEntsArray-1)
            {
                maxEntsArray = indxEnt+2;
                entsArray = realloc(entsArray, maxEntsArray*sizeof(node*));
            }
    }
}
//SEZIONE DELENT


//FINE ADDREL
relat* createNewRel(string relName, int indexEnt2InArray)
{
    //creo lo spazio per la relazione e ci salvo il nome
    relat* rel = malloc(sizeof(relat));
    rel->relName = relName;

    //creo il nuovo player (se nuova relazione vuol dire no player presenti)
    player* p1 = malloc(sizeof(player));
    p1->ent = entsArray[indexEnt2InArray];
    p1->score = 1;

    //creo leaderboard per la relazione
    rel->l1 = calloc(LOWDEFSP, sizeof(player*));
    rel->dim_lboard = LOWDEFSP;

    //salvo nella leaderboard il player in prima posizione dell'array
    rel->l1[0] = p1;
    rel->occpied_lboard = 1;
    rel->toPlayerScore = 1;

    //ritorno relazione da inserire in array di relazioni
    return rel;
}

void insertSortedPlayer(string target, relat* relat1, int indexEnt2InEntsArray)
{
    leadboard* l1 = relat1->l1;
    int indx;
    if(!binPlayerSearch(target, l1, (relat1->occpied_lboard)-1, &indx)) //se non l'ho trovato
    { //alloco in index shiftando tutto
        for(int right=relat1->occpied_lboard; right>indx; right--)
            l1[right] = l1[right-1];
        player* p1 = malloc(sizeof(player));
        p1->ent = entsArray[indexEnt2InEntsArray];
        p1->score = 1;
        l1[indx] = p1;
        relat1->l1 = l1;
        if(relat1->occpied_lboard == (relat1->dim_lboard)-1) //se leaderboard full ingrandisco
        {
            relat1->dim_lboard += LOWDEFSP;
            relat1->l1 = realloc(relat1->l1, (relat1->dim_lboard)*sizeof(player*));
        }
        relat1->occpied_lboard++;
        return;
    } else {
        //se trovato aumento lo score e se supera top player allora sostituisco max
        l1[indx]->score++;
        if(l1[indx]->score > relat1->toPlayerScore)
            relat1->toPlayerScore = l1[indx]->score;
    }
}

int binEntSearchInsert(string enTarget, node* ent1, string targRel, bool in)
{
    int right, left=0, med, strComp, end;
    arrayArc* arr;
    if(in)
    {
        right = ent1->occIn-1;
        arr = ent1->arcsIn;
        end = ent1->occIn;
    } else {
        right = ent1->occOut-1;
        arr = ent1->arcsOut;
        end = ent1->occOut;
    }
    while(left <= right) {
        med = (left + right) / 2;
        if ((strComp = strcmp(arr[med]->entTo->entName, enTarget)) < 0)
            left = med + 1;
        else if (strComp > 0)
            right = med - 1;
        else if (strComp == 0)
        {
            //devo controllare in ampiezza gli archi finche la entname corrisponde
            left = med;     //a sinistra
            do {
                if (strcmp(arr[left]->rel->relName, targRel) == 0)  //se trovo anche la rel allora ritorno -1 (comando non valido)
                    return -1;
                left--;
            } while (left >= 0 && strcmp(arr[left]->entTo->entName, enTarget) == 0);
            right = med + 1;        //non trovata a sinistra, vado a destra
            while (right < end && strcmp(arr[right]->entTo->entName, enTarget) == 0) {
                if (strcmp(arr[right]->rel->relName, targRel) == 0)   //lo stesso
                    return -1;
                right++;
            }
            return right;   //combinazione entName && relName non presente
        }

    }
    return left;      //non trovata la ent di destinazione negli archi uscenti da ent1

}

void addrelFunction(string ent1Name, string ent2Name, string relName)
{
    if(entsArray == NULL)
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }
    int indexEnt1InEntsArray;
    if(!entYetExistingSearch(&indexEnt1InEntsArray, ent1Name))
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }
    int indexEnt2InEntsArray;
    if(!entYetExistingSearch(&indexEnt2InEntsArray, ent2Name))
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }

    node* ent1 = entsArray[indexEnt1InEntsArray];
    node* ent2 = entsArray[indexEnt2InEntsArray];

    if(relsArray == NULL) //non ho relazioni
    {
        //creo array di relazioni e ci inserisco la relazione
        relsArray = calloc(LOWDEFSP, sizeof(relat*));
        maxRelsArray = LOWDEFSP;
        relsArray[0] = createNewRel(relName, indexEnt2InEntsArray);
        occRelsArray = 1;

        //linko arco
        arc* a1 = malloc(sizeof(arc));                      //preparo arco uscente(1)
        a1->entTo = entsArray[indexEnt2InEntsArray];
        a1->rel = relsArray[0];                             //preparo arco uscente(2)
        ent1->arcsOut = calloc(LOWDEFSP, sizeof(arc*));
        ent1->maxOut = LOWDEFSP;
        ent1->arcsOut[0] = a1;
        ent1->occOut = 1;

        //linko rel in ent ricevente
        a1 = malloc(sizeof(arc));                           //preparo arco entrante(1)
        a1->entTo = entsArray[indexEnt1InEntsArray];
        a1->rel = relsArray[0];                             //preparo arco entrante(2)
        ent2->arcsIn = calloc(LOWDEFSP, sizeof(arc*));
        ent2->maxIn = LOWDEFSP;
        ent2->arcsIn[0] = a1;
        ent2->occIn = 1;

        //finito di usarli, libero gli argomenti di input (tranne la relName dato che è in uso)
        free(ent1Name);
        free(ent2Name);
        return;

    } else {    //array non vuoto
        //se valido, collego arco uscente da ent1
        int indxOut;
        if(ent1->maxOut == 0)       //se ent1 non ha ancora relazioni so che la addrel è valida
        {
            ent1->maxOut = LOWDEFSP;
            ent1->arcsOut = calloc(LOWDEFSP, sizeof(arc*));
            indxOut = 0;
        } else {
            indxOut = binEntSearchInsert(ent2Name, ent1, relName, false);         //restituisce l'indice del successivo (se -1 comando non valido)
            if(indxOut == -1)
            {
                free(ent1Name);
                free(ent2Name);
                free(relName);
                return;
            }
            if (ent1->occOut == (ent1->maxOut)-1)
            {
                ent1->maxOut += LOWDEFSP;
                ent1->arcsOut = realloc(ent1->arcsOut, (ent1->maxOut)*sizeof(arc*));
            }

        }

        //da qua so che la addrel è valida
        arc* a1 = malloc(sizeof(arc));          //alloco arco (per uscente da ent1)
        a1->entTo = entsArray[indexEnt2InEntsArray];
        arrayArc* arr = ent1->arcsOut;
        for(int right = ent1->occOut; right > indxOut; right--)
            arr[right] = arr[right-1];
        arr[indxOut] = a1;                      //assegno arco a array di uscenti da ent1
        ent1->arcsOut = arr;
        ent1->occOut++;

        //cerco indice relazione in relsArray (restituisce o indice corrispondente o dell'elemento successivo)
        int indxRel = binRelSearch(relName);
        if(indxRel < 0)
            exit(1);

        //controllo se rel esiste gia nell'array
        int strCmp;
        if(relsArray[indxRel] == NULL || (strCmp = strcmp(relsArray[indxRel]->relName, relName)) != 0) {        //se non esiste creo la relazione
            if(occRelsArray == maxRelsArray-1) //se relsarray pieno ingrandisco
            {
                maxRelsArray += LOWDEFSP;
                relsArray = realloc(relsArray, maxRelsArray*sizeof(relat*));
            }
            for(int right = occRelsArray; right > indxRel; right--)
                relsArray[right] = relsArray[right-1];
            relsArray[indxRel] = createNewRel(relName, indexEnt2InEntsArray);
            occRelsArray++;
        } else {
            if(strCmp == 0)      //se relName esiste già allora inserisco il player o ne aggiorno lo score
                insertSortedPlayer(ent2Name, relsArray[indxRel], indexEnt2InEntsArray);
            free(relName);
        }
        a1->rel = relsArray[indxRel];

        //collego ad arco entrante in ent2
        int indxIn;
        if(ent2->maxIn == 0)       //se ent2 non ha ancora archi entranti alloco
        {
            ent2->maxIn = LOWDEFSP;
            ent2->arcsIn = calloc(LOWDEFSP, sizeof(arc*));
            indxIn = 0;
        } else {
            indxIn = binEntSearchInsert(ent1Name, ent2, relName, true);         //restituisce l'indice del successivo (se -1 comando non valido) TODO PROBLEMAAA
            if(indxIn < 0)
                exit(1);
            if(ent2->occIn == (ent2->maxIn)-1)     //se pieno ingrandisco
            {
                ent2->maxIn += LOWDEFSP;
                ent2->arcsIn = realloc(ent2->arcsIn, (ent2->maxIn)*sizeof(arc*));
            }
        }
        //alloco arco
        a1 = malloc(sizeof(arc));
        a1->entTo = entsArray[indexEnt1InEntsArray];
        arr = ent2->arcsIn;
        a1->rel = relsArray[indxRel];
        for(int right = ent2->occIn; right>indxIn; right--)
            arr[right] = arr[right-1];
        arr[indxIn] = a1;
        ent2->arcsIn = arr;
        ent2->occIn++;
        free(ent1Name);
        free(ent2Name);
    }

}
//SEZIONE ADDREL (DONE)


//FINE DELREL
int binRelInArcSearch(string targRelName, node* entFrom, string enTargetName, bool in)
{
    int right, left=0, med, strComp, end;
    arrayArc* arr;
    if(in)
    {
        right = entFrom->occIn-1;
        arr = entFrom->arcsIn;
        end = entFrom->occIn;
    } else {
        right = entFrom->occOut-1;
        arr = entFrom->arcsOut;
        end = entFrom->occOut;
    }
    while(left <= right) {
        med = (left + right) / 2;
        if ((strComp = strcmp(arr[med]->entTo->entName, enTargetName)) < 0)
            left = med + 1;
        else if (strComp > 0)
            right = med - 1;
        else if (strComp == 0)
        {
            //devo controllare in ampiezza gli archi
            left = med;     //a sinistra
            do {
                if (strcmp(arr[left]->rel->relName, targRelName) == 0)  //se trovo anche la rel allora ritorno l'indice (comando valido)
                    return left;
                left--;
            } while (left >= 0 && strcmp(arr[left]->entTo->entName, enTargetName) == 0);
            right = med + 1;        //non trovata a sinistra, vado a destra
            while (right < end && strcmp(arr[right]->entTo->entName, enTargetName) == 0) {
                if (strcmp(arr[right]->rel->relName, targRelName) == 0)   //lo stesso
                    return right;
                right++;
            }
            return -1;   //combinazione entName && relName non presente (comando non valido)
        }

    }
    return -1;      //non trovata la ent di destinazione negli archi uscenti da ent1 (comando non valido)
}

void delrelFunction(string ent1Name, string ent2Name, string relName)
{   //se non esiste almeno un'entità o la relazione, non fare niente
    if(entsArray == NULL || relsArray == NULL)
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }
    int indxEnt1;
    if(!entYetExistingSearch(&indxEnt1, ent1Name))
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }
    int indxEnt2;
    if(!entYetExistingSearch(&indxEnt2, ent2Name))
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }
    int indxRel= binRelSearch(relName);
    if(relsArray[indxRel] == NULL || strcmp(relsArray[indxRel]->relName, relName) != 0) //se non esiste o quella che trovo non è giusta ritorno
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }
    node* ent1 = entsArray[indxEnt1];
    node* ent2 = entsArray[indxEnt2];
    relat* rel = relsArray[indxRel];

    //cerco gli indici di ogni array
    int indxArcOut = binRelInArcSearch(relName, ent1, ent2Name, false);
    if(indxArcOut == -1) //se almeno il primo indice calcolato è negativo allora non esiste la relazione tra le due entità
    {
        free(ent1Name);
        free(ent2Name);
        free(relName);
        return;
    }
    //da qua sono sicuro che il comando sia corretto
    int indxArcIn = binRelInArcSearch(relName, ent2, ent1Name, true);
    if(indxArcIn < 0)
        exit(1);                //TODO trouble
    int indxPlayer;
    binPlayerSearch(ent2Name, rel->l1, rel->occpied_lboard-1, &indxPlayer);
    if(indxPlayer < 0)
        exit(1);
    //calcolati gli indici libero gli argomenti di input
    free(ent1Name);
    free(ent2Name);
    free(relName);

    //libero l'arco out da ent1
    arrayArc* arr = ent1->arcsOut;
    free(arr[indxArcOut]);
    for(int max = ent1->occOut; indxArcOut<max; indxArcOut++)
        arr[indxArcOut] = arr[indxArcOut + 1];
    ent1->arcsOut = arr;
    if(ent1->occOut == 1)
    {
        free(ent1->arcsOut);
        ent1->arcsOut = NULL;
        ent1->maxOut = 0;
        ent1->occOut = 0;
    } else {
        if(ent1->occOut == (ent1->maxOut)-LOWDEFSP)
        {
            ent1->maxOut -= LOWDEFSP;
            ent1->arcsOut = realloc(ent1->arcsOut, (ent1->maxOut)*sizeof(arc*));
        }
        ent1->occOut--;
    }

    //arco in da ent2
    arr = ent2->arcsIn;
    free(arr[indxArcIn]);
    for(int max = ent2->occIn; indxArcIn<max; indxArcIn++)
        arr[indxArcIn] = arr[indxArcIn + 1];
    ent2->arcsIn = arr;
    if(ent2->occIn == 1)
    {
        free(ent2->arcsIn);
        ent2->arcsIn = NULL;
        ent2->maxIn = 0;
        ent2->occIn = 0;
    } else {
        if(ent2->occIn == (ent2->maxIn)-LOWDEFSP)
        {
            ent2->maxIn -= LOWDEFSP;
            ent2->arcsIn = realloc(ent2->arcsIn, (ent2->maxIn)*sizeof(arc*));
        }
        ent2->occIn--;
    }

    //riduco lo score del player ent2Name
    player* p1 = rel->l1[indxPlayer];
    leadboard* l1 = rel->l1;
    if(p1->score == 1)      //se score == 1 elimino il player
    {
        for(int max = rel->occpied_lboard; indxPlayer<max; indxPlayer++)
            l1[indxPlayer] = l1[indxPlayer+1];
        rel->l1 = l1;
        free(p1);
        if(rel->occpied_lboard == 1)        //libero la rel
        {
            free(rel->relName);
            free(rel);
            for(int right=occRelsArray; indxRel<right; indxRel++)
                relsArray[indxRel] = relsArray[indxRel+1];
            if(occRelsArray == 1)           //libero il relsArray
            {
                free(relsArray);
                relsArray = NULL;
                maxRelsArray = 0;
                occRelsArray = 0;
            } else {                        //riduco gli occupati nel relsArray
                if(occRelsArray == maxRelsArray-LOWDEFSP)
                {
                    maxRelsArray -= LOWDEFSP;
                    relsArray = realloc(relsArray, maxRelsArray*sizeof(relat*));
                }
                occRelsArray--;
            }
            return;
        } else {            //se occupati nella board di rel > 1, riduco gli occupati
            if(rel->occpied_lboard == (rel->dim_lboard)-LOWDEFSP)
            {
                rel->dim_lboard -= LOWDEFSP;
                rel->l1 = realloc(rel->l1, (rel->dim_lboard)*sizeof(player*));
            }
            rel->occpied_lboard--;
            return;
        }
    } else {                //se score > 1 devo abbassarlo dopo aver aggiornato il topPlayer
        p1->score--;
        int targScore = p1->score;
        for(int right = rel->occpied_lboard-1; right>=0; right--) //se trovo un player con score maggiore del player che sto valutando, allora questo non è più topPlayer
            if(l1[right]->score > targScore)
            {
                targScore = -1;
                right = -1;
            }
        if(targScore != -1)
            rel->toPlayerScore--;
    }
}
//SEZIONE DELREL (DONE)


//FINE REPORT
void stampTopPlayers(relat* rel, string dApexes, string space)
{
    int topScore = rel->toPlayerScore;
    leadboard* l1 = rel->l1;
    char bufferInt[BUFFERINTDIM];
    char semiColoumn[] = {';', '\0'};
    for(int i=0; i<rel->occpied_lboard; i++)
    {
        if(l1[i]->score == topScore)
        {
            fputs(space, stdout);
            fputs(dApexes, stdout);
            fputs(l1[i]->ent->entName, stdout);
            fputs(dApexes, stdout);
        }

    }
    fputs(space, stdout);
    sprintf(bufferInt, "%d", topScore);
    fputs(bufferInt, stdout);
    fputs(semiColoumn, stdout);
}

void reportFunction()
{
    char dApexes[] = {'"', '\0'};
    char space[] = {' ', '\0'};
    char newLine[] = {'\n', '\0'};
    if(occRelsArray == 0)
    {
        char none[] = {'n', 'o', 'n', 'e', '\0'};
        fputs(none, stdout);
    } else
        for(int i=0; i<occRelsArray; i++)
        {
            fputs(dApexes, stdout);
            fputs(relsArray[i]->relName, stdout);
            fputs(dApexes, stdout);
            stampTopPlayers(relsArray[i], &dApexes[0], &space[0]);
            if(i != occRelsArray-1)
                fputs(space, stdout);
        }
    fputs(newLine, stdout);
}
//SEZIONE REPORT (DONE)


//FINE SEZIONE ACQUISIZIONE INPUT
int strngcpy(string from, string* to, int startingIndex)
{
    int dim=1;
    for(int i=startingIndex; from[i] != '"'; i++)
        dim++;
    *to = malloc(dim*sizeof(char));
    for(int i=0; from[startingIndex] != '"'; i++)
    {
        (*to)[i] = from[startingIndex];
        startingIndex++;
    }
    (*to)[dim-1] = '\0';
    return startingIndex+3; // 1 spazio 2 " 3 primo carattere utile successivo
}

void cmdRecognize(string input)
{
    //basta controllare terza e quarta lettera dell'input
    string ent1, ent2, rel;
    int newIndex;
    switch (input[2])
    {
        case 'd':
            switch (input[3])
            {
                case 'e': //addent
                    strngcpy(input, &ent1, 8);
                    addentFunction(ent1);
                    break;
                case 'r': //addrel
                    newIndex = strngcpy(input, &ent1, 8);
                    newIndex = strngcpy(input, &ent2, newIndex);
                    strngcpy(input, &rel, newIndex);
                    addrelFunction(ent1, ent2, rel);
                    break;
            }
            break;
        case 'l':
            switch (input[3])
            {
                case 'e': //delent
                    strngcpy(input, &ent1, 8);
                    delentFunction(ent1);
                    break;
                case 'r': //delrel
                    newIndex = strngcpy(input, &ent1, 8);
                    newIndex = strngcpy(input, &ent2, newIndex);
                    strngcpy(input, &rel, newIndex);
                    delrelFunction(ent1, ent2, rel);
                    break;
            }
            break;
        case 'p': //report
            reportFunction();
            break;
    }
}

void processInput()
{
    char input[INPUTSIZE];
    scanf("%[^\n]s", input);
    getc(stdin);
    if(input[0] == 'e')     //unico comando che comincia per e è end
        exit(0);
    cmdRecognize(input);
}
//SEZIONE ACQUISIZIONE INPUT


int main()
{
    entsArray = NULL;
    relsArray = NULL;
    while(1) {
        processInput();
    }
    return 1;
}
