from pathlib import Path
from typing import List, Optional, Dict
import pexpect
import re

from test.integration.classes.spawnable import Spawnable
from test.integration.classes.logger import get_logger

LOGGER = get_logger(__name__)


class Client(Spawnable):
    PROMPT = r"#>"
    USER_REGEX = re.compile(r"Name: \[(?P<name>\w+)\]")
    MESSAGE_REGEX = re.compile(
        r"PUBLIC MESSAGE FROM \[(?P<from>\w+)\]: (?P<message>\w.*)"
    )

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

    def get_users(self) -> List[Dict[str, str]]:
        """
        Issue the getusers command.
        :return: list of dict of usernames.
        """
        res = self.send_message("/getusers")
        users = []
        for line in res:
            match = re.search(Client.USER_REGEX, line)
            if match:
                users.append(match.groupdict())

        return users

    def get_messages(self) -> List[Dict[str, str]]:
        """
        Reads until prompt to get new messages.
        :return: list of dict of messages.
        """
        res = self.flush()
        messages = []
        for line in res:
            match = re.search(Client.MESSAGE_REGEX, line)
            if match:
                messages.append(match.groupdict())

        return messages