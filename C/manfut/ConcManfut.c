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
#include "manfut.h"

#define GetPorter(j) (Jugadors[j])
#define GetDefensor(j) (Jugadors[NPorters+j])
#define GetMitg(j) (Jugadors[NPorters+NDefensors+j])
#define GetDelanter(j) (Jugadors[NPorters+NDefensors+NMitjos+j])
#define DDEFAULT_THREADS 2
char *color_red = "\033[01;31m";
char *color_green = "\033[01;32m";
char *color_blue = "\033[01;34m";
char *end_color = "\033[00m";
//----my functions------------------------------------------------------------------------------------------------------
char *yellow_color = "\033[0;33m";

struct ThreadArgs {
    TEquip inicio ;
    TEquip fin;
    long int PresupostFitxatges;
    TJugadorsEquip* MillorEquip_parcial;
};
typedef struct ThreadArgs ThreadArgs;

void *howdy(void *input);
void* funcion_barata(void *input);
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


void funcion_del_for(TEquip first, TEquip anEnd, long fitxatges, TJugadorsEquip *pEquip);

// Global variables definition
TJugador Jugadors[DMaxJugadors];
int num_threads;
int NJugadors, NPorters, NDefensors, NMitjos, NDelanters;
char cad[256];

int main(int argc, char *argv[])
{
    TJugadorsEquip MillorEquip, AuxEquip;
    long int PresupostFitxatges;
    float IntervalBegin=-1, IntervalEnd=-1;

    if (argc<3)
        error("Error in arguments: ManFut <presupost> <fitxer_jugadors> <fitxer_jugadors> <num_threads>");

    if (argc>1)
        PresupostFitxatges = atoi(argv[1]);

    if (argc>2)
        LlegirFitxerJugadors(argv[2]);
    if (argc>3){
        num_threads =  atoi(argv[3]);
    } else{
        num_threads = DDEFAULT_THREADS;
    }


    // Calculate the best team.
    CalcularEquipOptim(PresupostFitxatges, &MillorEquip);

    //perror("\n\033[01;31mfinalització prematura necessaria per no tenir SIGSEGV al Main");
    //exit(1);
    write(1,color_blue,strlen(color_blue));
    write(1,"-- Best Team -------------------------------------------------------------------------------------\n",strlen("-- Best Team -------------------------------------------------------------------------------------\n"));
    PrintEquipJugadors(MillorEquip); // error aqui
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

    if ((fdin=open(pathJugadors, O_RDONLY)) < 0)
        error("Error opening input file.");

    // Read players.
    NJugadors=NPorters=NDefensors=NMitjos=NDelanters=0;
    do
    {
        int x=0,i,f;

        while((nofi=read(fdin,&buffer[x],1))!=0 && buffer[x++]!='\n');
        buffer[x]='\0';

        if (buffer[0]=='#') continue;

        // Player's identificator
        i=0;
        for (f=0;buffer[f]!=';';f++);
        buffer[f]=0;
        Jugadors[NJugadors].id = atoi(&(buffer[i]));

        // Player's name
        i=++f;
        for (;buffer[f]!=';';f++);
        buffer[f]=0;
        strcpy(Jugadors[NJugadors].nom,&(buffer[i]));

        // Player's position
        i=++f;
        for (;buffer[f]!=';';f++);
        buffer[f]=0;
        if (strcmp(&(buffer[i]),"Portero")==0)
        {
            NPorters++;
            Jugadors[NJugadors].tipus=JPorter;
        }
        else if (strcmp(&(buffer[i]),"Defensa")==0)
        {
            NDefensors++;
            Jugadors[NJugadors].tipus=JDefensor;
        }
        else if (strcmp(&(buffer[i]),"Medio")==0)
        {
            NMitjos++;
            Jugadors[NJugadors].tipus=JMitg;
        }
        else if (strcmp(&(buffer[i]),"Delantero")==0)
        {
            NDelanters++;
            Jugadors[NJugadors].tipus=JDelanter;
        }
        else error("Error player type.");


        // Player's cost
        i=++f;
        for (f=0;buffer[f]!=';';f++);
        buffer[f]=0;
        Jugadors[NJugadors].cost = atoi(&(buffer[i]));

        // Player's team
        i=++f;
        for (f=0;buffer[f]!=';';f++);
        buffer[f]=0;
        strcpy(Jugadors[NJugadors].equip,&(buffer[i]));

        // Player's points
        i=++f;
        for (f=0;buffer[f]!='\n';f++);
        buffer[f]=0;
        Jugadors[NJugadors].punts = atoi(&(buffer[i]));

        NJugadors++;
    }
    while(nofi);

    sprintf(cad,"Number of players: %d, Port:%d, Def:%d, Med:%d, Del:%d.\n",NJugadors, NPorters, NDefensors, NMitjos, NDelanters);
    write(1,cad,strlen(cad));

    close(fdin);
}

































