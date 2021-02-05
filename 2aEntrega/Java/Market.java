/* ---------------------------------------------------------------
Práctica 1.
Código fuente: Market.java
Grau Informàtica
39939768S - Pere Antoni Rollon Baiges
47433543E - Francisco Javier Roig Gregorio
--------------------------------------------------------------- */

package eps.udl.cat;

import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.locks.*;


public class Market {
    public static CyclicBarrier JoinBarrier = new CyclicBarrier(Manfut.num_threads+1);
    public static int pending_local_print = 0;
    public static JugadorsEquip shared_MillorEquip = null;
    public static int MaxPuntuacio = -1;

    ArrayList<Jugador> Jugadors;

    int NJugadors;
    int NPorters;
    int NDefensors;
    int NMitjos;
    int NDelanters;

    Market() {
        Jugadors = new ArrayList<Jugador>();
    }

    Jugador GetJugador(int j) {
        return Jugadors.get(j);
    }

    Jugador GetPorter(int j) {
        return Jugadors.get(j);
    }

    Jugador GetDefensor(int j) {
        return Jugadors.get(NPorters + j);
    }

    Jugador GetMitg(int j) {
        return Jugadors.get(NPorters + NDefensors + j);
    }

    Jugador GetDelanter(int j) {
        return Jugadors.get(NPorters + NDefensors + NMitjos + j);
    }

    // Read file with the market players list (each line containts a plater: "Id.;Name;Position;Cost;Team;Points")
    Error LlegirFitxerJugadors(String pathJugadors) {
        long Offset = 0;
        FileInputStream fis;

        try {
            fis = new FileInputStream(pathJugadors);
        } catch (FileNotFoundException e) {
            System.err.println("[Market:LlegirFitxerJugadors] File " + pathJugadors + " not found.");
            e.printStackTrace();
            return (Error.CErrorOpenInputFile);
        }

        try {
            //Construct BufferedReader from InputStreamReader
            BufferedReader br = new BufferedReader(new InputStreamReader(fis));
            br.readLine(); // skip csv header

            // Read players.
            NJugadors = NPorters = NDefensors = NMitjos = NDelanters = 0;
            String line = null;
            while ((line = br.readLine()) != null) {
                String fields[] = line.split(";");
                Jugador player = new Jugador();

                // Player's identificator
                player.setId(Integer.parseInt(fields[0]));

                // Player's name
                player.setNom(fields[1]);

                // Player's position
                switch (fields[2]) {
                    case "Portero":
                        player.setTipus(TipusJug.JPorter);
                        NPorters++;
                        break;

                    case "Defensa":
                        player.setTipus(TipusJug.JDefensor);
                        NDefensors++;
                        break;

                    case "Medio":
                        player.setTipus(TipusJug.JMitg);
                        NMitjos++;
                        break;

                    case "Delantero":
                        player.setTipus(TipusJug.JDelanter);
                        NDelanters++;
                        break;

                    default:
                        System.err.println("[Market:LlegirFitxerJugadors] Error player type.");
                        return (Error.CErrorPlayerType);
                }

                // Player's cost
                player.setCost(Integer.parseInt(fields[3]));

                // Player's team
                player.setEquip(fields[4]);
                ;

                // Player's points
                player.setPunts(Integer.parseInt(fields[5]));

                NJugadors++;
                Jugadors.add(player);
            }
            br.close();

        } catch (IOException e) {
            System.err.println("[Market:LlegirFitxerJugadors] Reading file " + pathJugadors + ".");
            e.printStackTrace();
            return (Error.CErrorReadingFile);
        }

        return (Error.COk);
    }


