package server.tasks;

import communication.Protocol;
import server.TaskManager;

import java.io.OutputStream;
import java.net.Socket;

/**
 * Created by JDima on 13/04/16.
 */
public class SubmitTask extends TaskThread {

    public SubmitTask(Socket connectionSocket, Protocol.ServerRequest request, TaskManager taskManager) {
        super(connectionSocket, request, taskManager);

    }

    @Override
    public void run() {
        Protocol.Task task = request.getSubmit().getTask();
        int taskId = taskManager.addTask(request.getClientId(), task);

        Protocol.SubmitTaskResponse.Builder submitTaskResponse = Protocol.SubmitTaskResponse.newBuilder().
                setSubmittedTaskId(taskId).
                setStatus(Protocol.Status.OK);
        response.setSubmitResponse(submitTaskResponse);
        super.run();
    }
}
