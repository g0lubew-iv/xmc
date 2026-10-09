from typing import final

__version__: str

@final
class RNG:
    def __init__(self, seed: int = 0) -> None: ...
    def seed(self, s: int) -> None: ...
    def random(self) -> float: ...
    def randint(self, a: int, b: int) -> int: ...

@final
class MCResult:
    estimate: float
    stderr: float
    n: int
    a: float
    b: float
    method: str

def estimate_pi(n: int, seed: int = 0) -> float: ...

def integrate(
    f, a: float, b: float, n: int, seed: int = 0
) -> MCResult: ...

def integrate_antithetic(
    f, a: float, b: float, n: int, seed: int = 0
) -> MCResult: ...
