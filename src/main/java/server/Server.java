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
                        Protocol.SubmitTaskResponse.Builder submitTaskResponse = Protocol.SubmitTaskResponse.newBuilder().
                                setSubmittedTaskId(5).
                                setStatus(Protocol.Status.OK);
                        response.setSubmitResponse(submitTaskResponse);
                    } else if (request.hasList()) {

                    } else if (request.hasSubscribe()) {

                    } else {
                        System.err.println("Unknown type of request");
                    }

                    Protocol.WrapperMessage responseMessage = Protocol.WrapperMessage.newBuilder().
                            setResponse(response).
                            build();

                    Common.printTaskRepsonse(responseMessage.getResponse());
                    responseMessage.writeTo(out);
                } catch (IOException e) {
                    System.err.println("Messaging problems");
                    e.printStackTrace();
                }
            }
        } catch (IOException e) {
            System.err.println("Error create server at port: " + this.port);
            e.printStackTrace();
        }
    }
}
