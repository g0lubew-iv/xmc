"""xmc; xoshiro256 RNG + Monte Carlo, implemented in C."""

from ._xmc import (
    RNG,
    MCResult,
    estimate_pi,
    integrate,
    integrate_antithetic,
    __version__,
)

__all__ = [
    "RNG",
    "MCResult",
    "estimate_pi",
    "integrate",
    "integrate_antithetic",
    "__version__",
]
