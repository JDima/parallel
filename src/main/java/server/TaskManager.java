package server;
import communication.Protocol;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

public class TaskManager {

    private AtomicInteger taskCount;

    private HashMap<Integer, ClientTask> taskStore;

    public TaskManager() {
        taskCount = new AtomicInteger();
        taskStore = new HashMap<Integer, ClientTask>();
    }

    public int addTask(String clientId, Protocol.Task task) {
        int taskId = taskCount.addAndGet(1);
        ClientTask clientTask = new ClientTask(clientId, task);
        taskStore.put(taskId, clientTask);
        return taskId;
    }

    public LinkedList<Protocol.ListTasksResponse.TaskDescription> getTasks() {
        LinkedList<Protocol.ListTasksResponse.TaskDescription> listTasks = new LinkedList<>();
        for (Map.Entry<Integer, ClientTask> taskInfo : taskStore.entrySet()) {
            ClientTask task = taskInfo.getValue();
            Protocol.ListTasksResponse.TaskDescription.Builder descDuilder = Protocol.ListTasksResponse.TaskDescription.newBuilder();

            descDuilder.setTask(task.getTask());
            descDuilder.setTaskId(taskInfo.getKey());
            if (task.isSolved())
                descDuilder.setResult(task.getResult());
            descDuilder.setClientId(task.getClientId());
        }
        return listTasks;
    }

    public long getResult(int taskId) {
        return 0;
    }
}
