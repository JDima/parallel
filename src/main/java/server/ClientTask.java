package server;

import communication.Protocol;

public class ClientTask {

    private Protocol.Task task;
    private boolean isSolved;
    private int result;
    private String clientId;

    public ClientTask(String clientId, Protocol.Task task) {
        this.task = task;
        this.isSolved = false;
        this.clientId = clientId;
    }

    public Protocol.Task getTask() {
        return task;
    }

    public void setTask(Protocol.Task task) {
        this.task = task;
    }

    public boolean isSolved() {
        return isSolved;
    }

    public int getResult() {
        return result;
    }

    public String getClientId() {
        return clientId;
    }
}
