from pathlib import Path

import pytest
import pexpect
from typing import Generator

from test.integration.classes.server import Server


@pytest.fixture(scope="function")
def client():
    print("I'm a client")
    yield


@pytest.fixture(scope="function")
def server():
    server_path = Path("bin", "server", "server.bin")
    assert server_path.exists(), f"{server_path} does not exist!"

    with Server(server_path) as s:
        yield s