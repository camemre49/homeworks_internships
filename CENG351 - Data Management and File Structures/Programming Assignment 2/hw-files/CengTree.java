import java.util.ArrayList;
import java.util.Objects;

public class CengTree
{
    public CengTreeNode root;
    // Any extra attributes...
    public CengTreeNodeLeaf treeRoot;

    public Integer treeOrder;

    public CengTree(Integer order)
    {
        CengTreeNode.order = order;
        treeOrder = order;
        treeRoot = new CengTreeNodeLeaf(null);
        treeRoot.type = CengNodeType.Leaf;
        root = treeRoot;
    }

    public void addBook(CengBook book)
    {
        Integer bookId = book.getBookID();
        CengTreeNode currentNode = root;
        while(currentNode.getType() != CengNodeType.Leaf){
            currentNode = (CengTreeNodeInternal) currentNode;
            Integer childIndex = findChildIndex(currentNode.getKeys(), bookId);
            currentNode = currentNode.getChildren().get(childIndex);
        }
        currentNode = (CengTreeNodeLeaf) currentNode;
        if(currentNode.getBooks().size() < 2 * CengTreeNode.order){
            currentNode.addBook(book);
        }
        else{
            currentNode.addBook(book);
            ArrayList<CengBook> firstLeafBooks = new ArrayList<>(currentNode.getBooks().subList(0, CengTreeNode.order));
            ArrayList<CengBook> secondLeafBooks = new ArrayList<>(currentNode.getBooks().subList(CengTreeNode.order, currentNode.getBooks().size()));
            CengTreeNodeLeaf firstLeaf = new CengTreeNodeLeaf(null);
            firstLeaf.setBooks(firstLeafBooks);
            CengTreeNodeLeaf secondLeaf = new CengTreeNodeLeaf(null);
            secondLeaf.setBooks(secondLeafBooks);

            // If parent is null, i.e while adding to the root
            if(currentNode.getParent() == null){
                CengTreeNodeInternal newNode = new CengTreeNodeInternal(null);
                newNode.addKey(secondLeafBooks.get(0).getBookID());
                root = newNode;
                root.type = CengNodeType.Internal;
                newNode.addChild(0, secondLeaf);
                newNode.addChild(0, firstLeaf);
            }
            else{
                CengTreeNodeInternal parentNode = (CengTreeNodeInternal) currentNode.getParent();
                int index = parentNode.removeChild(currentNode);
                parentNode.addKey(secondLeafBooks.get(0).getBookID());
                parentNode.addChild(index, secondLeaf);
                parentNode.addChild(index, firstLeaf);

                handleInternalNodeSplit(parentNode);
            }
        }
    }

    public ArrayList<CengTreeNode> searchBook(Integer bookID)
    {
        // TODO: Search within whole Tree, return visited nodes.
        CengTreeNode currentNode = root;
        ArrayList<CengTreeNode> visitedNodes = new ArrayList<CengTreeNode>();
        StringBuilder accumulatedString = new StringBuilder();
        int indent = 0;
        while(currentNode.getType() != CengNodeType.Leaf){
            Integer childIndex = findChildIndex(currentNode.getKeys(), bookID);
            accumulatedString.append(writeKeys((CengTreeNodeInternal) currentNode, indent));
            visitedNodes.add(currentNode);
            currentNode = currentNode.getChildren().get(childIndex);
            indent += 1;
        }
        visitedNodes.add(currentNode);
        boolean isFound = writeRecord((CengTreeNodeLeaf) currentNode, bookID, accumulatedString.toString(), indent);

        return isFound ? visitedNodes : null;
    }

    public void printTree()
    {
        depthFirstSearch(root, 0);
    }

    private void depthFirstSearch(CengTreeNode node, int indent){
        if(node == null) return;

        if(node instanceof CengTreeNodeLeaf){
            writeAllRecords((CengTreeNodeLeaf) node, indent);
            return;
        }

        System.out.print(writeKeys((CengTreeNodeInternal) node, indent));

        for(CengTreeNode childNodes: node.getChildren()){
            depthFirstSearch(childNodes, indent + 1);
        }
    }

    // Any extra functions...

