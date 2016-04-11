import client.Client;
import communication.Protocol;
import org.junit.Test;
import server.Server;

public class SimpleTests {


    private Protocol.Task getSimpleTask() {
        return Protocol.Task.newBuilder().
                setA(Protocol.Task.Param.newBuilder().setValue(1)).
                setB(Protocol.Task.Param.newBuilder().setValue(1)).
                setM(Protocol.Task.Param.newBuilder().setValue(1)).
                setP(Protocol.Task.Param.newBuilder().setValue(1)).
                setN(5).
                build();
    }

    @Test
    public void SimpleTest() throws InterruptedException {

        Thread serverThread = new Thread(new Server("localhost", 1500));
        serverThread.start();

        Thread.sleep(1000);

        Client client = new Client("localhost", 1500);
        client.submitTask(getSimpleTask());

        serverThread.stop();
    }
}