void CalcularEquipOptim(long int PresupostFitxatges, PtrJugadorsEquip MillorEquip)
{
    unsigned int maxbits;
    TEquip equip, primerEquip, ultimEquip, first, end;
    int MaxPuntuacio=-1;

    // Calculated number of bits required for all teams codification.
    maxbits=Log2(NPorters)*DPosPorters+Log2(NDefensors)*DPosDefensors+Log2(NMitjos)*DPosMitjos+Log2(NDelanters)*DPosDelanters;
    if (maxbits>Log2(ULLONG_MAX))
        error("The number of player overflow the maximum width supported.");

    // Calculate first and end team that have to be evaluated.
    first=primerEquip=GetEquipInicial();
    end=ultimEquip=pow(2,maxbits);


    pthread_t Threads[num_threads];
    PtrJugadorsEquip Parcial[num_threads];

    TEquip start = primerEquip;
    TEquip taskPart= (ultimEquip-primerEquip)/num_threads;
    TEquip end_ = taskPart;

    sprintf (cad,"WORK divided range [%lld-%lld]\n",start,ultimEquip);
    write(1,cad,strlen(cad));

    for (int i = 0; i < num_threads; ++i) {
        end_ = (i == num_threads-1) ? ultimEquip :start+taskPart;

        Parcial[i] = malloc(sizeof(TJugadorsEquip));

        ThreadArgs *parameters = (ThreadArgs *) malloc(sizeof(struct ThreadArgs));
        parameters->inicio = start;
        parameters->fin = end_;
        parameters->MillorEquip_parcial = Parcial[i];
        parameters->PresupostFitxatges = PresupostFitxatges;
        if(pthread_create(&Threads[i], NULL, (void *(*)(void *)) funcion_barata, (void *)parameters)){
            perror("ERROR THREAD");
            exit(1);
        } else{
            sprintf (cad,"Thread created Evaluator  [%lld-%lld]\n",start,end_);
            write(1,cad,strlen(cad));
        }

        start = end_ +1;
    }
    int max = 0;
    for (int i = 0; i <  num_threads; ++i) {

        if(pthread_join(Threads[i], NULL)){		//Espera a que termine la ejecucion del hilo j.
            printf("Error al hacer join\n");
            exit(1);
        }else{
            if (PuntuacioEquip(*Parcial[i]) > max){
                max = PuntuacioEquip(*Parcial[i]);
                memcpy(MillorEquip->Delanters, Parcial[i]->Delanters,sizeof(MillorEquip->Delanters));
                memcpy(MillorEquip->Porter, Parcial[i]->Porter,sizeof(MillorEquip->Porter));
                memcpy(MillorEquip->Mitjos, Parcial[i]->Mitjos,sizeof(MillorEquip->Mitjos));
                memcpy(MillorEquip->Defensors, Parcial[i]->Defensors,sizeof(MillorEquip->Defensors));
            }
            free(Parcial[i]);
        }
    }


}

