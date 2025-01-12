package bgu.spl.net.srv;

import java.util.Map;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.Connections;
import bgu.spl.net.srv.StompMessageParser.StompMessage;

public class StompMessagingProtocolImpl<T> {
    private Connections<T> connections;
    boolean terminate = false;
    int connectionId;

    /**
     * Used to initiate the current client protocol with it's personal connection ID
     * and the connections implementation
     **/

    void start(int connectionId, Connections<T> connections) {
        this.terminate = true;
        this.connectionId = connectionId;
        this.connections = connections;
        connections.connect(connectionId, connections.getConnectionById(connectionId));
    }

    void process(T message) {
        /*
         * • CONNECT (as defined above)
         * • SEND
         * • SUBSCRIBE
         * • UNSUBSCRIBE
         * • DISCONNECT
         */

        StompMessage MessageParser = StompMessageParser.parseMessage((String) message);
        if (MessageParser.getCommand().equals("CONNECT")) {

        } else if (MessageParser.getCommand().equals("DISCONNECT")) {
            connections.disconnect(connectionId);
            this.terminate = true;

        } else if (MessageParser.getCommand().equals("SEND")) {
            String destination = MessageParser.getHeaders().get("destintion");

        } else if (MessageParser.getCommand().equals("SUBSCRIBE")) {

            String destination = MessageParser.getHeaders().get("destintion");
            boolean subscribe = connections.subscribe(connectionId, destination);
        }

    }

    /**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate() {
        return terminate;

    }
}
