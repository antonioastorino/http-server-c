# TCP - Transmission control protocol

From the main folder, compile using

```
make SHELL=/bin/bash update     # generate Makefile
make                            # build
```

You need at least two terminal windows. From one of them, run the server:
```
./server-out-0
```
From the other(s), run

```
./client-out-0
```
and type the message you want to send to the server. Press `ENTER` to send the message. The server will respond by sending back the received message.

A client terminates when the message sent is `\`.

## Resources

- [https://www.youtube.com/watch?v=LtXEMwSG5-8](https://www.youtube.com/watch?v=LtXEMwSG5-8)
- [https://www.youtube.com/watch?v=mStnzIEprH8](https://www.youtube.com/watch?v=mStnzIEprH8)
- [Properly close socket - no CLOSE_WAIT](https://stackoverflow.com/questions/57704263/c-server-socket-moves-to-close-wait-state-after-client-closes-the-connection)