public class Main {
    public static void main(String[] args) {
        Account account1 = new Account(1000);
        Account account2 = new Account(2000);

        Thread thread1 = new Thread(() -> {
            var amount = new EurosAmount(500);
            if (Transfer.transferMoney(account1, account2, amount)) {
                System.out.println("Transfer successful!");
            } else {
                System.out.println("Transfer failed!");
            }
        });

        Thread thread2 = new Thread(() -> {
            var amount = new EurosAmount(700);
            if (Transfer.transferMoney(account1, account2, amount)) {
                System.out.println("Transfer successful!");
            } else {
                System.out.println("Transfer failed!");
            }
        });

        thread1.start();
        thread2.start();
    }
}
