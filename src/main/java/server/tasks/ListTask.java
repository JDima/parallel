package server.tasks;

import communication.Protocol;
import server.TaskManager;

import java.io.OutputStream;
import java.net.Socket;
import java.util.LinkedList;

/**
 * Created by JDima on 13/04/16.
 */
public class ListTask extends TaskThread {
    public ListTask(Socket connectionSocket, Protocol.ServerRequest request, TaskManager taskManager) {
        super(connectionSocket, request, taskManager);
    }

    @Override
    public void run() {
        LinkedList<Protocol.ListTasksResponse.TaskDescription> tasks = taskManager.getTasks();
        Protocol.ListTasksResponse.Builder submitTaskResponse = Protocol.ListTasksResponse.newBuilder().addAllTasks(tasks).setStatus(Protocol.Status.OK);
        response.setListResponse(submitTaskResponse);
        super.run();
    }
}
