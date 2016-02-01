//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

#ifndef ecmdExtInterpreter_h
#define ecmdExtInterpreter_h


/**
 @file ecmdExtInterpreter.H
 @brief eCMD Extention Command Interpreter function prototypes
*/

//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//  Forward References                                                
//--------------------------------------------------------------------

/**
 @brief Walk through the enabled extension interpreters
 @param argc Command line arguments
 @param argv Command line arguments
 @param io_rc The return code from calling the interpreters
 @retval ECMD_SUCCESS On successful command
 @retval nonzero On command failure
*/
uint32_t ecmdCallExtInterpreters(int argc, char* argv[], uint32_t & io_rc);

#endif /* ecmdExtInterpreter_h */
