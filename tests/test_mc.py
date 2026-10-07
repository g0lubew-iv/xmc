import math
import xmc


def test_pi_rough():
    # n=1e6 gives stderr around 0.0005.
    v = xmc.estimate_pi(1_000_000, seed=42)
    assert abs(v - math.pi) < 0.01


def test_pi_deterministic_with_seed():
    assert xmc.estimate_pi(100_000, seed=123) == xmc.estimate_pi(100_000, seed=123)
