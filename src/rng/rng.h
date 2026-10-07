#ifndef XMC_RNG_H
#define XMC_RNG_H

#include <Python.h>
#include "rng/rng_core.h"

/* Python type: xmc.RNG */
typedef struct {
    PyObject_HEAD 
    /* ob_refcnt + *ob_type, see Python API docs. */
    xmc_xoshiro_state_t *state;
} XmcRNG;

extern PyTypeObject XmcRNG_Type;

/* Register type in module. Returns 0 on success, -1 on error. */
int XmcRNG_Register(PyObject *module);

#endif /* XMC_RNG_H */
