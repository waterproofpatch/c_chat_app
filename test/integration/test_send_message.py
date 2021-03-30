import pytest

from test.integration.classes.client import Client


@pytest.mark.parametrize("message", ["test123", "who is this?", "some 123 numbers"])
def test_send_message(server, client_path, message):
    name_1 = "test_client_1"
    name_2 = "test_client_2"

    _ = server.flush()

    client_1 = Client(client_path, name_1)
    client_2 = Client(client_path, name_2)
    client_1.connect()
    client_2.connect()

    _ = server.flush()

    _ = client_1.send_message(message)
    msgs = client_2.get_messages()
    assert msgs
    assert msgs[0]["from"] == client_1.name
    assert msgs[0]["message"] == message

    _ = server.flush()
    _ = client_1.flush()

    client_2.send_message(message)
    msgs = client_1.get_messages()
    assert msgs
    assert msgs[0]["from"] == client_2.name
    assert msgs[0]["message"] == message
