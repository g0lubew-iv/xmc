import math
import xmc


def test_pi_rough():
    # n=1e6 gives stderr around 0.0005.
    v = xmc.estimate_pi(1_000_000, seed=42)
    assert abs(v - math.pi) < 0.01


def test_pi_deterministic_with_seed():
    assert xmc.estimate_pi(100_000, seed=123) == xmc.estimate_pi(100_000, seed=123)


def test_create_and_read():
    r = xmc.MCResult(1.5, 0.1, 1000)
    assert r.estimate == 1.5
    assert r.stderr == 0.1
    assert r.n == 1000
    assert r.a == 0.0
    assert r.b == 0.0
    assert r.method == "plain"


def test_method_override():
    r = xmc.MCResult(1.0, 0.1, 100, method="custom")
    assert r.method == "custom"


def test_readonly():
    r = xmc.MCResult(1.5, 0.1, 1000)
    try:
        r.estimate = 2.0
    except AttributeError:
        pass
    else:
        assert False, "MCResult should be immutable"


def test_repr():
    r = xmc.MCResult(1.2345, 0.0067, 10**6, a=0.0, b=1.0)
    s = repr(r)
    assert "MCResult" in s
    assert "estimate=1.2345" in s
    assert "method='plain'" in s
