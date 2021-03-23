from pathlib import Path
from typing import List, Optional
import pexpect


class Spawnable:
    def __init__(self, path: Path, args: List[str] = []):
        self.handle: Optional[pexpect.pty_spawn] = None
        self.path: Path = path
        self.args: List[Str] = args
        self.prompt: Optional[str] = None

    def __enter__(self):
        print("Entered!")
        self.handle = pexpect.spawn(str(self.path), args=self.args)
        print("Waiting for prompt...")
        self.handle.expect(self.prompt, timeout=1)
        print("Got prompt!")
        return self

    def __exit__(self, *kwargs):
        print("Exited!")
        if self.handle:
            print("Shutting server!")
            self.handle.sendcontrol("c")
            self.handle.close()
            self.handle = None


class Server(Spawnable):
    PROMPT = r"\r\n"

    def __init__(self, path: Path):
        Spawnable.__init__(self, path)
        self.prompt = Server.PROMPT


class Client(Spawnable):
    PROMPT = r">\r\n"

    def __init__(self, path: Path, name: str):
        Spawnable.__init__(self, path, args=[f"{name}"])
        self.prompt = Client.PROMPT

    def connect(self):
        pass

    def disconnect(self):
        pass
