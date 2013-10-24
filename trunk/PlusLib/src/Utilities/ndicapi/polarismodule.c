#include "polaris.h"
#include "polaris_math.h"
#include "Python.h"

#ifdef _WIN32
  #define POLARIS_EXPORT __declspec( dllexport )
#else
  #define POLARIS_EXPORT
#endif

/*=================================================================
  polaris structure
*/

typedef struct {
  PyObject_HEAD
  polaris *pl_polaris;
} PyPolaris;

static void PyPolaris_PyDelete(PyObject *self)
{
  polaris *pol;

  pol = ((PyPolaris *)self)->pl_polaris;
  plClose(pol);
  PyMem_DEL(self);
}

static char *PyPolaris_PrintHelper(PyObject *self, char *space)
{
  polaris *pol;

  pol = ((PyPolaris *)self)->pl_polaris;

  sprintf(space, "<polaris object %p, %s>", (void *)pol, plGetDeviceName(pol));

  return space;
}

static int PyPolaris_PyPrint(PyObject *self, FILE *fp, int dummy)
{
  char space[256];

  PyPolaris_PrintHelper(self, space);
  fprintf(fp,"%s",space);
  return 0;
}

static PyObject *PyPolaris_PyString(PyObject *self)
{
  char space[256];

  PyPolaris_PrintHelper(self, space);
  return PyString_FromString(space);
}

static PyObject *PyPolaris_PyRepr(PyObject *self)
{
  char space[256];

  PyPolaris_PrintHelper(self, space);
  return PyString_FromString(space);
}

static PyObject *PyPolaris_PyGetAttr(PyObject *self, char *name)
{
  polaris *pol;

  pol = ((PyPolaris *)self)->pl_polaris;
  PyErr_SetString(PyExc_AttributeError, name);
  return NULL;
}

static PyTypeObject PyPolarisType = {
  PyObject_HEAD_INIT(NULL) /* (&PyType_Type) */
  0,
  "polaris",                             /* tp_name */
  sizeof(PyPolaris),                     /* tp_basicsize */
  0,                                     /* tp_itemsize */
  (destructor)PyPolaris_PyDelete,        /* tp_dealloc */
  (printfunc)PyPolaris_PyPrint,          /* tp_print */
  (getattrfunc)PyPolaris_PyGetAttr,      /* tp_getattr */
  0,                                     /* tp_setattr */
  (cmpfunc)0,                            /* tp_compare */
  (reprfunc)PyPolaris_PyRepr,            /* tp_repr */
  0,                                     /* tp_as_number  */
  0,                                     /* tp_as_sequence */
  0,                                     /* tp_as_mapping */
  (hashfunc)0,                           /* tp_hash */
  (ternaryfunc)0,                        /* tp_call */
  (reprfunc)PyPolaris_PyString,          /* tp_string */
  (getattrofunc)0,                       /* tp_getattro */
  (setattrofunc)0,                       /* tp_setattro */
  0,                                     /* tp_as_buffer */
  0,                                     /* tp_flags */
  "polaris: a POLARIS tracking system"   /* tp_doc */
};

int PyPolaris_Check(PyObject *obj)
{
  return (obj->ob_type == &PyPolarisType);
}

/*=================================================================
  bitfield type: this code is a ripoff of the python integer type
  that prints itself as a hexidecimal value
*/

typedef struct {
  PyObject_HEAD
  unsigned long ob_ival;
} PyPLBitfieldObject;

PyObject *
PyPLBitfield_FromUnsignedLong(unsigned long ival);

static void
bitfield_dealloc(v)
  PyIntObject *v;
{
  PyMem_DEL(v);
}

static int
bitfield_print(v, fp, flags)
  PyIntObject *v;
  FILE *fp;
  int flags; /* Not used but required by interface */
{
  fprintf(fp, "0x%lX", v->ob_ival);
  return 0;
}

static PyObject *
bitfield_repr(v)
  PyIntObject *v;
{
  char buf[20];
  sprintf(buf, "0x%lX", v->ob_ival);
  return PyString_FromString(buf);
}

static int
bitfield_compare(v, w)
  PyIntObject *v, *w;
{
  register unsigned long i = v->ob_ival;
  register unsigned long j = w->ob_ival;
  return (i < j) ? -1 : (i > j) ? 1 : 0;
}

static int
bitfield_nonzero(v)
  PyIntObject *v;
{
  return v->ob_ival != 0;
}

static PyObject *
bitfield_invert(v)
  PyIntObject *v;
{
  return PyPLBitfield_FromUnsignedLong(~v->ob_ival);
}

static PyObject *
bitfield_lshift(v, w)
  PyIntObject *v;
  PyIntObject *w;
{
  register unsigned long a, b;
  a = v->ob_ival;
  b = w->ob_ival;
  if (b < 0) {
    PyErr_SetString(PyExc_ValueError, "negative shift count");
    return NULL;
  }
  if (a == 0 || b == 0) {
    Py_INCREF(v);
    return (PyObject *) v;
  }
  if (b >= 8*sizeof(long)) {
    return PyPLBitfield_FromUnsignedLong(0L);
  }
  a = (unsigned long)a << b;
  return PyPLBitfield_FromUnsignedLong(a);
}

static PyObject *
bitfield_rshift(v, w)
  PyIntObject *v;
  PyIntObject *w;
{
  register unsigned long a, b;
  a = v->ob_ival;
  b = w->ob_ival;
  if (b < 0) {
    PyErr_SetString(PyExc_ValueError, "negative shift count");
    return NULL;
  }
  if (a == 0 || b == 0) {
    Py_INCREF(v);
    return (PyObject *) v;
  }
  if (b >= 8*sizeof(long)) {
    if (a < 0)
      a = -1;
    else
      a = 0;
  }
  else {
    if (a < 0)
      a = ~( ~(unsigned long)a >> b );
    else
      a = (unsigned long)a >> b;
  }
  return PyPLBitfield_FromUnsignedLong(a);
}

