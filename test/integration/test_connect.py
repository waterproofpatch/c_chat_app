import pytest


@pytest.mark.parametrize("message", ["test123", "who is this?", "some 123 numbers"])
def test_connect(server, client, message):
    client.sendline(message)
    server.expect(message)
    server.flush()
    client.flush()