    JugadorsEquip CalcularEquipOptim(int PresupostFitxatges) {
        long maxbits;
        int first, end, equip, primerEquip, ultimEquip;
        //int MaxPuntuacio = -1;
        //JugadorsEquip MillorEquip = null;

        //PrintJugadors();

        // Calculated number of bits required for all teams codification.
        maxbits = Manfut.Log2(NPorters) * JugadorsEquip.DPosPorters + Manfut.Log2(NDefensors) * JugadorsEquip.DPosDefensors + Manfut.Log2(NMitjos) * JugadorsEquip.DPosMitjos + Manfut.Log2(NDelanters) * JugadorsEquip.DPosDelanters;
        if (maxbits > Manfut.Log2(Long.MAX_VALUE))
            Error.showError("[Market:CalcularEquipOptim] The number of player overflow the maximum width supported.");

        // Calculate first and end team that have to be evaluated.
        first = primerEquip = GetEquipInicial();
        end = ultimEquip = (int) Math.pow(2, maxbits);

        // Evaluating different teams/combinations.
        System.out.println("Evaluating form " + String.format("%x",first) + "H to " + String.format("%x",end) + "H (Maxbits: "+ maxbits + "). Evaluating "+ (end-first)+"  teams...");

        //------------------AREA CONCURRENT ----------------------------------------
        int num_threads = Manfut.num_threads;

        try {
            ManfutMessages messages = new ManfutMessages();
            messages.start();
        } catch (Exception e) {
            e.printStackTrace();
        }

        int num_steps = end - first;
        int[][] intervals = new int[num_threads][2];
        ManfutThread[] threads = new ManfutThread[num_threads];

        for (int h = 0; h < num_threads; h++) {
            if (h > 0) {
                intervals[h][0] = intervals[h - 1][1] + 1;
            } else {
                intervals[h][0] = first;
            }
            int block = (num_steps / (num_threads - h));

            if (h == (num_threads - 1)) {
                intervals[h][1] = end;
            } else {
                intervals[h][1] = intervals[h][0] + block;
            }
            try {
                threads[h] = new ManfutThread(intervals[h][0], intervals[h][1] , PresupostFitxatges, this);
                threads[h].start();
            } catch (Exception e) {
                e.printStackTrace();
            }

            num_steps -= block;
        }

        try{
            //Join
            JoinBarrier.await();
            ManfutMessages.mutex.lock();
            ManfutMessages.stop_();
            ManfutMessages.stopped_thread.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }finally {
            ManfutMessages.mutex.unlock();
        }

        return(Market.shared_MillorEquip);
    }


    // Calculate the initial team combination.
    int GetEquipInicial() {
        int p, equip = 0;
        int bitsPorters, bitsDefensors, bitsMitjos, bitsDelanters;

        bitsPorters = Manfut.Log2(NPorters);
        bitsDefensors = Manfut.Log2(NDefensors);
        bitsMitjos = Manfut.Log2(NMitjos);
        bitsDelanters = Manfut.Log2(NDelanters);

        for (p = JugadorsEquip.DPosDelanters - 1; p >= 0; p--) {
            equip += p;
            equip = equip << bitsDelanters;
        }

        for (p = JugadorsEquip.DPosMitjos - 1; p >= 0; p--) {
            equip += p;
            equip = equip << bitsMitjos;
        }

        for (p = JugadorsEquip.DPosDefensors - 1; p >= 0; p--) {
            equip += p;
            equip = equip << bitsDefensors;
        }

        for (p = JugadorsEquip.DPosPorters - 1; p > 0; p--) {
            equip += p;
            equip = equip << bitsPorters;
        }

        return (equip);
    }