static PyObject *
bitfield_and(v, w)
  PyIntObject *v;
  PyIntObject *w;
{
  register unsigned long a, b;
  a = v->ob_ival;
  b = w->ob_ival;
  return PyPLBitfield_FromUnsignedLong(a & b);
}

static PyObject *
bitfield_xor(v, w)
  PyIntObject *v;
  PyIntObject *w;
{
  register unsigned long a, b;
  a = v->ob_ival;
  b = w->ob_ival;
  return PyPLBitfield_FromUnsignedLong(a ^ b);
}

static PyObject *
bitfield_or(v, w)
  PyIntObject *v;
  PyIntObject *w;
{
  register unsigned long a, b;
  a = v->ob_ival;
  b = w->ob_ival;
  return PyPLBitfield_FromUnsignedLong(a | b);
}

static int
bitfield_coerce(pv, pw)
  PyObject **pv;
  PyObject **pw;
{
  if (PyInt_Check(*pw)) {
    *pw = PyPLBitfield_FromUnsignedLong(PyInt_AsLong(*pw));
    Py_INCREF(*pv);
    return 0;
  }
  else if (PyLong_Check(*pw)) {
    *pw = PyPLBitfield_FromUnsignedLong(PyLong_AsLong(*pw));
    Py_INCREF(*pv);
    return 0;
  }
  return 1; /* Can't do it */
}

static PyObject *
bitfield_int(v)
  PyIntObject *v;
{
  return PyInt_FromLong((v -> ob_ival));
}

static PyObject *
bitfield_long(v)
  PyIntObject *v;
{
  return PyLong_FromLong((v -> ob_ival));
}

static PyObject *
bitfield_float(v)
  PyIntObject *v;
{
  return PyFloat_FromDouble((double)(v -> ob_ival));
}

static PyObject *
bitfield_oct(v)
  PyIntObject *v;
{
  char buf[100];
  long x = v -> ob_ival;
  if (x == 0)
    strcpy(buf, "0");
  else
    sprintf(buf, "0%lo", x);
  return PyString_FromString(buf);
}

static PyObject *
bitfield_hex(v)
  PyIntObject *v;
{
  char buf[100];
  long x = v -> ob_ival;
  sprintf(buf, "0x%lx", x);
  return PyString_FromString(buf);
}

static PyNumberMethods bitfield_as_number = {
  (binaryfunc)0, /*nb_add*/
  (binaryfunc)0, /*nb_subtract*/
  (binaryfunc)0, /*nb_multiply*/
  (binaryfunc)0, /*nb_divide*/
  (binaryfunc)0, /*nb_remainder*/
  (binaryfunc)0, /*nb_divmod*/
  (ternaryfunc)0, /*nb_power*/
  (unaryfunc)0, /*nb_negative*/
  (unaryfunc)0, /*nb_positive*/
  (unaryfunc)0, /*nb_absolute*/
  (inquiry)bitfield_nonzero, /*nb_nonzero*/
  (unaryfunc)bitfield_invert, /*nb_invert*/
  (binaryfunc)bitfield_lshift, /*nb_lshift*/
  (binaryfunc)bitfield_rshift, /*nb_rshift*/
  (binaryfunc)bitfield_and, /*nb_and*/
  (binaryfunc)bitfield_xor, /*nb_xor*/
  (binaryfunc)bitfield_or, /*nb_or*/
  (coercion)bitfield_coerce, /*nb_coerce*/
  (unaryfunc)bitfield_int, /*nb_int*/
  (unaryfunc)bitfield_long, /*nb_long*/
  (unaryfunc)bitfield_float, /*nb_float*/
  (unaryfunc)bitfield_oct, /*nb_oct*/
  (unaryfunc)bitfield_hex, /*nb_hex*/
};

PyTypeObject PyPLBitfield_Type = {
  PyObject_HEAD_INIT(0)  /* (&PyType_Type) */
  0,
  "bitfield",
  sizeof(PyIntObject),
  0,
  (destructor)bitfield_dealloc, /*tp_dealloc*/
  (printfunc)bitfield_print, /*tp_print*/
  0,    /*tp_getattr*/
  0,    /*tp_setattr*/
  (cmpfunc)bitfield_compare, /*tp_compare*/
  (reprfunc)bitfield_repr, /*tp_repr*/
  &bitfield_as_number,  /*tp_as_number*/
  0,    /*tp_as_sequence*/
  0,    /*tp_as_mapping*/
  (hashfunc)0, /*tp_hash*/
};

PyObject *PyPLBitfield_FromUnsignedLong(unsigned long ival)
{
  PyPLBitfieldObject *v;
  v = PyObject_NEW(PyPLBitfieldObject, &PyPLBitfield_Type);
  
  v->ob_ival = ival;
  return (PyObject *) v;
}

/*=================================================================
  helper functions
*/

static PyObject *_plErrorHelper(int errnum, PyObject *rval)
{
  char errtext[512];

  if (errnum) {
    Py_DECREF(rval);
    if ((errnum & 0xff) == errnum) {
      sprintf(errtext, "POLARIS %#4.2x: %s", errnum, plErrorString(errnum));
    }
    else {
      sprintf(errtext, "Error %#6.4x: %s", errnum, plErrorString(errnum));
    }
    PyErr_SetString(PyExc_IOError, errtext);
    return NULL;
  }

  return rval;
} 

static int _plConverter(PyObject *obj, polaris **polptr)
{
  if (PyPolaris_Check(obj)) {
    *polptr = ((PyPolaris *)obj)->pl_polaris;
  }
  else {
    PyErr_SetString(PyExc_ValueError, "expected a polaris object.");
    return 0;
  }
  return 1;
}

static PyObject *_PyString_FromChar(char value)
{
  return PyString_FromStringAndSize(&value, 1);
}

/*=================================================================
  methods
*/

static PyObject *Py_plHexToUnsignedLong(PyObject *module, PyObject *args)
{
  char *cp;
  int n;
  unsigned long result;

  if (PyArg_ParseTuple(args, "si:plHexToUnsignedLong", &cp, &n)) {
    result = plHexToUnsignedLong(cp, n);
    return PyPLBitfield_FromUnsignedLong(result);
  }

  return NULL;
}

