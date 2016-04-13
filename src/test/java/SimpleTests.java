import client.Client;
import communication.Protocol;
import org.junit.Test;
import server.Server;

import java.util.List;

public class SimpleTests {


    private Protocol.Task getSimpleTask() {
        return Protocol.Task.newBuilder().
                setA(Protocol.Task.Param.newBuilder().setValue(1)).
                setB(Protocol.Task.Param.newBuilder().setValue(1)).
                setM(Protocol.Task.Param.newBuilder().setValue(1)).
                setP(Protocol.Task.Param.newBuilder().setValue(1)).
                setN(1).
                build();
    }

    private Protocol.Task getHardTask() {
        return Protocol.Task.newBuilder().
                setA(Protocol.Task.Param.newBuilder().setValue(12345)).
                setB(Protocol.Task.Param.newBuilder().setValue(23456)).
                setM(Protocol.Task.Param.newBuilder().setValue(34567)).
                setP(Protocol.Task.Param.newBuilder().setValue(45678)).
                setN(10000000).
                build();
    }

    @Test
    public void SimpleTest() throws InterruptedException {

        Thread serverThread = new Thread(new Server("localhost", 1500));
        serverThread.start();

        Thread.sleep(1000);

        Client client = new Client("localhost", 1500);
        client.submitTask(getSimpleTask());
        client.submitTask(getHardTask());

        List<Protocol.ListTasksResponse.TaskDescription> tasks = client.getList();

        for (Protocol.ListTasksResponse.TaskDescription task : tasks) {
            if (task.hasResult()) {
                System.out.println(task.getTaskId() +  " " + task.getResult());
            } else {
                System.out.println(task.getTaskId());
            }
        }

        System.out.println(client.subscribe(2));

        serverThread.stop();
    }
}