    // Convert team combination to an struct with all the player by position.
    // Returns false if the team is not valid.
    JugadorsEquip ObtenirJugadorsEquip(IdEquip equip) {
        int p;
        int bitsPorters, bitsDefensors, bitsMitjos, bitsDelanters;
        JugadorsEquip jugadors = new JugadorsEquip(this);

        bitsPorters = Manfut.Log2(NPorters);
        bitsDefensors = Manfut.Log2(NDefensors);
        bitsMitjos = Manfut.Log2(NMitjos);
        bitsDelanters = Manfut.Log2(NDelanters);

        for (p = 0; p < JugadorsEquip.DPosPorters; p++) {
            jugadors.setPorter((equip.getIdEquip() >> (bitsPorters * p)) & ((int) Math.pow(2, bitsPorters) - 1));
            if (jugadors.getPorter() >= NPorters)
                return null;
        }

        for (p = 0; p < JugadorsEquip.DPosDefensors; p++) {
            jugadors.setDefensor(p, (equip.getIdEquip() >> ((bitsPorters * JugadorsEquip.DPosPorters) + (bitsDefensors * p))) & ((int) Math.pow(2, bitsDefensors) - 1));
            if (jugadors.getDefensor(p) >= NDefensors)
                return null;
        }

        for (p = 0; p < JugadorsEquip.DPosMitjos; p++) {
            jugadors.setMitg(p, (equip.getIdEquip() >> ((bitsPorters * JugadorsEquip.DPosPorters) + (bitsDefensors * JugadorsEquip.DPosDefensors) + (bitsMitjos * p))) & ((int) Math.pow(2, bitsMitjos) - 1));
            if (jugadors.getMitg(p) >= NMitjos)
                return null;
        }

        for (p = 0; p < JugadorsEquip.DPosDelanters; p++) {
            jugadors.setDelanter(p, (equip.getIdEquip() >> ((bitsPorters * JugadorsEquip.DPosPorters) + (bitsDefensors * JugadorsEquip.DPosDefensors) + (bitsMitjos * JugadorsEquip.DPosMitjos) + (bitsDelanters * p))) & ((int) Math.pow(2, bitsDelanters) - 1));
            if (jugadors.getDelanter(p) >= NDelanters)
                return null;
        }

        return jugadors;
    }


    // Prints all market players information,
    void PrintJugadors() {

        int j;

        for (j = 0; j < NJugadors; j++) {
            GetJugador(j).print();
        }
    }

    static class ManfutThread extends Thread{

        private final int first;
        private final int end;
        private final int PresupostFitxatges;

        private final Market market;
        private final ManfutStats local_statistics = new ManfutStats();

        public static final Lock mutex = new ReentrantLock();

        ManfutThread(int first, int end, int PresupostFitxatges, Market market){
            this.first = first;
            this.end = end;
            this.market = market;
            this.PresupostFitxatges = PresupostFitxatges;

        }

        @Override
        public void run(){
            // El método super no hace nada, solo se invoca por buenas prácticas de programación
            super.run();
            int equip;

            String msg= String.format("%sThread: %d  Evaluating form %xH to %xH Evaluating %d teams...\n%s",Error.color_orange,Thread.currentThread().getId(),first,end,(end-first),Error.end_color);
            ManfutMessages.sendToStorge(msg);

            for (equip=first;equip<=end;equip++) {
                JugadorsEquip jugadors;

                this.local_statistics.evalComb++;

                // Get playes from team number. Returns false if the team is not valid.
                if (this.local_statistics.evalComb%Manfut.M == 0){
                    print_local_statistics();
                }

                // Get playes from team number. Returns false if the team is not valid.
                if ((jugadors=market.ObtenirJugadorsEquip(new IdEquip(equip)))==null) {
                    this.local_statistics.invld++;
                    continue;
                }

                // Reject teams with repeated players.
                if (jugadors.JugadorsRepetits()){
                    msg = "Team " + equip + "->"+Error.color_red +" Invalid." + "\r"+ Error.end_color;
                    //ManfutMessages.sendToStorge(msg);
                    local_statistics.invld++;
                    continue;	// Equip no valid.
                }
                mutex.lock();
                int costEquip = jugadors.CostEquip();
                int puntuacioEquip = jugadors.PuntuacioEquip();
                // Check if the team points is bigger than current optimal team, then evaluate if the cost is lower than the available budget
                if (puntuacioEquip > Market.MaxPuntuacio && costEquip < this.PresupostFitxatges) {
                    msg="Team " + equip + "->"+Error.color_green + " Cost: " + jugadors.CostEquip() + " Points: " + jugadors.PuntuacioEquip() + ". " + Error.end_color+"\n";
                    ManfutMessages.sendToStorge(msg);
                    // We have a new partial optimal team.
                    Market.MaxPuntuacio=puntuacioEquip;
                    Market.shared_MillorEquip = jugadors;
                }
                else{
                    msg=Error.end_color+"Team " + equip + "->" + " Cost: " + jugadors.CostEquip() + " Points: " + jugadors.PuntuacioEquip() + ". " + Error.end_color+"\n";
                    //ManfutMessages.sendToStorge(msg);
                }
                mutex.unlock();

                this.local_statistics.make_statistics(jugadors, costEquip, puntuacioEquip, this.PresupostFitxatges,equip);
            }

            Manfut.remainingThreads++;

            while (Manfut.remainingThreads <= Manfut.num_threads){

                print_local_statistics();

                try {
                    Market.JoinBarrier.await();
                } catch (InterruptedException | BrokenBarrierException e) {
                    e.printStackTrace();
                }
            }
        }