static PyObject *Py_plSignedToLong(PyObject *module, PyObject *args)
{
  char *cp;
  int n;
  long result;

  if (PyArg_ParseTuple(args, "si:plSignedToLong", &cp, &n)) {
    result = plSignedToLong(cp, n);
    return PyInt_FromLong(result);
  }

  return NULL;
}

static PyObject *Py_plHexEncode(PyObject *module, PyObject *args)
{
  char *result;
  void *data;
  char *cp;
  int m, n;
  PyObject *obj;

  if (PyArg_ParseTuple(args, "s#i:plHexEncode", &data, &m, &n)) {
    cp = (char *)malloc(2*n);
    if (m < n) {
      PyErr_SetString(PyExc_ValueError, "data string is not long enough");
      free(cp);
      return NULL;
    }
    result = plHexEncode(cp, data, n);
    obj = PyString_FromStringAndSize(result, 2*n);
    free(cp);
    return obj;
  }

  return NULL;
}

static PyObject *Py_plHexDecode(PyObject *module, PyObject *args)
{
  void *result;
  void *data;
  char *cp;
  int m, n;
  PyObject *obj;

  if (PyArg_ParseTuple(args, "s#i:plHexDecode", &cp, &m, &n)) {
    data = malloc(n);
    if (m < 2*n) {
      PyErr_SetString(PyExc_ValueError, "encoded string is not long enough");
      free(data);
      return NULL;
    }
    result = plHexDecode(data, cp, n);
    obj = PyString_FromStringAndSize((char *)result, n);
    free(data);
    return obj;
  }

  return NULL;
}

