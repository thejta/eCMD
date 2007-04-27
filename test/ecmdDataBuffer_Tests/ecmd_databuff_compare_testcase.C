/************************************************************
USAGE:  This file is used to test a new implementation of an 
ecmdDataBuffer Member Function.  It runs the new implementation
and compares the result and performance to the original/old
implementation.
Look for 'CHANGE_CODE_HERE' flags below to see where you have
to update this file
************************************************************/
// Change Log *********************************************************
//                                                                      
//  Flag Reason   Vers Date     Coder     Description                       
//  ---- -------- ---- -------- -----     -----------------------------
//  initial Creation   04/20/07 Shashank  Common TestCase framework to compare 
//                                        and analyze old and new function in ecmdDataBuffer.C
// End Change Log *****************************************************

#include <list>
#include <string>
#include <stdio.h>
#include <unistd.h>          
#include <sys/times.h>
#include <stdlib.h> // for random()

// eCMD Includes
#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <ecmdStructs.H>


/************************************************************
CHANGE_CODE_HERE
Include the headers and define the macros required for the 
preferred function(old version) which user want to analyze
and compare with the new version of that fucntion.The old 
function is defined later (see bottom).
************************************************************/  

const char* program_name ;

void print_usage(FILE* stream, int exit_code)
{
    fprintf(stream,"Usage: %s  :options \n",program_name);
    fprintf(stream,
            "  help               Display the usage information\n"
            " -bitMax     value   The maximum number of bits user want to set in the DataBuffer\n"
            " -increment  value   The value of increment in the for loop\n"
            " -seed       value   Value for random() # seed that creates test array"
            "  eg. \n"   
            "  ecmd_DataBuffer_tc help \n"
            "  ecmd_DataBuffer_tc -bitMax 300 -increment 2 \n");
    exit(exit_code);
}

