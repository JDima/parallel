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

    public class Request extends Thread {
        private Socket connectionSocket;

        public Request (Socket connectionSocket) {
            this.connectionSocket = connectionSocket;
        }

        @Override
        public void run() {
            try (InputStream in = connectionSocket.getInputStream()) {

                Protocol.ServerRequest request = Protocol.WrapperMessage.parseDelimitedFrom(in).getRequest();
                Thread thread;
                if (request.hasSubmit()) {
                    thread = new Thread(new SubmitTask(connectionSocket, request, taskManager));
                } else if (request.hasList()) {
                    thread = new Thread(new ListTask(connectionSocket, request, taskManager));
                } else if (request.hasSubscribe()) {
                    thread = new Thread(new SubscribeTask(connectionSocket, request, taskManager));
                } else {
                    System.err.println("Unknown type of request");
                    throw new IOException();
                }
                thread.start();
                thread.join();

            } catch (IOException e) {
                System.err.println("Server problems");
                e.printStackTrace();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void run() {
        try (ServerSocket serverSock = new ServerSocket()) {
            serverSock.bind(new InetSocketAddress(this.host, this.port));
            System.out.println("Server was started.");

            while(true) {
                new Request(serverSock.accept()).start();
            }
        } catch (IOException e) {
            System.err.println("Error create server at port: " + this.port);
            e.printStackTrace();
        }
    }
}
