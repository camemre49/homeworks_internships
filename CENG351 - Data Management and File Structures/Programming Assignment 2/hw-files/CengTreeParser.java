import java.io.*;
import java.util.ArrayList;
import java.util.Scanner;

public class CengTreeParser
{
    /**
     * Parse the input files and convert into cengBook list
     * @param filename
     * @return
     */
    public static ArrayList<CengBook> parseBooksFromFile(String filename)
    {
        ArrayList<CengBook> bookList = new ArrayList<CengBook>();

        // Read the file
        Scanner scanner = null;
        try {
            scanner = new Scanner(new File(filename));
        } catch (Throwable e1) {
            System.out.println("HERE");
            e1.printStackTrace();
        }

        // Parsing the input file in order to use GUI tables.
        String currentLine = "";
        while (scanner != null && scanner.hasNextLine())
        {
            currentLine = scanner.nextLine();
            // Parse the line
            bookList.add(parseBook(currentLine));
        }

        return bookList;
    }

    public static void startParsingCommandLine() throws IOException
    {
        ArrayList<CengBook> bookList = parseBooksFromFile(CengBookRunner.getFilenameToParse());

        // TODO: Start listening and parsing command line -System.in-.
        // There are 4 commands:
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        while(true){
            String input = reader.readLine(); // Read and convert to lowercase
            String lowCase = input.toLowerCase();

            // 1) quit : End the app, gracefully. Print nothing, call nothing, just break off your command line loop.
            if(lowCase.startsWith("quit")){
                System.exit(0);
                return;
            }

            // 2) add : Parse and create the book, and call CengBookRunner.addBook(newlyCreatedBook).
            if(lowCase.startsWith("add")){
                String bookInfo = input.substring(4);
                CengBook book = parseBook(bookInfo);
                CengBookRunner.addBook(book);

            }

            // 3) search : Parse the bookID, and call CengBookRunner.searchBook(bookID).
            if(lowCase.startsWith("search")){
                String searchKey = input.substring(7);
                CengBookRunner.searchBook(Integer.parseInt(searchKey));
            }

            // 4) print : Print the whole tree, call CengBookRunner.printTree().
            if(lowCase.startsWith("print")){
                CengBookRunner.printTree();
            }
        }

        // Commands (quit, add, search, print) are case-insensitive.
    }

    private static CengBook parseBook (String currentLine)
    {
        String[] cengBook = currentLine.split("[|]");
        Integer bookId = Integer.parseInt(cengBook[0]);
        String bookTitle = cengBook[1];
        String author = cengBook[2];
        String genre = cengBook[3];
        return new CengBook(bookId, bookTitle, author, genre);
    }
}
