class OrderHandler implements Runnable {
    private final OrderQueue queue;

    public OrderHandler(OrderQueue queue) {
        this.queue = queue;
    }

    @Override
    public void run() {
        while (!Thread.interrupted()) {
            try {
                Order order = queue.add();
                System.out.println("Order Handler removed order: " + order.id());
                Thread.sleep(500);
            } catch (InterruptedException e) {
                System.out.println("Thread was interrupted!");
                break;
            }
        }
    }
}
