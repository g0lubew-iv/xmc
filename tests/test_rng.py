import xmc


def reproducible_with_same_seed():
    a = xmc.RNG(42)
    b = xmc.RNG(42)

    assert [a.random() for _ in range(10)] == [b.random() for _ in range(10)]


def different_seeds():
    assert xmc.RNG(1).random() != xmc.RNG(2).random()


def random_in_unit():
    r = xmc.RNG(0)
    for _ in range(1000):
        assert 0.0 <= r.random() < 1.0


def randint_bounds():
    r = xmc.RNG(123)
    vals = [r.randint(5, 10) for _ in range(1000)]

    assert min(vals) >= 5
    assert max(vals) <= 10
    assert set(vals) == {5, 6, 7, 8, 9, 10}


def seed_method_resets():
    r = xmc.RNG(7)
    first = r.random()
    r.seed(7)

    assert r.random() == first
