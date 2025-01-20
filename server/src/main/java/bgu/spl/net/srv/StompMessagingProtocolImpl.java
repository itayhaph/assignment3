package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.Map;
import java.util.Random;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompFrameUtils;
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
        Map<String, String> headers = new HashMap<>();

        // TODO: check where do we get receipt headers and add them to the error headers
        if (MessageParser.getCommand().equals("CONNECT")) {
            String username = MessageParser.getHeaders().get("login");
            String password = MessageParser.getHeaders().get("passcode");
            String isConnected = connections.connect(connectionId, username, password);

            // if there is no error message thus no error
            if (isConnected == null) {
                headers.put("version", "1.2");
                String connectedMessage = StompFrameUtils.createStompFrame("CONNECTED", headers, null);
                connections.send(connectionId, connectedMessage);
            } else {
                headers.put("message", isConnected);
                String errorMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, errorMessage);
                this.terminate = true;
            }
        } else if (MessageParser.getCommand().equals("DISCONNECT")) {
            String receiptId = MessageParser.getHeaders().get("receipt");
            connections.disconnect(connectionId);

            headers.put("receipt-id", receiptId);
            String disconnectMessage = StompFrameUtils.createStompFrame("RECEIPT", headers, null);
            connections.send(connectionId, disconnectMessage);
            this.terminate = true;
        } else if (MessageParser.getCommand().equals("SEND")) {
            String channel = MessageParser.getHeaders().get("destination");
            String messageBody = MessageParser.getBody();
            Random random = new Random();
            // TODO: check if we need to generate message id or use global counter
            String subscriptionId = String.valueOf(connections.getUserSubscription(connectionId, channel));
            String messageId = String.valueOf(random.nextInt());
            // TODO: add error for when the headers is not good?
            if(!connections.isChannelExist(channel)) {
                headers.put("message", "The channel " + channel + " doesn't exist");
                String stompMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, stompMessage);
                this.terminate = true;
            }
            // TODO check if its possible and complete
            else if(subscriptionId == "-1") {
                String stompMessage = StompFrameUtils.createStompFrame("ERROR", null, null);
                connections.send(channel, stompMessage);
            }
            else {
                headers.put("subscription", subscriptionId);
                headers.put("message-id", messageId);
                headers.put("destination", channel);
                String stompMessage = StompFrameUtils.createStompFrame("MESSAGE", headers, messageBody);
                connections.send(channel, stompMessage);
            }            
        } else if (MessageParser.getCommand().equals("SUBSCRIBE")) {
            String channel = MessageParser.getHeaders().get("destintion");
            Integer subscriptionId = Integer.parseInt(MessageParser.getHeaders().get("id"));
            String isSubscribed = connections.subscribe(connectionId, channel, subscriptionId);

            // only return frame if there is an error
            if (isSubscribed != null) {
                headers.put("message", isSubscribed);
                String errorMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, errorMessage);
                this.terminate = true;
            }
        } else if (MessageParser.getCommand().equals("UNSUBSCRIBE")) {
            int subscriptionId = Integer.parseInt(MessageParser.getHeaders().get("id"));
            // unsubscribe from the channel that has the subscription id from headers
            String isUnSubscribed = connections.unsubscribe(connectionId, subscriptionId);

            // only return frame if there is an error
            if (isUnSubscribed != null) {
                headers.put("message", isUnSubscribed);
                String errorMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, errorMessage);
                this.terminate = true;
            }
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
