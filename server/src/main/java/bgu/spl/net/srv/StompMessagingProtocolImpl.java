package bgu.spl.net.srv;

import java.util.Map;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.StompMessageParser.StompMessage;

public class StompMessagingProtocolImpl implements MessagingProtocol<String> {
    private Connections<String> connections;
    boolean terminate = false;
    int connectionId;

    /**
     * Used to initiate the current client protocol with it's personal connection ID
     * and the connections implementation
     **/

    @Override
    public void start(int connectionId, Connections<String> connections ) {
        this.terminate = true;
        this.connectionId = connectionId;
        this.connections = connections;
        // connections.connect(connectionId, connections.getConnectionById(connectionId));
    }

    @Override
    public void process(String message) {
        StompMessage MessageParser = StompMessageParser.parseMessage((String) message);
        if (MessageParser.getCommand().equals("CONNECT")) {
            
        } else if (MessageParser.getCommand().equals("DISCONNECT")) {
            connections.disconnect(connectionId);
            this.terminate = true;

        } else if (MessageParser.getCommand().equals("SEND")) {
            String channel = MessageParser.getHeaders().get("destination");
            connections.send(channel, message);
        } else if (MessageParser.getCommand().equals("SUBSCRIBE")) {
            String channel = MessageParser.getHeaders().get("destintion");
            String id = MessageParser.getHeaders().get("id");
            boolean isSubscribed = connections.subscribe(connectionId, channel);
            boolean subscribe = connections.subscribe(connectionId, channel);
        } else if (MessageParser.getCommand().equals("UNSUBSCRIBE")) {
            String id = MessageParser.getHeaders().get("id");
            // unsubscribe from every channel that has the connectionId/the id from headers
            // connections.unsubscribe(connectionId)
        }
    }

    /**
     * @return true if the connection should be terminated
     */
    @Override
    public boolean shouldTerminate() {
        return terminate;
    }
}
