#include "mc/mc.h"
#include "rng/rng_core.h"

static PyObject *
XmcMC_estimate_pi(PyObject *Py_UNUSED(self), PyObject *args, PyObject *kwds) {
    
    static char *kwlist[] = {"n", "seed", NULL};
    unsigned long long n;
    unsigned long long seed = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "K|K", kwlist, &n, &seed)) {
        return NULL;
    }

    xmc_xoshiro_state_t st;
    xmc_xoshiro_seed(&st, (uint64_t) seed);

    /* Throw n random points into unit square and count. */
    unsigned long long inside = 0;
    
    for (unsigned long long i = 0; i < n; ++i) {

        double x = xmc_xoshiro_next_double(&st);
        double y = xmc_xoshiro_next_double(&st);
        
        if (x * x + y * y <= 1.0) {
            inside++;
        }
    }

    return PyFloat_FromDouble(4.0 * (double) inside / (double) n);
}

static PyMethodDef XmcMC_methods[] = {
    {"estimate_pi", _PyCFunction_CAST(XmcMC_estimate_pi), METH_VARARGS | METH_KEYWORDS,
     "estimate_pi(n, seed=0) -> float\n\n"
     "Monte Carlo estimate of pi (single-threaded)."},
    {NULL, NULL, 0, NULL}
};

int XmcMC_Register(PyObject *module) {
    return PyModule_AddFunctions(module, XmcMC_methods);
}
