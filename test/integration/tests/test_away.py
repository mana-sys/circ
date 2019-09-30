import pytest
import socket
import subprocess
import telnetlib
import time

from .client import ClientIRC


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


def test_away(circ_server):
    try:
        client1 = ClientIRC('localhost', 50002, .01)
        client2 = ClientIRC('localhost', 50002, .01)

        client1.send_msg('NICK nick')
        client1.send_msg('USER user * * :user name')

        assert b'001 nick :Welcome to the Internet Relay Network nick!user@localhost\r\n' == client1.recv_msg()

        client2.send_msg('NICK nick2')
        client2.send_msg('USER user2 * * :user name2')

        assert b'001 nick2 :Welcome to the Internet Relay Network nick2!user2@localhost\r\n' == client2.recv_msg()

        client1.send_msg('AWAY :Back in 5 min')

        assert b'306 nick :You have been marked as being away\r\n' == client1.recv_msg_skip_prefix()

        client2.send_msg('PRIVMSG nick :Hey there!')

        assert b'301 nick2 nick :Back in 5 min\r\n' == client2.recv_msg_skip_prefix()

    except socket.error as err:
        pytest.fail("Socket creation failed with error %s" % err)