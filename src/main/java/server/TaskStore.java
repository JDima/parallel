package server;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * Created by JDima on 13/04/16.
 */
public class TaskStore {
    private HashMap<Integer, ClientTask> taskStore;

    public TaskStore () {
        taskStore = new HashMap<Integer, ClientTask>();
    }

    public boolean isSolved(int taskId) {
        return taskStore.get(taskId).isSolved();
    }

    public synchronized void addTask(int taskId, ClientTask clientTask) {
        taskStore.put(taskId, clientTask);
    }

    public Set<Map.Entry<Integer, ClientTask> > entrySet() {
        return taskStore.entrySet();
    }

    public long getSolution(int taskId) {
        return taskStore.get(taskId).getResult();
    }

    public synchronized void updateSolution(int taskId, long result) {
        taskStore.get(taskId).updateSolution(result);
    }
}
