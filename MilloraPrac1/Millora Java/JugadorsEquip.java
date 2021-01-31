package eps.udl.cat;

class IdEquip {
    static int  IdCount=1;

    int     Id;

    IdEquip() {
        Id = IdCount++;
    }
    IdEquip(int id) {
        Id = id;
    }

    public int getIdEquip() {
        return(Id);
    }

    public void setId(int id) {
        Id = id;
    }
}

public class JugadorsEquip {
    // Players by position (7-footbal)
    static final int DPosPorters = 1;
    static final int DPosDefensors = 3;
    static final int DPosMitjos =  2;
    static final int DPosDelanters = 1;

    int 	Porter;
    int 	Defensors[];
    int 	Mitjos[];
    int 	Delanters[];

    Market  myMarket;

    public int getPorter() {
        return Porter;
    }
    public void setPorter(int porter) {
        Porter = porter;
    }

    public int getDefensor(int p) {
        return Defensors[p];
    }
    public void setDefensor(int p, int defensor) {
        Defensors[p] = defensor;
    }

    public int getMitg(int p) {
        return Mitjos[p];
    }
    public void setMitg(int p, int mitjo) {
        Mitjos[p] = mitjo;
    }

    public int getDelanter(int p) {
        return Delanters[p];
    }
    public void setDelanter(int p, int delanter) {
        Delanters[p] = delanter;
    }


    JugadorsEquip(Market market) {
        Defensors = new int[DPosDefensors];
        Mitjos = new int[DPosMitjos];
        Delanters = new int[DPosDelanters];
        myMarket = market;
    }


    // Check if the team have any repeated player.
    // Returns true if the team have repeated players.
    Boolean JugadorsRepetits()
    {
        // Returns True if the equip have some repeated players (is not valid).
        int i,j;

        // Defensors.
        for(i=0;i<DPosDefensors-1;i++)
            for(j=i+1;j<=DPosDefensors-1;j++)
                if (Defensors[i]==Defensors[j])
                    return true;

        // Mitjos.
        for(i=0;i<DPosMitjos-1;i++)
            for(j=i+1;j<=DPosMitjos-1;j++)
                if (Mitjos[i]==Mitjos[j])
                    return true;

        // Delanters
        for(i=0;i<DPosDelanters-1;i++)
            for(j=i+1;j<=DPosDelanters-1;j++)
                if (Delanters[i]==Delanters[j])
                    return true;

        return false;
    }


    // Calculates the team cost adding the individual cost of all team players.
    // Returns the cost.
    int CostEquip()
    {
        int x;
        int cost=0;

        cost += myMarket.GetPorter(Porter).cost;

        for(x=0;x<DPosDefensors;x++)
            cost += myMarket.GetDefensor(Defensors[x]).cost;

        for(x=0;x<DPosMitjos;x++)
            cost += myMarket.GetMitg(Mitjos[x]).cost;

        for(x=0;x<DPosDelanters;x++)
            cost += myMarket.GetDelanter(Delanters[x]).cost;

        return (cost);
    }



    // Calculates the team points adding the individual points of all team players.
    // Returns the points.
    int PuntuacioEquip()
    {
        int x;
        int punts=0;

        punts += myMarket.GetPorter(Porter).punts;

        for(x=0;x<DPosDefensors;x++)
            punts += myMarket.GetDefensor(Defensors[x]).punts;

        for(x=0;x<DPosMitjos;x++)
            punts += myMarket.GetMitg(Mitjos[x]).punts;

        for(x=0;x<DPosDelanters;x++)
            punts += myMarket.GetDelanter(Delanters[x]).punts;

        return(punts);
    }


    // Prints team players.
    void PrintEquipJugadors()
    {
        int x;

        System.out.print("   Porters: ");
        myMarket.GetPorter(Porter).printCostPunts();
        System.out.println();


        System.out.print("   Defenses: ");
        for(x=0;x<DPosDefensors;x++)
        {
            myMarket.GetDefensor(Defensors[x]).printCostPunts();
            System.out.print(", ");
        }
        System.out.println();

        System.out.print("   Mitjos: ");
        for(x=0;x<DPosMitjos;x++)
        {
            myMarket.GetMitg(Mitjos[x]).printCostPunts();
            System.out.print(", ");
        }
        System.out.println();

        System.out.print("   Delanters: ");
        for(x=0;x<DPosDelanters;x++)
        {
            myMarket.GetDelanter(Delanters[x]).printCostPunts();
            System.out.print(", ");
        }
        System.out.println();
    }


}
