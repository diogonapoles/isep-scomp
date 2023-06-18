import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;

public class Main {
    private static final int N_THREADS = 5;
    private static final int N_PARTITIONS = 10;
    private static final int ARRAY_SIZE = 100;

    public static void main(String[] args) {
        var array = new ArrayList<Integer>();
        for (int i = 1; i <= ARRAY_SIZE; i++)
            array.add(i);

        var executor = Executors.newFixedThreadPool(N_THREADS);

        int size = array.size() / N_PARTITIONS;
        int startIdx = 0;
        int endIdx = size;

        for (int i = 0; i < N_PARTITIONS; i++) {
            if (i == N_PARTITIONS - 1) {
                // last partition can be bigger than the rest
                endIdx = array.size();
            }

            var sum = new Sum(startIdx, endIdx, array);
            executor.submit(sum);

            startIdx = endIdx;
            endIdx += size;
        }

        executor.shutdown();
    }

    private static List<Integer> createArray() {
        List<Integer> array = new ArrayList<>();
        for (int i = 1; i <= 100; i++) {
            array.add(i);
        }
        return array;
    }
}