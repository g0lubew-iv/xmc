#include <string.h>

#include "rng/rng.h"
#include "util/errors.h"

// tp_new: magic method __new__

static PyObject *
XmcRNG_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {

    static char *kwlist[] = {"seed", NULL};
    unsigned long long seed = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|K", kwlist, &seed)) {
        return NULL;
    }

    XmcRNG *self = (XmcRNG *) type->tp_alloc(type, 0);
    if (self == NULL) {
        return NULL;
    }

    self->state = PyMem_Malloc(sizeof(xmc_xoshiro_state_t));
    if (self->state == NULL) {
        Py_DECREF(self);
        return PyErr_NoMemory();
    }

    xmc_xoshiro_seed(self->state, (uint64_t) seed);

    return (PyObject *) self;
}

// tp_dealloc

static void
XmcRNG_dealloc(XmcRNG *self) {
    if (self->state != NULL) {
        /* Zero out state before free. */
        memset(self->state, 0, sizeof(xmc_xoshiro_state_t));
        PyMem_Free(self->state);
        self->state = NULL;
    }
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// tp_methods

static PyObject *
XmcRNG_seed(XmcRNG *self, PyObject *arg) {

    unsigned long long seed = PyLong_AsUnsignedLongLong(arg);
    if (PyErr_Occurred()) {
        return NULL;
    }

    xmc_xoshiro_seed(self->state, (uint64_t) seed);

    Py_RETURN_NONE;
    /* Equivalent to: Py_INCREF(Py_None); return Py_None; */
}

static PyObject *
XmcRNG_random(XmcRNG *self, PyObject *Py_UNUSED(ignored)) {
    return PyFloat_FromDouble(xmc_xoshiro_next_double(self->state));
}

static PyObject *
XmcRNG_randint(XmcRNG *self, PyObject *args) {

    unsigned long long a, b;
    if (!PyArg_ParseTuple(args, "KK", &a, &b)) {
        return NULL;
    }

    if (a > b) {
        XMC_RETURN_VALUE_ERR("randint: a must be <= b");
        return NULL;
    }

    /* randint(a, b) is inclusive on both sides. */
    unsigned long long bound = b - a + 1ULL;

    if (bound == 0) {
        /* Overflow: full uint64 range. */
        return PyLong_FromUnsignedLongLong(xmc_xoshiro_next(self->state));
    }
    
    unsigned long long r = xmc_xoshiro_next_bounded(self->state, bound);

    return PyLong_FromUnsignedLongLong(a + r);
}

// tp_repr: magic method __repr__

static PyObject *
XmcRNG_repr(XmcRNG *self) {
    return PyUnicode_FromFormat("<xmc.RNG state=%p s0=%llu>", (void*) self,
                                (unsigned long long) self->state->s[0]);
}

static PyObject *
XmcRNG_str(XmcRNG *self) {
    return PyUnicode_FromFormat("RNG(s0=%llu)",
                                (unsigned long long) self->state->s[0]);
}

// tables

static PyMethodDef XmcRNG_methods[] = {
    {"seed",    (PyCFunction) XmcRNG_seed,    METH_O,
     "seed(s) -> None\n\nReseed the generator."},
    {"random",  (PyCFunction) XmcRNG_random,  METH_NOARGS,
     "random() -> float in [0.0, 1.0)\n\nNext uniform value."},
    {"randint", (PyCFunction) XmcRNG_randint, METH_VARARGS,
     "randint(a, b) -> int in [a, b]\n\nInclusive integer in range."},
    {NULL, NULL, 0, NULL}
};

PyTypeObject XmcRNG_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name      = "xmc.RNG",
    .tp_basicsize = sizeof(XmcRNG),
    .tp_itemsize  = 0,
    .tp_dealloc   = (destructor) XmcRNG_dealloc,
    .tp_repr      = (reprfunc)   XmcRNG_repr,
    .tp_str       = (reprfunc)   XmcRNG_str,
    .tp_flags     = Py_TPFLAGS_DEFAULT,
    .tp_doc       = "xoshiro256** random number generator.",
    .tp_methods   = XmcRNG_methods,
    .tp_new       = XmcRNG_new,
};

int XmcRNG_Register(PyObject *module) {
    if (PyType_Ready(&XmcRNG_Type) < 0) {
        return -1;
    }

    if (PyModule_AddObjectRef(module, "RNG", (PyObject*) &XmcRNG_Type) < 0) {
        return -1;
    }

    return 0;
}
