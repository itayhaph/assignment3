package bgu.spl.net.api;

import java.util.concurrent.ConcurrentHashMap;

public class User {
    private String username;
    private String password;
    private ConcurrentHashMap<String,Integer> mapChannelToId;

    public User(String username, String password) {
        this.username = username;
        this.password = password;
        this.mapChannelToId = new ConcurrentHashMap<>();
    }

    public boolean isPasswordValid(String password) {
        return this.password == password;
    }

    public Integer getIdByChannel(String channel) {
        return mapChannelToId.get(channel);
    }

    public String getUsername(){
        return this.username;
    }
}