static PyObject *Py_plGetError(PyObject *module, PyObject *args)
{
  polaris *pol;
  int result;

  if (PyArg_ParseTuple(args, "O&:plGetError", &_plConverter, &pol)) {
    result = plGetError(pol);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plErrorString(PyObject *module, PyObject *args)
{
  int errnum;
  char *result;

  if (PyArg_ParseTuple(args, "i:plErrorString", &errnum)) {
    result = plErrorString(errnum);
    return PyString_FromString(result);
  }

  return NULL;
}

static PyObject *Py_plDeviceName(PyObject *module, PyObject *args)
{
  int n;
  char *result;

  if (PyArg_ParseTuple(args, "i:plDeviceName", &n)) {
    result = plDeviceName(n);
    if (result) {
      return PyString_FromString(result);
    }
    else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  }

  return NULL;
}

static PyObject *Py_plProbe(PyObject *module, PyObject *args)
{
  char *device;
  int result;

  if (PyArg_ParseTuple(args, "s:plProbe", &device)) {
    result = plProbe(device);
    return PyPLBitfield_FromUnsignedLong(result);
  }

  return NULL;
}

static PyObject *Py_plOpen(PyObject *module, PyObject *args)
{
  polaris *pol;
  char *device;
  PyPolaris *self;

  if (PyArg_ParseTuple(args, "s:plOpen", &device)) {
    pol = plOpen(device);
    if (pol == NULL) {
      Py_INCREF(Py_None);
      return Py_None;
    }
    self = PyObject_NEW(PyPolaris, &PyPolarisType);
    self->pl_polaris = pol;
    return (PyObject *)self;
  }

  return NULL;
}

static PyObject *Py_plGetDeviceName(PyObject *module, PyObject *args)
{
  polaris *pol;
  char *result;

  if (PyArg_ParseTuple(args, "O&:plGetDeviceName", &_plConverter, &pol)) {
    result = plGetDeviceName(pol);
    if (result == NULL) {
      Py_INCREF(Py_None);
      return Py_None;
    }
    return PyString_FromString(result);
  }
  
  return NULL;
}

static PyObject *Py_plClose(PyObject *module, PyObject *args)
{
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&:plClose", &_plConverter, &pol)) {
    plClose(pol);
    Py_INCREF(Py_None);
    return Py_None;
  }
  
  return NULL;
}

static PyObject *Py_plSetThreadMode(PyObject *module, PyObject *args)
{
  polaris *pol;
  int mode;

  if (PyArg_ParseTuple(args, "O&i:plSetThreadMode", &_plConverter, &pol,
                       &mode)) {
    plSetThreadMode(pol, mode);
    Py_INCREF(Py_None);
    return Py_None;
  }
  
  return NULL;
}

static PyObject *Py_plCommand(PyObject *module, PyObject *args)
{
  int n;
  polaris *pol;
  char *format;
  char *result;
  PyObject *initial;
  PyObject *remainder;
  PyObject *newstring = NULL;
  PyObject *obj;

  if ((n = PySequence_Length(args)) < 2) {
    PyErr_SetString(PyExc_TypeError,
                    "plCommand requires at least 2 arguments");
    return NULL;
  }

  remainder = PySequence_GetSlice(args,2,n);
  initial = PySequence_GetSlice(args,0,2); 

  if (!PyArg_ParseTuple(initial, "O&z:plCommand",
      &_plConverter, &pol, &format)) {
    Py_DECREF(initial);
    Py_DECREF(remainder);
    return NULL;
  }

  if (format != NULL) {
    obj = PySequence_GetItem(args, 1);
    newstring = PyString_Format(obj, remainder);
    Py_DECREF(obj);
    Py_DECREF(initial);
    Py_DECREF(remainder);
  
    if (newstring == NULL) {
      return NULL;
    }

    result = plCommand(pol, "%s", PyString_AsString(newstring));
  }
  else {
    result = plCommand(pol, NULL);
  }

  if (newstring != NULL) {
    Py_DECREF(newstring);
  }

  if (result == NULL) {
    Py_INCREF(Py_None);
    obj = Py_None;
  }
  else {
    obj = PyString_FromString(result);
  }

  return _plErrorHelper(plGetError(pol), obj);
}

static PyObject *Py_plCommand2(PyObject *module, char *format, PyObject *args)
{
  int i, n;
  PyObject *newargs;
  PyObject *obj;

  if ((n = PySequence_Length(args)) < 1) {
    PyErr_SetString(PyExc_TypeError,
                    "plCommand requires at least 2 arguments");
    return NULL;
  }

  newargs = PyTuple_New(n + 1);
  obj = PySequence_GetItem(args, 0);
  Py_INCREF(obj);
  PyTuple_SET_ITEM(newargs, 0, obj);
  
  if (format != NULL) {
    obj = PyString_FromString(format);
  }
  else {
    Py_INCREF(Py_None);
    obj = Py_None;
  }
  PyTuple_SET_ITEM(newargs, 1, obj);
   
  for (i = 1; i < n; i++) {
    obj = PySequence_GetItem(args, i);
    Py_INCREF(obj);
    PyTuple_SET_ITEM(newargs, i+1, obj);
  }
    
  obj = Py_plCommand(module, newargs);

  Py_DECREF(newargs);

  return obj;
} 

#define PyCommandMacro(name,format) \
  static PyObject *Py_##name(PyObject *module, PyObject *args) \
  { \
    return Py_plCommand2(module, format, args); \
  }

PyCommandMacro(plRESET,NULL)
PyCommandMacro(plINIT,"INIT:")
PyCommandMacro(plCOMM,"COMM:%d%03d%d")
PyCommandMacro(plPVWR,"PVWR:%c%04X%.128s")
PyCommandMacro(plPVCLR,"PVCLR:%c")
PyCommandMacro(plPINIT,"PINIT:%c")
PyCommandMacro(plPENA,"PENA:%c%c")
PyCommandMacro(plPDIS,"PDIS:%c")
PyCommandMacro(plTSTART,"TSTART:")
PyCommandMacro(plTSTOP,"TSTOP:")
PyCommandMacro(plGX,"GX:%04X")
PyCommandMacro(plLED,"LED:%c%d%c")
PyCommandMacro(plBEEP,"BEEP:%i")
PyCommandMacro(plVER,"VER:%d")
PyCommandMacro(plVSEL,"VSEL:%d")
PyCommandMacro(plSFLIST,"SFLIST:%02X")
PyCommandMacro(plPSTAT,"PSTAT:%04X")
PyCommandMacro(plSSTAT,"SSTAT:%04X")

PyCommandMacro(plPPRD,"PPRD:%c%04X")
PyCommandMacro(plPPWR,"PPWR:%c%04X%.128s")
PyCommandMacro(plPURD,"PURD:%c%04X")
PyCommandMacro(plPUWR,"PPWR:%c%04X%.128s")
PyCommandMacro(plPSEL,"PSEL:%c%s")
PyCommandMacro(plPSRCH,"PSRCH:%c")
PyCommandMacro(plPVTIP,"PVTIP:%c%d%d")
PyCommandMacro(plTCTST,"TCTST:%c")
PyCommandMacro(plTTCFG,"TTCFG:%c")

PyCommandMacro(plDSTART,"DSTART:")
PyCommandMacro(plDSTOP,"DSTOP:")
PyCommandMacro(plIRINIT,"IRINIT:")
PyCommandMacro(plIRCHK,"IRCHK:%04X")
PyCommandMacro(plIRED,"IRED:%c%08X")
PyCommandMacro(pl3D,"3D:%c%d")


static PyObject *Py_plPVWRFromFile(PyObject *module, PyObject *args)
{
  char port;
  int result;
  char *filename;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&cs:plPVWRFromFile", 
      &_plConverter, &pol, &port, &filename)) {
    result = plPVWRFromFile(pol, port, filename);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXTransform(PyObject *module, PyObject *args)
{
  char port;
  int result;
  double transform[8];
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetGXTransform", 
      &_plConverter, &pol, &port)) {
    result = plGetGXTransform(pol, port, transform);

    if (result == PL_MISSING) {
      return PyString_FromString("MISSING");
    }
    else if (result == PL_DISABLED) {
      return PyString_FromString("DISABLED");
    }

    return Py_BuildValue("(dddddddd)", transform[0], transform[1],
       transform[2], transform[3], transform[4],
       transform[5], transform[6], transform[7]);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXPortStatus(PyObject *module, PyObject *args)
{
  char port;
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetGXPortStatus", 
      &_plConverter, &pol, &port)) {
    result = plGetGXPortStatus(pol, port);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXSystemStatus(PyObject *module, PyObject *args)
{
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&:plGetGXSystemStatus", 
      &_plConverter, &pol)) {
    result = plGetGXSystemStatus(pol);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXToolInfo(PyObject *module, PyObject *args)
{
  char port;
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetGXToolInfo", 
      &_plConverter, &pol, &port)) {
    result = plGetGXToolInfo(pol, port);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXMarkerInfo(PyObject *module, PyObject *args)
{
  char port;
  char marker;
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&ci:plGetGXMarkerInfo", 
      &_plConverter, &pol, &port, &marker)) {
    result = plGetGXMarkerInfo(pol, port, marker);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXSingleStray(PyObject *module, PyObject *args)
{
  char port;
  int result;
  double coord[3];
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetGXSingleStray", 
      &_plConverter, &pol, &port)) {
    result = plGetGXSingleStray(pol, port, coord);

    if (result == PL_MISSING) {
      return PyString_FromString("MISSING");
    }
    else if (result == PL_DISABLED) {
      return PyString_FromString("DISABLED");
    }

    return Py_BuildValue("(ddd)", coord[0], coord[1], coord[2]);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXFrame(PyObject *module, PyObject *args)
{
  char port;
  unsigned long result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetGXFrame", 
      &_plConverter, &pol, &port)) {
    result = plGetGXFrame(pol, port);
    return PyLong_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXNumberOfPassiveStrays(PyObject *module,
            PyObject *args)
{
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&:plGetGXNumberOfPassiveStrays", 
      &_plConverter, &pol)) {
    result = plGetGXNumberOfPassiveStrays(pol);
    return PyInt_FromLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetGXPassiveStray(PyObject *module, PyObject *args)
{
  int result;
  int i;
  double coord[3];
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&i:plGetGXPassiveStray", 
      &_plConverter, &pol, &i)) {
    result = plGetGXPassiveStray(pol, i, coord);

    if (result == PL_MISSING) {
      return PyString_FromString("MISSING");
    }
    else if (result == PL_DISABLED) {
      return PyString_FromString("DISABLED");
    }

    return Py_BuildValue("(ddd)", coord[0], coord[1], coord[2]);
  }
  
  return NULL;
}

