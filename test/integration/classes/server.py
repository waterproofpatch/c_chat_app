from pathlib import Path
from test.integration.classes.spawnable import Spawnable


class Server(Spawnable):
    PROMPT = r"\n"

    def __init__(self, path: Path):
        Spawnable.__init__(self, path)
        self.prompt = Server.PROMPT