package bgu.spl.net.api;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class User {
    private String username;
    private String password;
    private ConcurrentHashMap<Integer, String> subscriptionIdToChannelMap;

    public User(String username, String password) {
        this.username = username;
        this.password = password;
        this.subscriptionIdToChannelMap = new ConcurrentHashMap<>();
    }

    public boolean isPasswordValid(String password) {
        return this.password == password;
    }

    public int getSubscriptionIdByChannel(String channel) {
        for (Map.Entry<Integer, String> entry : subscriptionIdToChannelMap.entrySet()) {
            if (entry.getValue().equals(channel))
                return entry.getKey();
        }

        return -1;
    }

    public String getUsername() {
        return this.username;
    }

    public void setSubscription(int id, String channel) {
        subscriptionIdToChannelMap.put(id, channel);
    }

    public String unsubscribe(int id) {
        return subscriptionIdToChannelMap.remove(id);
    }
}
