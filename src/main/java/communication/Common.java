package communication;

public class Common {

    public static void printTaskInfo(Protocol.ServerRequest request) {
        System.out.println("Request id  " + request.getRequestId());

        if (request.hasList()) {
            System.out.println("Request for task list.");
        } else if (request.hasSubmit()) {
            System.out.println("Submit new task:");
            printTask(request.getSubmit().getTask());
        } else if (request.hasSubscribe()) {
            System.out.println("Subscribe for task " + request.getSubscribe().getTaskId());
        } else {
            System.out.println("Unknown type of request");
        }
        System.out.println();
        System.out.println();
    }

    public static void printTaskRepsonse(Protocol.ServerResponse response) {

        if (response.hasListResponse()) {
            int countTask = response.getListResponse().getTasksCount();
            System.out.println("Tasks on the server:");
            for (int iTask = 0; iTask < countTask; iTask++)
            {
                System.out.println(response.getListResponse().getTasks(iTask).getTaskId());
            }
        } else if (response.hasSubmitResponse()) {
            System.out.println("Task " + response.getSubmitResponse().getSubmittedTaskId() +" was submitted");
        } else if (response.hasSubscribeResponse()) {
            if (response.getSubscribeResponse().getStatus() == Protocol.Status.ERROR) {
                System.out.println("Not calculated for request " + response.getRequestId());
            } else {
                System.out.println("Result is " + response.getSubscribeResponse().getValue() + " for request " + response.getRequestId());
            }
        } else {
            System.out.println("Unknown type of request");
        }
        System.out.println();
        System.out.println();

    }


    public static void printServerRequest(Protocol.ServerRequest request) {
        System.out.println("Server get task from client " + request.getClientId());
        System.out.println("Request id  " + request.getRequestId());

        printTaskInfo(request);
    }

    public static void printClientRequest(Protocol.ServerRequest request) {
        System.out.println("Client " + request.getClientId() + " send new task.");

        printTaskInfo(request);
    }

    public static void printParam(String paramValue, Protocol.Task.Param param) {
        if (param.hasDependentTaskId()) {
            System.out.print(paramValue + "is depends on " + param.getDependentTaskId());
        } else {
            System.out.print(paramValue + " = " + param.getValue());
        }
    }

    public static void printTask(Protocol.Task task) {
        printParam("A", task.getA());
        printParam(", B", task.getB());
        printParam(", P", task.getP());
        printParam(", M", task.getM());
        System.out.print(", N = " + task.getN());
    }
}
