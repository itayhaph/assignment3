package bgu.spl.net.srv;

import bgu.spl.net.api.MessagingProtocol;
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
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.connections = connections;
    }

    @Override
    public void process(String message) {
        StompMessage MessageParser = StompMessageParser.parseMessage((String) message);
        if (MessageParser.getCommand().equals("CONNECT")) {
            String username = MessageParser.getHeaders().get("login");
            String password = MessageParser.getHeaders().get("passcode");
            boolean isConnected = connections.connect(connectionId, username, password);
            
            if (isConnected) {
                // do something
            }
        } else if (MessageParser.getCommand().equals("DISCONNECT")) {
            String receiptId = MessageParser.getHeaders().get("receipt");
            connections.disconnect(connectionId);
            this.terminate = true;

            // send "receipt" stomp frame with the receipt id
        } else if (MessageParser.getCommand().equals("SEND")) {
            String channel = MessageParser.getHeaders().get("destination");
            String messageBody = MessageParser.getBody();
            // TODO: make "message" stomp frame and send it
            connections.send(channel, messageBody);
        } else if (MessageParser.getCommand().equals("SUBSCRIBE")) {
            String channel = MessageParser.getHeaders().get("destintion");
            Integer id = Integer.parseInt(MessageParser.getHeaders().get("id"));
            boolean isSubscribed = connections.subscribe(connectionId, channel, id);

            if (isSubscribed) {
                // send subscribed frame
            } else {
                // send error frame
            }
        } else if (MessageParser.getCommand().equals("UNSUBSCRIBE")) {
            int subscriptionId = Integer.parseInt(MessageParser.getHeaders().get("id"));
            // unsubscribe from the channel that has the subscription id from headers
            connections.unsubscribe(connectionId, subscriptionId);
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