static PyObject *Py_plGetPSTATPortStatus(PyObject *module, PyObject *args)
{
  char port;
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetPSTATPortStatus", 
      &_plConverter, &pol, &port)) {
    result = plGetPSTATPortStatus(pol, port);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetPSTATToolInfo(PyObject *module, PyObject *args)
{
  char port;
  char result[30];
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetPSTATToolInfo", 
      &_plConverter, &pol, &port)) {
    if (plGetPSTATToolInfo(pol, port, result) != PL_UNOCCUPIED) {
      return PyString_FromStringAndSize(result, 30);
    }
    return PyString_FromString("UNOCCUPIED");
  }
  
  return NULL;
}

static PyObject *Py_plGetPSTATCurrentTest(PyObject *module, PyObject *args)
{
  char port;
  unsigned long result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetPSTATCurrentTest", 
      &_plConverter, &pol, &port)) {
    result = plGetPSTATCurrentTest(pol, port);
    return PyLong_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetPSTATPartNumber(PyObject *module, PyObject *args)
{
  char port;
  char result[20];
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetPSTATPartNumber", 
      &_plConverter, &pol, &port)) {
    if (plGetPSTATPartNumber(pol, port, result) != PL_OKAY) {
      Py_INCREF(Py_None);
      return Py_None;
    }
    return PyString_FromStringAndSize(result, 20);
  }
  
  return NULL;
}

static PyObject *Py_plGetPSTATAccessories(PyObject *module, PyObject *args)
{
  char port;
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetPSTATAccessories", 
      &_plConverter, &pol, &port)) {
    result = plGetPSTATAccessories(pol, port);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetPSTATMarkerType(PyObject *module, PyObject *args)
{
  char port;
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&c:plGetPSTATMarkerType", 
      &_plConverter, &pol, &port)) {
    result = plGetPSTATMarkerType(pol, port);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetSSTATControl(PyObject *module, PyObject *args)
{
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&:plGetSSTATControl", 
      &_plConverter, &pol)) {
    result = plGetSSTATControl(pol);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetSSTATSensors(PyObject *module, PyObject *args)
{
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&:plGetSSTATSensors", 
      &_plConverter, &pol)) {
    result = plGetSSTATSensors(pol);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetSSTATTIU(PyObject *module, PyObject *args)
{
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&:plGetSSTATTIU", 
      &_plConverter, &pol)) {
    result = plGetSSTATTIU(pol);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetIRCHKDetected(PyObject *module, PyObject *args)
{
  int result;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&:plGetIRCHKDetected", 
      &_plConverter, &pol)) {
    result = plGetIRCHKDetected(pol);
    return PyPLBitfield_FromUnsignedLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetIRCHKNumberOfSources(PyObject *module, PyObject *args)
{
  int result;
  int side;
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&i:plGetIRCHKNumberOfSources", 
      &_plConverter, &pol, &side)) {
    result = plGetIRCHKNumberOfSources(pol, side);
    return PyInt_FromLong(result);
  }
  
  return NULL;
}

static PyObject *Py_plGetIRCHKSourceXY(PyObject *module, PyObject *args)
{
  int result;
  int side;
  int i;
  double xy[2];
  polaris *pol;

  if (PyArg_ParseTuple(args, "O&ii:plGetIRCHKSourceXY", 
      &_plConverter, &pol, &side, &i)) {
    result = plGetIRCHKSourceXY(pol, side, i, xy);
    if (result != PL_OKAY) {
      Py_INCREF(Py_None);
      return Py_None;
    }
    return Py_BuildValue("(ff)", xy[0], xy[1]);
  }
  
  return NULL;
}

static PyObject *Py_plRelativeTransform(PyObject *module, PyObject *args)
{
  double a[8];
  double b[8];

  if (PyArg_ParseTuple(args, "(dddddddd)(dddddddd):plRelativeTransform", 
           &a[0], &a[1], &a[2], &a[3],
           &a[4], &a[5], &a[6], &a[7],
           &b[0], &b[1], &b[2], &b[3],
           &b[4], &b[5], &b[6], &b[7])) {
    plRelativeTransform(a, b, a);

    return Py_BuildValue("(dddddddd)", a[0], a[1], a[2], a[3],
                                       a[4], a[5], a[6], a[7]);
  }
  
  return NULL;
}

static PyObject *Py_plTransformToMatrixd(PyObject *module, PyObject *args)
{
  double a[8];
  double c[16];

  if (PyArg_ParseTuple(args, "(dddddddd):plTransformToMatrixd", 
           &a[0], &a[1], &a[2], &a[3],
           &a[4], &a[5], &a[6], &a[7])) {
    plTransformToMatrixd(a, c);

    return Py_BuildValue("(dddddddddddddddd)",
       c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7],
       c[8], c[9], c[10], c[11], c[12], c[13], c[14], c[15]);
  }
  
  return NULL;
}

static PyObject *Py_plTransformToMatrixf(PyObject *module, PyObject *args)
{
  double a[8];
  float c[16];

  if (PyArg_ParseTuple(args, "(dddddddd):plTransformToMatrixf", 
           &a[0], &a[1], &a[2], &a[3],
           &a[4], &a[5], &a[6], &a[7])) {
    plTransformToMatrixf(a, c);

    return Py_BuildValue("(ffffffffffffffff)",
       c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7],
       c[8], c[9], c[10], c[11], c[12], c[13], c[14], c[15]);
  }
  
  return NULL;
}

