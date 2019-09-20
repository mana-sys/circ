import pytest
import socket
import subprocess
import time

@pytest.fixture(scope="function")
def circ_server(circ_sv_exe):

    print("Starting circ server child process.")
    circ_sv = subprocess.Popen(circ_sv_exe)
    time.sleep(.01)
    res = circ_sv.poll()
    if res is not None:
        pytest.fail("Could not start server.")
    yield circ_sv

    print("Terminating circ server child process...")
    circ_sv.kill()
    print("circ server child process terminated successfully.")


def test_privmsg(circ_server):
    try:
        s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s1.connect(("localhost", 50002))
        s1.send(b"NICK nick1\r\n")
        s1.send(b"USER user1 * * :user one\r\n")

        s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s2.connect(("localhost", 50002))
        s2.send(b"NICK nick2\r\n")
        s2.send(b"USER user2 * * :user two\r\n")

        resp = s1.recv(512)
        assert resp == b'001 nick1 :Welcome to the Internet Relay Network nick1!user1@localhost\r\n'

        resp = s2.recv(512)
        assert resp == b'001 nick2 :Welcome to the Internet Relay Network nick2!user2@localhost\r\n'

        s1.send(b"PRIVMSG nick2 :Message from nick1\r\n")

        resp = s2.recv(512)
        assert resp == b'nick1!user1@localhost PRIVMSG nick2 :Message from nick1\r\n'

        s2.send(b'PRIVMSG nick1 :Message from nick2\r\n')

        resp = s1.recv(512)
        assert resp == b'nick2!user2@localhost PRIVMSG nick1 :Message from nick2\r\n'

    except socket.error as err:
        pytest.fail("Socket creation failed with error %s" % err)
