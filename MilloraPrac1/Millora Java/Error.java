package eps.udl.cat;



public enum Error {

    COk, CError, CErrorPlayerType, CErrorOpenInputFile, CErrorReadingFile;

    static String color_red = "\33[01;31m";
    static String color_green = "\033[01;32m";
    static String color_blue = "\033[01;34m";
    static String end_color = "\033[00m";

    public static void showError(String message)
    {
        System.err.println(message);
        System.exit(1);
    }

}