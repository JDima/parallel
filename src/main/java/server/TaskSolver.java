package server;

import communication.Protocol;

/**
 * Created by JDima on 13/04/16.
 */
public class TaskSolver {

    private static final Object monitor = new Object();

    private TaskStore taskStore;

    public TaskSolver(TaskStore taskStore) {
        this.taskStore = taskStore;
    }


    private boolean haveDependentAndIsNotSolved(Protocol.Task.Param param) {
        return param.hasDependentTaskId() && !taskStore.isSolved(param.getDependentTaskId());
    }

    private boolean notReady(ClientTask clientTask) {
        Protocol.Task task = clientTask.getTask();
        return haveDependentAndIsNotSolved(task.getA()) && haveDependentAndIsNotSolved(task.getB()) &&
                haveDependentAndIsNotSolved(task.getM()) && haveDependentAndIsNotSolved(task.getP());
    }

    private long solve(long a, long b, long p, long m, long n) {
        while (n-- > 0) {
            b = (a * p + b) % m;
            a = b;
        }
        return a;
    }

    private long getValue(Protocol.Task.Param param) {
        if (param.hasDependentTaskId())
            return taskStore.getSolution(param.getDependentTaskId());
        return param.getValue();
    }

    public long solveTask(ClientTask clientTask) {
        synchronized(monitor) {
            while (notReady(clientTask)) {
                try {
                    monitor.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        Protocol.Task task = clientTask.getTask();
        long result = solve(getValue(task.getA()), getValue(task.getB()),
                getValue(task.getP()), getValue(task.getM()), task.getN());

        synchronized(monitor) {
            monitor.notifyAll();
        }

        return result;
    }
}
