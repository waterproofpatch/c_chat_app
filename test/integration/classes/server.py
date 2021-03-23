from pathlib import Path
import pexpect


class Server:
    def __init__(self, path: Path):
        """
        Create a server object.

        :param path: full path to server binary.
        """
        self.handle: Optional[pexpect.pty_spawn] = None
        self.path: Path = path

    def __enter__(self):
        print("Server entered!")
        self.handle = pexpect.spawn(str(self.path))
        return self

    def __exit__(self, *kwargs):
        print("Server exited!")
        if self.handle:
            print("Shutting server!")
            self.handle.sendcontrol("c")
            self.handle.close()
            self.handle = None