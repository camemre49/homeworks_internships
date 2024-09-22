import java.util.ArrayList;

public class CengTreeNodeLeaf extends CengTreeNode
{
    private ArrayList<CengBook> books;
    // TODO: Any extra attributes
    private Integer order;

    public CengTreeNodeLeaf(CengTreeNode parent)
    {
        super(parent);

        // TODO: Extra initializations
        books = new ArrayList<CengBook>();
        order = CengTreeNode.order;
        type = CengNodeType.Leaf;
    }

    // GUI Methods - Do not modify
    public int bookCount()
    {
        return books.size();
    }
    public Integer bookKeyAtIndex(Integer index)
    {
        if(index >= this.bookCount()) {
            return -1;
        } else {
            CengBook book = this.books.get(index);

            return book.getBookID();
        }
    }

    // Extra Functions
    public void addBook(CengBook book){
        for(int i = 0; i < books.size(); i++){
            if(book.getBookID() < books.get(i).getBookID()){
                books.add(i, book);
                return;
            }
        }
        books.add(book);
    }

    @Override
    public ArrayList<CengTreeNode> getChildren() {
        return null;
    }

    @Override
    public void setKeys(ArrayList<Integer> bookKeys) {
        return;
    }

    @Override
    public void setBooks(ArrayList<CengBook> books) {
        this.books = books;
    }

    @Override
    public ArrayList<Integer> getKeys() {
        return null;
    }

    @Override
    public ArrayList<CengBook> getBooks() {
        return books;
    }

    @Override
    public void addKey(Integer key) {
        return;
    }
}
