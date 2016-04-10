package server;


import communication.Common;
import communication.Protocol;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class Server extends Thread{

    private int port;
    private String host;

    public Server(String host, int port) {
        this.host = host;
        this.port = port;
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
        String clientSentence;
        String capitalizedSentence;

        try {
            ServerSocket serverSock = new ServerSocket();
            serverSock.bind(new InetSocketAddress(this.host, this.port));

            System.out.println("Server was started.");
            while(true) {
                Socket connectionSocket = serverSock.accept();
                InputStream in = connectionSocket.getInputStream();
                OutputStream out = connectionSocket.getOutputStream();

                Protocol.WrapperMessage requestMessage = Protocol.WrapperMessage.parseDelimitedFrom(in);
                Protocol.ServerRequest request = requestMessage.getRequest();

                Protocol.ServerResponse.Builder response = Protocol.ServerResponse.newBuilder().
                        setRequestId(request.getRequestId());

                Protocol.WrapperMessage wrapperMessage;


                Protocol.SubmitTask submit = request.getSubmit();

                int taskId = 5;

                Common.printServerRequest(request);

                Protocol.SubmitTaskResponse.Builder submitTaskResponse = Protocol.SubmitTaskResponse.newBuilder().
                        setSubmittedTaskId(taskId).
                        setStatus(Protocol.Status.OK);
                response.setSubmitResponse(submitTaskResponse);




                Protocol.WrapperMessage responseMessage = Protocol.WrapperMessage.newBuilder().
                        setResponse(response).
                        build();

                Common.printTaskRepsonse(responseMessage.getResponse());
                responseMessage.writeTo(out);
                connectionSocket.close();
            }
        } catch (IOException e) {
            System.out.println("Error create server at port: " + this.port);
            e.printStackTrace();
        }
    }
}
