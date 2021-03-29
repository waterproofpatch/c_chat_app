import pytest


@pytest.mark.parametrize("message", ["test123", "who is this?", "some 123 numbers"])
def test_connect(server, client, message):
    client.send_message(message)
    server.expect(message)


def test_connect_reconnect(server, client):
    server.expect("Accepted new client!")
    client.disconnect(graceful=False)
    server.expect(f"REMOTE_HOST_CLOSED, disconnecting client {client.name}")
    client.connect()
    server.expect("Accepted new client!")
    client.disconnect(graceful=False)
    server.expect(f"REMOTE_HOST_CLOSED, disconnecting client {client.name}")