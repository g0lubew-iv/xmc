#include <math.h>

#include "mc/mc.h"
#include "mc/mc_result.h"
#include "rng/rng_core.h"
#include "util/errors.h"

/* Call f(x), return result as double.
 * Returns 0 on success, -1 on error. */
static int
evaluate_callback(PyObject *f, double x, double *out) {

    PyObject *arg = PyFloat_FromDouble(x);
    if (arg == NULL) {
        return -1;
    }

    PyObject *result = PyObject_CallOneArg(f, arg);
    Py_DECREF(arg);
    if (result == NULL) {
        return -1;
    }

    double y = PyFloat_AsDouble(result);
    Py_DECREF(result);
    if (y == -1.0 && PyErr_Occurred()) {
        return -1;
    }

    *out = y; return 0;
}

// accumulator

typedef struct {
    double sum;
    double sum_sq;
    unsigned long long n;
} mc_accum_t;

static void
mc_accum_init(mc_accum_t *acc) {
    acc->sum = 0.0;
    acc->sum_sq = 0.0;
    acc->n = 0;
}

static void
mc_accum_add(mc_accum_t *acc, double value) {
    acc->sum += value;
    acc->sum_sq += value * value;
    acc->n++;
}

static void
mc_accum_stats(const mc_accum_t *acc, double *mean_out, double *stderr_out) {

    double n = (double)acc->n;
    double mean = acc->sum / n;
    double var = acc->sum_sq / n - mean * mean;

    if (var < 0.0) {
        var = 0.0;
    }

    *mean_out = mean;
    *stderr_out = sqrt(var / n);
}

// argument parsing

static int
parse_integrate_args(PyObject *args, PyObject *kwds, PyObject **f_out,
                     double *a_out, double *b_out,
                     unsigned long long *n_out, unsigned long long *seed_out) {

    static char *kwlist[] = {"f", "a", "b", "n", "seed", NULL};
    PyObject *f; double a, b;
    unsigned long long n;
    unsigned long long seed = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "OddK|K", kwlist,
                                     &f, &a, &b, &n, &seed)) {
        return -1;
    }

    if (!PyCallable_Check(f)) {
        PyErr_SetString(PyExc_TypeError, "f must be callable");
        return -1;
    }

    if (n == 0) {
        PyErr_SetString(PyExc_ValueError, "n must be > 0");
        return -1;
    }

    *f_out = f;
    *a_out = a;
    *b_out = b;
    *n_out = n;

    *seed_out = seed;
    return 0;
}

// integrate

static PyObject *
XmcMC_integrate(PyObject *Py_UNUSED(self), PyObject *args, PyObject *kwds) {
    PyObject *f; double a, b;
    unsigned long long n, seed;

    if (parse_integrate_args(args, kwds, &f, &a, &b, &n, &seed) < 0) {
        return NULL;
    }

    xmc_xoshiro_state_t st;
    xmc_xoshiro_seed(&st, (uint64_t)seed);

    mc_accum_t acc;
    mc_accum_init(&acc);

    double scale = b - a;

    for (unsigned long long i = 0; i < n; ++i) {
        double x = a + scale * xmc_xoshiro_next_double(&st);
        double y;
        if (evaluate_callback(f, x, &y) < 0) return NULL;
        mc_accum_add(&acc, y * scale);
    }

    double estimate, stderr;
    mc_accum_stats(&acc, &estimate, &stderr);

    return XmcMCResult_Create(estimate, stderr, n, a, b, "plain");
}

// integrate_antithetic

static PyObject *
XmcMC_integrate_antithetic(PyObject *Py_UNUSED(self),
                           PyObject *args, PyObject *kwds) {

    PyObject *f; double a, b;
    unsigned long long n, seed;

    if (parse_integrate_args(args, kwds, &f, &a, &b, &n, &seed) < 0) {
        return NULL;
    }

    xmc_xoshiro_state_t st;
    xmc_xoshiro_seed(&st, (uint64_t)seed);

    mc_accum_t acc;
    mc_accum_init(&acc);

    double scale = b - a;

    for (unsigned long long i = 0; i < n; ++i) {
        double u = xmc_xoshiro_next_double(&st);
        double x1 = a + scale * u;
        double x2 = a + b - x1;

        double y1, y2;
        if (evaluate_callback(f, x1, &y1) < 0) {
            return NULL;
        }
        if (evaluate_callback(f, x2, &y2) < 0) {
            return NULL;
        }

        double pair_mean = 0.5 * (y1 + y2);
        mc_accum_add(&acc, pair_mean * scale);
    }

    double estimate, stderr;
    mc_accum_stats(&acc, &estimate, &stderr);

    return XmcMCResult_Create(estimate, stderr, n, a, b, "antithetic");
}

// methods table

static PyMethodDef XmcMC_Integrate_methods[] = {
    {"integrate",
     _PyCFunction_CAST(XmcMC_integrate),
     METH_VARARGS | METH_KEYWORDS,
     "integrate(f, a, b, n, seed=0) -> MCResult\n\n"
     "Estimate the integral of f over [a, b] using n Monte Carlo samples.\n"
     "GIL is held throughout; expect ~200-500 ns per evaluation."},

    {"integrate_antithetic",
     _PyCFunction_CAST(XmcMC_integrate_antithetic),
     METH_VARARGS | METH_KEYWORDS,
     "integrate_antithetic(f, a, b, n, seed=0) -> MCResult\n\n"
     "Same as integrate, but uses antithetic variates:\n"
     "each sample generates a pair (x, a+b-x)."},
    {NULL, NULL, 0, NULL}
};

int
XmcMC_Integrate_Register(PyObject *module) {
    return PyModule_AddFunctions(module, XmcMC_Integrate_methods);
}
