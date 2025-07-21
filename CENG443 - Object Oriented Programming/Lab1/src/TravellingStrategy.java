import java.util.ArrayList;
import java.util.List;

public class TravellingStrategy extends Strategy{

    protected ArrayList<Store> visitedStores = new ArrayList<>();

    @Override
    public String getStrategyType() {
        return "Tr";
    }

    @Override
    public void setNextTarget(Customer customer, List<Store> storeArrayList) {
        double closest = Double.POSITIVE_INFINITY;
        for (Store store : storeArrayList) {
            if (!visitedStores.contains(store)) {
                double dist = customer.getPosition().distanceTo(store.getPosition());
                if (dist < closest) {
                    closest = dist;
                    target = store;
                }
            }
        }
        if (closest == Double.POSITIVE_INFINITY) {
            visitedStores = new ArrayList<Store>();
        }
    }

    @Override
    public void tryToBuy(Customer customer) {
        for (int i = 0; i < customer.shoppingList.size(); i++) {
            try {
                target.sell(customer.shoppingList.get(i).getProductType());
                customer.shoppingList.remove(i);
                return;
            }
            catch (IllegalStateException e){
                if (e.getMessage().contains("Out of Stock")){
                    break;
                }
            }
        }
        visitedStores.add(target);
        target = null;
    }
}
