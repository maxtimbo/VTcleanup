// This file contains helper functions that are automatically created from
// templates.

#include "nuitka/prelude.h"

extern PyObject *callPythonFunction( PyObject *func, PyObject **args, int count );


PyObject *CALL_FUNCTION_WITH_ARGS1(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 1 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 1; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 1 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 1 * sizeof(PyObject *));
            memcpy(python_pars + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 1)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 1 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 1; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 1 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 1 * sizeof(PyObject *));
                memcpy(python_pars+1 + 1, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 1)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 1 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (1 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 1 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (1 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 1);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 1, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 1);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            1
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 1);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS2(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 2; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 2 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 2; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 2 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 2 * sizeof(PyObject *));
            memcpy(python_pars + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 2)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 2 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 2; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 2 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 2 * sizeof(PyObject *));
                memcpy(python_pars+1 + 2, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 2)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 2 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (2 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 2 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (2 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 2);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 2, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 2);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            2
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 2);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS3(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 3; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 3 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 3; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 3 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 3 * sizeof(PyObject *));
            memcpy(python_pars + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 3)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 3 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 3; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 3 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 3 * sizeof(PyObject *));
                memcpy(python_pars+1 + 3, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 3)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 3 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (3 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 3 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (3 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 3);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 3, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 3);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            3
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 3);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS4(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 4; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 4 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 4; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 4 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 4 * sizeof(PyObject *));
            memcpy(python_pars + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 4)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 4 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 4; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 4 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 4 * sizeof(PyObject *));
                memcpy(python_pars+1 + 4, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 4)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 4 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (4 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 4 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (4 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 4);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 4, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 4);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            4
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 4);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_FUNCTION_WITH_ARGS5(PyObject *called, PyObject **args) {
    CHECK_OBJECT(called);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 5; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    if (Nuitka_Function_Check(called)) {
        if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
            return NULL;
        }

        struct Nuitka_FunctionObject *function = (struct Nuitka_FunctionObject *)called;
        PyObject *result;

        if (function->m_args_simple && 5 == function->m_args_positional_count){
            for (Py_ssize_t i = 0; i < 5; i++) {
                Py_INCREF(args[i]);
            }

            result = function->m_c_code(function, args);
        } else if (function->m_args_simple && 5 + function->m_defaults_given == function->m_args_positional_count) {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
            PyObject *python_pars[function->m_args_positional_count];
#endif
            memcpy(python_pars, args, 5 * sizeof(PyObject *));
            memcpy(python_pars + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

            for (Py_ssize_t i = 0; i < function->m_args_positional_count; i++) {
                Py_INCREF(python_pars[i]);
            }

            result = function->m_c_code(function, python_pars);
        } else {
#ifdef _MSC_VER
            PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
            PyObject *python_pars[function->m_args_overall_count];
#endif
            memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

            if (parseArgumentsPos(function, python_pars, args, 5)) {
                result = function->m_c_code(function, python_pars);
            } else {
                result = NULL;
            }
        }

        Py_LeaveRecursiveCall();

        return result;
    } else if (Nuitka_Method_Check(called)) {
        struct Nuitka_MethodObject *method = (struct Nuitka_MethodObject *)called;

        // Unbound method without arguments, let the error path be slow.
        if (method->m_object != NULL) {
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }

            struct Nuitka_FunctionObject *function = method->m_function;

            PyObject *result;

            if (function->m_args_simple && 5 + 1 == function->m_args_positional_count) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                for (Py_ssize_t i = 0; i < 5; i++) {
                    python_pars[i+1] = args[i];
                    Py_INCREF(args[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else if ( function->m_args_simple && 5 + 1 + function->m_defaults_given == function->m_args_positional_count ) {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_positional_count);
#else
                PyObject *python_pars[function->m_args_positional_count];
#endif
                python_pars[0] = method->m_object;
                Py_INCREF(method->m_object);

                memcpy(python_pars+1, args, 5 * sizeof(PyObject *));
                memcpy(python_pars+1 + 5, &PyTuple_GET_ITEM(function->m_defaults, 0), function->m_defaults_given * sizeof(PyObject *));

                for (Py_ssize_t i = 1; i < function->m_args_overall_count; i++) {
                    Py_INCREF(python_pars[i]);
                }

                result = function->m_c_code(function, python_pars);
            } else {
#ifdef _MSC_VER
                PyObject **python_pars = (PyObject **)_alloca(sizeof(PyObject *) * function->m_args_overall_count);
#else
                PyObject *python_pars[function->m_args_overall_count];
#endif
                memset(python_pars, 0, function->m_args_overall_count * sizeof(PyObject *));

                if (parseArgumentsMethodPos(function, python_pars, method->m_object, args, 5)) {
                    result = function->m_c_code(function, python_pars);
                } else {
                    result = NULL;
                }
            }

            Py_LeaveRecursiveCall();

            return result;
        }
    } else if (PyCFunction_Check(called)) {
        // Try to be fast about wrapping the arguments.
        int flags = PyCFunction_GET_FLAGS(called) & ~(METH_CLASS | METH_STATIC | METH_COEXIST);

        if (flags & METH_NOARGS) {
#if 5 == 0
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, NULL );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(
                PyExc_TypeError,
                "%s() takes no arguments (5 given)",
                ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_O) {
#if 5 == 1
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

            PyObject *result = (*method)( self, args[0] );

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
            // Some buggy C functions do set an error, but do not indicate it
            // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                return NULL;
            }
#else
            PyErr_Format(PyExc_TypeError,
                "%s() takes exactly one argument (5 given)",
                 ((PyCFunctionObject *)called)->m_ml->ml_name
            );
            return NULL;
#endif
        } else if (flags & METH_VARARGS) {
            PyCFunction method = PyCFunction_GET_FUNCTION(called);
            PyObject *self = PyCFunction_GET_SELF(called);

            PyObject *pos_args = MAKE_TUPLE(args, 5);

            PyObject *result;

            // Recursion guard is not strictly necessary, as we already have
            // one on our way to here.
#ifdef _NUITKA_FULL_COMPAT
            if (unlikely(Py_EnterRecursiveCall((char *)" while calling a Python object"))) {
                return NULL;
            }
#endif

#if PYTHON_VERSION < 360
            if (flags & METH_KEYWORDS) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else {
                result = (*method)(self, pos_args);
            }
#else
            if (flags == (METH_VARARGS|METH_KEYWORDS)) {
                result = (*(PyCFunctionWithKeywords)method)(self, pos_args, NULL);
            } else if (flags == METH_FASTCALL) {
#if PYTHON_VERSION < 370
                result = (*(_PyCFunctionFast)method)(self, &PyTuple_GET_ITEM(pos_args, 0), 5, NULL);
#else
                result = (*(_PyCFunctionFast)method)(self, &pos_args, 5);
#endif
            } else {
                result = (*method)(self, pos_args);
            }
#endif

#ifdef _NUITKA_FULL_COMPAT
            Py_LeaveRecursiveCall();
#endif

            if (result != NULL) {
                // Some buggy C functions do set an error, but do not indicate it
                // and Nuitka inner workings can get upset/confused from it.
                DROP_ERROR_OCCURRED();

                Py_DECREF(pos_args);
                return result;
            } else {
                // Other buggy C functions do this, return NULL, but with
                // no error set, not allowed.
                if (unlikely(!ERROR_OCCURRED())) {
                    SET_CURRENT_EXCEPTION_TYPE0_STR(
                        PyExc_SystemError,
                        "NULL result without error in PyObject_Call"
                    );
                }

                Py_DECREF(pos_args);
                return NULL;
            }
        }
    } else if (PyFunction_Check(called)) {
        return callPythonFunction(
            called,
            args,
            5
        );
    }

    PyObject *pos_args = MAKE_TUPLE(args, 5);

    PyObject *result = CALL_FUNCTION(called, pos_args, NULL);

    Py_DECREF(pos_args);

    return result;
}