void* funcion_barata(void *input){


    TEquip first =((ThreadArgs*)input)->inicio;
    TEquip end = ((ThreadArgs*)input)->fin;
    long int PresupostFitxatges = ((ThreadArgs*)input)->PresupostFitxatges;
    TJugadorsEquip* MillorEquip = ((ThreadArgs*)input)->MillorEquip_parcial;
    free(input);


    int MaxPuntuacio = -1;
    TJugadorsEquip MillorEquip_parcial;
    TEquip equip;

    for (equip=first;equip<=end;equip++)
    {
        TJugadorsEquip jugadors;

        // Get playes from team number. Returns false if the team is not valid.
        if (!ObtenirJugadorsEquip(equip, &jugadors))
            continue;


        sprintf(cad,"Team %lld ->",equip);
        write(1,cad,strlen(cad));
        // Reject teams with repeated players.
        if (JugadorsRepetits(jugadors))
        {

            sprintf(cad,"%s Invalid.\r%s", color_red, end_color);
            write(1,cad,strlen(cad));
            continue;	// Equip no valid.
        }

        // Chech if the team points is bigger than current optimal team, then evaluate if the cost is lower than the available budget
        if (PuntuacioEquip(jugadors)>MaxPuntuacio && CostEquip(jugadors)<PresupostFitxatges)
        {
            // We have a new partial optimal team.
            MaxPuntuacio=PuntuacioEquip(jugadors);
            memcpy(MillorEquip,&jugadors,sizeof(TJugadorsEquip));
            sprintf(cad,"%s Cost: %d  Points: %d. %s\n", yellow_color, CostEquip(jugadors), PuntuacioEquip(jugadors), end_color);
            write(1,cad,strlen(cad));
        }
        else
        {
            sprintf(cad," Cost: %d  Points: %d. \r", CostEquip(jugadors), PuntuacioEquip(jugadors));
            write(1,cad,strlen(cad));
        }
    }




    pthread_exit(NULL);
}



































































// Calculate the initial team combination.
TEquip
GetEquipInicial()
{
    int p;
    TEquip equip=0, equip2=0;
    unsigned bitsPorters, bitsDefensors, bitsMitjos, bitsDelanters;

    bitsPorters = Log2(NPorters);
    bitsDefensors = Log2(NDefensors);
    bitsMitjos = Log2(NMitjos);
    bitsDelanters = Log2(NDelanters);

    for (p=DPosDelanters-1;p>=0;p--)
    {
        equip+=p;
        equip = equip << bitsDelanters;
    }

    for (p=DPosMitjos-1;p>=0;p--)
    {
        equip+=p;
        equip = equip << bitsMitjos;
    }

    for (p=DPosDefensors-1;p>=0;p--)
    {
        equip+=p;
        equip = equip << bitsDefensors;
    }

    for (p=DPosPorters-1;p>0;p--)
    {
        equip+=p;
        equip = equip << bitsPorters;
    }

    return (equip);
}


// Convert team combinatio to an struct with all the player by position.
// Returns false if the team is not valid.

