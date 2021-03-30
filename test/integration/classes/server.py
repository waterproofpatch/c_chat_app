from pathlib import Path
from test.integration.classes.spawnable import Spawnable
from test.integration.classes.logger import get_logger

LOGGER = get_logger(__name__)


class Server(Spawnable):
    PROMPT = r"\n"

    def __init__(self, path: Path):
        Spawnable.__init__(self, path)
        self.prompt = Server.PROMPT