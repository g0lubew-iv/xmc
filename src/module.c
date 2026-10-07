#include <Python.h>

#include "mc/mc.h"
#include "rng/rng.h"

static struct PyModuleDef xmc_module = {
    PyModuleDef_HEAD_INIT,
    .m_name    = "_xmc",
    .m_doc     = "xmc: xoshiro256 RNG + Monte-Carlo.",
    .m_size    = -1,
    .m_methods = NULL,
};

PyMODINIT_FUNC
PyInit__xmc(void) {
    PyObject *m = PyModule_Create(&xmc_module);
    if (m == NULL) {
        return NULL;
    }

    if (XmcRNG_Register(m) < 0) {
        Py_DECREF(m);
        return NULL;
    }

    if (XmcMC_Register(m)  < 0) {
        Py_DECREF(m);
        return NULL;
    }

    if (PyModule_AddStringConstant(m, "__version__", "0.1.0") < 0) {
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
