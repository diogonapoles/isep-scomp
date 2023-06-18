public class Ex1 {
    public static void main(String[] args) {
        var orderQueue = new OrderQueue();

        var orderTaker1 = new Thread(new OrderTaker(orderQueue));
        var orderTaker2 = new Thread(new OrderTaker(orderQueue));
        orderTaker1.start();
        orderTaker2.start();

        var orderHandler1 = new Thread(new OrderHandler(orderQueue));
        var orderHandler2 = new Thread(new OrderHandler(orderQueue));
        orderHandler1.start();
        orderHandler2.start();

        // Join the threads
        try {
            Thread.sleep(15000);
            orderTaker1.interrupt();
            orderTaker2.interrupt();
            orderHandler1.interrupt();
            orderHandler2.interrupt();

            orderTaker1.join();
            orderTaker2.join();
            orderHandler1.join();
            orderHandler2.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        System.out.println("All threads have finished execution.");
    }
}
