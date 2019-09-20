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


def test_ping(circ_server):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(("localhost", 50002))
        s.send(b"NICK ben\r\n")
        s.send(b"USER ben * * :Ben Llanes\r\n")

        resp = s.recv(512)
        assert resp == b'001 ben :Welcome to the Internet Relay Network ben!ben@localhost\r\n'

        s.send(b"PING\r\n")

        resp = s.recv(512)
        assert resp.startswith(b"PONG ")

        s.close()
    except socket.error as err:
        pytest.fail("Socket creation failed with error %s" % err)
