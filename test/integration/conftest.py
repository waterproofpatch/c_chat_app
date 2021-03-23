from pathlib import Path

import pytest
import pexpect
from typing import Generator

from test.integration.classes.server import Server
from test.integration.classes.server import Client


@pytest.fixture(scope="function")
def client():
    print("I'm a client")
    yield


@pytest.fixture(scope="function")
def server():
    path = Path("bin", "server", "server.bin")
    assert path.exists(), f"{path} does not exist!"

    with Server(path) as s:
        yield s


@pytest.fixture(scope="function")
def client():
    path = Path("bin", "client", "client.bin")
    assert path.exists(), f"{path} does not exist!"

    with Client(path, name="test_client") as s:
        yield s