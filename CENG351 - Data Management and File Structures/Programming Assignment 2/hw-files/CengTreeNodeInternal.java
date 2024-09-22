import java.util.ArrayList;

public class CengTreeNodeInternal extends CengTreeNode
{
    private ArrayList<Integer> keys;
    private ArrayList<CengTreeNode> children;

    private Integer order;
    public CengTreeNodeInternal(CengTreeNode parent)
    {
        super(parent);

        // TODO: Extra initializations, if necessary.
        keys = new ArrayList<Integer>();
        children = new ArrayList<CengTreeNode>();
        order = CengTreeNode.order;
        type = CengNodeType.Internal;
    }

    // GUI Methods - Do not modify
    public ArrayList<CengTreeNode> getAllChildren()
    {
        return this.children;
    }
    public Integer keyCount()
    {
        return this.keys.size();
    }
    public Integer keyAtIndex(Integer index)
    {
        if(index >= this.keyCount() || index < 0)
        {
            return -1;
        }
        else
        {
            return this.keys.get(index);
        }
    }

    // Extra Functions

    public ArrayList<CengTreeNode> getChildren() {
        return children;
    }

    public ArrayList<Integer> getKeys() {
        return keys;
    }

    public void addBook(CengBook book) {
        if(keys.size() < order){
            keys.add(book.getBookID());
        }
    }

    @Override
    public void setKeys(ArrayList<Integer> bookKeys) {
        this.keys = bookKeys;
    }

    @Override
    public void setBooks(ArrayList<CengBook> books) {
        return;
    }

    @Override
    public ArrayList<CengBook> getBooks() {
        return null;
    }

    @Override
    public void addKey(Integer key) {
        for(int i = 0; i < keys.size(); i++){
            if(key < keys.get(i)){
                keys.add(i, key);
                return;
            }
        }
        keys.add(key);
    }

    public void addChild(Integer index, CengTreeNode node){
        node.setParent(this);
        if(index != null){
            children.add(index, node);
        }
        else {
            children.add(node);
        }
    }

    public int removeChild(CengTreeNode node){
        int index = children.indexOf(node);
        children.remove(index);
        return index;
    }

    public void setChildren(ArrayList<CengTreeNode> children) {
        this.children.clear();
        for(CengTreeNode node: children){
            this.addChild(null, node);
        }
    }
}
