#ifndef XMC_UTIL_ERRORS_H
#define XMC_UTIL_ERRORS_H

#include <Python.h>
#include <errno.h>

#define XMC_RETURN_ERRNO(msg) PyErr_SetFromErrnoWithFilename(PyExc_OSError, (msg))

#define XMC_RETURN_VALUE_ERR(msg) PyErr_SetString(PyExc_ValueError, (msg))

#endif /* XMC_UTIL_ERRORS_H */
