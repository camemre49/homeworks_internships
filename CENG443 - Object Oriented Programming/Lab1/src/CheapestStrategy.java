import java.util.ArrayList;
import java.util.List;

public class CheapestStrategy extends Strategy{

    @Override
    public String getStrategyType() {
        return "Ch";
    }

    @Override
    public void setNextTarget(Customer customer, List<Store> stores) {
        int cheapest = Integer.MAX_VALUE;
        for (Store store : stores) {
            String productType = customer.getShoppingList().get(0).getProductType();
            try {
                int price = store.getPrice(productType);
                if (price < cheapest) {
                    cheapest = price;
                    target = store;
                }
            } catch (IllegalStateException e) {
            }
        }
    }

    @Override
    public void tryToBuy(Customer customer) {
        try {
            target.sell(customer.getShoppingList().get(0).getProductType());
            customer.getShoppingList().remove(0);
            target = null;
        } catch (IllegalStateException e) {
        }
    }
}
