package bgu.spl.net.api;

import java.util.Map;

public class StompFrameUtils {

    public static String createStompFrame(String command, Map<String, String> headers, String body) {
        StringBuilder frame = new StringBuilder();
        frame.append(command).append("\n");

        if (headers != null) {
            for (Map.Entry<String, String> header : headers.entrySet()) {
                frame.append(header.getKey()).append(":").append(header.getValue()).append("\n");
            }
        }

        // separate headers from the body
        frame.append("\n");

        if (body != null) {
            frame.append(body);
        }

        // End with a null character
        frame.append("\0");

        return frame.toString();
    }
}
