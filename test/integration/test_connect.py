import pytest

from test.integration.classes.spawnable import Client


@pytest.mark.parametrize("message", ["test123", "who is this?", "some 123 numbers"])
def test_connect(server, client, message):
    res = client.send_message(message)
    server.expect(message)


@pytest.mark.parametrize("graceful", [True, False])
def test_connect_reconnect(server, client, graceful):
    server.expect("Accepted new client!")
    client.disconnect(graceful=graceful)
    server.expect(
        f"{'REMOTE_HOST_CLOSED' if not graceful else 'Status is OK'}, disconnecting client {client.name}"
    )
    client.connect()
    server.expect("Accepted new client!")
    client.disconnect(graceful=graceful)
    server.expect(
        f"{'REMOTE_HOST_CLOSED' if not graceful else 'Status is OK'}, disconnecting client {client.name}"
    )


def test_connect_invalid_name(server, client_path):
    name = "testaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
    client = Client(client_path, name)
    server.flush()
    client.connect()
    server.expect("Username supplied was invalid length!")
    server.expect("ERR_INVALID_COMMAND")
    client.expect("Server requested that we disconnect for reason invalid client name")
    client.disconnect()
