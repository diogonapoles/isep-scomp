class Transfer {
    public static boolean transferMoney(Account fromAcct, Account toAcct, EurosAmount amount) {
        synchronized (fromAcct) {
            synchronized (toAcct) {
                if (fromAcct.withdraw(amount.amount())) {
                    toAcct.deposit(amount.amount());
                    return true;
                }
            }
        }
        return false;
    }
}
