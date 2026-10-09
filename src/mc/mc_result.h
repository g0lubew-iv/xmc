#ifndef XMC_MC_RESULT_H
#define XMC_MC_RESULT_H

#include <Python.h>

/* Python type: xmc.MCResult
 *
 * Immutable container for a Monte Carlo estimate:
 *   estimate : float   - estimate,
 *   stderr   : float   - standard error,
 *   n        : int     - number of samples used,
 *   a, b     : float   - integration bounds,
 *   method   : str     - "plain", "antithetic", etc.
 */
typedef struct {
    PyObject_HEAD
    double estimate;
    double stderr;
    unsigned long long n;
    double a;
    double b;
    PyObject *method;   /* str, owned */
} XmcMCResult;

extern PyTypeObject XmcMCResult_Type;

/* Register type in module. Returns 0 on success, -1 on error. */
int
XmcMCResult_Register(PyObject *module);

/* Convenience constructor used from other C files.
 * Steals no references; creates a new XmcMCResult or returns NULL.
 */
PyObject *
XmcMCResult_Create(double estimate, double stderr,
                   unsigned long long n, double a,
                   double b, const char *method);

#endif /* XMC_MC_RESULT_H */
