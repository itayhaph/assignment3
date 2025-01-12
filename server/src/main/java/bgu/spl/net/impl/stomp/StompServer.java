package bgu.spl.net.impl.stomp;

import bgu.spl.net.impl.echo.EchoProtocol;
import bgu.spl.net.impl.echo.LineMessageEncoderDecoder;
import bgu.spl.net.srv.Server;
import bgu.spl.net.srv.StompMessagingProtocolImpl;

public class StompServer {

    public static void main(String[] args) {
       int port =Integer.parseInt(args[0]);
        if (args[1] .equals("tpc")) {
            Server.threadPerClient(
                    port, // port
                    () -> new StompMessagingProtocolImpl(), // protocol factory
                    LineMessageEncoderDecoder::new // message encoder decoder factory
            ).serve();
        } else {
            Server.reactor(
                    Runtime.getRuntime().availableProcessors(),
                    port, // port
                    () -> new StompMessagingProtocolImpl(), // protocol factory
                    LineMessageEncoderDecoder::new // message encoder decoder factory
            ).serve();

        }

    }
}
