import pytest

from test.integration.classes.client import Client


@pytest.mark.parametrize("message", ["test123", "who is this?", "some 123 numbers"])
def test_connect(server, client, message):
    _ = client.send_message(message)
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


def test_connect_two_clients(server, client_path):
    name_1 = "test_client_1"
    name_2 = "test_client_2"

    server.flush()

    client_1 = Client(client_path, name_1)
    client_2 = Client(client_path, name_2)

    client_1.connect()
    client_2.connect()

    data_so_far = server.flush()
    for line in data_so_far:
        if f"Added user {client_1.name}" in line:
            user_1_found = True
        if f"Added user {client_2.name}" in line:
            user_2_found = True
    assert user_1_found == user_2_found == True


@pytest.mark.parametrize("graceful", [True, False])
def test_connect_two_clients_disconnect(server, client_path, graceful):
    name_1 = "test_client_1"
    name_2 = "test_client_2"

    _ = server.flush()

    client_1 = Client(client_path, name_1)
    client_2 = Client(client_path, name_2)

    client_1.connect()
    client_2.connect()

    _ = server.flush()

    client_1.disconnect(graceful=graceful)
    server.expect(
        f"{'REMOTE_HOST_CLOSED' if not graceful else 'Status is OK'}, disconnecting client {client_1.name}"
    )

    client_2.disconnect(graceful=graceful)
    server.expect(
        f"{'REMOTE_HOST_CLOSED' if not graceful else 'Status is OK'}, disconnecting client {client_2.name}"
    )
