import java.awt.*;

public class Store extends Entity{
    protected final Product product;
    private final int maxStock;
    int stock;
    int sellingPrice;


    public Store(double x, double y, Product product, int sellingPrice, int maximumStock) {
        super(x, y);
        this.product = product;
        this.maxStock = maximumStock;
        this.stock = maximumStock;
        this.sellingPrice = sellingPrice;
    }

    public String getProductType() {
        return product.getProductType();
    }

    public int getPrice(String productType) {
        if(this.product.getProductType().equals(productType)){
            return sellingPrice;
        }
        else{
            throw new IllegalStateException("Product Not Found");
        }
    }

    public void sell(String productType) {
        if (!getProductType().contains(productType)) {
            throw new IllegalStateException("Product not found");
        }
        if (stock == 0) {
            throw new IllegalStateException("Out of Stock");
        }
        stock -= 1;
    }

    @Override
    public Position getPosition() {
        return super.getPosition();
    }

    @Override
    public void draw(Graphics2D g2d) {
        String text = product.getProductType();
        drawHelper(g2d, text + "," + stock + "," + sellingPrice, Color.ORANGE);

    }

    @Override
    protected void drawHelper(Graphics2D g2d, String text, Color color) {
        super.drawHelper(g2d, text, color);
    }

    @Override
    public void step() {

    }

    public void replenish() {
        stock = maxStock;
    }
}
