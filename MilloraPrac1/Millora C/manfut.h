
// Players by position (7-footbal)
#define  DPosPorters 1
#define  DPosDefensors 3
#define  DPosMitjos 2
#define  DPosDelanters 1

/*
// Players by position (11-footbal)
#define  DPosPorters 1
#define  DPosDefensors 4
#define  DPosMitjos 4
#define  DPosDelanters 2
*/

typedef enum {JPorter, JDefensor, JMitg, JDelanter} TTipusJug;
typedef unsigned long long int TEquip;
typedef enum {False, True} TBoolean;

struct TJugador
{
	int 			id; 
	char			nom[100];
	TTipusJug 	tipus;
	int			cost;
	char 			equip[4]; 
	int			punts;
};
typedef struct TJugador *PtrJugador, TJugador;

struct TJugadorsEquip
{
	int 			Porter[DPosPorters]; 
	int 			Defensors[DPosDefensors];
	int 			Mitjos[DPosMitjos];
	int 			Delanters[DPosDelanters];
};
typedef struct TJugadorsEquip *PtrJugadorsEquip, TJugadorsEquip;

struct TBestEquip
{
	TEquip		Equip;
	int			Puntuacio;
	int			IdEmisor;
};
typedef struct TBestEquip *PtrBestEquip, TBestEquip;

#define DMaxJugadors 1000
