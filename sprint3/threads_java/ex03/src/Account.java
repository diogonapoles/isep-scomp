class Account {
    private int balance;

    public Account(int balance) {
        this.balance = balance;
    }

    public synchronized int balance() {
        return balance;
    }

    public synchronized void deposit(int amount) {
        balance += amount;
    }

    public synchronized boolean withdraw(int amount) {
        if (this.balance() >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }
}
