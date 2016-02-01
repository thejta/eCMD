#ifdef ECMD_CIP_EXTENSION_SUPPORT
  /* Cronus/IP Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("cip",argv[0],3))) {
    io_rc = cipInitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = cipCommandInterpreter(argc, argv);
    }
  }
#endif
