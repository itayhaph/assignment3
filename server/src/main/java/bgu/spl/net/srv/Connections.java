package bgu.spl.net.srv;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    boolean connect(int connectionId, ConnectionHandler<T> handler);

    boolean subscribe(int connectionId, String channel);

    boolean unsubscribe(int connectionId, String channel);

    ConnectionHandler<T> getConnectionById(int connectionId);
}
