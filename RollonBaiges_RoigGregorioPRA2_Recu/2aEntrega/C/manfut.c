/* ---------------------------------------------------------------
Práctica 1.
Código fuente: ConcManfut.c
Grau Informàtica
39939768S - Pere Antoni Rollon Baiges
47433543E - Francisco Javier Roig Gregorio
--------------------------------------------------------------- */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include "manfut.h"


#define GetPorter(j) (Jugadors[j])
#define GetDefensor(j) (Jugadors[NPorters + j])
#define GetMitg(j) (Jugadors[NPorters + NDefensors + j])
#define GetDelanter(j) (Jugadors[NPorters + NDefensors + NMitjos + j])
#define DEFAULT_MANFUT_THREADS 2
#define MANFUT_BUFFER_LIMIT 1024
#define DEFAULT_MANFUT_M 25000
#define MAX_MESSAGES 100

char *color_orange = "\033[38;5;209m";
char *color_yellow = "\033[93m";
char *color_red = "\033[01;31m";
char *color_green = "\033[01;32m";
char *color_blue = "\033[01;34m";
char *end_color = "\033[00m";

//----Estructura de dades-----------------------------------------------------------------------------------------------
struct TeamInterval
{
    TEquip inicio;
    TEquip final;
    long int PresupostFitxatges;
};
typedef struct TeamInterval TeamInterval;

struct ManfutStats
{
    int evalComb;
    int invld;
    int vld;

    float avg_cost;
    float avg_points;

    TEquip bestName;
    TJugadorsEquip best;
    int best_cost;
    int best_points;

    TEquip worstName;
    TJugadorsEquip worst;
    int worst_cost;
    int worst_points;
};
typedef struct ManfutStats ManfutStats;

// Definition functions prototype---------------------------------------------------------------------------------------
void LlegirFitxerJugadors(char *pathJugadors);
void CalcularEquipOptim(long int PresupostFitxatges,  TJugadorsEquip *  MillorEquip);
TBoolean ObtenirJugadorsEquip (TEquip equip,  TJugadorsEquip *  jugadors);
TEquip GetEquipInicial();
TBoolean JugadorsRepetits(TJugadorsEquip jugadors);
int CostEquip(TJugadorsEquip equip);
int PuntuacioEquip(TJugadorsEquip equip);
void error(char *str);
unsigned int Log2(unsigned long long int n);
void PrintJugadors();
void PrintEquipJugadors(TJugadorsEquip equip);
void cancel_threads(pthread_t Theads[]);

// Global variables definition
TJugador Jugadors[DMaxJugadors];
int NJugadors, NPorters, NDefensors, NMitjos, NDelanters;
char cad[256];

//Manfut Shared Variables-------------------------------------------------
ManfutStats general_stats;
int num_threads;
int remainingThreads=0;
int M;

//Market shared variables-------------------------------------------------
int pending_local_print = 0;
TJugadorsEquip Shared_MillorEquip;
int MaxPuntuacio = -1;

// ManfutThread class thread ---------------------------------------------
void *CalcularEquipOptim_Thread(TeamInterval *input);
void print_local_statistics(ManfutStats local_statistics, char *buffer);
pthread_mutex_t mutex_shared_variables = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t JoinBarrier;

// ManfutMessages class Thread -------------------------------------------
void * messages_Thread();
void printAll_();
void sendToStorage(char *message);
void stop_();
char storage[MAX_MESSAGES][1024];
int iterator = 0;
bool running = false;
pthread_mutex_t mutex_messages_thread = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t addCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t stopped_thread = PTHREAD_COND_INITIALIZER;
sem_t semaphore_messages_thread;

// ManfutStats class Thread -------------------------------------------
void make_statistics(ManfutStats *local_statistics,TJugadorsEquip jugadors, int costEquip, int puntuacioEquip,
                     long int PresupostFitxatges, TEquip equip);
