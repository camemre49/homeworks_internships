import java.awt.Color;
import java.util.ArrayList;

public abstract class CengTreeNode
{
    static protected Integer order;
    private CengTreeNode parent;
    protected CengNodeType type; // Type needs to be set for proper GUI. Check CengNodeType.java.

    // GUI Accessors - do not modify
    public Integer level;
    public Color color;

    // Any extra attributes, if necessary

    public CengTreeNode(CengTreeNode parent)
    {
        this.parent = parent;

        this.color = CengGUI.getRandomBorderColor();

        // TODO: Extra initializations, if necessary.
    }

    // Getters-Setters - Do not modify
    public CengTreeNode getParent()
    {
        return parent;
    }

    public void setParent(CengTreeNode parent)
    {
        this.parent = parent;
    }

    public CengNodeType getType()
    {
        return type;
    }

    // GUI Methods - Do not modify
    public Color getColor()
    {
        return this.color;
    }

    // Extra Functions
    abstract public ArrayList<CengTreeNode> getChildren();

    public Integer getOrder(){
        return order;
    }

    abstract public void addBook(CengBook book);

    abstract public void setKeys(ArrayList<Integer> bookKeys);

    abstract public void setBooks(ArrayList<CengBook> books);

    abstract public ArrayList<Integer> getKeys();
    abstract public ArrayList<CengBook> getBooks();

    abstract public void addKey(Integer key);
}
