package bgu.spl.net.srv;

import java.util.List;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.locks.ReentrantLock;

import bgu.spl.net.api.User;

public class ConnectionImpl<T> implements Connections<T> {
    private final ConcurrentHashMap<Integer, ConnectionHandler<T>> connectionHandlers;
    private final ConcurrentHashMap<String, Set<Integer>> subscriptions;
    private ConcurrentHashMap<Integer, User> connectionIdToUserMap;
    private ConcurrentHashMap<String, User> allUsers;
    private List<String> connectedUsers;
    private ConcurrentHashMap<String, ReentrantLock> lockMap = new ConcurrentHashMap<>();

    private static ConnectionImpl<?> instance = null;
    private static final Object lock = new Object();

    private ConnectionImpl() {
        connectionHandlers = new ConcurrentHashMap<>();
        subscriptions = new ConcurrentHashMap<>();
        allUsers = new ConcurrentHashMap<>();
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

        ReentrantLock lock = lockMap.get(channel);
        lock.lock();

        if (subscribers != null) {
            for (Integer id : subscribers) {
                send(id, msg);
            }
        }

        lock.unlock();
    }

    @Override
    public void disconnect(int connectionId) {
        connectionHandlers.remove(connectionId);

        synchronized (subscriptions) {
            subscriptions.values().forEach(subscribers -> {
                subscribers.remove(connectionId);
            });
        }

        User currentUser = connectionIdToUserMap.remove(connectionId);
        connectedUsers.remove(currentUser.getUsername());
    }

    public void createConnection(int connectionId, ConnectionHandler<T> handler) {
        connectionHandlers.put(connectionId, handler);
    }

    public synchronized String connect(int connectionId, String username, String password) {
        // if the user is new add him to map and list of users
        if (!allUsers.contains(username)) {
            User user = new User(username, password);
            allUsers.put(username, user);
            connectionIdToUserMap.put(connectionId, user);
            return null;
        } else { // the user exists
            // checking if the user is not connected
            if (!connectedUsers.contains(username)) {
                // checking credentials
                if (allUsers.get(username).isPasswordValid(password)) {
                    connectedUsers.add(username);
                    return null;
                } else {
                    return "Password is incorrect";
                }
            } else {
                return "User is already connected from another client";
            }
        }
    }

    public String subscribe(int connectionId, String channel, int subscriptionId) {
        // if the connection is already subscribed to this channel return false
        if (subscriptions.get(channel).contains(connectionId)) {
            return "the user is alrady subscribed to this channel: " + channel;
        } else {
            lockMap.putIfAbsent(channel, new ReentrantLock());
            subscriptions.computeIfAbsent(channel, k -> ConcurrentHashMap.newKeySet()).add(connectionId);
            connectionIdToUserMap.get(connectionId).setSubscription(subscriptionId, channel);
            return null;
        }
    }

    public String unsubscribe(int connectionId, int subscriptionId) {
        String channel = connectionIdToUserMap.get(connectionId).unsubscribe(subscriptionId);
        ReentrantLock lock = lockMap.get(channel);
        lock.lock();
        Set<Integer> subscribers = subscriptions.get(channel);

        if (subscribers != null) {
            subscribers.remove(connectionId);
            lock.unlock();
            return null;
        } else {
            lock.unlock();
            return "isnt subscribed";
        }
    }

    public int getUserSubscription(int connectionId, String channel) {
        return connectionIdToUserMap.get(connectionId)
                .getSubscriptionIdByChannel(channel);
    }

    public boolean isChannelExist(String channel) {
        return subscriptions.containsKey(channel);
    }
}