TBoolean
ObtenirJugadorsEquip (TEquip equip, TJugadorsEquip * jugadors)
{
    int p;
    unsigned bitsPorters, bitsDefensors, bitsMitjos, bitsDelanters;

    bitsPorters = Log2(NPorters);
    bitsDefensors = Log2(NDefensors);
    bitsMitjos = Log2(NMitjos);
    bitsDelanters = Log2(NDelanters);

    for (p=0;p<DPosPorters;p++)
    {
        jugadors->Porter[p]=(equip>>(bitsPorters*p)) & ((int)pow(2,bitsPorters)-1);
        if (jugadors->Porter[p]>=NPorters)
            return False;
    }

    for (p=0;p<DPosDefensors;p++)
    {
        jugadors->Defensors[p]=(equip>>((bitsPorters*DPosPorters)+(bitsDefensors*p))) & ((int)pow(2,bitsDefensors)-1);
        if (jugadors->Defensors[p]>=NDefensors)
            return False;
    }

    for (p=0;p<DPosMitjos;p++)
    {
        jugadors->Mitjos[p]=(equip>>((bitsPorters*DPosPorters)+(bitsDefensors*DPosDefensors)+(bitsMitjos*p))) & ((int)pow(2,bitsMitjos)-1);
        if (jugadors->Mitjos[p]>=NMitjos)
            return False;
    }

    for (p=0;p<DPosDelanters;p++)
    {
        jugadors->Delanters[p]=(equip>>((bitsPorters*DPosPorters)+(bitsDefensors*DPosDefensors)+(bitsMitjos*DPosMitjos)+(bitsDelanters*p))) & ((int)pow(2,bitsDelanters)-1);
        if (jugadors->Delanters[p]>=NDelanters)
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
    int i,j;

    // Porters.
    for(i=0;i<DPosPorters-1;i++)
        for(j=i+1;j<=DPosPorters-1;j++)
            if (jugadors.Porter[i]==jugadors.Porter[j])
                return True;

    // Defensors.
    for(i=0;i<DPosDefensors-1;i++)
        for(j=i+1;j<=DPosDefensors-1;j++)
            if (jugadors.Defensors[i]==jugadors.Defensors[j])
                return True;

    // Mitjos.
    for(i=0;i<DPosMitjos-1;i++)
        for(j=i+1;j<=DPosMitjos-1;j++)
            if (jugadors.Mitjos[i]==jugadors.Mitjos[j])
                return True;

    // Delanters
    for(i=0;i<DPosDelanters-1;i++)
        for(j=i+1;j<=DPosDelanters-1;j++)
            if (jugadors.Delanters[i]==jugadors.Delanters[j])
                return True;

    return False;
}


// Calculates the team cost adding the individual cost of all team players.
// Returns the cost.

int
CostEquip(TJugadorsEquip equip)
{
    int x;
    int cost=0;

    for(x=0;x<DPosPorters;x++)
        cost += GetPorter(equip.Porter[x]).cost;

    for(x=0;x<DPosDefensors;x++)
        cost += GetDefensor(equip.Defensors[x]).cost;

    for(x=0;x<DPosMitjos;x++)
        cost += GetMitg(equip.Mitjos[x]).cost;

    for(x=0;x<DPosDelanters;x++)
        cost += GetDelanter(equip.Delanters[x]).cost;

    return (cost);
}



// Calculates the team points adding the individual points of all team players.
// Returns the points.

int
PuntuacioEquip(TJugadorsEquip equip)
{
    int x;
    int punts=0;

    for(x=0;x<DPosPorters;x++)
        punts += GetPorter(equip.Porter[x]).punts;

    for(x=0;x<DPosDefensors;x++)
        punts += GetDefensor(equip.Defensors[x]).punts;

    for(x=0;x<DPosMitjos;x++)
        punts += GetMitg(equip.Mitjos[x]).punts;

    for(x=0;x<DPosDelanters;x++)
        punts += GetDelanter(equip.Delanters[x]).punts;

    return(punts);
}


// Prints an error message.ç

void error(char *str)
{
    char s[255];

    sprintf(s, "[%d] ManFut: %s (%s))\n", getpid(), str,strerror(errno));
    write(2, s, strlen(s));
    exit(1);
}


// Rounded log2

unsigned int Log2(unsigned long long int n)
{
    return(ceil(log2((double)n)));
}


// Prints all market players information,

void PrintJugadors()
{
    int j;

    for(j=0;j<NJugadors;j++)
    {
        sprintf(cad,"Jugador: %s (%d), Posició: %d, Cost: %d, Puntuació: %d.\n", Jugadors[j].nom, Jugadors[j].id, Jugadors[j].tipus, Jugadors[j].cost, Jugadors[j].punts);
        write(1,cad,strlen(cad));
    }
}



// Prints team players.
void PrintEquipJugadors(TJugadorsEquip equip)
{
    int x;

    write(1,"   Porters: ",strlen("   Porters: "));
    for(x=0;x<DPosPorters;x++)
    {
        sprintf(cad,"%s (%d/%d), ",GetPorter(equip.Porter[x]).nom, GetPorter(equip.Porter[x]).cost, GetPorter(equip.Porter[x]).punts);
        write(1,cad,strlen(cad));
    }
    write(1,"\n",strlen("\n"));

    write(1,"   Defenses: ",strlen("   Defenses: "));
    for(x=0;x<DPosDefensors;x++)
    {
        sprintf(cad,"%s (%d/%d), ",GetDefensor(equip.Defensors[x]).nom, GetDefensor(equip.Defensors[x]).cost, GetDefensor(equip.Defensors[x]).punts);
        write(1,cad,strlen(cad));
    }
    write(1,"\n",strlen("\n"));

    write(1,"   Mitjos: ",strlen("   Mitjos: "));
    for(x=0;x<DPosMitjos;x++)
    {
        sprintf(cad,"%s (%d/%d), ",GetMitg(equip.Mitjos[x]).nom, GetMitg(equip.Mitjos[x]).cost, GetMitg(equip.Mitjos[x]).punts);
        write(1,cad,strlen(cad));
    }
    write(1,"\n",strlen("\n"));

    write(1,"   Delanters: ",strlen("   Delanters: "));
    for(x=0;x<DPosDelanters;x++)
    {
        sprintf(cad,"%s (%d/%d), ",GetDelanter(equip.Delanters[x]).nom, GetDelanter(equip.Delanters[x]).cost, GetDelanter(equip.Delanters[x]).punts);
        write(1,cad,strlen(cad));
    }
    write(1,"\n",strlen("\n"));
}
