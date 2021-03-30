from pathlib import Path

import pytest
import pexpect
from typing import Iterable

from test.integration.classes.client import Client
from test.integration.classes.server import Server


@pytest.fixture(scope="session")
def server_path() -> Path:
    return Path("bin", "server", "server.bin")


@pytest.fixture(scope="session")
def client_path() -> Path:
    return Path("bin", "client", "client.bin")


@pytest.fixture(scope="function")
def server(server_path) -> Iterable[Server]:
    assert server_path.exists(), f"{path} does not exist!"

    with Server(server_path) as _server:
        _server.flush()
        yield _server


@pytest.fixture(scope="function")
def client(client_path) -> Iterable[Client]:
    assert client_path.exists(), f"{path} does not exist!"

    with Client(client_path, name="test_client") as s:
        yield s