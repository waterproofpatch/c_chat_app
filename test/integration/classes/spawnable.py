from pathlib import Path
from typing import List, Optional
import pexpect
import re

from test.integration.classes.logger import get_logger

LOGGER = get_logger(__name__)


class Spawnable:
    def __init__(self, path: Path, args: List[str] = []):
        self.handle: Optional[pexpect.pty_spawn] = None
        self.path: Path = path
        self.args: List[Str] = args
        self.prompt: Optional[str] = None

    def __enter__(self):
        LOGGER.debug("Entered!")
        self.connect()
        return self

    def __exit__(self, *kwargs):
        LOGGER.debug("Exited!")
        self.disconnect()

    def flush(self):
        """
        Read until timeout to flush input.

        :return: list of strings read
        """
        if not self.handle:
            raise RuntimeError("No handle to flush!")

        before = []
        while True:
            LOGGER.debug(f"{self}flushing...")
            try:
                self.handle.expect(self.prompt, timeout=1)
                before.extend(self.handle.before.decode().splitlines())
            except pexpect.TIMEOUT:
                LOGGER.info("Got timeout, done flushing...")
                break

        return before

    def disconnect(self):
        if self.handle:
            LOGGER.debug(f"{self} shutting down!")
            self.handle.sendcontrol("c")
            self.handle.close()
            self.handle = None

    def connect(self) -> List[str]:
        self.handle = pexpect.spawn(str(self.path), args=self.args)
        LOGGER.debug("Waiting for prompt...")
        self.handle.expect(self.prompt, timeout=1)
        LOGGER.debug(f"{self} Got prompt!")
        return self.before.decode().splitlines()

    def get_prompt(self):
        self.handle.expect(self.prompt, timeout=1)
        return self.before.decode()

    @property
    def before(self):
        return self.handle.before

    def expect(self, expected: str, timeout=1):
        LOGGER.info(f"expecting {expected}")
        self.handle.expect(expected, timeout=timeout)

    def sendline(self, cmd: str):
        LOGGER.info(f"sending {cmd}")
        self.handle.sendline(cmd)

    def send(self, cmd: str):
        LOGGER.info(f"sending {cmd}")
        self.handle.send(cmd)
