import java.util.List;

public abstract class Strategy {

    protected Store target = null;

    public abstract String getStrategyType();
    public abstract void setNextTarget(Customer customer, List<Store> storeArrayList);

    public boolean isTargetSet() {
        return target != null;
    }

    public Position getTargetPosition() {
        return target.position;
    }

    public abstract void tryToBuy(Customer customer);
}