PyObject *CALL_METHOD_WITH_ARGS1(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 1; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    1
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS1(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    1
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS1(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS1(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS1(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS1(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}

PyObject *CALL_METHOD_WITH_ARGS2(PyObject *source, PyObject *attr_name, PyObject **args) {
    CHECK_OBJECT(source);
    CHECK_OBJECT(attr_name);

    // Check if arguments are valid objects in debug mode.
#ifndef __NUITKA_NO_ASSERT__
    for (size_t i = 0; i < 2; i++) {
        CHECK_OBJECT(args[i]);
    }
#endif

    PyTypeObject *type = Py_TYPE(source);

    if (type->tp_getattro == PyObject_GenericGetAttr) {
        // Unfortunately this is required, although of cause rarely necessary.
        if (unlikely(type->tp_dict == NULL)) {
            if (unlikely(PyType_Ready(type) < 0)) {
                return NULL;
            }
        }

        PyObject *descr = _PyType_Lookup(type, attr_name);
        descrgetfunc func = NULL;

        if (descr != NULL)
        {
            Py_INCREF(descr);

#if PYTHON_VERSION < 300
            if (PyType_HasFeature(Py_TYPE(descr), Py_TPFLAGS_HAVE_CLASS)) {
#endif
                func = Py_TYPE(descr)->tp_descr_get;

                if (func != NULL && PyDescr_IsData(descr))
                {
                    PyObject *called_object = func(descr, source, (PyObject *)type);
                    Py_DECREF(descr);

                    PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                        called_object,
                        args
                    );
                    Py_DECREF(called_object);
                    return result;
                }
#if PYTHON_VERSION < 300
            }
#endif
        }

        Py_ssize_t dictoffset = type->tp_dictoffset;
        PyObject *dict = NULL;

        if ( dictoffset != 0 )
        {
            // Negative dictionary offsets have special meaning.
            if ( dictoffset < 0 )
            {
                Py_ssize_t tsize;
                size_t size;

                tsize = ((PyVarObject *)source)->ob_size;
                if (tsize < 0)
                    tsize = -tsize;
                size = _PyObject_VAR_SIZE( type, tsize );

                dictoffset += (long)size;
            }

            PyObject **dictptr = (PyObject **) ((char *)source + dictoffset);
            dict = *dictptr;
        }

        if (dict != NULL)
        {
            CHECK_OBJECT(dict);

            Py_INCREF(dict);

            PyObject *called_object = PyDict_GetItem(dict, attr_name);

            if (called_object != NULL)
            {
                Py_INCREF(called_object);
                Py_XDECREF(descr);
                Py_DECREF(dict);

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF(called_object);
                return result;
            }

            Py_DECREF(dict);
        }

        if (func != NULL) {
            if (func == Nuitka_Function_Type.tp_descr_get) {
                PyObject *result = Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)descr,
                    source,
                    args,
                    2
                );

                Py_DECREF(descr);

                return result;
            } else {
                PyObject *called_object = func(descr, source, (PyObject *)type);
                CHECK_OBJECT(called_object);

                Py_DECREF(descr);

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                    called_object,
                    args
                );
                Py_DECREF(called_object);

                return result;
            }
        }

        if (descr != NULL) {
            CHECK_OBJECT(descr);

            PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                descr,
                args
            );
            Py_DECREF(descr);

            return result;
        }

#if PYTHON_VERSION < 300
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            PyString_AS_STRING( attr_name )
        );
#else
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%U'",
            type->tp_name,
            attr_name
        );
#endif
        return NULL;
    }
#if PYTHON_VERSION < 300
    else if (type == &PyInstance_Type) {
        PyInstanceObject *source_instance = (PyInstanceObject *)source;

        // The special cases have their own variant on the code generation level
        // as we are called with constants only.
        assert(attr_name != const_str_plain___dict__);
        assert(attr_name != const_str_plain___class__);

        // Try the instance dict first.
        PyObject *called_object = GET_STRING_DICT_VALUE(
            (PyDictObject *)source_instance->in_dict,
            (PyStringObject *)attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            return CALL_FUNCTION_WITH_ARGS2(called_object, args);
        }

        // Then check the class dictionaries.
        called_object = FIND_ATTRIBUTE_IN_CLASS(
            source_instance->in_class,
            attr_name
        );

        // Note: The "called_object" was found without taking a reference,
        // so we need not release it in this branch.
        if (called_object != NULL) {
            descrgetfunc descr_get = Py_TYPE(called_object)->tp_descr_get;

            if (descr_get == Nuitka_Function_Type.tp_descr_get) {
                return Nuitka_CallMethodFunctionPosArgs(
                    (struct Nuitka_FunctionObject const *)called_object,
                    source,
                    args,
                    2
                );
            } else if (descr_get != NULL) {
                PyObject *method = descr_get(
                    called_object,
                    source,
                    (PyObject *)source_instance->in_class
                );

                if (unlikely(method == NULL)) {
                    return NULL;
                }

                PyObject *result = CALL_FUNCTION_WITH_ARGS2(method, args);
                Py_DECREF(method);
                return result;
            } else {
                return CALL_FUNCTION_WITH_ARGS2(called_object, args);
            }

        } else if (unlikely(source_instance->in_class->cl_getattr == NULL)) {
            PyErr_Format(
                PyExc_AttributeError,
                "%s instance has no attribute '%s'",
                PyString_AS_STRING( source_instance->in_class->cl_name ),
                PyString_AS_STRING( attr_name )
            );

            return NULL;
        } else {
            // Finally allow the "__getattr__" override to provide it or else
            // it's an error.

            PyObject *args2[] = {
                source,
                attr_name
            };

            called_object = CALL_FUNCTION_WITH_ARGS2(
                source_instance->in_class->cl_getattr,
                args2
            );

            if (unlikely(called_object == NULL))
            {
                return NULL;
            }

            PyObject *result = CALL_FUNCTION_WITH_ARGS2(
                called_object,
                args
            );
            Py_DECREF(called_object);
            return result;
        }
    }
