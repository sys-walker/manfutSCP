/* --------------------------------------------------------------- Práctica 1.
Código fuente: Manfut.java
Grau Informàtica
39939768S - Pere Antoni Rollon Baiges
47433543E - Francisco Javier Roig Gregorio
--------------------------------------------------------------- */

package eps.udl.cat;

public class Manfut {
    public static final int DEFAULT_MANFUT_THREADS=2;
    public static int num_threads;

    public static void main(String[] args)
    {
        Market          PlayersMarket;
        int             PresupostFitxatges;
        JugadorsEquip   MillorEquip;
        Error           err;

        // Procesar argumentos.
        if (args.length<2)
            throw new IllegalArgumentException("Error in arguments: ManFut <presupost> <fitxer_jugadors>");
        if (args.length==3) {
            if(Integer.parseInt(args[2])>0){
                num_threads=Integer.parseInt(args[2]);
                if (num_threads==1){
                    System.out.print(Error.color_red);
                    System.out.println("Secuential mode");
                    System.out.println(Error.end_color);
                }
            }else{

                System.out.print(Error.color_red);
                System.out.println("Negative numbers are not allowed, setting to defaults num_threads=2");
                System.out.println(Error.end_color);
                num_threads=DEFAULT_MANFUT_THREADS;
            }



        }else{
            System.out.print(Error.color_red);
            System.out.println("No  <Num threads>, setting to defaults num_threads=2");
            System.out.println(Error.end_color);
            num_threads=DEFAULT_MANFUT_THREADS;
        }

        PresupostFitxatges = Integer.parseInt(args[0]);
        PlayersMarket = new Market();

         err = PlayersMarket.LlegirFitxerJugadors(args[1]);
         if (err!=Error.COk)
             Error.showError("[Manfut] ERROR Reading players file.");

        // Calculate the best team.
        MillorEquip=PlayersMarket.CalcularEquipOptim(PresupostFitxatges);

        System.out.print(Error.color_blue);
        System.out.println("-- Best Team -------------------------------------------------------------------------------------");
        MillorEquip.PrintEquipJugadors();
        System.out.println("   Cost " + MillorEquip.CostEquip() +", Points: " + MillorEquip.PuntuacioEquip() + ".");
        System.out.println("--------------------------------------------------------------------------------------------------");
        System.out.print(Error.end_color);

        System.exit(0);
    }


    // Methods Definition
    static int log(int x, int base)
    {
        return (int) Math.ceil((Math.log(x) / Math.log(base)));
    }

    static long log(long x, int base)
    {
        return (long) Math.ceil((Math.log(x) / Math.log(base)));
    }

    static int Log2(int x)
    {
        return (int) (log(x,2) );
    }

    static long Log2(long x)
    {
        return (long) (log(x,2) );
    }
}
