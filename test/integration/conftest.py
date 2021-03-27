from pathlib import Path

import pytest
import pexpect
from typing import Generator

from test.integration.classes.spawnable import Server, Client


@pytest.fixture(scope="function")
def client():
    path = Path("bin", "client", "client.bin")
    assert path.exists(), f"{path} does not exist!"

    with Client(path, name="testclient") as _client:
        _client.flush()
        yield _client
    yield


@pytest.fixture(scope="function")
def server():
    path = Path("bin", "server", "server.bin")
    assert path.exists(), f"{path} does not exist!"

    with Server(path) as _server:
        _server.flush()
        yield _server


@pytest.fixture(scope="function")
def client():
    path = Path("bin", "client", "client.bin")
    assert path.exists(), f"{path} does not exist!"

    with Client(path, name="test_client") as s:
        yield s