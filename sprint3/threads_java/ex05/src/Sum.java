import java.util.List;

public class Sum implements Runnable {
    private final int startIncl;
    private final int endExcl;
    private final List<Integer> array;

    public Sum(int startIncl, int endExcl, List<Integer> array) {
        this.startIncl = startIncl;
        this.endExcl = endExcl;
        this.array = array;
    }

    @Override
    public void run() {
        int sum = 0;
        for (int i = startIncl; i < endExcl; i++) {
            sum += array.get(i);
        }
        System.out.println("Sum: " + sum);
    }
}