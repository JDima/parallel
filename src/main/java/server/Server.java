package server;


import communication.Common;
import communication.Protocol;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.LinkedList;
import java.util.List;

public class Server extends Thread{

    private int port;
    private String host;
    private TaskManager taskManager;

    public Server(String host, int port) {
        this.host = host;
        this.port = port;
        this.taskManager = new TaskManager();

    }

    public static void main(String args[]) {

    }

    public int getPort() {
        return port;
    }

    public String getHost() {
        return host;
    }

    public Protocol.ServerResponse.Builder getSubmitResponse(Protocol.ServerRequest request,
                                                             Protocol.ServerResponse.Builder response) {

        Protocol.Task task = request.getSubmit().getTask();
        int taskId = taskManager.addTask(task);

        Protocol.SubmitTaskResponse.Builder submitTaskResponse = Protocol.SubmitTaskResponse.newBuilder().
                setSubmittedTaskId(taskId).
                setStatus(Protocol.Status.OK);
        response.setSubmitResponse(submitTaskResponse);
        return response;
    }

    public Protocol.ServerResponse.Builder getListResponse(Protocol.ServerResponse.Builder response) {
        LinkedList<Protocol.ListTasksResponse.TaskDescription> tasks = taskManager.getTasks();
        Protocol.ListTasksResponse.Builder submitTaskResponse = Protocol.ListTasksResponse.newBuilder().addAllTasks(tasks);
        response.setListResponse(submitTaskResponse);
        return response;
    }

    public Protocol.ServerResponse.Builder getSubscribeResponse(Protocol.ServerRequest request,
                                                                Protocol.ServerResponse.Builder response) {
        long result = taskManager.getResult(request.getSubscribe().getTaskId());
        Protocol.SubscribeResponse.Builder subscribeTaskResponse = Protocol.SubscribeResponse.newBuilder().setValue(result);
        response.setSubscribeResponse(subscribeTaskResponse);
        return response;
    }

    public void writeMessageToStream(Protocol.ServerResponse.Builder response, OutputStream out) throws IOException {
        Protocol.WrapperMessage responseMessage = Protocol.WrapperMessage.newBuilder().
                setResponse(response).
                build();

        Common.printTaskRepsonse(responseMessage.getResponse());
        responseMessage.writeTo(out);
    }

    public void run() {
        try (ServerSocket serverSock = new ServerSocket()) {
            serverSock.bind(new InetSocketAddress(this.host, this.port));
            System.out.println("Server was started.");

            while(true) {
                try ( Socket connectionSocket = serverSock.accept();
                      InputStream in = connectionSocket.getInputStream();
                      OutputStream out = connectionSocket.getOutputStream()) {

                    Protocol.ServerRequest request = Protocol.WrapperMessage.parseDelimitedFrom(in).getRequest();
                    Common.printServerRequest(request);

                    Protocol.ServerResponse.Builder response = Protocol.ServerResponse.newBuilder().
                            setRequestId(request.getRequestId());

                    if (request.hasSubmit()) {
                        response = getSubmitResponse(request, response);
                    } else if (request.hasList()) {
                        response = getListResponse(response);
                    } else if (request.hasSubscribe()) {
                        response = getSubscribeResponse(request, response);
                    } else {
                        System.err.println("Unknown type of request");
                    }

                    writeMessageToStream(response, out);
                } catch (IOException e) {
                    System.err.println("Server problems");
                    e.printStackTrace();
                }
            }
        } catch (IOException e) {
            System.err.println("Error create server at port: " + this.port);
            e.printStackTrace();
        }
    }
}
