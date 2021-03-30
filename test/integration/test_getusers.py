import pytest


def test_getusers(server, client):
    res = client.get_users()
    assert res and res["name"] == client.name
    server.expect(r"User \[{}\] requests user list from server...".format(client.name))