static PyObject *Py_plAnglesFromMatrixd(PyObject *module, PyObject *args)
{
  double a[16];
  double c[3];

  if (PyArg_ParseTuple(args, "(dddddddddddddddd):plAnglesFromMatrixd", 
           &a[0], &a[1], &a[2], &a[3],
           &a[4], &a[5], &a[6], &a[7],
           &a[8], &a[9], &a[10], &a[11],
           &a[12], &a[13], &a[14], &a[15])) {
    plAnglesFromMatrixd(c, a);

    return Py_BuildValue("(ddd)", c[0], c[1], c[2]);
  }
  
  return NULL;
}

static PyObject *Py_plAnglesFromMatrixf(PyObject *module, PyObject *args)
{
  float a[16];
  float c[3];

  if (PyArg_ParseTuple(args, "(ffffffffffffffff):plAnglesFromMatrixf", 
           &a[0], &a[1], &a[2], &a[3],
           &a[4], &a[5], &a[6], &a[7],
           &a[8], &a[9], &a[10], &a[11],
           &a[12], &a[13], &a[14], &a[15])) {
    plAnglesFromMatrixf(c, a);

    return Py_BuildValue("(fff)", c[0], c[1], c[2]);
  }
  
  return NULL;
}

static PyObject *Py_plCoordsFromMatrixd(PyObject *module, PyObject *args)
{
  double a[16];
  double c[3];

  if (PyArg_ParseTuple(args, "(dddddddddddddddd):plCoordsFromMatrixd", 
           &a[0], &a[1], &a[2], &a[3],
           &a[4], &a[5], &a[6], &a[7],
           &a[8], &a[9], &a[10], &a[11],
           &a[12], &a[13], &a[14], &a[15])) {
    plCoordsFromMatrixd(c, a);

    return Py_BuildValue("(ddd)", c[0], c[1], c[2]);
  }
  
  return NULL;
}

static PyObject *Py_plCoordsFromMatrixf(PyObject *module, PyObject *args)
{
  float a[16];
  float c[3];

  if (PyArg_ParseTuple(args, "(ffffffffffffffff):plCoordsFromMatrixf", 
           &a[0], &a[1], &a[2], &a[3],
           &a[4], &a[5], &a[6], &a[7],
           &a[8], &a[9], &a[10], &a[11],
           &a[12], &a[13], &a[14], &a[15])) {
    plCoordsFromMatrixf(c, a);

    return Py_BuildValue("(fff)", c[0], c[1], c[2]);
  }
  
  return NULL;
}

/*=================================================================
  Module definition
*/
#define Py_PLMethodMacro(name) {#name,  Py_##name,  METH_VARARGS}

static PyMethodDef PolarisMethods[] = {
  Py_PLMethodMacro(plHexToUnsignedLong),
  Py_PLMethodMacro(plSignedToLong),
  Py_PLMethodMacro(plHexEncode),
  Py_PLMethodMacro(plHexDecode),

  Py_PLMethodMacro(plDeviceName),
  Py_PLMethodMacro(plProbe),
  Py_PLMethodMacro(plOpen),
  Py_PLMethodMacro(plGetDeviceName),
  Py_PLMethodMacro(plClose),
  Py_PLMethodMacro(plSetThreadMode),
  Py_PLMethodMacro(plCommand),

  Py_PLMethodMacro(plGetError),
  Py_PLMethodMacro(plErrorString),

  Py_PLMethodMacro(plRESET),
  Py_PLMethodMacro(plINIT),
  Py_PLMethodMacro(plCOMM),

  Py_PLMethodMacro(plPVWRFromFile),

  Py_PLMethodMacro(plPVWR),
  Py_PLMethodMacro(plPVCLR),
  Py_PLMethodMacro(plPINIT),
  Py_PLMethodMacro(plPENA),
  Py_PLMethodMacro(plPDIS),
  Py_PLMethodMacro(plTSTART),
  Py_PLMethodMacro(plTSTOP),
  Py_PLMethodMacro(plGX),

  Py_PLMethodMacro(plGetGXTransform),
  Py_PLMethodMacro(plGetGXPortStatus),
  Py_PLMethodMacro(plGetGXSystemStatus),
  Py_PLMethodMacro(plGetGXToolInfo),
  Py_PLMethodMacro(plGetGXMarkerInfo),
  Py_PLMethodMacro(plGetGXSingleStray),
  Py_PLMethodMacro(plGetGXFrame),
  Py_PLMethodMacro(plGetGXNumberOfPassiveStrays),
  Py_PLMethodMacro(plGetGXPassiveStray),

  Py_PLMethodMacro(plLED),
  Py_PLMethodMacro(plBEEP),
  Py_PLMethodMacro(plVER),
  Py_PLMethodMacro(plSFLIST),
  Py_PLMethodMacro(plVSEL),
  Py_PLMethodMacro(plPSTAT),

  Py_PLMethodMacro(plGetPSTATPortStatus),
  Py_PLMethodMacro(plGetPSTATToolInfo),
  Py_PLMethodMacro(plGetPSTATCurrentTest),
  Py_PLMethodMacro(plGetPSTATPartNumber),
  Py_PLMethodMacro(plGetPSTATAccessories),
  Py_PLMethodMacro(plGetPSTATMarkerType),

  Py_PLMethodMacro(plSSTAT),

  Py_PLMethodMacro(plGetSSTATControl),
  Py_PLMethodMacro(plGetSSTATSensors),
  Py_PLMethodMacro(plGetSSTATTIU),

  Py_PLMethodMacro(plPPRD),
  Py_PLMethodMacro(plPPWR),
  Py_PLMethodMacro(plPURD),
  Py_PLMethodMacro(plPUWR),
  Py_PLMethodMacro(plPSEL),
  Py_PLMethodMacro(plPSRCH),
  Py_PLMethodMacro(plPVTIP),
  Py_PLMethodMacro(plTCTST),
  Py_PLMethodMacro(plTTCFG),

  Py_PLMethodMacro(plDSTART),
  Py_PLMethodMacro(plDSTOP),
  Py_PLMethodMacro(plIRINIT),
  Py_PLMethodMacro(plIRCHK),

  Py_PLMethodMacro(plGetIRCHKDetected),
  Py_PLMethodMacro(plGetIRCHKNumberOfSources),
  Py_PLMethodMacro(plGetIRCHKSourceXY),

  Py_PLMethodMacro(plIRED),
  Py_PLMethodMacro(pl3D),

  Py_PLMethodMacro(plRelativeTransform),
  Py_PLMethodMacro(plTransformToMatrixd),
  Py_PLMethodMacro(plTransformToMatrixf),
  Py_PLMethodMacro(plAnglesFromMatrixd),
  Py_PLMethodMacro(plAnglesFromMatrixf),
  Py_PLMethodMacro(plCoordsFromMatrixd),
  Py_PLMethodMacro(plCoordsFromMatrixf),

  {NULL, NULL}
};