int main (int argc, char *argv[])
{

  printf("***Start main***\n");
  uint32_t rc = ECMD_SUCCESS;

  printf("***Start of Testcase...\n");
  printf("***TestCase : To test, compare, and analyze the new function with the old function\n");  
  
  
  /****************************************
  parse the CommandLine options
  *****************************************/ 
    
  program_name=argv[0];
  
  char * curArg;

  // Default Values
  // this variable is used for the increment value user want to use in the "for" loop
  int increment=1;

  // this variable is used for the maximum bit size user want to set in databuffer.
  int bitMax=100;

  // this variable is used for the 'seed' of srandom
  int seed=1;
 
  //To display help 
  if (argc > 1)
  {
    //To display help 
    if ( ( !strcasecmp(argv[1], "help") || !strcasecmp(argv[1], "-h") || !strcasecmp(argv[1], "--help")) 
       && (argc > 1) ) 
      print_usage(stdout,0);

    curArg = ecmdParseOptionWithArgs(&argc, &argv, "-bitMax");
    if(curArg!=NULL)
        bitMax = (uint32_t)atoi(curArg);
  
    curArg = ecmdParseOptionWithArgs(&argc, &argv, "-increment");
    if(curArg!=NULL)
        increment = (uint32_t)atoi(curArg); 

    curArg = ecmdParseOptionWithArgs(&argc, &argv, "-seed");
    if(curArg!=NULL)
        seed = (uint32_t)atoi(curArg);
  } // end of argc>1 check
      
  printf("***Value of bitMax = %d, increment = %d, seed = %d\n***(Note: These values can be set via cmdline - see 'help')\n",bitMax, increment, seed);

  /****************************************
  Create Variables
  *****************************************/

  //Declaration of DataBuffer  
  ecmdDataBuffer buf_old,buf_new;
  
  //variable declared to record the time taken by the function in CPU ticks
  clock_t start_new,stop_new,start_old,stop_old;
  static clock_t old_time = 0,new_time = 0;

  //Variable used to record the number of times the function is called.
  static uint32_t FunCnt_old = 0,FunCnt_new = 0;

  // Create test array
  uint32_t wordMax = bitMax % 32 ? (bitMax / 32 + 1) : bitMax / 32;
  uint32_t * test_array = new uint32_t[wordMax];
  srandom(seed); // set random 'seed'
  for (uint32_t test_array_loop = 0; test_array_loop < wordMax ; test_array_loop++)
    test_array[test_array_loop] = random();


  /****************************************
  Test Loop
  *****************************************/

  for(int i=0;i<bitMax ; i++){
        for(int j=0 ;j< i+1;j=j+increment){
                    
            //bit size is set in the databuffer for new function
            buf_new.setBitLength(j+1);
                    
            //start time stamp for new function
            start_new=times(NULL);
                    
            /*****************************************************************
            CHANGE_CODE_HERE
            Call new version of function that user want to analyze
            eg.
            rc=buf_new.insert(array,0,j+1);
            This function need to be defined in the ecmaDataBuffer.C
            *****************************************************************/
                    
            //stop time stamp for new function
            stop_new=times(NULL);
                    
            //calculate the time taken by new function in the processing
            new_time=new_time + (stop_new-start_new);
                    
            //calculate the number of times the new function is called
            FunCnt_new=FunCnt_new+1; 
                    
            //check if there is no error in the called new function.                        
            if(rc){
                    printf("\nnew function failed at break point j= %d for i= %d, rc= 0x%x",j,i,rc);
                    break;
            }
            //otherwise process the old function
            else {
                    //bit size is set in the databuffer for the old function
                    buf_old.setBitLength(j+1);
                        
                    //start time stamp for the old function
                    start_old=times(NULL);
                        
                    /**************************************************************
                    CHANGE_CODE_HERE
                    Call old version of function that user want to analyze
                    eg.                  
                        rc=buf_old.old_insert( array,0,j+1);
                    This function need to be defined here in the test case only(see 
                    bottom). 
                    **************************************************************/
                        
                    //stop time stamp for the old function.        
                    stop_old=times(NULL);
                        
                    //calculate the time taken by old function in the processing
                    old_time=old_time + (stop_old-start_old);
                         
                    //calculate the number of times the new function is called        
                    FunCnt_old=FunCnt_old+1;
                        
                    //check if there is no error in the called old function.
                    if(rc){
                            printf("\nold function failed at break point j= %d for i= %d, rc= 0x%x\n",j,i,rc);
                            break;
                    }                               
                    //comparison is made to check whether the new function and old function give the same result
                    if(buf_old!=buf_new){  
                            printf("comparison Failed:the new function and old function do not match\n"
                                   "comparison failed at break point j= %d for i= %d\n"
                                   "the string should be:(0x%s)\n"
                                   "but the string is :(0x%s)\n",
                                    j,i,buf_old.genHexLeftStr().c_str(),buf_new.genHexLeftStr().c_str());
                            rc= ECMD_FAILURE;
                            break;
                    }                      
            } 
                                         
        }
  }
   
  printf("TOTAL TIME in CPU ticks for old_function = %lu for Function Count = %d\n",old_time,FunCnt_old);
  
  printf("TOTAL TIME in CPU ticks for new_function = %lu for Function Count = %d\n",new_time,FunCnt_new); 
   
  if (rc != ECMD_SUCCESS) 
        printf("\n****ERROR: TESTCASE FAILED ,exit returning rc (0x%08x)***\n",rc);
  else {
        printf("\n***Testcase Passed***");
        printf("\n***End of Testcase***\n");
  }
  return rc;

}


/*******************************************************************************************************************
CHANGE_CODE_HERE
 Define here the old version of the function that user want to analyze and compare.This function has been 
 copied from the ecmdDataBuffer.C 
 
 eg.
 
 uint32_t ecmdDataBuffer::old_insert(const uint32_t *i_dataIn, uint32_t i_targetStart, uint32_t i_len, uint32_t i_sourceStart){

    uint32_t rc = ECMD_DBUF_SUCCESS;

    if (i_targetStart+i_len > iv_NumBits) {
        ETRAC3("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d + i_len %d > iv_NumBits (%d)", i_targetStart, i_len, iv_NumBits);
        RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    } else if (i_targetStart >= iv_NumBits) {
        ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_targetStart %d >= iv_NumBits (%d)", i_targetStart, iv_NumBits);
        RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    } else if (i_len > iv_NumBits) {
        ETRAC2("**** ERROR : ecmdDataBuffer::insert: i_len %d > iv_NumBits (%d)", i_len, iv_NumBits);
        RETURN_ERROR(ECMD_DBUF_BUFFER_OVERFLOW);
    } else {
    
            uint32_t mask = 0x80000000 >> (i_sourceStart % 32);
            const uint32_t * sourcePtr = i_dataIn;
            for (uint32_t i = 0; i < i_len; i++) {
                    if (sourcePtr[(i+i_sourceStart)/32] & mask) {
                        rc = this->setBit(i_targetStart+i);
                    }
                    else { 
                        rc = this->clearBit(i_targetStart+i);
                    }

                    mask >>= 1;
                    if (mask == 0x00000000) {
                        mask = 0x80000000;
                    }
                    if (rc) break;
            }
    }
    return rc;
}
 
********************************************************************************************************************/
