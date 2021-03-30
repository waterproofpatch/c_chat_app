from pathlib import Path

import pytest
import pexpect
from typing import Generator

from test.integration.classes.client import Client
from test.integration.classes.server import Server


@pytest.fixture(scope="session")
def server_path():
    return Path("bin", "server", "server.bin")


@pytest.fixture(scope="session")
def client_path():
    return Path("bin", "client", "client.bin")


@pytest.fixture(scope="function")
def server(server_path):
    assert server_path.exists(), f"{path} does not exist!"

    with Server(server_path) as _server:
        _server.flush()
        yield _server


@pytest.fixture(scope="function")
def client(client_path):
    assert client_path.exists(), f"{path} does not exist!"

    with Client(client_path, name="test_client") as s:
        yield s