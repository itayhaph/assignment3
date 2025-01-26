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

        //CONNECT
        if (MessageParser.getCommand().equals("CONNECT")) {
            String username = MessageParser.getHeaders().get("login");
            String password = MessageParser.getHeaders().get("passcode");
            String isConnected = connections.connect(connectionId, username, password);
            
            if (isConnected == null) {
                headers.put("version", "1.2");
                String connectedMessage = StompFrameUtils.createStompFrame("CONNECTED", headers, null);
                connections.send(connectionId, connectedMessage);
            } else {
                headers.put("message", isConnected);
                String errorMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, errorMessage);
                connections.disconnect(connectionId);
            }
        } 
        
        
        //DISCONNECT
        else if (MessageParser.getCommand().equals("DISCONNECT")) {
            String receiptId = MessageParser.getHeaders().get("receipt");
            headers.put("receipt-id", receiptId);
            String disconnectMessage = StompFrameUtils.createStompFrame("RECEIPT", headers, null);

            connections.send(connectionId, disconnectMessage);

            connections.disconnect(connectionId);
            this.terminate = true;
        } 
        
        //SEND
        else if (MessageParser.getCommand().equals("SEND")) {
            
            String channel = MessageParser.getHeaders().get("destination");
            String messageBody = MessageParser.getBody();
            Random random = new Random();
            int randomId = random.nextInt(Integer.MAX_VALUE); 

            String subscriptionId = String.valueOf(connections.getUserSubscription(connectionId, channel));
            String messageId = String.valueOf(randomId);
            
            if (!connections.isChannelExist(channel)) {
                StringBuilder errorBody = new StringBuilder(); 
                errorBody.append("The message:\n");
                errorBody.append("-----\n");
                errorBody.append(messageBody+"\n");
                errorBody.append("-----\n");
                errorBody.append("was sent to a channel that does not exist");
                headers.put("message", "The channel " + channel + " doesn't exist");
                String stompMessage = StompFrameUtils.createStompFrame("ERROR", headers, errorBody.toString());
                connections.send(connectionId, stompMessage);
                connections.disconnect(connectionId);
            }
            // checking if the user sended report when he is not subscribed to the channel
            else if (subscriptionId.equals("-1")) {
                headers.put("message", "The user is not subscribed to channel: " + channel);
                String stompMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, stompMessage);
                connections.disconnect(connectionId);

            } else {
                headers.put("subscription", subscriptionId);
                headers.put("message-id", messageId);
                headers.put("destination", channel);
                String stompMessage = StompFrameUtils.createStompFrame("MESSAGE", headers, messageBody);
                connections.send(channel, stompMessage);
            }
        } 
        
        //SUBSCRIBE
        else if (MessageParser.getCommand().equals("SUBSCRIBE")) {
            String channel = MessageParser.getHeaders().get("destination");
            Integer subscriptionId = Integer.parseInt(MessageParser.getHeaders().get("id"));
            String isSubscribed = connections.subscribe(connectionId, channel, subscriptionId);
            String receiptId = MessageParser.getHeaders().get("receipt");
            headers.put("receipt-id", receiptId);

            if(isSubscribed==null){
                String Message = StompFrameUtils.createStompFrame("RECEIPT", headers, null);
                connections.send(connectionId, Message);
            }
            else {
                headers.put("message", isSubscribed);
                String errorMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, errorMessage);
                connections.disconnect(connectionId);
            }
        } 
        
        
        //UNSUBSCRIBE
        else if (MessageParser.getCommand().equals("UNSUBSCRIBE")) {
            int subscriptionId = Integer.parseInt(MessageParser.getHeaders().get("id"));
            // unsubscribe from the channel that has the subscription id from headers
            String isUnSubscribed = connections.unsubscribe(connectionId, subscriptionId);
            String receiptId = MessageParser.getHeaders().get("receipt");
            headers.put("receipt-id", receiptId);

            // only return frame if there is an error
            if (isUnSubscribed == null) {
                String Message = StompFrameUtils.createStompFrame("RECEIPT", headers, null);
                connections.send(connectionId, Message);
            }
            else{
                
                headers.put("message", isUnSubscribed);
                String errorMessage = StompFrameUtils.createStompFrame("ERROR", headers, null);
                connections.send(connectionId, errorMessage);
                connections.disconnect(connectionId);
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
