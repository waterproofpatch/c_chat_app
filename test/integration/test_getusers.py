import pytest

from test.integration.classes.client import Client


def test_getusers(server, client):
    res = client.get_users()
    assert res and res[0]["name"] == client.name
    server.expect(r"User \[{}\] requests user list from server...".format(client.name))


def test_getusers_two_clients(server, client_path):
    name_1 = "test_client_1"
    name_2 = "test_client_2"

    _ = server.flush()

    client_1 = Client(client_path, name_1)
    client_2 = Client(client_path, name_2)
    client_1.connect()
    client_2.connect()

    _ = server.flush()

    users = client_1.get_users()
    assert len(users) == 2
    assert client_1.name in [x["name"] for x in users]
    assert client_2.name in [x["name"] for x in users]
