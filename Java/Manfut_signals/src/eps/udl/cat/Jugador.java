package eps.udl.cat;

enum TipusJug {
    JPorter,
    JDefensor,
    JMitg,
    JDelanter
}

public class Jugador {
    int 		id;
    String		nom;
    TipusJug 	tipus;
    int			cost;
    String 		equip;
    int			punts;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getNom() {
        return nom;
    }

    public void setNom(String nom) {
        this.nom = nom;
    }

    public TipusJug getTipus() {
        return tipus;
    }

    public void setTipus(TipusJug tipus) {
        this.tipus = tipus;
    }

    public int getCost() {
        return cost;
    }

    public void setCost(int cost) {
        this.cost = cost;
    }

    public String getEquip() {
        return equip;
    }

    public void setEquip(String equip) {
        this.equip = equip;
    }

    public int getPunts() {
        return punts;
    }

    public void setPunts(int punts) {
        this.punts = punts;
    }

    void print() {
        System.out.println("Jugador: "+ nom +" ("+ id +"), Posició: "+ tipus +", Cost: "+ cost +", Puntuació: "+ punts +".");
    }

    void printCostPunts(){
        System.out.print(nom + "("+ cost + "/" + punts + ")");
    }
}
