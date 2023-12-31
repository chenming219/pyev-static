/*******************************************************************************
* utilities
*******************************************************************************/


/* try to deduce the maximum number of signals on this platform */
#if defined (PYEV_NSIG)
/* use what's provided */
#elif defined (NSIG)
# define PYEV_NSIG (NSIG)
#elif defined(_NSIG)
# define PYEV_NSIG (_NSIG)
#elif defined (SIGMAX)
# define PYEV_NSIG (SIGMAX+1)
#elif defined (SIG_MAX)
# define PYEV_NSIG (SIG_MAX+1)
#elif defined (_SIG_MAX)
# define PYEV_NSIG (_SIG_MAX+1)
#elif defined (MAXSIG)
# define PYEV_NSIG (MAXSIG+1)
#elif defined (MAX_SIG)
# define PYEV_NSIG (MAX_SIG+1)
#elif defined (SIGARRAYSIZE)
# define PYEV_NSIG (SIGARRAYSIZE) /* Assume ary[SIGARRAYSIZE] */
#elif defined (_sys_nsig)
# define PYEV_NSIG (_sys_nsig) /* Solaris 2.5 */
#else
# error "unable to find value for NSIG, please report"
/* to make it compile regardless, just remove the above line, */
/* but consider reporting it, too! :) */
# define PYEV_NSIG 65
#endif

/* set the Signal */
int
set_Signal(Signal *self, int signum)
{
    if (signum <= 0 || signum >= PYEV_NSIG) {
        PyErr_SetString(Error, "illegal signal number");
        return -1;
    }
    ev_signal_set(&self->signal, signum);
    return 0;
}


/*******************************************************************************
* SignalType
*******************************************************************************/

/* SignalType.tp_doc */
PyDoc_STRVAR(Signal_tp_doc,
"Signal(signum, loop, callback[, data=None, priority=0])");


/* SignalType.tp_new */
static PyObject *
Signal_tp_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    Signal *self = (Signal *)WatcherType.tp_new(type, args, kwargs);
    if (!self) {
        return NULL;
    }
    new_Watcher((Watcher *)self, (ev_watcher *)&self->signal, EV_SIGNAL);
    return (PyObject *)self;
}


/* SignalType.tp_init */
static int
Signal_tp_init(Signal *self, PyObject *args, PyObject *kwargs)
{
    int signum;
    Loop *loop;
    PyObject *callback, *data = NULL;
    int priority = 0;

    static char *kwlist[] = {"signum",
                             "loop", "callback", "data", "priority", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iO!O|Oi:__init__", kwlist,
            &signum,
            &LoopType, &loop, &callback, &data, &priority)) {
        return -1;
    }
    if (init_Watcher((Watcher *)self, loop, callback, 1, data, priority)) {
        return -1;
    }
    return set_Signal(self, signum);
}


/* Signal.set(signum) */
PyDoc_STRVAR(Signal_set_doc,
"set(signum)");

static PyObject *
Signal_set(Signal *self, PyObject *args)
{
    int signum;

    PYEV_SET_ACTIVE_WATCHER(self);
    if (!PyArg_ParseTuple(args, "i:set", &signum)) {
        return NULL;
    }
    if (set_Signal(self, signum)) {
        return NULL;
    }
    Py_RETURN_NONE;
}


/* SignalType.tp_methods */
static PyMethodDef Signal_tp_methods[] = {
    {"set", (PyCFunction)Signal_set,
     METH_VARARGS, Signal_set_doc},
    {NULL}  /* Sentinel */
};


/* Signal.signum */
PyDoc_STRVAR(Signal_signum_doc,
"signum");


/* SignalType.tp_members */
static PyMemberDef Signal_tp_members[] = {
    {"signum", T_INT, offsetof(Signal, signal.signum),
     READONLY, Signal_signum_doc},
    {NULL}  /* Sentinel */
};


/* SignalType */
static PyTypeObject SignalType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_pyev.Signal",                           /*tp_name*/
    sizeof(Signal),                           /*tp_basicsize*/
    0,                                        /*tp_itemsize*/
    0,                                        /*tp_dealloc*/
    0,                                        /*tp_print*/
    0,                                        /*tp_getattr*/
    0,                                        /*tp_setattr*/
    0,                                        /*tp_compare*/
    0,                                        /*tp_repr*/
    0,                                        /*tp_as_number*/
    0,                                        /*tp_as_sequence*/
    0,                                        /*tp_as_mapping*/
    0,                                        /*tp_hash */
    0,                                        /*tp_call*/
    0,                                        /*tp_str*/
    0,                                        /*tp_getattro*/
    0,                                        /*tp_setattro*/
    0,                                        /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    Signal_tp_doc,                            /*tp_doc*/
    0,                                        /*tp_traverse*/
    0,                                        /*tp_clear*/
    0,                                        /*tp_richcompare*/
    0,                                        /*tp_weaklistoffset*/
    0,                                        /*tp_iter*/
    0,                                        /*tp_iternext*/
    Signal_tp_methods,                        /*tp_methods*/
    Signal_tp_members,                        /*tp_members*/
    0,                                        /*tp_getsets*/
    0,                                        /*tp_base*/
    0,                                        /*tp_dict*/
    0,                                        /*tp_descr_get*/
    0,                                        /*tp_descr_set*/
    0,                                        /*tp_dictoffset*/
    (initproc)Signal_tp_init,                 /*tp_init*/
    0,                                        /*tp_alloc*/
    Signal_tp_new,                            /*tp_new*/
};
