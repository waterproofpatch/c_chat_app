import pytest


def test_getusers(server, client):
    res = client.get_users()
    assert "Requesting user list from server" in "".join(res)
    assert "1 users connected" in "".join(res)
    assert f"Name: [{client.name}]" in "".join(res)
    server.expect(r"User \[{}\] requests user list from server...".format(client.name))