import telnetlib


class ClientIRC:

    def __init__(self, host: str, port: int, timeout: float):
        self.client = telnetlib.Telnet(host, port, 1)
        self.timeout = timeout

    def send_msg(self, msg):
        self.client.write(str.encode('%s\r\n' % msg))

    def recv_msg(self):
        return self.client.read_until(b'\r\n', self.timeout)

    def recv_msg_skip_prefix(self):
        return self.recv_msg().split(b' ', 1)[1]
