package bgu.spl.net.srv;

public class ConnectionImpl<T> implements Connections<T> {

    @Override
    public boolean send(int connectionId, T msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'send'");
    }

    @Override
    public void send(String channel, T msg) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'send'");
    }

    @Override
    public void disconnect(int connectionId) {
        // TODO Auto-generated method stub
        throw new UnsupportedOperationException("Unimplemented method 'disconnect'");
    }
    
}
