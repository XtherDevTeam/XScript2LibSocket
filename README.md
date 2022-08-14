### Example

```python
def socket_test () {
    IO.println("CNM");
    var Client = SocketClient.connect("192.168.1.105:80");
    IO.println("SocketClient created");
    Client.send(String.fromBuffer("GET /xpm-mirror/ HTTP/1.1\r\nHost: www.xiaokang00010.top:2002\r\n\r\n").toBytes());
    
    var Head = Bytes.fromBuffer("");
    while (Head.endsWith(String.fromBuffer("\r\n\r\n").toBytes()) != True) {
        var buf = Client.receive(1);
        if (Error.isError(buf)) {
            break;
        }
        Head += Bytes.fromBuffer(buf);
    }
    Client.close();
    IO.println(String.fromBytes(Head.__buffer__));
    return 0;
}
```