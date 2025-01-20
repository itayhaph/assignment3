package bgu.spl.net.srv;

public interface Connections<T> {
    boolean send(int connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(int connectionId);

    String connect(int connectionId, String username, String password);

    String subscribe(int connectionId, String channel, int subscriptionId);

    String unsubscribe(int connectionId, int subscriptionId);

    int getUserSubscription(int connectionId, String channel);

    boolean isChannelExist(String channel);
}