void sync_local_statistics(ManfutStats stats);
void print_(ManfutStats statistics, char *buffer);
void print_statistics_(char *buffer);
void reset_(ManfutStats *statistics);

int main(int argc, char *argv[])
{

    TJugadorsEquip MillorEquip, AuxEquip;
    long int PresupostFitxatges;
    float IntervalBegin=-1, IntervalEnd=-1;

    if (argc<3)
        error("Error in arguments: ManFut <presupost> <fitxer_jugadors> <fitxer_jugadors> [num_threads] [M]");

    if (argc>1)
        PresupostFitxatges = atoi(argv[1]);

    if (argc>2)
        LlegirFitxerJugadors(argv[2]);
    if (argc>3){
        if (atoi(argv[3])>0){
            num_threads =  atoi(argv[3]);
            if (num_threads==1){
                sprintf (cad,"%sSecuential mode%s\n",color_red,end_color);
                write(1,cad,strlen(cad));
            }
        } else{
            sprintf (cad,"%sNegative numbers are not allowed, setting to defaults num_threads=2%s\n",color_red,end_color);
            write(1,cad,strlen(cad));
            num_threads=DEFAULT_MANFUT_THREADS;
        }

    } else{
        sprintf (cad,"%sNo  <Num threads>, setting to defaults num_threads=2%s\n",color_red,end_color);
        write(1,cad,strlen(cad));
        num_threads = DEFAULT_MANFUT_THREADS;
    }
    if (argc>4){
        if (atoi(argv[4])>0){
            M = atoi(argv[4]);
        } else{
            sprintf (cad,"%sNegative numbers are not allowed, setting to defaults M=25000%s\n",color_red,end_color);
            write(1,cad,strlen(cad));
            M=DEFAULT_MANFUT_M;
        }
    }else {
        sprintf (cad,"%sNo  <M>, setting to defaults M=25000%s\n",color_red,end_color);
        write(1,cad,strlen(cad));
        M = DEFAULT_MANFUT_M;
    }



    // Calculate the best team.
    CalcularEquipOptim(PresupostFitxatges, &MillorEquip);


    write(1,color_blue,strlen(color_blue));
    write(1,"-- Best Team -------------------------------------------------------------------------------------\n",strlen("-- Best Team -------------------------------------------------------------------------------------\n"));
    PrintEquipJugadors(MillorEquip);
    sprintf(cad,"   Cost %d, Points: %d.\n", CostEquip(MillorEquip), PuntuacioEquip(MillorEquip));
    write(1,cad,strlen(cad));
    write(1,"-----------------------------------------------------------------------------------------------------\n",strlen("-----------------------------------------------------------------------------------------------------\n"));
    write(1,end_color,strlen(end_color));

    exit(0);
}
// Read file with the market players list (each line containts a plater: "Id.;Name;Position;Cost;Team;Points")
void LlegirFitxerJugadors(char *pathJugadors)
{
    char buffer[256], tipus[10];
    int fdin;
    int nofi;

    if ((fdin = open(pathJugadors, O_RDONLY)) < 0)
        error("Error opening input file.");

    // Read players.
    NJugadors = NPorters = NDefensors = NMitjos = NDelanters = 0;
    do
    {
        int x = 0, i, f;

        while ((nofi = read(fdin, &buffer[x], 1)) != 0 && buffer[x++] != '\n')
            ;
        buffer[x] = '\0';

        if (buffer[0] == '#')
            continue;

        // Player's identificator
        i = 0;
        for (f = 0; buffer[f] != ';'; f++)
            ;
        buffer[f] = 0;
        Jugadors[NJugadors].id = atoi(&(buffer[i]));

        // Player's name
        i = ++f;
        for (; buffer[f] != ';'; f++)
            ;
        buffer[f] = 0;
        strcpy(Jugadors[NJugadors].nom, &(buffer[i]));

        // Player's position
        i = ++f;
        for (; buffer[f] != ';'; f++)
            ;
        buffer[f] = 0;
        if (strcmp(&(buffer[i]), "Portero") == 0)
        {
            NPorters++;
            Jugadors[NJugadors].tipus = JPorter;
        }
        else if (strcmp(&(buffer[i]), "Defensa") == 0)
        {
            NDefensors++;
            Jugadors[NJugadors].tipus = JDefensor;
        }
        else if (strcmp(&(buffer[i]), "Medio") == 0)
        {
            NMitjos++;
            Jugadors[NJugadors].tipus = JMitg;
        }
        else if (strcmp(&(buffer[i]), "Delantero") == 0)
        {
            NDelanters++;
            Jugadors[NJugadors].tipus = JDelanter;
        }
        else
            error("Error player type.");

        // Player's cost
        i = ++f;
        for (f = 0; buffer[f] != ';'; f++)
            ;
        buffer[f] = 0;
        Jugadors[NJugadors].cost = atoi(&(buffer[i]));

        // Player's team
        i = ++f;
        for (f = 0; buffer[f] != ';'; f++)
            ;
        buffer[f] = 0;
        strcpy(Jugadors[NJugadors].equip, &(buffer[i]));

        // Player's points
        i = ++f;
        for (f = 0; buffer[f] != '\n'; f++)
            ;
        buffer[f] = 0;
        Jugadors[NJugadors].punts = atoi(&(buffer[i]));

        NJugadors++;
    } while (nofi);

    sprintf(cad, "Number of players: %d, Port:%d, Def:%d, Med:%d, Del:%d.\n", NJugadors, NPorters, NDefensors, NMitjos, NDelanters);
    write(1, cad, strlen(cad));

    close(fdin);
}

