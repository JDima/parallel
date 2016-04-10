package client;

import communication.Common;
import communication.Protocol;

import java.io.*;
import java.net.Socket;

public class Client{

    private String host;
    private int port;

    public Client(String host, int port) {
        this.host = host;
        this.port = port;
    }

    public String getHost() {
        return host;
    }

    public int getPort() {
        return port;
    }

    public int submitTask(Protocol.Task task) {
        Protocol.ServerRequest.Builder requestBuilder = Protocol.ServerRequest.newBuilder().
                setSubmit(Protocol.SubmitTask.newBuilder().
                        setTask(task));
        Protocol.ServerResponse response = run(requestBuilder);
        Common.printTaskRepsonse(response);
        return response.getSubmitResponse().getSubmittedTaskId();
    }

    public Protocol.ServerResponse run(Protocol.ServerRequest.Builder requestBuilder) {
        try (   Socket clientSocket = new Socket(this.host, this.port);
                InputStream in = clientSocket.getInputStream();
                OutputStream out = clientSocket.getOutputStream()
                ) {

            requestBuilder.setClientId("0");
            requestBuilder.setRequestId(0);

            Protocol.WrapperMessage requestMessage = Protocol.WrapperMessage.newBuilder().setRequest(requestBuilder).build();
            Common.printClientRequest(requestMessage.getRequest());

            requestMessage.writeDelimitedTo(out);
            out.flush();

            Protocol.WrapperMessage responseMessage = Protocol.WrapperMessage.parseFrom(in);
            return responseMessage.getResponse();
        } catch (IOException e) {
            System.out.println("Error create client at port: " + this.port);
            e.printStackTrace();
        }
        return null;
    }
}