#ifdef __cplusplus
extern "C" {
#endif

#define Py_PLBitfieldMacro(a) \
     PyDict_SetItemString(dict, #a, PyPLBitfield_FromUnsignedLong(a))
#define Py_PLConstantMacro(a) \
     PyDict_SetItemString(dict, #a, PyInt_FromLong(a))
#define Py_PLErrcodeMacro(a) \
     PyDict_SetItemString(dict, #a, PyPLBitfield_FromUnsignedLong(a))
#define Py_PLCharMacro(a) \
     PyDict_SetItemString(dict, #a, _PyString_FromChar(a))

void POLARIS_EXPORT initpolaris()
{
  PyObject *module;
  PyObject *dict;

  PyPolarisType.ob_type = &PyType_Type;
  PyPLBitfield_Type.ob_type = &PyType_Type;

  module = Py_InitModule("polaris", PolarisMethods);
  dict = PyModule_GetDict(module);

  Py_PLConstantMacro(POLARIS_MAJOR_VERSION);
  Py_PLConstantMacro(POLARIS_MINOR_VERSION);
  
  Py_PLConstantMacro(PL_OKAY);

  Py_PLErrcodeMacro(PL_INVALID);
  Py_PLErrcodeMacro(PL_TOO_LONG);
  Py_PLErrcodeMacro(PL_TOO_SHORT);
  Py_PLErrcodeMacro(PL_BAD_COMMAND_CRC);
  Py_PLErrcodeMacro(PL_INTERN_TIMEOUT);
  Py_PLErrcodeMacro(PL_COMM_FAIL);
  Py_PLErrcodeMacro(PL_PARAMETERS);
  Py_PLErrcodeMacro(PL_INVALID_PORT);
  Py_PLErrcodeMacro(PL_INVALID_MODE);
  Py_PLErrcodeMacro(PL_INVALID_LED);
  Py_PLErrcodeMacro(PL_LED_STATE);
  Py_PLErrcodeMacro(PL_BAD_MODE);
  Py_PLErrcodeMacro(PL_NO_TOOL);
  Py_PLErrcodeMacro(PL_PORT_NOT_INIT);
  Py_PLErrcodeMacro(PL_PORT_DISABLED);
  Py_PLErrcodeMacro(PL_INITIALIZATION);
  Py_PLErrcodeMacro(PL_TSTOP_FAIL);
  Py_PLErrcodeMacro(PL_TSTART_FAIL);
  Py_PLErrcodeMacro(PL_PINIT_FAIL);
  Py_PLErrcodeMacro(PL_DSTART_FAIL);
  Py_PLErrcodeMacro(PL_DSTOP_FAIL);
  Py_PLErrcodeMacro(PL_IRCHK_FAIL);
  Py_PLErrcodeMacro(PL_FIRMWARE);
  Py_PLErrcodeMacro(PL_INTERNAL);
  Py_PLErrcodeMacro(PL_IRINIT_FAIL);
  Py_PLErrcodeMacro(PL_IRED_FAIL);
  Py_PLErrcodeMacro(PL_SROM_FAIL);
  Py_PLErrcodeMacro(PL_SROM_READ);
  Py_PLErrcodeMacro(PL_SROM_WRITE);
  Py_PLErrcodeMacro(PL_SROM_SELECT);
  Py_PLErrcodeMacro(PL_PORT_CURRENT);
  Py_PLErrcodeMacro(PL_WAVELENGTH);
  Py_PLErrcodeMacro(PL_PARAMETER_RANGE);
  Py_PLErrcodeMacro(PL_VOLUME);
  Py_PLErrcodeMacro(PL_FEATURES);

  Py_PLErrcodeMacro(PL_ENVIRONMENT);

  Py_PLErrcodeMacro(PL_EPROM_READ);
  Py_PLErrcodeMacro(PL_EPROM_WRITE);
  Py_PLErrcodeMacro(PL_EPROM_ERASE);

  Py_PLErrcodeMacro(PL_BAD_CRC);
  Py_PLErrcodeMacro(PL_OPEN_ERROR);
  Py_PLErrcodeMacro(PL_BAD_COMM);
  Py_PLErrcodeMacro(PL_TIMEOUT);
  Py_PLErrcodeMacro(PL_WRITE_ERROR);
  Py_PLErrcodeMacro(PL_READ_ERROR);
  Py_PLErrcodeMacro(PL_PROBE_FAIL);
  
  Py_PLConstantMacro(PL_9600);
  Py_PLConstantMacro(PL_14400);
  Py_PLConstantMacro(PL_19200);
  Py_PLConstantMacro(PL_38400);
  Py_PLConstantMacro(PL_57600);
  Py_PLConstantMacro(PL_115200);

  Py_PLConstantMacro(PL_8N1);
  Py_PLConstantMacro(PL_8N2);
  Py_PLConstantMacro(PL_8O1);
  Py_PLConstantMacro(PL_8O2);
  Py_PLConstantMacro(PL_8E1);
  Py_PLConstantMacro(PL_8E2);
  Py_PLConstantMacro(PL_7N1);
  Py_PLConstantMacro(PL_7N2);
  Py_PLConstantMacro(PL_7O1);
  Py_PLConstantMacro(PL_7O2);
  Py_PLConstantMacro(PL_7E1);
  Py_PLConstantMacro(PL_7E2);

  Py_PLConstantMacro(PL_NOHANDSHAKE);
  Py_PLConstantMacro(PL_HANDSHAKE);

  Py_PLCharMacro(PL_STATIC);
  Py_PLCharMacro(PL_DYNAMIC);
  Py_PLCharMacro(PL_BUTTON_BOX);

  Py_PLBitfieldMacro(PL_XFORMS_AND_STATUS);
  Py_PLBitfieldMacro(PL_ADDITIONAL_INFO);
  Py_PLBitfieldMacro(PL_SINGLE_STRAY);
  Py_PLBitfieldMacro(PL_FRAME_NUMBER);
  Py_PLBitfieldMacro(PL_PASSIVE);
  Py_PLBitfieldMacro(PL_PASSIVE_EXTRA);
  Py_PLBitfieldMacro(PL_PASSIVE_STRAY);

  Py_PLConstantMacro(PL_DISABLED);
  Py_PLConstantMacro(PL_MISSING);

  Py_PLBitfieldMacro(PL_TOOL_IN_PORT);
  Py_PLBitfieldMacro(PL_SWITCH_1_ON);
  Py_PLBitfieldMacro(PL_SWITCH_2_ON);
  Py_PLBitfieldMacro(PL_SWITCH_3_ON);
  Py_PLBitfieldMacro(PL_INITIALIZED);
  Py_PLBitfieldMacro(PL_ENABLED);
  Py_PLBitfieldMacro(PL_OUT_OF_VOLUME);
  Py_PLBitfieldMacro(PL_PARTIALLY_IN_VOLUME);
  Py_PLBitfieldMacro(PL_CURRENT_DETECT);

  Py_PLBitfieldMacro(PL_COMM_SYNC_ERROR);
  Py_PLBitfieldMacro(PL_TOO_MUCH_EXTERNAL_INFRARED);
  Py_PLBitfieldMacro(PL_COMM_CRC_ERROR);

  Py_PLBitfieldMacro(PL_BAD_TRANSFORM_FIT);
  Py_PLBitfieldMacro(PL_NOT_ENOUGH_MARKERS);
  Py_PLBitfieldMacro(PL_TOOL_FACE_USED);

  Py_PLBitfieldMacro(PL_MARKER_MISSING);
  Py_PLBitfieldMacro(PL_MARKER_EXCEEDED_MAX_ANGLE);
  Py_PLBitfieldMacro(PL_MARKER_EXCEEDED_MAX_ERROR);
  Py_PLBitfieldMacro(PL_MARKER_USED);

  Py_PLCharMacro(PL_BLANK);
  Py_PLCharMacro(PL_FLASH);
  Py_PLCharMacro(PL_SOLID);

  Py_PLBitfieldMacro(PL_BASIC);
  Py_PLBitfieldMacro(PL_TESTING);
  Py_PLBitfieldMacro(PL_PART_NUMBER);
  Py_PLBitfieldMacro(PL_ACCESSORIES);
  Py_PLBitfieldMacro(PL_MARKER_TYPE);

  Py_PLConstantMacro(PL_UNOCCUPIED);

  Py_PLBitfieldMacro(PL_TOOL_IN_PORT_SWITCH);
  Py_PLBitfieldMacro(PL_SWITCH_1);
  Py_PLBitfieldMacro(PL_SWITCH_2);
  Py_PLBitfieldMacro(PL_SWITCH_3);
  Py_PLBitfieldMacro(PL_TOOL_TRACKING_LED);
  Py_PLBitfieldMacro(PL_LED_1);
  Py_PLBitfieldMacro(PL_LED_2);
  Py_PLBitfieldMacro(PL_LED_3);

  Py_PLBitfieldMacro(PL_950NM);
  Py_PLBitfieldMacro(PL_850NM);

  Py_PLBitfieldMacro(PL_NDI_ACTIVE);
  Py_PLBitfieldMacro(PL_NDI_CERAMIC);
  Py_PLBitfieldMacro(PL_PASSIVE_ANY);
  Py_PLBitfieldMacro(PL_PASSIVE_SPHERE);
  Py_PLBitfieldMacro(PL_PASSIVE_DISC);

  Py_PLBitfieldMacro(PL_CONTROL);
  Py_PLBitfieldMacro(PL_SENSORS);
  Py_PLBitfieldMacro(PL_TIU);

  Py_PLBitfieldMacro(PL_EPROM_CODE_CHECKSUM);
  Py_PLBitfieldMacro(PL_EPROM_SYSTEM_CHECKSUM);

  Py_PLBitfieldMacro(PL_LEFT_ROM_CHECKSUM);
  Py_PLBitfieldMacro(PL_LEFT_SYNC_TYPE_1);
  Py_PLBitfieldMacro(PL_LEFT_SYNC_TYPE_2);
  Py_PLBitfieldMacro(PL_RIGHT_ROM_CHECKSUM);
  Py_PLBitfieldMacro(PL_RIGHT_SYNC_TYPE_1);
  Py_PLBitfieldMacro(PL_RIGHT_SYNC_TYPE_2);

  Py_PLBitfieldMacro(PL_ROM_CHECKSUM);
  Py_PLBitfieldMacro(PL_OPERATING_VOLTAGES);
  Py_PLBitfieldMacro(PL_MARKER_SEQUENCING);
  Py_PLBitfieldMacro(PL_SYNC);
  Py_PLBitfieldMacro(PL_COOLING_FAN);
  Py_PLBitfieldMacro(PL_INTERNAL_ERROR);

  Py_PLBitfieldMacro(PL_DETECTED);
  Py_PLBitfieldMacro(PL_SOURCES);

  Py_PLConstantMacro(PL_LEFT);
  Py_PLConstantMacro(PL_RIGHT);
}

#ifdef __cplusplus
}
#endif