void CalcularEquipOptim(long int PresupostFitxatges, PtrJugadorsEquip MillorEquip)
{
    unsigned int maxbits;
    TEquip equip, primerEquip, ultimEquip, first, end;
    //int MaxPuntuacio=-1;

    // Calculated number of bits required for all teams codification.
    maxbits = Log2(NPorters) * DPosPorters + Log2(NDefensors) * DPosDefensors + Log2(NMitjos) * DPosMitjos + Log2(NDelanters) * DPosDelanters;
    if (maxbits > Log2(ULLONG_MAX))
        error("The number of player overflow the maximum width supported.");

    // Calculate first and end team that have to be evaluated.
    first = primerEquip = GetEquipInicial();
    end = ultimEquip = pow(2, maxbits);

    // Evaluating different teams/combinations.
    sprintf(cad, " Evaluating form %llXH to %llXH (Maxbits: %d). Evaluating %lld teams...\n", first, end, maxbits, end - first);
    write(1, cad, strlen(cad));

    //------------------AREA CONCURRENT ----------------------------------------

    // ManfutMessages class thread -------------------------------------------
    pthread_t messages_thread;
    if (sem_init(&semaphore_messages_thread, 0, MAX_MESSAGES) != 0){
        perror("Can't create messages_thread semaphore.\n");
        exit(1);
    }
    if(pthread_create(&messages_thread, NULL, (void *(*)(void *)) messages_Thread, NULL)) {
        perror("ERROR ON MESSAGES_THREAD CREATION");
        pthread_cancel(messages_thread);
        exit(1);
    }

    // ManfutThread class thread -------------------------------------------
    TEquip num_steps = end - first;
    TEquip intervals[num_threads][2];
    pthread_t Threads[num_threads];
    if (pthread_barrier_init(&JoinBarrier, NULL, num_threads+1)){
        perror("Can't create a join barrier.\n");
        exit(1);
    }

    for (int h = 0; h < num_threads; ++h)
    {
        if (h > 0) {
            intervals[h][0] = intervals[h - 1][1] + 1;
        } else {
            intervals[h][0] = first;
        }
        TEquip block = (num_steps / (num_threads - h));

        if (h == (num_threads - 1)) {
            intervals[h][1] = end;
        } else {
            intervals[h][1] = intervals[h][0] + block;
        }

        TeamInterval *parameters = (TeamInterval *) malloc(sizeof (TeamInterval));
        parameters->inicio = intervals[h][0];
        parameters->final = intervals[h][1];
        parameters->PresupostFitxatges = PresupostFitxatges;

        if(pthread_create(&Threads[h], NULL, (void *(*)(void *)) CalcularEquipOptim_Thread, parameters)) {
            perror("ERROR ON THREADS CREATION");
            cancel_threads(Threads);
            exit(1);
        }
        num_steps -= block;
    }

    //join
    int rc = pthread_barrier_wait (&JoinBarrier);
    if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD){
        perror("Can't wait for JoinBarrier");
        exit(1);
    }
    pthread_mutex_lock(&mutex_messages_thread);
    stop_();
    pthread_cond_wait(&stopped_thread, &mutex_messages_thread);
    pthread_mutex_unlock(&mutex_messages_thread);

    memcpy(MillorEquip, &Shared_MillorEquip, sizeof(TJugadorsEquip));

    // ManfutThread class thread -------------------------------------------
    if (pthread_mutex_destroy(&mutex_shared_variables))
        error("Error has ocurred on mutex_shared_variables destroy");
    if (pthread_barrier_destroy(&JoinBarrier) != 0)
        error("Error has ocurred on JoinBarrier destroy");

    // ManfutMessages class Thread -------------------------------------------
    if (pthread_mutex_destroy(&mutex_messages_thread))
        error("Error has ocurred on mutex_messages_thread destroy ");
    if (pthread_cond_destroy(&addCondition))
        error("Error has ocurred on  addCondition destroy");
    if (pthread_cond_destroy(&stopped_thread))
        error("Error has ocurred on  stopped_thread condition destroy");
    if (sem_destroy(&semaphore_messages_thread))
        error("Error has ocurred on  semaphore_messages_thread destroy");
}

