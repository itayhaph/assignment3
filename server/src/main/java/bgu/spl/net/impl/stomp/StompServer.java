package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Server;
import bgu.spl.net.srv.StompMessagingProtocolImpl;
import bgu.spl.net.srv.MessageEncoderDecoderImpl;

public class StompServer {
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Usage: StompServer <port> <tpc/reactor>");
            return;
        }
        int port = Integer.parseInt(args[0]);
        String serverType = args[1].toLowerCase();

        if (serverType.equals("tpc")) {
            Server.threadPerClient(
                    port,
                    () -> new StompMessagingProtocolImpl(),
                    MessageEncoderDecoderImpl::new // message encoder decoder factory
            ).serve();
       } else if (serverType.equals("reactor")) {
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port,
                    StompMessagingProtocolImpl::new, // Protocol factory
                    MessageEncoderDecoderImpl::new  // Message encoder/decoder factory
            ).serve();
        } else {
            System.out.println("Invalid server type. Use 'tpc' or 'reactor'.");
        }
    }
}
