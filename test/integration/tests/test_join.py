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


def test_join(circ_server):
    try:

        client = ClientIRC('localhost', 50002, .01)

        client.send_msg('NICK nick')
        client.send_msg('USER user * * :user name')
        client.send_msg('JOIN #channel')

        assert b'001 nick :Welcome to the Internet Relay Network nick!user@localhost\r\n' == client.recv_msg()
        assert b'JOIN #channel\r\n' == client.recv_msg_skip_prefix()
        assert b'353 nick #channel :nick\r\n' == client.recv_msg_skip_prefix()
        assert b'366 nick #channel :End of NAMES list\r\n' == client.recv_msg_skip_prefix()

    except socket.error as err:
        pytest.fail("Socket creation failed with error %s" % err)


def test_join_two_channels(circ_server):
    try:

        client = ClientIRC('localhost', 50002, .01)

        client.send_msg('NICK nick')
        client.send_msg('USER user * * :user name')
        client.send_msg('JOIN #channel1,#channel2')

        assert b'001 nick :Welcome to the Internet Relay Network nick!user@localhost\r\n' == client.recv_msg()

        assert b'JOIN #channel1\r\n' == client.recv_msg_skip_prefix()
        assert b'353 nick #channel1 :nick\r\n' == client.recv_msg_skip_prefix()
        assert b'366 nick #channel1 :End of NAMES list\r\n' == client.recv_msg_skip_prefix()

        assert b'JOIN #channel2\r\n' == client.recv_msg_skip_prefix()
        assert b'353 nick #channel2 :nick\r\n' == client.recv_msg_skip_prefix()
        assert b'366 nick #channel2 :End of NAMES list\r\n' == client.recv_msg_skip_prefix()

    except socket.error as err:
        pytest.fail("Socket creation failed with error %s" % err)


# def test_join_existing_channel(circ_server):
#
#     try:
#
#         client1 = ClientIRC('localhost', 50002, .01)
#         client2 = ClientIRC('localhost', 50002, .01)
#
#         client1.send_msg('NICK nick1')
#         client1.send_msg('USER user1 * * :user name')
#         client1.send_msg('JOIN #channel1')
#
#         client2.send_msg('NICK nick1')
#         client2.send_msg('USER user1 * * :user name')
#         client2.send_msg('JOIN #channel1')
#
#     except socket.error as err:
#         pytest.fail("Socket creation failed with error %s" % err)



