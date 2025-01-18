package bgu.spl.net.srv;

public interface Connections<T> {
    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    boolean connect(int connectionId, String username, String password);

    boolean subscribe(int connectionId, String channel, int subscriptionId);

    boolean unsubscribe(int connectionId, int subscriptionId);
}
