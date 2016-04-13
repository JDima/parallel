package server;


import communication.Common;
import communication.Protocol;
import server.tasks.ListTask;
import server.tasks.SubmitTask;
import server.tasks.SubscribeTask;

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

    public void run() {
        try (ServerSocket serverSock = new ServerSocket()) {
            serverSock.bind(new InetSocketAddress(this.host, this.port));
            System.out.println("Server was started.");

            while(true) {
                try {
                    Socket connectionSocket = serverSock.accept();
                    InputStream in = connectionSocket.getInputStream();

                    Protocol.ServerRequest request = Protocol.WrapperMessage.parseDelimitedFrom(in).getRequest();

                    if (request.hasSubmit()) {
                        new Thread(new SubmitTask(connectionSocket, request, taskManager)).start();
                    } else if (request.hasList()) {
                        new Thread(new ListTask(connectionSocket, request, taskManager)).start();
                    } else if (request.hasSubscribe()) {
                        new Thread(new SubscribeTask(connectionSocket, request, taskManager)).start();
                    } else {
                        System.err.println("Unknown type of request");
                    }

                    in.close();
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
