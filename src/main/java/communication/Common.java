package communication;

public class Common {

    public static void printTaskInfo(Protocol.ServerRequest request) {
        StringBuilder sb = new StringBuilder();

        sb.append("Request id: " + request.getRequestId());
        sb.append(" Client id: " + request.getClientId());


        if (request.hasList()) {
            sb.append(" Type: getlist");
        } else if (request.hasSubmit()) {
            sb.append(" Type: submit\n");
            printTask(sb, request.getSubmit().getTask());
        } else if (request.hasSubscribe()) {
            sb.append(" Type: subscribe " + request.getSubscribe().getTaskId());
        } else {
            sb.append(" Unknown type of request");
        }
        System.out.println(sb.toString());
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
            System.out.println("Task " + response.getSubmitResponse().getSubmittedTaskId() + " was submitted");
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

    public static void printParam(StringBuilder sb, String paramValue, Protocol.Task.Param param) {
        if (param.hasDependentTaskId()) {
            sb.append(paramValue + " depends on " + param.getDependentTaskId());
        } else {
            sb.append(paramValue + " = " + param.getValue());
        }
    }

    public static void printTask(StringBuilder sb, Protocol.Task task) {
        printParam(sb, "A", task.getA());
        printParam(sb, ", B", task.getB());
        printParam(sb, ", P", task.getP());
        printParam(sb, ", M", task.getM());
        sb.append(", N = " + task.getN());
    }
}
