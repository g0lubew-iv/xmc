#include "mc/mc_result.h" // Python.h
#include <structmember.h>
#include <stddef.h>

#include "util/errors.h"

/* ---------- tp_new ---------- */

static PyObject *
XmcMCResult_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"estimate", "stderr", "n", "a", "b", "method", NULL};
    double estimate, stderr, a = 0.0, b = 0.0;
    unsigned long long n;
    PyObject *method = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "ddK|ddO", kwlist,
                                     &estimate, &stderr, &n, &a, &b, &method)) {
        return NULL;
    }

    XmcMCResult *self = (XmcMCResult *)type->tp_alloc(type, 0);
    if (self == NULL) return NULL;

    self->estimate = estimate;
    self->stderr   = stderr;
    self->n        = n;
    self->a        = a;
    self->b        = b;

    if (method == NULL) {
        self->method = PyUnicode_FromString("plain");
    } else {
        Py_INCREF(method);
        self->method = method;
    }
    if (self->method == NULL) {
        Py_DECREF(self);
        return NULL;
    }

    return (PyObject *)self;
}

/* ---------- tp_dealloc ---------- */

static void
XmcMCResult_dealloc(XmcMCResult *self)
{
    Py_XDECREF(self->method);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

/* ---------- members ---------- */

static PyMemberDef XmcMCResult_members[] = {
    {"estimate", T_DOUBLE, offsetof(XmcMCResult, estimate), READONLY,
     "Point estimate (float)."},
    {"stderr",   T_DOUBLE, offsetof(XmcMCResult, stderr),   READONLY,
     "Standard error of the estimate (float)."},
    {"n",        T_ULONGLONG, offsetof(XmcMCResult, n),     READONLY,
     "Number of samples used (int)."},
    {"a",        T_DOUBLE, offsetof(XmcMCResult, a),        READONLY,
     "Lower bound (float)."},
    {"b",        T_DOUBLE, offsetof(XmcMCResult, b),        READONLY,
     "Upper bound (float)."},
    {"method",   T_OBJECT_EX, offsetof(XmcMCResult, method), READONLY,
     "Method name: 'plain', 'antithetic', etc. (str)."},
    {NULL, 0, 0, 0, NULL}
};

/* ---------- repr ---------- */

static PyObject *
XmcMCResult_repr(XmcMCResult *self)
{
    const char *method_str = PyUnicode_AsUTF8(self->method);
    if (method_str == NULL) return NULL;

    char buf[256];
    PyOS_snprintf(buf, sizeof(buf),
        "MCResult(estimate=%.6g, stderr=%.3g, n=%llu, "
        "a=%.6g, b=%.6g, method='%s')",
        self->estimate, self->stderr,
        (unsigned long long)self->n,
        self->a, self->b,
        method_str);
    return PyUnicode_FromString(buf);
}

/* ---------- type object ---------- */

PyTypeObject XmcMCResult_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name      = "xmc.MCResult",
    .tp_basicsize = sizeof(XmcMCResult),
    .tp_itemsize  = 0,
    .tp_dealloc   = (destructor)XmcMCResult_dealloc,
    .tp_repr      = (reprfunc)XmcMCResult_repr,
    .tp_flags     = Py_TPFLAGS_DEFAULT,
    .tp_doc       = "Result of a Monte Carlo integration.",
    .tp_members   = XmcMCResult_members,
    .tp_new       = XmcMCResult_new,
};

int
XmcMCResult_Register(PyObject *module)
{
    if (PyType_Ready(&XmcMCResult_Type) < 0) return -1;
    if (PyModule_AddObjectRef(module, "MCResult",
                              (PyObject *)&XmcMCResult_Type) < 0) {
        return -1;
    }
    return 0;
}

/* ---------- factory ---------- */

PyObject *
XmcMCResult_Create(double estimate, double stderr,
                   unsigned long long n,
                   double a, double b,
                   const char *method)
{
    XmcMCResult *self = (XmcMCResult *)XmcMCResult_Type.tp_alloc(&XmcMCResult_Type, 0);
    if (self == NULL) return NULL;

    self->estimate = estimate;
    self->stderr   = stderr;
    self->n        = n;
    self->a        = a;
    self->b        = b;
    self->method   = PyUnicode_FromString(method ? method : "plain");
    if (self->method == NULL) {
        Py_DECREF(self);
        return NULL;
    }
    return (PyObject *)self;
}
