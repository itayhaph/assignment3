package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.Map;

public class StompMessageParser {

    public static class StompMessage {
        private String command;
        private Map<String, String> headers = new HashMap<>();
        private String body;

        public String getCommand() {
            return command;
        }

        public Map<String, String> getHeaders() {
            return headers;
        }

        public String getBody() {
            return body;
        }
    }

    public static StompMessage parseMessage(String message) {
        StompMessage stompMessage = new StompMessage();
        String[] lines = message.split("\n");
        
        // Step 1: Parse the command
        stompMessage.command = lines[0].trim();

        // Step 2: Parse headers
        for (int i =0;i < lines.length && !lines[i].trim().isEmpty();i++) {
            String line = lines[i];
            int colonIndex = line.indexOf(':');
            if (colonIndex > 0) {
                String key = line.substring(0, colonIndex).trim();
                String value = line.substring(colonIndex + 1).trim();
                stompMessage.headers.put(key, value);
            }
        }

        // Step 3: Parse body (if present)
        // StringBuilder body = new StringBuilder();
        // while (i < lines.length) {
        //     body.append(lines[i]).append("\n");
        //     i++;
        // }
        // stompMessage.body = body.toString().trim();

        return stompMessage;
    }
}