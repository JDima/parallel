package server;
import communication.Protocol;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;

public class TaskManager {

    private AtomicInteger taskCount;

    private HashMap<Integer, ClientTask> taskStore;

    public TaskManager() {
        taskCount = new AtomicInteger();
        taskStore = new HashMap<Integer, ClientTask>();
    }

    public int addTask(Protocol.Task task) {
        int taskId = taskCount.addAndGet(1);
        ClientTask clientTask = new ClientTask(task);
        taskStore.put(taskId, clientTask);
        return taskId;
    }

    public LinkedList<Protocol.ListTasksResponse.TaskDescription> getTasks() {
        return null;
    }

    public long getResult(int taskId) {
        return 0;
    }
}