#endif
    else if (type->tp_getattro != NULL) {
        PyObject *called_object = (*type->tp_getattro)(
            source,
            attr_name
        );

        if (unlikely(called_object == NULL)) {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else if (type->tp_getattr != NULL) {
        PyObject *called_object = (*type->tp_getattr)(
            source,
            (char *)Nuitka_String_AsString_Unchecked(attr_name)
        );

        if (unlikely(called_object == NULL))
        {
            return NULL;
        }

        PyObject *result = CALL_FUNCTION_WITH_ARGS2(
            called_object,
            args
        );
        Py_DECREF(called_object);
        return result;
    } else {
        PyErr_Format(
            PyExc_AttributeError,
            "'%s' object has no attribute '%s'",
            type->tp_name,
            Nuitka_String_AsString_Unchecked(attr_name)
        );

        return NULL;
    }
}
/* Code to register embedded modules for meta path based loading if any. */

#include "nuitka/unfreezing.h"

/* Table for lookup to find compiled or bytecode modules included in this
 * binary or module, or put along this binary as extension modules. We do
 * our own loading for each of these.
 */
extern PyObject *modulecode___main__(PyObject *);
extern PyObject *modulecode_tkinter$$45$preLoad(PyObject *);
static struct Nuitka_MetaPathBasedLoaderEntry meta_path_loader_entries[] = {
    {"__main__", modulecode___main__, 0, 0, },
    {"_asyncio", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_bz2", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ctypes", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_decimal", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_elementtree", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_hashlib", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_lzma", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_msi", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_multiprocessing", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_overlapped", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_queue", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_socket", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_sqlite3", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_ssl", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"_tkinter", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"configparser", NULL, 4514, 45713, NUITKA_BYTECODE_FLAG},
    {"datetime", NULL, 50227, 56973, NUITKA_BYTECODE_FLAG},
    {"os", NULL, 107200, 31392, NUITKA_BYTECODE_FLAG},
    {"pyexpat", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"select", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"shutil", NULL, 138592, 36564, NUITKA_BYTECODE_FLAG},
    {"site", NULL, 175156, 13302, NUITKA_BYTECODE_FLAG},
    {"threading", NULL, 188458, 39971, NUITKA_BYTECODE_FLAG},
    {"tkinter", NULL, 228429, 175997, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"tkinter-preLoad", modulecode_tkinter$$45$preLoad, 0, 0, },
    {"tkinter.filedialog", NULL, 404426, 12223, NUITKA_BYTECODE_FLAG},
    {"unicodedata", NULL, 0, 0, NUITKA_SHLIB_FLAG},
    {"__future__", NULL, 416649, 4126, NUITKA_BYTECODE_FLAG},
    {"_bootlocale", NULL, 420775, 1238, NUITKA_BYTECODE_FLAG},
    {"_collections_abc", NULL, 422013, 28736, NUITKA_BYTECODE_FLAG},
    {"_compat_pickle", NULL, 450749, 5496, NUITKA_BYTECODE_FLAG},
    {"_compression", NULL, 456245, 4141, NUITKA_BYTECODE_FLAG},
    {"_dummy_thread", NULL, 460386, 6032, NUITKA_BYTECODE_FLAG},
    {"_markupbase", NULL, 466418, 7785, NUITKA_BYTECODE_FLAG},
    {"_osx_support", NULL, 474203, 10307, NUITKA_BYTECODE_FLAG},
    {"_py_abc", NULL, 484510, 4665, NUITKA_BYTECODE_FLAG},
    {"_pyio", NULL, 489175, 74074, NUITKA_BYTECODE_FLAG},
    {"_sitebuiltins", NULL, 563249, 3476, NUITKA_BYTECODE_FLAG},
    {"_strptime", NULL, 566725, 16039, NUITKA_BYTECODE_FLAG},
    {"_threading_local", NULL, 582764, 6441, NUITKA_BYTECODE_FLAG},
    {"_weakrefset", NULL, 589205, 7595, NUITKA_BYTECODE_FLAG},
    {"abc", NULL, 596800, 5329, NUITKA_BYTECODE_FLAG},
    {"aifc", NULL, 602129, 25469, NUITKA_BYTECODE_FLAG},
    {"argparse", NULL, 627598, 62272, NUITKA_BYTECODE_FLAG},
    {"ast", NULL, 689870, 16515, NUITKA_BYTECODE_FLAG},
    {"asynchat", NULL, 706385, 6846, NUITKA_BYTECODE_FLAG},
    {"asyncio", NULL, 713231, 745, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"asyncio.base_events", NULL, 713976, 51015, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_futures", NULL, 764991, 1718, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_subprocess", NULL, 766709, 9411, NUITKA_BYTECODE_FLAG},
    {"asyncio.base_tasks", NULL, 776120, 1939, NUITKA_BYTECODE_FLAG},
    {"asyncio.constants", NULL, 778059, 576, NUITKA_BYTECODE_FLAG},
    {"asyncio.coroutines", NULL, 778635, 6649, NUITKA_BYTECODE_FLAG},
    {"asyncio.events", NULL, 785284, 28052, NUITKA_BYTECODE_FLAG},
    {"asyncio.exceptions", NULL, 813336, 2493, NUITKA_BYTECODE_FLAG},
    {"asyncio.format_helpers", NULL, 815829, 2327, NUITKA_BYTECODE_FLAG},
    {"asyncio.futures", NULL, 818156, 11188, NUITKA_BYTECODE_FLAG},
    {"asyncio.locks", NULL, 829344, 16363, NUITKA_BYTECODE_FLAG},
    {"asyncio.log", NULL, 845707, 221, NUITKA_BYTECODE_FLAG},
    {"asyncio.proactor_events", NULL, 845928, 24089, NUITKA_BYTECODE_FLAG},
    {"asyncio.protocols", NULL, 870017, 8609, NUITKA_BYTECODE_FLAG},
    {"asyncio.queues", NULL, 878626, 8376, NUITKA_BYTECODE_FLAG},
    {"asyncio.runners", NULL, 887002, 1933, NUITKA_BYTECODE_FLAG},
    {"asyncio.selector_events", NULL, 888935, 29674, NUITKA_BYTECODE_FLAG},
    {"asyncio.sslproto", NULL, 918609, 21613, NUITKA_BYTECODE_FLAG},
    {"asyncio.staggered", NULL, 940222, 4113, NUITKA_BYTECODE_FLAG},
    {"asyncio.streams", NULL, 944335, 20635, NUITKA_BYTECODE_FLAG},
    {"asyncio.subprocess", NULL, 964970, 7353, NUITKA_BYTECODE_FLAG},
    {"asyncio.tasks", NULL, 972323, 23741, NUITKA_BYTECODE_FLAG},
    {"asyncio.transports", NULL, 996064, 12254, NUITKA_BYTECODE_FLAG},
    {"asyncio.trsock", NULL, 1008318, 8478, NUITKA_BYTECODE_FLAG},
    {"asyncio.windows_events", NULL, 1016796, 24578, NUITKA_BYTECODE_FLAG},
    {"asyncio.windows_utils", NULL, 1041374, 4466, NUITKA_BYTECODE_FLAG},
    {"asyncore", NULL, 1045840, 16023, NUITKA_BYTECODE_FLAG},
    {"base64", NULL, 1061863, 17066, NUITKA_BYTECODE_FLAG},
    {"bdb", NULL, 1078929, 24916, NUITKA_BYTECODE_FLAG},
    {"binhex", NULL, 1103845, 12130, NUITKA_BYTECODE_FLAG},
    {"bisect", NULL, 1115975, 2349, NUITKA_BYTECODE_FLAG},
    {"bz2", NULL, 1118324, 11440, NUITKA_BYTECODE_FLAG},
    {"cProfile", NULL, 1129764, 5329, NUITKA_BYTECODE_FLAG},
    {"calendar", NULL, 1135093, 27059, NUITKA_BYTECODE_FLAG},
    {"cgi", NULL, 1162152, 26247, NUITKA_BYTECODE_FLAG},
    {"cgitb", NULL, 1188399, 10145, NUITKA_BYTECODE_FLAG},
    {"chunk", NULL, 1198544, 4834, NUITKA_BYTECODE_FLAG},
    {"cmd", NULL, 1203378, 12621, NUITKA_BYTECODE_FLAG},
    {"code", NULL, 1215999, 9908, NUITKA_BYTECODE_FLAG},
    {"codecs", NULL, 1225907, 33951, NUITKA_BYTECODE_FLAG},
    {"codeop", NULL, 1259858, 6292, NUITKA_BYTECODE_FLAG},
    {"collections", NULL, 1266150, 46430, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"collections.abc", NULL, 422013, 28736, NUITKA_BYTECODE_FLAG},
    {"colorsys", NULL, 1312580, 3235, NUITKA_BYTECODE_FLAG},
    {"compileall", NULL, 1315815, 9405, NUITKA_BYTECODE_FLAG},
    {"concurrent", NULL, 1325220, 127, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"concurrent.futures", NULL, 1325347, 1103, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"concurrent.futures._base", NULL, 1326450, 21895, NUITKA_BYTECODE_FLAG},
    {"concurrent.futures.process", NULL, 1348345, 20295, NUITKA_BYTECODE_FLAG},
    {"concurrent.futures.thread", NULL, 1368640, 5813, NUITKA_BYTECODE_FLAG},
    {"configparser", NULL, 4514, 45713, NUITKA_BYTECODE_FLAG},
    {"contextlib", NULL, 1374453, 20224, NUITKA_BYTECODE_FLAG},
    {"contextvars", NULL, 1394677, 238, NUITKA_BYTECODE_FLAG},
    {"copy", NULL, 1394915, 6982, NUITKA_BYTECODE_FLAG},
    {"copyreg", NULL, 1401897, 4313, NUITKA_BYTECODE_FLAG},
    {"csv", NULL, 1406210, 11905, NUITKA_BYTECODE_FLAG},
    {"ctypes", NULL, 1418115, 16367, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes._aix", NULL, 1434482, 9839, NUITKA_BYTECODE_FLAG},
    {"ctypes._endian", NULL, 1444321, 1924, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib", NULL, 1446245, 294, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"ctypes.macholib.dyld", NULL, 1446539, 4372, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib.dylib", NULL, 1450911, 1911, NUITKA_BYTECODE_FLAG},
    {"ctypes.macholib.framework", NULL, 1452822, 2191, NUITKA_BYTECODE_FLAG},
    {"ctypes.util", NULL, 1455013, 7756, NUITKA_BYTECODE_FLAG},
    {"ctypes.wintypes", NULL, 1462769, 5098, NUITKA_BYTECODE_FLAG},
    {"dataclasses", NULL, 1467867, 23622, NUITKA_BYTECODE_FLAG},
    {"datetime", NULL, 50227, 56973, NUITKA_BYTECODE_FLAG},
    {"dbm", NULL, 1491489, 4187, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"dbm.dumb", NULL, 1495676, 7768, NUITKA_BYTECODE_FLAG},
    {"decimal", NULL, 1503444, 160730, NUITKA_BYTECODE_FLAG},
    {"difflib", NULL, 1664174, 59433, NUITKA_BYTECODE_FLAG},
    {"dis", NULL, 1723607, 15797, NUITKA_BYTECODE_FLAG},
    {"distutils", NULL, 1739404, 379, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils._msvccompiler", NULL, 1739783, 12961, NUITKA_BYTECODE_FLAG},
    {"distutils.archive_util", NULL, 1752744, 6534, NUITKA_BYTECODE_FLAG},
    {"distutils.bcppcompiler", NULL, 1759278, 6518, NUITKA_BYTECODE_FLAG},
    {"distutils.ccompiler", NULL, 1765796, 33292, NUITKA_BYTECODE_FLAG},
    {"distutils.cmd", NULL, 1799088, 13943, NUITKA_BYTECODE_FLAG},
    {"distutils.command", NULL, 1813031, 538, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"distutils.command.bdist", NULL, 1813569, 3661, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_dumb", NULL, 1817230, 3587, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_msi", NULL, 1820817, 19618, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_rpm", NULL, 1840435, 12423, NUITKA_BYTECODE_FLAG},
    {"distutils.command.bdist_wininst", NULL, 1852858, 8484, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build", NULL, 1861342, 3876, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_clib", NULL, 1865218, 4809, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_ext", NULL, 1870027, 16132, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_py", NULL, 1886159, 10457, NUITKA_BYTECODE_FLAG},
    {"distutils.command.build_scripts", NULL, 1896616, 4319, NUITKA_BYTECODE_FLAG},
    {"distutils.command.check", NULL, 1900935, 4880, NUITKA_BYTECODE_FLAG},
    {"distutils.command.clean", NULL, 1905815, 2095, NUITKA_BYTECODE_FLAG},
    {"distutils.command.config", NULL, 1907910, 10222, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install", NULL, 1918132, 13567, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_data", NULL, 1931699, 2284, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_egg_info", NULL, 1933983, 2991, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_headers", NULL, 1936974, 1703, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_lib", NULL, 1938677, 5108, NUITKA_BYTECODE_FLAG},
    {"distutils.command.install_scripts", NULL, 1943785, 2136, NUITKA_BYTECODE_FLAG},
    {"distutils.command.register", NULL, 1945921, 8450, NUITKA_BYTECODE_FLAG},
    {"distutils.command.sdist", NULL, 1954371, 14511, NUITKA_BYTECODE_FLAG},
    {"distutils.command.upload", NULL, 1968882, 4934, NUITKA_BYTECODE_FLAG},
    {"distutils.config", NULL, 1973816, 3504, NUITKA_BYTECODE_FLAG},
    {"distutils.core", NULL, 1977320, 6609, NUITKA_BYTECODE_FLAG},
    {"distutils.cygwinccompiler", NULL, 1983929, 8607, NUITKA_BYTECODE_FLAG},
    {"distutils.debug", NULL, 1992536, 189, NUITKA_BYTECODE_FLAG},
    {"distutils.dep_util", NULL, 1992725, 2709, NUITKA_BYTECODE_FLAG},
    {"distutils.dir_util", NULL, 1995434, 5818, NUITKA_BYTECODE_FLAG},
    {"distutils.dist", NULL, 2001252, 34487, NUITKA_BYTECODE_FLAG},
    {"distutils.errors", NULL, 2035739, 5245, NUITKA_BYTECODE_FLAG},
    {"distutils.extension", NULL, 2040984, 6918, NUITKA_BYTECODE_FLAG},
    {"distutils.fancy_getopt", NULL, 2047902, 10651, NUITKA_BYTECODE_FLAG},
    {"distutils.file_util", NULL, 2058553, 5928, NUITKA_BYTECODE_FLAG},
    {"distutils.filelist", NULL, 2064481, 9862, NUITKA_BYTECODE_FLAG},
    {"distutils.log", NULL, 2074343, 2310, NUITKA_BYTECODE_FLAG},
    {"distutils.msvc9compiler", NULL, 2076653, 17505, NUITKA_BYTECODE_FLAG},
    {"distutils.msvccompiler", NULL, 2094158, 14718, NUITKA_BYTECODE_FLAG},
    {"distutils.spawn", NULL, 2108876, 5101, NUITKA_BYTECODE_FLAG},
    {"distutils.sysconfig", NULL, 2113977, 12096, NUITKA_BYTECODE_FLAG},
    {"distutils.text_file", NULL, 2126073, 8434, NUITKA_BYTECODE_FLAG},
    {"distutils.unixccompiler", NULL, 2134507, 6613, NUITKA_BYTECODE_FLAG},
    {"distutils.util", NULL, 2141120, 15541, NUITKA_BYTECODE_FLAG},
    {"distutils.version", NULL, 2156661, 7306, NUITKA_BYTECODE_FLAG},
    {"distutils.versionpredicate", NULL, 2163967, 5130, NUITKA_BYTECODE_FLAG},
    {"doctest", NULL, 2169097, 75969, NUITKA_BYTECODE_FLAG},
    {"dummy_threading", NULL, 2245066, 1105, NUITKA_BYTECODE_FLAG},
    {"email", NULL, 2246171, 1686, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email._encoded_words", NULL, 2247857, 5681, NUITKA_BYTECODE_FLAG},
    {"email._header_value_parser", NULL, 2253538, 79782, NUITKA_BYTECODE_FLAG},
    {"email._parseaddr", NULL, 2333320, 12449, NUITKA_BYTECODE_FLAG},
    {"email._policybase", NULL, 2345769, 14805, NUITKA_BYTECODE_FLAG},
    {"email.base64mime", NULL, 2360574, 3230, NUITKA_BYTECODE_FLAG},
    {"email.charset", NULL, 2363804, 11454, NUITKA_BYTECODE_FLAG},
    {"email.contentmanager", NULL, 2375258, 7338, NUITKA_BYTECODE_FLAG},
    {"email.encoders", NULL, 2382596, 1607, NUITKA_BYTECODE_FLAG},
    {"email.errors", NULL, 2384203, 5900, NUITKA_BYTECODE_FLAG},
    {"email.feedparser", NULL, 2390103, 10637, NUITKA_BYTECODE_FLAG},
    {"email.generator", NULL, 2400740, 12477, NUITKA_BYTECODE_FLAG},
    {"email.header", NULL, 2413217, 16434, NUITKA_BYTECODE_FLAG},
    {"email.headerregistry", NULL, 2429651, 21895, NUITKA_BYTECODE_FLAG},
    {"email.iterators", NULL, 2451546, 1915, NUITKA_BYTECODE_FLAG},
    {"email.message", NULL, 2453461, 37873, NUITKA_BYTECODE_FLAG},
    {"email.mime", NULL, 2491334, 127, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"email.mime.application", NULL, 2491461, 1454, NUITKA_BYTECODE_FLAG},
    {"email.mime.audio", NULL, 2492915, 2619, NUITKA_BYTECODE_FLAG},
    {"email.mime.base", NULL, 2495534, 1036, NUITKA_BYTECODE_FLAG},
    {"email.mime.image", NULL, 2496570, 1899, NUITKA_BYTECODE_FLAG},
    {"email.mime.message", NULL, 2498469, 1277, NUITKA_BYTECODE_FLAG},
    {"email.mime.multipart", NULL, 2499746, 1497, NUITKA_BYTECODE_FLAG},
    {"email.mime.nonmultipart", NULL, 2501243, 759, NUITKA_BYTECODE_FLAG},
    {"email.mime.text", NULL, 2502002, 1306, NUITKA_BYTECODE_FLAG},
    {"email.parser", NULL, 2503308, 5717, NUITKA_BYTECODE_FLAG},
    {"email.policy", NULL, 2509025, 9653, NUITKA_BYTECODE_FLAG},
    {"email.quoprimime", NULL, 2518678, 7673, NUITKA_BYTECODE_FLAG},
    {"email.utils", NULL, 2526351, 9542, NUITKA_BYTECODE_FLAG},
    {"encodings", NULL, 2535893, 3898, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"encodings.aliases", NULL, 2539791, 6325, NUITKA_BYTECODE_FLAG},
    {"encodings.ascii", NULL, 2546116, 1876, NUITKA_BYTECODE_FLAG},
    {"encodings.base64_codec", NULL, 2547992, 2394, NUITKA_BYTECODE_FLAG},
    {"encodings.big5", NULL, 2550386, 1404, NUITKA_BYTECODE_FLAG},
    {"encodings.big5hkscs", NULL, 2551790, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.bz2_codec", NULL, 2553204, 3285, NUITKA_BYTECODE_FLAG},
    {"encodings.charmap", NULL, 2556489, 2886, NUITKA_BYTECODE_FLAG},
    {"encodings.cp037", NULL, 2559375, 2417, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1006", NULL, 2561792, 2493, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1026", NULL, 2564285, 2421, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1125", NULL, 2566706, 8124, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1140", NULL, 2574830, 2407, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1250", NULL, 2577237, 2444, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1251", NULL, 2579681, 2441, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1252", NULL, 2582122, 2444, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1253", NULL, 2584566, 2457, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1254", NULL, 2587023, 2446, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1255", NULL, 2589469, 2465, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1256", NULL, 2591934, 2443, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1257", NULL, 2594377, 2451, NUITKA_BYTECODE_FLAG},
    {"encodings.cp1258", NULL, 2596828, 2449, NUITKA_BYTECODE_FLAG},
    {"encodings.cp273", NULL, 2599277, 2403, NUITKA_BYTECODE_FLAG},
    {"encodings.cp424", NULL, 2601680, 2447, NUITKA_BYTECODE_FLAG},
    {"encodings.cp437", NULL, 2604127, 7841, NUITKA_BYTECODE_FLAG},
    {"encodings.cp500", NULL, 2611968, 2417, NUITKA_BYTECODE_FLAG},
    {"encodings.cp720", NULL, 2614385, 2514, NUITKA_BYTECODE_FLAG},
    {"encodings.cp737", NULL, 2616899, 8163, NUITKA_BYTECODE_FLAG},
    {"encodings.cp775", NULL, 2625062, 7871, NUITKA_BYTECODE_FLAG},
    {"encodings.cp850", NULL, 2632933, 7502, NUITKA_BYTECODE_FLAG},
    {"encodings.cp852", NULL, 2640435, 7879, NUITKA_BYTECODE_FLAG},
    {"encodings.cp855", NULL, 2648314, 8132, NUITKA_BYTECODE_FLAG},
    {"encodings.cp856", NULL, 2656446, 2479, NUITKA_BYTECODE_FLAG},
    {"encodings.cp857", NULL, 2658925, 7482, NUITKA_BYTECODE_FLAG},
    {"encodings.cp858", NULL, 2666407, 7472, NUITKA_BYTECODE_FLAG},
    {"encodings.cp860", NULL, 2673879, 7820, NUITKA_BYTECODE_FLAG},
    {"encodings.cp861", NULL, 2681699, 7835, NUITKA_BYTECODE_FLAG},
    {"encodings.cp862", NULL, 2689534, 8024, NUITKA_BYTECODE_FLAG},
    {"encodings.cp863", NULL, 2697558, 7835, NUITKA_BYTECODE_FLAG},
    {"encodings.cp864", NULL, 2705393, 7979, NUITKA_BYTECODE_FLAG},
    {"encodings.cp865", NULL, 2713372, 7835, NUITKA_BYTECODE_FLAG},
    {"encodings.cp866", NULL, 2721207, 8168, NUITKA_BYTECODE_FLAG},
    {"encodings.cp869", NULL, 2729375, 7859, NUITKA_BYTECODE_FLAG},
    {"encodings.cp874", NULL, 2737234, 2545, NUITKA_BYTECODE_FLAG},
    {"encodings.cp875", NULL, 2739779, 2414, NUITKA_BYTECODE_FLAG},
    {"encodings.cp932", NULL, 2742193, 1406, NUITKA_BYTECODE_FLAG},
    {"encodings.cp949", NULL, 2743599, 1406, NUITKA_BYTECODE_FLAG},
    {"encodings.cp950", NULL, 2745005, 1406, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jis_2004", NULL, 2746411, 1420, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jisx0213", NULL, 2747831, 1420, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_jp", NULL, 2749251, 1408, NUITKA_BYTECODE_FLAG},
    {"encodings.euc_kr", NULL, 2750659, 1408, NUITKA_BYTECODE_FLAG},
    {"encodings.gb18030", NULL, 2752067, 1410, NUITKA_BYTECODE_FLAG},
    {"encodings.gb2312", NULL, 2753477, 1408, NUITKA_BYTECODE_FLAG},
    {"encodings.gbk", NULL, 2754885, 1402, NUITKA_BYTECODE_FLAG},
    {"encodings.hex_codec", NULL, 2756287, 2381, NUITKA_BYTECODE_FLAG},
    {"encodings.hp_roman8", NULL, 2758668, 2618, NUITKA_BYTECODE_FLAG},
    {"encodings.hz", NULL, 2761286, 1400, NUITKA_BYTECODE_FLAG},
    {"encodings.idna", NULL, 2762686, 5612, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp", NULL, 2768298, 1421, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_1", NULL, 2769719, 1425, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_2", NULL, 2771144, 1425, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_2004", NULL, 2772569, 1431, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_3", NULL, 2774000, 1425, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_jp_ext", NULL, 2775425, 1429, NUITKA_BYTECODE_FLAG},
    {"encodings.iso2022_kr", NULL, 2776854, 1421, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_1", NULL, 2778275, 2416, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_10", NULL, 2780691, 2421, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_11", NULL, 2783112, 2515, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_13", NULL, 2785627, 2424, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_14", NULL, 2788051, 2442, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_15", NULL, 2790493, 2421, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_16", NULL, 2792914, 2423, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_2", NULL, 2795337, 2416, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_3", NULL, 2797753, 2423, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_4", NULL, 2800176, 2416, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_5", NULL, 2802592, 2417, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_6", NULL, 2805009, 2461, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_7", NULL, 2807470, 2424, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_8", NULL, 2809894, 2455, NUITKA_BYTECODE_FLAG},
    {"encodings.iso8859_9", NULL, 2812349, 2416, NUITKA_BYTECODE_FLAG},
    {"encodings.johab", NULL, 2814765, 1406, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_r", NULL, 2816171, 2468, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_t", NULL, 2818639, 2379, NUITKA_BYTECODE_FLAG},
    {"encodings.koi8_u", NULL, 2821018, 2454, NUITKA_BYTECODE_FLAG},
    {"encodings.kz1048", NULL, 2823472, 2431, NUITKA_BYTECODE_FLAG},
    {"encodings.latin_1", NULL, 2825903, 1888, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_arabic", NULL, 2827791, 7735, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_centeuro", NULL, 2835526, 2455, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_croatian", NULL, 2837981, 2463, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_cyrillic", NULL, 2840444, 2453, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_farsi", NULL, 2842897, 2397, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_greek", NULL, 2845294, 2437, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_iceland", NULL, 2847731, 2456, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_latin2", NULL, 2850187, 2597, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_roman", NULL, 2852784, 2454, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_romanian", NULL, 2855238, 2464, NUITKA_BYTECODE_FLAG},
    {"encodings.mac_turkish", NULL, 2857702, 2457, NUITKA_BYTECODE_FLAG},
    {"encodings.mbcs", NULL, 2860159, 1706, NUITKA_BYTECODE_FLAG},
    {"encodings.oem", NULL, 2861865, 1519, NUITKA_BYTECODE_FLAG},
    {"encodings.palmos", NULL, 2863384, 2444, NUITKA_BYTECODE_FLAG},
    {"encodings.ptcp154", NULL, 2865828, 2538, NUITKA_BYTECODE_FLAG},
    {"encodings.punycode", NULL, 2868366, 6310, NUITKA_BYTECODE_FLAG},
    {"encodings.quopri_codec", NULL, 2874676, 2410, NUITKA_BYTECODE_FLAG},
    {"encodings.raw_unicode_escape", NULL, 2877086, 1757, NUITKA_BYTECODE_FLAG},
    {"encodings.rot_13", NULL, 2878843, 2996, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jis", NULL, 2881839, 1414, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jis_2004", NULL, 2883253, 1424, NUITKA_BYTECODE_FLAG},
    {"encodings.shift_jisx0213", NULL, 2884677, 1424, NUITKA_BYTECODE_FLAG},
    {"encodings.tis_620", NULL, 2886101, 2506, NUITKA_BYTECODE_FLAG},
    {"encodings.undefined", NULL, 2888607, 2090, NUITKA_BYTECODE_FLAG},
    {"encodings.unicode_escape", NULL, 2890697, 1737, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16", NULL, 2892434, 4867, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16_be", NULL, 2897301, 1645, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_16_le", NULL, 2898946, 1645, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32", NULL, 2900591, 4760, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32_be", NULL, 2905351, 1538, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_32_le", NULL, 2906889, 1538, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_7", NULL, 2908427, 1566, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_8", NULL, 2909993, 1625, NUITKA_BYTECODE_FLAG},
    {"encodings.utf_8_sig", NULL, 2911618, 4541, NUITKA_BYTECODE_FLAG},
    {"encodings.uu_codec", NULL, 2916159, 3244, NUITKA_BYTECODE_FLAG},
    {"encodings.zlib_codec", NULL, 2919403, 3104, NUITKA_BYTECODE_FLAG},
    {"enum", NULL, 2922507, 24394, NUITKA_BYTECODE_FLAG},
    {"filecmp", NULL, 2946901, 8422, NUITKA_BYTECODE_FLAG},
    {"fileinput", NULL, 2955323, 13368, NUITKA_BYTECODE_FLAG},
    {"fnmatch", NULL, 2968691, 3327, NUITKA_BYTECODE_FLAG},
    {"formatter", NULL, 2972018, 17540, NUITKA_BYTECODE_FLAG},
    {"fractions", NULL, 2989558, 18734, NUITKA_BYTECODE_FLAG},
    {"ftplib", NULL, 3008292, 27844, NUITKA_BYTECODE_FLAG},
    {"functools", NULL, 3036136, 27896, NUITKA_BYTECODE_FLAG},
    {"genericpath", NULL, 3064032, 3996, NUITKA_BYTECODE_FLAG},
    {"getopt", NULL, 3068028, 6266, NUITKA_BYTECODE_FLAG},
    {"getpass", NULL, 3074294, 4173, NUITKA_BYTECODE_FLAG},
    {"gettext", NULL, 3078467, 17878, NUITKA_BYTECODE_FLAG},
    {"glob", NULL, 3096345, 4338, NUITKA_BYTECODE_FLAG},
    {"gzip", NULL, 3100683, 18186, NUITKA_BYTECODE_FLAG},
    {"hashlib", NULL, 3118869, 6722, NUITKA_BYTECODE_FLAG},
    {"heapq", NULL, 3125591, 14065, NUITKA_BYTECODE_FLAG},
    {"hmac", NULL, 3139656, 6383, NUITKA_BYTECODE_FLAG},
    {"html", NULL, 3146039, 3613, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"html.entities", NULL, 3149652, 50524, NUITKA_BYTECODE_FLAG},
    {"html.parser", NULL, 3200176, 11193, NUITKA_BYTECODE_FLAG},
    {"http", NULL, 3211369, 6059, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"http.client", NULL, 3217428, 34298, NUITKA_BYTECODE_FLAG},
    {"http.cookiejar", NULL, 3251726, 53637, NUITKA_BYTECODE_FLAG},
    {"http.cookies", NULL, 3305363, 15263, NUITKA_BYTECODE_FLAG},
    {"http.server", NULL, 3320626, 34387, NUITKA_BYTECODE_FLAG},
    {"imaplib", NULL, 3355013, 41337, NUITKA_BYTECODE_FLAG},
    {"imghdr", NULL, 3396350, 4099, NUITKA_BYTECODE_FLAG},
    {"imp", NULL, 3400449, 9804, NUITKA_BYTECODE_FLAG},
    {"importlib", NULL, 3410253, 3753, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"importlib._bootstrap", NULL, 3414006, 28600, NUITKA_BYTECODE_FLAG},
    {"importlib._bootstrap_external", NULL, 3442606, 43705, NUITKA_BYTECODE_FLAG},
    {"importlib.abc", NULL, 3486311, 13568, NUITKA_BYTECODE_FLAG},
    {"importlib.machinery", NULL, 3499879, 957, NUITKA_BYTECODE_FLAG},
    {"importlib.metadata", NULL, 3500836, 20835, NUITKA_BYTECODE_FLAG},
    {"importlib.resources", NULL, 3521671, 6475, NUITKA_BYTECODE_FLAG},
    {"importlib.util", NULL, 3528146, 9287, NUITKA_BYTECODE_FLAG},
    {"inspect", NULL, 3537433, 80378, NUITKA_BYTECODE_FLAG},
    {"io", NULL, 3617811, 3449, NUITKA_BYTECODE_FLAG},
    {"ipaddress", NULL, 3621260, 59554, NUITKA_BYTECODE_FLAG},
    {"json", NULL, 3680814, 12603, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"json.decoder", NULL, 3693417, 9839, NUITKA_BYTECODE_FLAG},
    {"json.encoder", NULL, 3703256, 11166, NUITKA_BYTECODE_FLAG},
    {"json.scanner", NULL, 3714422, 1946, NUITKA_BYTECODE_FLAG},
    {"json.tool", NULL, 3716368, 1891, NUITKA_BYTECODE_FLAG},
    {"keyword", NULL, 3718259, 993, NUITKA_BYTECODE_FLAG},
    {"lib2to3", NULL, 3719252, 124, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.btm_matcher", NULL, 3719376, 4872, NUITKA_BYTECODE_FLAG},
    {"lib2to3.btm_utils", NULL, 3724248, 6145, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_base", NULL, 3730393, 6248, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixer_util", NULL, 3736641, 12193, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes", NULL, 3748834, 130, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.fixes.fix_apply", NULL, 3748964, 1668, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_asserts", NULL, 3750632, 1267, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_basestring", NULL, 3751899, 651, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_buffer", NULL, 3752550, 796, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_dict", NULL, 3753346, 3323, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_except", NULL, 3756669, 2806, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exec", NULL, 3759475, 1137, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_execfile", NULL, 3760612, 1685, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_exitfunc", NULL, 3762297, 2294, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_filter", NULL, 3764591, 2432, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_funcattrs", NULL, 3767023, 964, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_future", NULL, 3767987, 772, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_getcwdu", NULL, 3768759, 776, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_has_key", NULL, 3769535, 2927, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_idioms", NULL, 3772462, 3909, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_import", NULL, 3776371, 2775, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports", NULL, 3779146, 4383, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_imports2", NULL, 3783529, 534, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_input", NULL, 3784063, 938, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_intern", NULL, 3785001, 1122, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_isinstance", NULL, 3786123, 1539, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools", NULL, 3787662, 1538, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_itertools_imports", NULL, 3789200, 1566, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_long", NULL, 3790766, 693, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_map", NULL, 3791459, 3085, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_metaclass", NULL, 3794544, 5335, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_methodattrs", NULL, 3799879, 926, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ne", NULL, 3800805, 799, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_next", NULL, 3801604, 3071, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_nonzero", NULL, 3804675, 911, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_numliterals", NULL, 3805586, 1011, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_operator", NULL, 3806597, 4208, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_paren", NULL, 3810805, 1378, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_print", NULL, 3812183, 2331, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raise", NULL, 3814514, 2241, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_raw_input", NULL, 3816755, 783, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_reduce", NULL, 3817538, 1116, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_reload", NULL, 3818654, 1134, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_renames", NULL, 3819788, 1995, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_repr", NULL, 3821783, 833, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_set_literal", NULL, 3822616, 1671, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_standarderror", NULL, 3824287, 708, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_sys_exc", NULL, 3824995, 1399, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_throw", NULL, 3826394, 1796, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_tuple_params", NULL, 3828190, 4579, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_types", NULL, 3832769, 1825, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_unicode", NULL, 3834594, 1537, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_urllib", NULL, 3836131, 5987, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_ws_comma", NULL, 3842118, 1107, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xrange", NULL, 3843225, 2517, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_xreadlines", NULL, 3845742, 1111, NUITKA_BYTECODE_FLAG},
    {"lib2to3.fixes.fix_zip", NULL, 3846853, 1571, NUITKA_BYTECODE_FLAG},
    {"lib2to3.main", NULL, 3848424, 8603, NUITKA_BYTECODE_FLAG},
    {"lib2to3.patcomp", NULL, 3857027, 5634, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2", NULL, 3862661, 160, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"lib2to3.pgen2.driver", NULL, 3862821, 5136, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.grammar", NULL, 3867957, 5660, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.literals", NULL, 3873617, 1549, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.parse", NULL, 3875166, 6493, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.pgen", NULL, 3881659, 9771, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.token", NULL, 3891430, 1882, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pgen2.tokenize", NULL, 3893312, 15267, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pygram", NULL, 3908579, 1255, NUITKA_BYTECODE_FLAG},
    {"lib2to3.pytree", NULL, 3909834, 24337, NUITKA_BYTECODE_FLAG},
    {"lib2to3.refactor", NULL, 3934171, 20435, NUITKA_BYTECODE_FLAG},
    {"linecache", NULL, 3954606, 3834, NUITKA_BYTECODE_FLAG},
    {"locale", NULL, 3958440, 34684, NUITKA_BYTECODE_FLAG},
    {"logging", NULL, 3993124, 64866, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"logging.config", NULL, 4057990, 23219, NUITKA_BYTECODE_FLAG},
    {"logging.handlers", NULL, 4081209, 43151, NUITKA_BYTECODE_FLAG},
    {"lzma", NULL, 4124360, 12013, NUITKA_BYTECODE_FLAG},
    {"mailbox", NULL, 4136373, 60259, NUITKA_BYTECODE_FLAG},
    {"mailcap", NULL, 4196632, 6471, NUITKA_BYTECODE_FLAG},
    {"mimetypes", NULL, 4203103, 15983, NUITKA_BYTECODE_FLAG},
    {"modulefinder", NULL, 4219086, 16110, NUITKA_BYTECODE_FLAG},
    {"msilib", NULL, 4235196, 16004, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"msilib.schema", NULL, 4251200, 52165, NUITKA_BYTECODE_FLAG},
    {"msilib.sequence", NULL, 4303365, 2595, NUITKA_BYTECODE_FLAG},
    {"msilib.text", NULL, 4305960, 8457, NUITKA_BYTECODE_FLAG},
    {"multiprocessing", NULL, 4314417, 657, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.connection", NULL, 4315074, 25229, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.context", NULL, 4340303, 12965, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.dummy", NULL, 4353268, 3898, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"multiprocessing.dummy.connection", NULL, 4357166, 2543, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.forkserver", NULL, 4359709, 8354, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.heap", NULL, 4368063, 7632, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.managers", NULL, 4375695, 41541, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.pool", NULL, 4417236, 25020, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.popen_spawn_win32", NULL, 4442256, 3456, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.process", NULL, 4445712, 10996, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.queues", NULL, 4456708, 9549, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.reduction", NULL, 4466257, 8198, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.resource_sharer", NULL, 4474455, 5251, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.resource_tracker", NULL, 4479706, 5180, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.shared_memory", NULL, 4484886, 14311, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.sharedctypes", NULL, 4499197, 7044, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.spawn", NULL, 4506241, 6691, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.synchronize", NULL, 4512932, 11285, NUITKA_BYTECODE_FLAG},
    {"multiprocessing.util", NULL, 4524217, 11481, NUITKA_BYTECODE_FLAG},
    {"netrc", NULL, 4535698, 3772, NUITKA_BYTECODE_FLAG},
    {"nntplib", NULL, 4539470, 33969, NUITKA_BYTECODE_FLAG},
    {"ntpath", NULL, 4573439, 14652, NUITKA_BYTECODE_FLAG},
    {"nturl2path", NULL, 4588091, 1596, NUITKA_BYTECODE_FLAG},
    {"numbers", NULL, 4589687, 12197, NUITKA_BYTECODE_FLAG},
    {"opcode", NULL, 4601884, 5415, NUITKA_BYTECODE_FLAG},
    {"operator", NULL, 4607299, 13686, NUITKA_BYTECODE_FLAG},
    {"optparse", NULL, 4620985, 48052, NUITKA_BYTECODE_FLAG},
    {"os", NULL, 107200, 31392, NUITKA_BYTECODE_FLAG},
    {"pathlib", NULL, 4669037, 43547, NUITKA_BYTECODE_FLAG},
    {"pdb", NULL, 4712584, 47149, NUITKA_BYTECODE_FLAG},
    {"pickle", NULL, 4759733, 46873, NUITKA_BYTECODE_FLAG},
    {"pickletools", NULL, 4806606, 67199, NUITKA_BYTECODE_FLAG},
    {"pipes", NULL, 4873805, 7790, NUITKA_BYTECODE_FLAG},
    {"pkgutil", NULL, 4881595, 16304, NUITKA_BYTECODE_FLAG},
    {"platform", NULL, 4897899, 24235, NUITKA_BYTECODE_FLAG},
    {"plistlib", NULL, 4922134, 26648, NUITKA_BYTECODE_FLAG},
    {"poplib", NULL, 4948782, 13454, NUITKA_BYTECODE_FLAG},
    {"posixpath", NULL, 4962236, 10423, NUITKA_BYTECODE_FLAG},
    {"pprint", NULL, 4972659, 16276, NUITKA_BYTECODE_FLAG},
    {"profile", NULL, 4988935, 14609, NUITKA_BYTECODE_FLAG},
    {"pstats", NULL, 5003544, 22048, NUITKA_BYTECODE_FLAG},
    {"py_compile", NULL, 5025592, 7389, NUITKA_BYTECODE_FLAG},
    {"pyclbr", NULL, 5032981, 10446, NUITKA_BYTECODE_FLAG},
    {"pydoc", NULL, 5043427, 84082, NUITKA_BYTECODE_FLAG},
    {"pydoc_data", NULL, 5127509, 127, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"pydoc_data.topics", NULL, 5127636, 420495, NUITKA_BYTECODE_FLAG},
    {"queue", NULL, 5548131, 10621, NUITKA_BYTECODE_FLAG},
    {"quopri", NULL, 5558752, 5743, NUITKA_BYTECODE_FLAG},
    {"random", NULL, 5564495, 20103, NUITKA_BYTECODE_FLAG},
    {"re", NULL, 5584598, 14417, NUITKA_BYTECODE_FLAG},
    {"reprlib", NULL, 5599015, 5298, NUITKA_BYTECODE_FLAG},
    {"rlcompleter", NULL, 5604313, 5750, NUITKA_BYTECODE_FLAG},
    {"runpy", NULL, 5610063, 8176, NUITKA_BYTECODE_FLAG},
    {"sched", NULL, 5618239, 6527, NUITKA_BYTECODE_FLAG},
    {"secrets", NULL, 5624766, 2185, NUITKA_BYTECODE_FLAG},
    {"selectors", NULL, 5626951, 16930, NUITKA_BYTECODE_FLAG},
    {"shelve", NULL, 5643881, 9485, NUITKA_BYTECODE_FLAG},
    {"shlex", NULL, 5653366, 7531, NUITKA_BYTECODE_FLAG},
    {"shutil", NULL, 138592, 36564, NUITKA_BYTECODE_FLAG},
    {"signal", NULL, 5660897, 2838, NUITKA_BYTECODE_FLAG},
    {"site", NULL, 175156, 13302, NUITKA_BYTECODE_FLAG},
    {"smtpd", NULL, 5663735, 26458, NUITKA_BYTECODE_FLAG},
    {"smtplib", NULL, 5690193, 35308, NUITKA_BYTECODE_FLAG},
    {"sndhdr", NULL, 5725501, 6970, NUITKA_BYTECODE_FLAG},
    {"socket", NULL, 5732471, 27782, NUITKA_BYTECODE_FLAG},
    {"socketserver", NULL, 5760253, 24002, NUITKA_BYTECODE_FLAG},
    {"sqlite3", NULL, 5784255, 155, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"sqlite3.dbapi2", NULL, 5784410, 2499, NUITKA_BYTECODE_FLAG},
    {"sqlite3.dump", NULL, 5786909, 1924, NUITKA_BYTECODE_FLAG},
    {"sre_compile", NULL, 5788833, 15137, NUITKA_BYTECODE_FLAG},
    {"sre_constants", NULL, 5803970, 6354, NUITKA_BYTECODE_FLAG},
    {"sre_parse", NULL, 5810324, 21642, NUITKA_BYTECODE_FLAG},
    {"ssl", NULL, 5831966, 44591, NUITKA_BYTECODE_FLAG},
    {"stat", NULL, 5876557, 4367, NUITKA_BYTECODE_FLAG},
    {"statistics", NULL, 5880924, 33504, NUITKA_BYTECODE_FLAG},
    {"string", NULL, 5914428, 7295, NUITKA_BYTECODE_FLAG},
    {"stringprep", NULL, 5921723, 11012, NUITKA_BYTECODE_FLAG},
    {"struct", NULL, 5932735, 325, NUITKA_BYTECODE_FLAG},
    {"subprocess", NULL, 5933060, 41935, NUITKA_BYTECODE_FLAG},
    {"sunau", NULL, 5974995, 17075, NUITKA_BYTECODE_FLAG},
    {"symbol", NULL, 5992070, 2399, NUITKA_BYTECODE_FLAG},
    {"symtable", NULL, 5994469, 11071, NUITKA_BYTECODE_FLAG},
    {"sysconfig", NULL, 6005540, 15615, NUITKA_BYTECODE_FLAG},
    {"tabnanny", NULL, 6021155, 7025, NUITKA_BYTECODE_FLAG},
    {"tarfile", NULL, 6028180, 62545, NUITKA_BYTECODE_FLAG},
    {"telnetlib", NULL, 6090725, 18232, NUITKA_BYTECODE_FLAG},
    {"tempfile", NULL, 6108957, 23450, NUITKA_BYTECODE_FLAG},
    {"textwrap", NULL, 6132407, 13514, NUITKA_BYTECODE_FLAG},
    {"this", NULL, 6145921, 1256, NUITKA_BYTECODE_FLAG},
    {"threading", NULL, 188458, 39971, NUITKA_BYTECODE_FLAG},
    {"timeit", NULL, 6147177, 11772, NUITKA_BYTECODE_FLAG},
    {"tkinter", NULL, 228429, 175997, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"tkinter.colorchooser", NULL, 6158949, 1120, NUITKA_BYTECODE_FLAG},
    {"tkinter.commondialog", NULL, 6160069, 1117, NUITKA_BYTECODE_FLAG},
    {"tkinter.constants", NULL, 6161186, 1649, NUITKA_BYTECODE_FLAG},
    {"tkinter.dialog", NULL, 6162835, 1467, NUITKA_BYTECODE_FLAG},
    {"tkinter.dnd", NULL, 6164302, 11257, NUITKA_BYTECODE_FLAG},
    {"tkinter.filedialog", NULL, 404426, 12223, NUITKA_BYTECODE_FLAG},
    {"tkinter.font", NULL, 6175559, 6192, NUITKA_BYTECODE_FLAG},
    {"tkinter.messagebox", NULL, 6181751, 2986, NUITKA_BYTECODE_FLAG},
    {"tkinter.scrolledtext", NULL, 6184737, 2172, NUITKA_BYTECODE_FLAG},
    {"tkinter.simpledialog", NULL, 6186909, 10611, NUITKA_BYTECODE_FLAG},
    {"tkinter.tix", NULL, 6197520, 79513, NUITKA_BYTECODE_FLAG},
    {"tkinter.ttk", NULL, 6277033, 56752, NUITKA_BYTECODE_FLAG},
    {"token", NULL, 6333785, 2480, NUITKA_BYTECODE_FLAG},
    {"tokenize", NULL, 6336265, 17155, NUITKA_BYTECODE_FLAG},
    {"trace", NULL, 6353420, 19991, NUITKA_BYTECODE_FLAG},
    {"traceback", NULL, 6373411, 19884, NUITKA_BYTECODE_FLAG},
    {"tracemalloc", NULL, 6393295, 17358, NUITKA_BYTECODE_FLAG},
    {"turtle", NULL, 6410653, 129935, NUITKA_BYTECODE_FLAG},
    {"types", NULL, 6540588, 9172, NUITKA_BYTECODE_FLAG},
    {"typing", NULL, 6549760, 62415, NUITKA_BYTECODE_FLAG},
    {"unittest", NULL, 6612175, 3121, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"unittest.async_case", NULL, 6615296, 4135, NUITKA_BYTECODE_FLAG},
    {"unittest.case", NULL, 6619431, 50470, NUITKA_BYTECODE_FLAG},
    {"unittest.loader", NULL, 6669901, 14531, NUITKA_BYTECODE_FLAG},
    {"unittest.main", NULL, 6684432, 7529, NUITKA_BYTECODE_FLAG},
    {"unittest.mock", NULL, 6691961, 77361, NUITKA_BYTECODE_FLAG},
    {"unittest.result", NULL, 6769322, 7286, NUITKA_BYTECODE_FLAG},
    {"unittest.runner", NULL, 6776608, 7011, NUITKA_BYTECODE_FLAG},
    {"unittest.signals", NULL, 6783619, 2215, NUITKA_BYTECODE_FLAG},
    {"unittest.suite", NULL, 6785834, 9930, NUITKA_BYTECODE_FLAG},
    {"unittest.util", NULL, 6795764, 4426, NUITKA_BYTECODE_FLAG},
    {"urllib", NULL, 6800190, 123, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"urllib.error", NULL, 6800313, 2804, NUITKA_BYTECODE_FLAG},
    {"urllib.parse", NULL, 6803117, 33927, NUITKA_BYTECODE_FLAG},
    {"urllib.request", NULL, 6837044, 72526, NUITKA_BYTECODE_FLAG},
    {"urllib.response", NULL, 6909570, 3284, NUITKA_BYTECODE_FLAG},
    {"urllib.robotparser", NULL, 6912854, 7322, NUITKA_BYTECODE_FLAG},
    {"uu", NULL, 6920176, 3600, NUITKA_BYTECODE_FLAG},
    {"uuid", NULL, 6923776, 23661, NUITKA_BYTECODE_FLAG},
    {"venv", NULL, 6947437, 14426, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"warnings", NULL, 6961863, 13647, NUITKA_BYTECODE_FLAG},
    {"wave", NULL, 6975510, 18144, NUITKA_BYTECODE_FLAG},
    {"weakref", NULL, 6993654, 19513, NUITKA_BYTECODE_FLAG},
    {"webbrowser", NULL, 7013167, 17023, NUITKA_BYTECODE_FLAG},
    {"wsgiref", NULL, 7030190, 719, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"wsgiref.handlers", NULL, 7030909, 16441, NUITKA_BYTECODE_FLAG},
    {"wsgiref.headers", NULL, 7047350, 7751, NUITKA_BYTECODE_FLAG},
    {"wsgiref.simple_server", NULL, 7055101, 5256, NUITKA_BYTECODE_FLAG},
    {"wsgiref.util", NULL, 7060357, 5407, NUITKA_BYTECODE_FLAG},
    {"wsgiref.validate", NULL, 7065764, 14750, NUITKA_BYTECODE_FLAG},
    {"xdrlib", NULL, 7080514, 8216, NUITKA_BYTECODE_FLAG},
    {"xml", NULL, 7088730, 687, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom", NULL, 7089417, 5525, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.dom.NodeFilter", NULL, 7094942, 962, NUITKA_BYTECODE_FLAG},
    {"xml.dom.domreg", NULL, 7095904, 2845, NUITKA_BYTECODE_FLAG},
    {"xml.dom.expatbuilder", NULL, 7098749, 27336, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minicompat", NULL, 7126085, 2737, NUITKA_BYTECODE_FLAG},
    {"xml.dom.minidom", NULL, 7128822, 55369, NUITKA_BYTECODE_FLAG},
    {"xml.dom.pulldom", NULL, 7184191, 10686, NUITKA_BYTECODE_FLAG},
    {"xml.dom.xmlbuilder", NULL, 7194877, 12489, NUITKA_BYTECODE_FLAG},
    {"xml.etree", NULL, 7207366, 126, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.etree.ElementInclude", NULL, 7207492, 1574, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementPath", NULL, 7209066, 8429, NUITKA_BYTECODE_FLAG},
    {"xml.etree.ElementTree", NULL, 7217495, 55607, NUITKA_BYTECODE_FLAG},
    {"xml.etree.cElementTree", NULL, 7273102, 168, NUITKA_BYTECODE_FLAG},
    {"xml.parsers", NULL, 7273270, 300, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.parsers.expat", NULL, 7273570, 329, NUITKA_BYTECODE_FLAG},
    {"xml.sax", NULL, 7273899, 3214, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xml.sax._exceptions", NULL, 7277113, 5439, NUITKA_BYTECODE_FLAG},
    {"xml.sax.expatreader", NULL, 7282552, 12490, NUITKA_BYTECODE_FLAG},
    {"xml.sax.handler", NULL, 7295042, 12417, NUITKA_BYTECODE_FLAG},
    {"xml.sax.saxutils", NULL, 7307459, 12914, NUITKA_BYTECODE_FLAG},
    {"xml.sax.xmlreader", NULL, 7320373, 16839, NUITKA_BYTECODE_FLAG},
    {"xmlrpc", NULL, 7337212, 123, NUITKA_BYTECODE_FLAG | NUITKA_PACKAGE_FLAG},
    {"xmlrpc.client", NULL, 7337335, 34534, NUITKA_BYTECODE_FLAG},
    {"xmlrpc.server", NULL, 7371869, 29403, NUITKA_BYTECODE_FLAG},
    {"zipapp", NULL, 7401272, 5849, NUITKA_BYTECODE_FLAG},
    {"zipfile", NULL, 7407121, 58452, NUITKA_BYTECODE_FLAG},
    {NULL, NULL, 0, 0, 0}
};

void setupMetaPathBasedLoader(void) {
    static bool init_done = false;

    if (init_done == false) {
        registerMetaPathBasedUnfreezer(meta_path_loader_entries);
        init_done = true;
    }
}
