from pathlib import Path
from typing import List, Optional
import pexpect
import re

from test.integration.classes.spawnable import Spawnable


class Client(Spawnable):
    PROMPT = r"#>"
    USER_REGEX = re.compile(r"Name: \[(?P<name>.*)\]")

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

    def send_message(self, message: str) -> List[str]:
        """
        Send a message to the server.
        :param message: the message to send.
        """
        self.sendline(message)
        return self.get_prompt().splitlines()

    def get_users(self) -> List[str]:
        """
        Issue the getusers command.
        :return: list of usernames
        """
        res = "".join(self.send_message("/getusers"))
        match = re.search(Client.USER_REGEX, res)
        if not match:
            return []
        return match.groupdict()