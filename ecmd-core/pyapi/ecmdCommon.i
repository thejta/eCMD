// These two char **io_argv typemaps are used by ecmdCommandArgs, ecmdParseOption and ecmdParseOptionWithArgs
// This allows the user to pass in a python list (such as sys.argv) and have the list be modified by the function
%typemap(in) (char **io_argv) {
  /* Check if is a list  */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (char **) malloc((size+1) * sizeof(char*));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
#ifdef ECMD_PY3API
     o = PyUnicode_AsUTF8String(o);
#endif
      if (PyString_Check(o))
        $1[i] = PyString_AsString(o);
      else {
        PyErr_SetString(PyExc_TypeError,"list must contain strings");
        free($1);
        return NULL;
      }
    }
    $1[i] = 0;
  } else if ($input == Py_None) {
    $1 =  NULL;
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}

// NOTE: This typemap only supports the char** array coming back being the same size or smaller
//       Support could be added, but it wasn't needed so the work wasn't put into making it happen
%typemap(argout) (char** io_argv) {
  int clen = 0, pylen;
  int i;
  // Get the length of the two lists
  pylen = PyList_Size($input);
  while ($1[clen]) clen++;
  // Copy the remining items from the C list into Python list
  for (i = 0; i < clen; i++) {
    PyList_SetItem($input, i, PyString_FromString($1[i]));
  }

  // Shorten up the Python list to the length of the c list 
  PyList_SetSlice($input, clen, pylen, NULL);

  // All done copying our data from the C array into python, free the malloc from in typemap
  free($1);
}
