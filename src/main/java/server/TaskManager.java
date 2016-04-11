package server;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by Dmitri_Jakovlev on 4/11/2016.
 */

public class TaskManager {

    private HashMap<Integer, ClientTask> taskStore;

    public TaskManager() {
        taskStore = new HashMap<Integer, ClientTask>();
    }

    public void addTask(int id, ClientTask task) {
        taskStore.put(id, task);
    }

}
