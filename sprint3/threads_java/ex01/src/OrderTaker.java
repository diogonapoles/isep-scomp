class OrderTaker implements Runnable {
    private static int orderCount = 0;
    private final OrderQueue queue;

    public OrderTaker(OrderQueue queue) {
        this.queue = queue;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            var order = new Order(generateOrderNumber());
            queue.add(order);
            System.out.println("Order Taker added order: " + order.id());
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                System.out.println("Thread was interrupted!");
                break;
            }
        }
    }

    private synchronized int generateOrderNumber() {
        return ++orderCount;
    }
}