    private Integer findChildIndex(ArrayList<Integer> keys, Integer bookId){
        Integer childIndex = -1;
        for(int i = 0; i < keys.size(); i++){
            if(keys.get(i) > bookId){
                return i;
            }
        }
        return keys.size();
    }

    private void handleInternalNodeSplit(CengTreeNodeInternal node){
        if(node.getKeys().size() <= 2 * CengTreeNode.order){
            return;
        }
        else{
            if(node.getParent() == null){
                CengTreeNodeInternal rootNode = new CengTreeNodeInternal(null);
                CengTreeNodeInternal firstNode = new CengTreeNodeInternal(rootNode);
                CengTreeNodeInternal secondNode = new CengTreeNodeInternal(rootNode);
                ArrayList<Integer> firstNodeKeys = new ArrayList<>(node.getKeys().subList(0, CengTreeNode.order));
                ArrayList<Integer> secondNodeKeys = new ArrayList<>(node.getKeys().subList(CengTreeNode.order + 1, node.getKeys().size()));
                firstNode.setKeys(firstNodeKeys);
                firstNode.setChildren(new ArrayList<>(node.getChildren().subList(0, CengTreeNode.order + 1)));
                secondNode.setKeys(secondNodeKeys);
                secondNode.setChildren(new ArrayList<>(node.getChildren().subList(CengTreeNode.order + 1, node.getChildren().size())));
                rootNode.getChildren().add(firstNode);
                rootNode.getChildren().add(secondNode);
                rootNode.addKey(node.getKeys().get(CengTreeNode.order));
                root = rootNode;
                node = null;
            }
            else{
                Integer keyWillPushed = node.getKeys().get(CengTreeNode.order);
                CengTreeNodeInternal secondNode = new CengTreeNodeInternal(node.getParent());
                ArrayList<Integer> firstNodeKeys = new ArrayList<>(node.getKeys().subList(0, CengTreeNode.order));
                ArrayList<Integer> secondNodeKeys = new ArrayList<>(node.getKeys().subList(CengTreeNode.order + 1, node.getKeys().size()));
                node.setKeys(firstNodeKeys);
                secondNode.setKeys(secondNodeKeys);
                secondNode.setChildren(new ArrayList<>(node.getChildren().subList(CengTreeNode.order + 1, node.getChildren().size())));
                node.setChildren(new ArrayList<>(node.getChildren().subList(0, CengTreeNode.order + 1)));
                secondNode.setKeys(secondNodeKeys);

                CengTreeNodeInternal parentNode = (CengTreeNodeInternal) node.getParent();
                int index = parentNode.getChildren().indexOf(node) + 1;
                parentNode.addKey(keyWillPushed);
                parentNode.addChild(index, secondNode);

                handleInternalNodeSplit(parentNode);
            }
        }
    }

    private String writeKeys(CengTreeNodeInternal node, int indent){
        String print = "";
        print = print.concat("\t".repeat(indent) + "<index>\n");
        for(Integer key: node.getKeys()){
            print = print.concat("\t".repeat(indent) + key + "\n");
        }
        return print.concat("\t".repeat(indent) + "</index>\n");
    }

    private boolean writeRecord(CengTreeNodeLeaf node, Integer bookId, String accumulatedString, int indent){
        CengBook book = null;
        for(CengBook bk: node.getBooks()){
            if(Objects.equals(bk.getBookID(), bookId)){
                book = bk;
                break;
            }
        }
        if(book != null){
            System.out.println(accumulatedString + "\t".repeat(indent) +"<record>" + book.getBookID() + "|" + book.getBookTitle() + "|" + book.getAuthor() + "|" + book.getGenre() + "</record>");
            return true;
        }
        else{
            System.out.println("Could not find " + bookId + ".");
            return false;
        }
    }

    private void writeAllRecords(CengTreeNodeLeaf node, int indent){
        System.out.println("\t".repeat(indent) + "<data>");
        for(CengBook book: node.getBooks()){
            System.out.println("\t".repeat(indent) +"<record>" + book.getBookID() + "|" + book.getBookTitle() + "|" + book.getAuthor() + "|" + book.getGenre() + "</record>");
        }
        System.out.println("\t".repeat(indent) + "</data>");
    }
}
