import java.util.LinkedList;

class OrderQueue {
    private final LinkedList<Order> orders;

    public OrderQueue() {
        orders = new LinkedList<>();
    }

    public synchronized void add(Order order) {
        orders.add(order);
        notifyAll();
    }

    public synchronized Order add() throws InterruptedException {
        while (orders.isEmpty()) {
            wait();
        }
        return orders.remove();
    }
}