        public void print_local_statistics() {
            try{
                mutex.lock();
                this.local_statistics.print_();
                Manfut.general_stats.sync_local_statistics(this.local_statistics);
                if (Market.pending_local_print == Manfut.num_threads - 1){
                    Manfut.general_stats.print_statistics_();
                    Market.pending_local_print = 0;
                    Manfut.general_stats.reset_();

                    if (Manfut.remainingThreads == Manfut.num_threads) {
                        Manfut.remainingThreads++;

                    }
                } else {
                    Market.pending_local_print++;
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                mutex.unlock();
            }
        }
    }
    static class ManfutMessages extends Thread{
        private static final int MAX_MESSAGES = 100;
        private static final List<String> storage = new ArrayList<>();
        private static boolean running = false;
        public static final Lock mutex = new ReentrantLock();
        private static final Condition addCondition = mutex.newCondition();
        public static final Condition stopped_thread = mutex.newCondition();
        private static final Semaphore semaphore = new Semaphore(MAX_MESSAGES);

        public ManfutMessages(){

        }
        private void printAll_(){
            for (String msg : storage) {
                System.out.print(msg);
            }
            storage.clear();
        }

        public static void sendToStorge(String message) {
            try {
                semaphore.acquire();
                mutex.lock();
                storage.add(message);
                addCondition.signal();
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                mutex.unlock();
            }
        }

        public static void stop_(){
            running=true;
            addCondition.signal();
        }

        @Override
        public void run(){
            // El método super no hace nada, solo se invoca por buenas prácticas de programación
            super.run();
            while (!running){
                try{
                    mutex.lock();

                    while (storage.size() != MAX_MESSAGES && !running)
                        addCondition.await();

                    if (storage.size() > 0) {
                        printAll_();
                        semaphore.release(MAX_MESSAGES);
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                } finally {
                    mutex.unlock();
                }
            }


            // received stop request
            try{
                mutex.lock(); // prevent IllegalMonitorStateException
                stopped_thread.signal();
            } finally {
                mutex.unlock();
            }
        }
    }
    public static class ManfutStats {

        public int evalComb;
        public int invld;
        public int vld;

        public float avg_cost;
        public float avg_points;

        public int bestName;
        public JugadorsEquip best;
        public int best_cost;
        public int best_points;

        public int worstName;
        public JugadorsEquip worst;
        public int worst_cost;
        public int worst_points;

        public ManfutStats(){
            this.evalComb = 0;
            this.invld = 0;
            this.vld = 0;
            this.avg_cost = 0;
            this.avg_points = 0;
        }

        public void reset_() {
            this.evalComb = 0;
            this.invld = 0;
            this.vld = 0;
            this.avg_cost = 0;
            this.avg_points = 0;
        }

