from setuptools import setup, Extension

ext = Extension(
    "xmc._xmc",
    sources=[
        "src/module.c",
        "src/rng/rng.c",
        "src/rng/rng_core.c",
        "src/mc/mc_single.c",
        "src/mc/mc_result.c",
        "src/mc/mc_integrate.c",
    ],
    include_dirs=["src"],
    extra_compile_args=["-O3", "-std=c11", "-Wall", "-Wextra"],
)

setup(ext_modules=[ext])
