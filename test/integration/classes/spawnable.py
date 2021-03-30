from pathlib import Path
from typing import List, Optional
import pexpect

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

    def connect(self):
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


class Server(Spawnable):
    PROMPT = r"\n"

    def __init__(self, path: Path):
        Spawnable.__init__(self, path)
        self.prompt = Server.PROMPT


class Client(Spawnable):
    PROMPT = r"#>"

    def __init__(self, path: Path, name: str):
        Spawnable.__init__(self, path, args=[f"{name}"])
        self.prompt = Client.PROMPT
        self.name = name

    def disconnect(self, graceful=True):
        if not self.handle:
            LOGGER.warning("no handle to close! should already be disconnected!")
            return

        if graceful:
            self.sendline("/quit")
            return
        super().disconnect()

    def send_message(self, message: str):
        self.sendline(message)
        return self.get_prompt().splitlines()

    def get_users(self):
        """
        Issue the getusers command.
        """
        return self.send_message("/getusers")