        public void make_statistics(JugadorsEquip jugadors, int costEquip, int puntuacioEquip, int PresupostFitxatges, int equip) {
            this.avg_cost = ((this.avg_cost * this.vld) + costEquip) / (this.vld+1);
            this.avg_points = ((this.avg_points * this.vld) + puntuacioEquip) / (this.vld+1);
            this.vld++;
            if (puntuacioEquip > this.best_points && costEquip < PresupostFitxatges){
                // We have a new partial optimal team.
                this.bestName = equip;
                this.best = jugadors;
                this.best_cost = costEquip;
                this.best_points = puntuacioEquip;

            }else if (this.worst_points == 0 || puntuacioEquip < this.worst_points) {
                this.worstName=equip;
                this.worst = jugadors;
                this.worst_cost = costEquip;
                this.worst_points = puntuacioEquip;

            }
        }

        public void sync_local_statistics(ManfutStats local_statistics){
            this.evalComb += local_statistics.evalComb;
            this.invld += local_statistics.invld;
            if (local_statistics.vld != 0) {
                this.avg_cost = ((this.avg_cost * this.vld) + (local_statistics.avg_cost * local_statistics.vld)) / (this.vld + local_statistics.vld);
                this.avg_points = ((this.avg_points * this.vld) + (local_statistics.avg_points * local_statistics.vld)) / (this.vld + local_statistics.vld);
                this.vld += local_statistics.vld;
                if (this.best_points == 0 || local_statistics.best_points > this.best_points) {
                    // We have a new partial optimal team
                    this.bestName = local_statistics.bestName;
                    this.best = local_statistics.best;
                    this.best_cost = local_statistics.best_cost;
                    this.best_points = local_statistics.best_points;

                }
                if (this.worst_points == 0 || (local_statistics.worst_points < this.worst_points && local_statistics.worst_points != 0)) {
                    this.worstName = local_statistics.worstName;
                    this.worst = local_statistics.worst;
                    this.worst_cost = local_statistics.worst_cost;
                    this.worst_points = local_statistics.worst_points;

                }
            }
        }

        public void print_(){
            String msg = String.format("" +
                            "%s+++++++++++++++++++++++++++++++ ManfutThread %d +++++++++++++++++++++++++++++++\n" +
                            "++ Eval Comb: %d \tValidas: %d \tInvalidas: %d\n" +
                            "++ Avg cost: %f \tAvg Score: %f\n" +
                            "++ Mejor Equipo (desde el punto de vista de la puntuación):Team %d -> Cost %d, Points: %d.\n" +
                            "++ Peor Equipo (desde el punto de vista de la puntuación):Team %d  -> Cost %d, Points: %d.\n" +
                            "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++%s\n",
                    Error.end_color, Thread.currentThread().getId(),
                    this.evalComb,this.vld,this.invld,
                    this.avg_cost,this.avg_points,
                    this.bestName,this.best_cost, this.best_points,
                    this.worstName,this.worst_cost,this.worst_points,
                    Error.end_color);

            ManfutMessages.sendToStorge( msg);
        }

        public void print_statistics_(){
            String msg = String.format("" +
                            "%s+++++++++++++++++++++++++++++++ general +++++++++++++++++++++++++++++++\n" +
                            "++ Eval Comb: %d \tValidas: %d \tInvalidas: %d\n" +
                            "++ Avg cost: %f \tAvg Score: %f\n" +
                            "++ Mejor Equipo (desde el punto de vista de la puntuación):Team %d -> Cost %d, Points: %d.\n" +
                            "++ Peor Equipo (desde el punto de vista de la puntuación):Team %d -> Cost %d, Points: %d.\n" +
                            "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++%s\n",
                    Error.color_yellow,
                    this.evalComb,this.vld,this.invld,
                    this.avg_cost,this.avg_points,
                    this.bestName,this.best_cost, this.best_points,
                    this.worstName,this.worst_cost,this.worst_points,
                    Error.end_color);

            ManfutMessages.sendToStorge( msg);

        }

    }
}
