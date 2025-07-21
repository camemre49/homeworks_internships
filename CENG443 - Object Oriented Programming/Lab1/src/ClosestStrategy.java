import java.util.List;

public class ClosestStrategy extends Strategy{

    private Store previousTarget;
    @Override
    public String getStrategyType() {
        return "Cl";
    }

    @Override
    public void setNextTarget(Customer customer, List<Store> storeArrayList) {
        customer.shoppingList.get(0).getProductType();
        double closest = Double.POSITIVE_INFINITY;
        for (Store store : storeArrayList) {
            if (store.getProductType().contains(customer.getShoppingList().get(0).getProductType()) && store != previousTarget) {
                double dist = store.position.distanceTo(customer.position);
                if (dist < closest) {
                    closest = dist;
                    target = store;
                }
            }
        }
    }

    @Override
    public void tryToBuy(Customer customer) {
        try {
            target.sell(customer.getShoppingList().get(0).getProductType());
            customer.getShoppingList().remove(0);
        } catch (IllegalStateException e) {
            previousTarget = target;
        }
        target = null;
    }
}
