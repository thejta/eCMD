#ifdef ECMD_FAPI2_EXTENSION_SUPPORT
  /* Fapi2 Extension */
  if ((io_rc == ECMD_INT_UNKNOWN_COMMAND) && (!strncmp("fapi2",argv[0],5))) {
    io_rc = fapi2InitExtension();
    if (io_rc == ECMD_SUCCESS) {
      io_rc = fapi2CommandInterpreter(argc, argv);
    }
  }
#endif