void cancel_threads(pthread_t Threads[]) {
    for (int i = 0; i < num_threads; ++i) {
        if (Threads[i]!=0){
            pthread_cancel(Threads[i]);
        }
    }
}

//ManfutThread class ---------------------------------------------------------------------
void *CalcularEquipOptim_Thread(TeamInterval *input) {
    char buffer[MANFUT_BUFFER_LIMIT];
    TEquip first =input->inicio;
    TEquip end = input->final;
    long int PresupostFitxatges =input->PresupostFitxatges;
    free(input);
    ManfutStats local_statistics;

    reset_(&local_statistics);

    memset(buffer,0, sizeof(buffer));
    sprintf (buffer,"%sThread: %lu Evaluating form %llXH to %llXH. Evaluating %lld teams...\n%s",color_orange,pthread_self(),first,end,end-first,end_color);
    sendToStorage(buffer);

    TEquip equip;
    for (equip = first; equip <= end; equip++)
    {
        TJugadorsEquip jugadors;

        local_statistics.evalComb++;

        if (local_statistics.evalComb % M == 0){
            print_local_statistics(local_statistics, buffer);
        }

        // Get playes from team number. Returns false if the team is not valid.
        if (!ObtenirJugadorsEquip(equip, &jugadors)) {
            local_statistics.invld++;
            continue;
        }

        // Reject teams with repeated players.
        if (JugadorsRepetits(jugadors)) {
            memset(buffer,0, sizeof(buffer));
            sprintf(buffer, "[Thread %lu] Team %lld ->%s Invalid.\r%s",pthread_self(), equip, color_red, end_color);
            sendToStorage(buffer);
            local_statistics.invld++;
            continue; // Equip no valid.
        }

        pthread_mutex_lock(&mutex_shared_variables);

        int costEquip = CostEquip(jugadors);
        int puntuacioEquip = PuntuacioEquip(jugadors);
        memset(buffer,0, sizeof(buffer));

        // Chech if the team points is bigger than current optimal team, then evaluate if the cost is lower than the available budget
        if (puntuacioEquip > MaxPuntuacio && costEquip < PresupostFitxatges) {
            // We have a new partial optimal team.
            MaxPuntuacio = puntuacioEquip;
            Shared_MillorEquip = jugadors;
            sprintf(buffer, "[Thread %lu]  Team %lld ->%s Cost: %d  Points: %d. %s\n",pthread_self(), equip, color_green, costEquip, puntuacioEquip, end_color);
            sendToStorage(buffer);
        } else {
            sprintf(buffer, "[Thread %lu]  Team %lld -> Cost: %d  Points: %d.\r%s",pthread_self(), equip, costEquip, puntuacioEquip, end_color);
            sendToStorage(buffer);
        }

        pthread_mutex_unlock(&mutex_shared_variables);

        make_statistics(&local_statistics,jugadors, costEquip, puntuacioEquip, PresupostFitxatges, equip);
    }

    remainingThreads++;
    while (remainingThreads <= num_threads) {

        print_local_statistics(local_statistics, buffer);

        int rc = pthread_barrier_wait(&JoinBarrier);
        if (rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
            perror("Can't wait for barrier");
    }
    pthread_exit(NULL);

}
void print_local_statistics(ManfutStats local_statistics, char *buffer) {
    pthread_mutex_lock(&mutex_shared_variables);
    print_(local_statistics, buffer);
    sync_local_statistics(local_statistics);
    if (pending_local_print == num_threads - 1) {
        print_statistics_(buffer);
        pending_local_print = 0;
        reset_(&general_stats);
        if (remainingThreads == num_threads) {
            remainingThreads++;
        }
    }
    else {
        pending_local_print++;
    }
    pthread_mutex_unlock(&mutex_shared_variables);
}

//ManfutStats class ---------------------------------------------------------------------
void reset_(ManfutStats *statistics) {
    statistics->evalComb = 0;
    statistics->invld = 0;
    statistics->vld = 0;
    statistics->avg_cost = 0;
    statistics->avg_points = 0;
}
void make_statistics( ManfutStats *local_statistics,TJugadorsEquip jugadors, int costEquip, int puntuacioEquip,
                      long int PresupostFitxatges, TEquip equip) {
    local_statistics->avg_cost = ((local_statistics->avg_cost * (float)local_statistics->vld) + (float)costEquip) /(float) (local_statistics->vld + 1);
    local_statistics->avg_points = ((local_statistics->avg_points * (float)local_statistics->vld) + (float)puntuacioEquip) /(float) (local_statistics->vld + 1);
    local_statistics->vld++;
    if (puntuacioEquip > local_statistics->best_points && costEquip < PresupostFitxatges) {
        local_statistics->bestName = equip;
        local_statistics->best = jugadors;
        local_statistics->best_cost=costEquip;
        local_statistics->best_points = puntuacioEquip;

    } else if (local_statistics->worst_points == 0 || puntuacioEquip < local_statistics->worst_points) {
        local_statistics->worstName = equip;
        local_statistics->worst = jugadors;
        local_statistics->worst_cost = costEquip;
        local_statistics->worst_points = puntuacioEquip;
    }
}

void sync_local_statistics(ManfutStats stats) {
    general_stats.evalComb += stats.evalComb;
    general_stats.invld += stats.invld;
    if (stats.vld != 0) {
        general_stats.avg_cost = ((general_stats.avg_cost * (float ) general_stats.vld) + (stats.avg_cost * (float )stats.vld)) /(float )(general_stats.vld + stats.vld);
        general_stats.avg_points = ((general_stats.avg_points * (float )general_stats.vld) + (stats.avg_points * (float )stats.vld)) /(float ) (general_stats.vld + stats.vld);
        general_stats.vld += stats.vld;
        if (general_stats.best_points == 0 || stats.best_points > general_stats.best_points) {
            general_stats.bestName = stats.bestName;
            general_stats.best = stats.best;
            general_stats.best_cost = stats.best_cost;
            general_stats.best_points = stats.best_points;

        }else if (general_stats.worst_points == 0 || (stats.worst_points < general_stats.worst_points && stats.worst_points != 0)){
            general_stats.worstName =stats.worstName;
            general_stats.worst = stats.worst;
            general_stats.worst_cost = stats.worst_cost;
            general_stats.worst_points = stats.worst_points;
        }
    }
}
void print_(ManfutStats statistics, char *buffer) {

    sprintf(buffer,"%s"
                   "+++++++++++++++++++++++++++++++ ManfutThread %lu  +++++++++++++++++++++++++++++++\n"
                   "++ Eval Comb: %d \tValidas: %d \tInvalidas: %d\n"
                   "++ Avg cost: %f  \tAvg Points: %f\n"
                   "++ Mejor Equipo (desde el punto de vista de la puntuación):Team %llu -> Cost %d, Points: %d. \n"
                   "++ Peor Equipo (desde el punto de vista de la puntuación):Team %llu -> Cost %d, Points: %d.\n"
                   "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n",
            end_color,pthread_self(),
            statistics.evalComb,statistics.vld,statistics.invld,
            statistics.avg_cost,statistics.avg_points,statistics.bestName,
            statistics.best_cost,
            statistics.best_points,
            statistics.worstName,
            statistics.worst_cost,
            statistics.worst_points);
    sendToStorage(buffer);
}
void print_statistics_(char *buffer) {
    sprintf(buffer,"%s"
                   "+++++++++++++++++++++++++++++++++++++++++ General ++++++++++++++++++++++++++++++++++++++++++++\n"
                   "++ Eval Comb: %d \tValidas: %d \tInvalidas: %d\n"
                   "++ Avg cost: %f  \tAvg Points: %f\n"
                   "++ Mejor Equipo (desde el punto de vista de la puntuación):Team %llu -> Cost %d, Points: %d. \n"
                   "++ Peor Equipo (desde el punto de vista de la puntuación):Team %llu -> Cost %d, Points: %d.\n"
                   "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n",
            color_yellow,
            general_stats.evalComb,general_stats.vld,general_stats.invld,
            general_stats.avg_cost,general_stats.avg_points,general_stats.bestName,
            general_stats.best_cost,
            general_stats.best_points,
            general_stats.worstName,
            general_stats.worst_cost,
            general_stats.worst_points);
    sendToStorage(buffer);
}

//ManfutMessages class ---------------------------------------------------------------------
void* messages_Thread() {
    while (!running) {
        pthread_mutex_lock(&mutex_messages_thread);

        while (iterator != MAX_MESSAGES && !running)
            pthread_cond_wait(&addCondition, &mutex_messages_thread);
        if (iterator > 0) {
            printAll_();
            for (int i = 0; i < MAX_MESSAGES; i++) {
                sem_post(&semaphore_messages_thread);
            }
        }
        pthread_mutex_unlock(&mutex_messages_thread);
    }
    pthread_cond_signal(&stopped_thread);
    pthread_exit(NULL);
}

void printAll_() {
    for (int i = 0; i < MAX_MESSAGES; i++)
    {
        write(1, storage[i], strlen(storage[i]));
    }
    memset(storage, 0, sizeof storage);
    iterator = 0;
}

void sendToStorage(char *message) {
    sem_wait(&semaphore_messages_thread);
    pthread_mutex_lock(&mutex_messages_thread);

    strcpy(storage[iterator++], message);

    pthread_cond_signal(&addCondition);
    pthread_mutex_unlock(&mutex_messages_thread);
}
void stop_() {
    running = true;
    pthread_cond_signal(&addCondition);
}

// Calculate the initial team combination.
TEquip
GetEquipInicial()
{
    int p;
    TEquip equip = 0, equip2 = 0;
    unsigned bitsPorters, bitsDefensors, bitsMitjos, bitsDelanters;

    bitsPorters = Log2(NPorters);
    bitsDefensors = Log2(NDefensors);
    bitsMitjos = Log2(NMitjos);
    bitsDelanters = Log2(NDelanters);

    for (p = DPosDelanters - 1; p >= 0; p--)
    {
        equip += p;
        equip = equip << bitsDelanters;
    }

    for (p = DPosMitjos - 1; p >= 0; p--)
    {
        equip += p;
        equip = equip << bitsMitjos;
    }

    for (p = DPosDefensors - 1; p >= 0; p--)
    {
        equip += p;
        equip = equip << bitsDefensors;
    }

    for (p = DPosPorters - 1; p > 0; p--)
    {
        equip += p;
        equip = equip << bitsPorters;
    }

    return (equip);
}

// Convert team combinatio to an struct with all the player by position.
// Returns false if the team is not valid.

TBoolean
ObtenirJugadorsEquip(TEquip equip, PtrJugadorsEquip jugadors)
{
    int p;
    unsigned bitsPorters, bitsDefensors, bitsMitjos, bitsDelanters;

    bitsPorters = Log2(NPorters);
    bitsDefensors = Log2(NDefensors);
    bitsMitjos = Log2(NMitjos);
    bitsDelanters = Log2(NDelanters);

    for (p = 0; p < DPosPorters; p++)
    {
        jugadors->Porter[p] = (equip >> (bitsPorters * p)) & ((int)pow(2, bitsPorters) - 1);
        if (jugadors->Porter[p] >= NPorters)
            return False;
    }

    for (p = 0; p < DPosDefensors; p++)
    {
        jugadors->Defensors[p] = (equip >> ((bitsPorters * DPosPorters) + (bitsDefensors * p))) & ((int)pow(2, bitsDefensors) - 1);
        if (jugadors->Defensors[p] >= NDefensors)
            return False;
    }

    for (p = 0; p < DPosMitjos; p++)
    {
        jugadors->Mitjos[p] = (equip >> ((bitsPorters * DPosPorters) + (bitsDefensors * DPosDefensors) + (bitsMitjos * p))) & ((int)pow(2, bitsMitjos) - 1);
        if (jugadors->Mitjos[p] >= NMitjos)
            return False;
    }

    for (p = 0; p < DPosDelanters; p++)
    {
        jugadors->Delanters[p] = (equip >> ((bitsPorters * DPosPorters) + (bitsDefensors * DPosDefensors) + (bitsMitjos * DPosMitjos) + (bitsDelanters * p))) & ((int)pow(2, bitsDelanters) - 1);
        if (jugadors->Delanters[p] >= NDelanters)
            return False;
    }

    return True;
}

// Check if the team have any repeated player.
// Returns true if the team have repeated players.

TBoolean
JugadorsRepetits(TJugadorsEquip jugadors)
{
    // Returns True if the equip have some repeated players (is not valid).
    int i, j;

    // Porters.
    for (i = 0; i < DPosPorters - 1; i++)
        for (j = i + 1; j <= DPosPorters - 1; j++)
            if (jugadors.Porter[i] == jugadors.Porter[j])
                return True;

    // Defensors.
    for (i = 0; i < DPosDefensors - 1; i++)
        for (j = i + 1; j <= DPosDefensors - 1; j++)
            if (jugadors.Defensors[i] == jugadors.Defensors[j])
                return True;

    // Mitjos.
    for (i = 0; i < DPosMitjos - 1; i++)
        for (j = i + 1; j <= DPosMitjos - 1; j++)
            if (jugadors.Mitjos[i] == jugadors.Mitjos[j])
                return True;

    // Delanters
    for (i = 0; i < DPosDelanters - 1; i++)
        for (j = i + 1; j <= DPosDelanters - 1; j++)
            if (jugadors.Delanters[i] == jugadors.Delanters[j])
                return True;

    return False;
}

// Calculates the team cost adding the individual cost of all team players.
// Returns the cost.

int CostEquip(TJugadorsEquip equip)
{
    int x;
    int cost = 0;

    for (x = 0; x < DPosPorters; x++)
        cost += GetPorter(equip.Porter[x]).cost;

    for (x = 0; x < DPosDefensors; x++)
        cost += GetDefensor(equip.Defensors[x]).cost;

    for (x = 0; x < DPosMitjos; x++)
        cost += GetMitg(equip.Mitjos[x]).cost;

    for (x = 0; x < DPosDelanters; x++)
        cost += GetDelanter(equip.Delanters[x]).cost;

    return (cost);
}

// Calculates the team points adding the individual points of all team players.
// Returns the points.

int PuntuacioEquip(TJugadorsEquip equip)
{
    int x;
    int punts = 0;

    for (x = 0; x < DPosPorters; x++)
        punts += GetPorter(equip.Porter[x]).punts;

    for (x = 0; x < DPosDefensors; x++)
        punts += GetDefensor(equip.Defensors[x]).punts;

    for (x = 0; x < DPosMitjos; x++)
        punts += GetMitg(equip.Mitjos[x]).punts;

    for (x = 0; x < DPosDelanters; x++)
        punts += GetDelanter(equip.Delanters[x]).punts;

    return (punts);
}

// Prints an error message.ç

void error(char *str)
{
    char s[255];

    sprintf(s, "[%d] ManFut: %s (%s))\n", getpid(), str, strerror(errno));
    write(2, s, strlen(s));
    exit(1);
}

// Rounded log2

unsigned int Log2(unsigned long long int n)
{
    return (ceil(log2((double)n)));
}

// Prints all market players information,

void PrintJugadors()
{
    int j;

    for (j = 0; j < NJugadors; j++)
    {
        sprintf(cad, "Jugador: %s (%d), Posició: %d, Cost: %d, Puntuació: %d.\n", Jugadors[j].nom, Jugadors[j].id, Jugadors[j].tipus, Jugadors[j].cost, Jugadors[j].punts);
        write(1, cad, strlen(cad));
    }
}

// Prints team players.
void PrintEquipJugadors(TJugadorsEquip equip)
{
    int x;

    write(1, "   Porters: ", strlen("   Porters: "));
    for (x = 0; x < DPosPorters; x++)
    {
        sprintf(cad, "%s (%d/%d), ", GetPorter(equip.Porter[x]).nom, GetPorter(equip.Porter[x]).cost, GetPorter(equip.Porter[x]).punts);
        write(1, cad, strlen(cad));
    }
    write(1, "\n", strlen("\n"));

    write(1, "   Defenses: ", strlen("   Defenses: "));
    for (x = 0; x < DPosDefensors; x++)
    {
        sprintf(cad, "%s (%d/%d), ", GetDefensor(equip.Defensors[x]).nom, GetDefensor(equip.Defensors[x]).cost, GetDefensor(equip.Defensors[x]).punts);
        write(1, cad, strlen(cad));
    }
    write(1, "\n", strlen("\n"));

    write(1, "   Mitjos: ", strlen("   Mitjos: "));
    for (x = 0; x < DPosMitjos; x++)
    {
        sprintf(cad, "%s (%d/%d), ", GetMitg(equip.Mitjos[x]).nom, GetMitg(equip.Mitjos[x]).cost, GetMitg(equip.Mitjos[x]).punts);
        write(1, cad, strlen(cad));
    }
    write(1, "\n", strlen("\n"));

    write(1, "   Delanters: ", strlen("   Delanters: "));
    for (x = 0; x < DPosDelanters; x++)
    {
        sprintf(cad, "%s (%d/%d), ", GetDelanter(equip.Delanters[x]).nom, GetDelanter(equip.Delanters[x]).cost, GetDelanter(equip.Delanters[x]).punts);
        write(1, cad, strlen(cad));
    }
    write(1, "\n", strlen("\n"));
}
