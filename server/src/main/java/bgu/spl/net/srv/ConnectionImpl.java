package bgu.spl.net.srv;

import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionImpl<T> implements Connections<T> {
    private final ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<String, Set<Integer>> subscriptions = new ConcurrentHashMap<>();

    @Override
    public boolean send(int connectionId, T msg) {//// TODO
        ConnectionHandler<T> handler = connectionHandlers.get(connectionId);
        if (handler == null)
            return false;
        handler.send(msg);
        return true;
    }

    @Override
    public void send(String channel, T msg) {
        Set<Integer> subscribers = subscriptions.get(channel);
        if (subscribers != null) {
            for (Integer id : subscribers) {
                send(id, msg);
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {
        connectionHandlers.remove(connectionId);
        subscriptions.values().forEach(subscribers -> subscribers.remove(connectionId));
    }

    public boolean connect(int connectionId, ConnectionHandler<T> handler) {
        if (connectionHandlers.get(connectionId) != null) {
            return false;
        }

        connectionHandlers.put(connectionId, handler);
        return true;
    }

    public boolean subscribe(int connectionId, String channel) {
        if (connectionHandlers.get(connectionId) != null) {
            return false;
        }

        subscriptions.computeIfAbsent(channel, k -> ConcurrentHashMap.newKeySet()).add(connectionId);
        return true;
    }

    public boolean unsubscribe(int connectionId, String channel) {
        if (connectionHandlers.get(connectionId) == null) {
            return false;
        }

        Set<Integer> subscribers = subscriptions.get(channel);
        if (subscribers != null)
            subscribers.remove(connectionId);

        return true;
    }

    public ConnectionHandler<T> getConnectionById(int connectionId) {
        return connectionHandlers.get(connectionId);
    }
}
