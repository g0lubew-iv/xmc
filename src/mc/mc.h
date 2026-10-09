#ifndef XMC_MC_H
#define XMC_MC_H

#include <Python.h>

// Register Monte-Carlo functions in module.
int XmcMC_Register(PyObject *module);

// Register integrate functions.
int XmcMC_Integrate_Register(PyObject *module);

#endif /* XMC_MC_H */
