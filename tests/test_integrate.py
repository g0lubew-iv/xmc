import math
import pytest
import xmc


def test_sin_on_0_pi():
    # integral of sin(x) on [0, pi] = 2
    r = xmc.integrate(math.sin, 0, math.pi, n=1_000_000, seed=42)
    assert abs(r.estimate - 2.0) < 4 * r.stderr
    assert r.method == "plain"
    assert r.n == 1_000_000
    assert r.a == 0
    assert r.b == math.pi


def test_exp_on_0_1():
    # integral of exp(x) on [0, 1] = e - 1 ≈ 1.71828
    r = xmc.integrate(math.exp, 0, 1, n=1_000_000, seed=42)
    assert abs(r.estimate - (math.e - 1)) < 4 * r.stderr


def test_callback_with_lambda():
    r = xmc.integrate(lambda x: x * x, 0, 1, n=1_000_000, seed=42)
    # integral of x^2 on [0, 1] = 1/3
    assert abs(r.estimate - 1 / 3) < 4 * r.stderr


def test_callback_exception_propagates():
    def bad(x):
        raise ValueError("nope")

    with pytest.raises(ValueError, match="nope"):
        xmc.integrate(bad, 0, 1, n=100)


def test_non_callable_raises():
    with pytest.raises(TypeError):
        xmc.integrate(42, 0, 1, n=100)


def test_n_zero_raises():
    with pytest.raises(ValueError):
        xmc.integrate(math.sin, 0, 1, n=0)


def test_reproducible_with_seed():
    a = xmc.integrate(math.sin, 0, 1, n=10_000, seed=123)
    b = xmc.integrate(math.sin, 0, 1, n=10_000, seed=123)
    assert a.estimate == b.estimate
    assert a.stderr == b.stderr


def test_antithetic_reduces_stderr_for_monotone():
    # exp is monotone on [0, 1]
    plain = xmc.integrate(math.exp, 0, 1, n=100_000, seed=42)
    anti = xmc.integrate_antithetic(math.exp, 0, 1, n=100_000, seed=42)
    assert anti.stderr < plain.stderr
    assert anti.method == "antithetic"


def test_antithetic_correct_mean():
    r = xmc.integrate_antithetic(math.exp, 0, 1, n=1_000_000, seed=42)
    assert abs(r.estimate - (math.e - 1)) < 4 * r.stderr
