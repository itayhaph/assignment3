package bgu.spl.net.srv;

import java.util.List;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

import bgu.spl.net.api.User;

//TODO synchronize everything here

public class ConnectionImpl<T> implements Connections<T> {
    private final ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers;
    private final ConcurrentHashMap<String, Set<Integer>> subscriptions;
    private ConcurrentHashMap<String, User> usernameToUserMap;
    private List<String> allUsers;
    private List<String> connectedUsers;

    private static ConnectionImpl<?> instance = null;
    private static final Object lock = new Object();

    private ConnectionImpl() {
        connectionHandlers = new ConcurrentHashMap<>();
        subscriptions = new ConcurrentHashMap<>();
        allUsers = new CopyOnWriteArrayList<>();
        connectedUsers = new CopyOnWriteArrayList<>();
    }

    @SuppressWarnings("unchecked")
    public static <T> ConnectionImpl<T> getInstance() {
        if (instance == null) {
            synchronized (lock) {
                if (instance == null) {
                    instance = new ConnectionImpl<>();
                }
            }
        }
        return (ConnectionImpl<T>) instance;
    }

    @Override
    public boolean send(int connectionId, T msg) {
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

    public void createConnection(int connectionId, ConnectionHandler<T> handler) {
        connectionHandlers.put(connectionId, handler);
    }

    public boolean connect(String username, String password) {
        // if the user is new add him to map and list of users
        if (!allUsers.contains(username)) {
            User user = new User(username, password);
            allUsers.add(username);
            usernameToUserMap.put(username, user);
            return true;
        } else { // the user exists
            // checking if the user is not connected
            if (!connectedUsers.contains(username)) {
                // checking credentials
                if (usernameToUserMap.get(username).isPasswordValid(password)) {
                    connectedUsers.add(username);
                    return true;
                }
            }
        }

        return false;
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
