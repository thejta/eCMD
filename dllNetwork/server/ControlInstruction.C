//IBM_PROLOG_BEGIN_TAG
/* 
 * Copyright 2003,2017 IBM International Business Machines Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//IBM_PROLOG_END_TAG


//--------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------
#include <ControlInstruction.H>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <git_version.H>

#ifdef OTHER_USE
#include <OutputLite.H>
extern OutputLite out;
#else
#include <CronusData.H>
#endif

#ifndef SERVER_TYPE
  #define SERVER_TYPE SERVER_BMC
#endif

/*****************************************************************************/
/* ControlInstruction Implementation *****************************************/
/*****************************************************************************/
ControlInstruction::ControlInstruction(void) : Instruction(),
controls(NULL)
{
  version = 0x4;
  type = CONTROL;
  majorIstepNum = 0;
  minorIstepNum = 0;
  timeout = 0;
}
ControlInstruction::ControlInstruction(InstructionCommand i_command, uint32_t i_flags, const char * i_commandToRun, uint32_t i_fileStart, uint32_t i_fileChunkSize) : Instruction(),
controls(NULL)
{
  version = 0x1;
  type = CONTROL;
  command = i_command;
  flags = i_flags;
  fileStart = i_fileStart;
  fileChunkSize = i_fileChunkSize;
  if(i_commandToRun != NULL) {
    commandToRun = i_commandToRun;
  }
  if (command == QUERYSP) {
    version = 0x4;
  }
}  

ControlInstruction::ControlInstruction(InstructionCommand i_command, uint32_t i_flags, const char * i_commandToRun) : Instruction(),
controls(NULL)
{
  version = 0x1;
  type = CONTROL;
  command = i_command;
  flags = i_flags;
  if(i_commandToRun != NULL) {
    commandToRun = i_commandToRun;
  }
  if (command == QUERYSP) {
    version = 0x4;
  }
}

ControlInstruction::ControlInstruction(ServerControls * i_controls) : Instruction(),
controls(i_controls)
{
  version = 0x4;
  type = CONTROL;
}

ControlInstruction::~ControlInstruction(void) {
}

uint32_t ControlInstruction::setup(InstructionCommand i_command, uint32_t i_flags, const char * i_commandToRun) {
  version = 0x1;
  command = i_command;
  flags = i_flags;
  if(i_commandToRun != NULL) {
    commandToRun = i_commandToRun;
  }
  if (command == QUERYSP) {
    version = 0x4;
  }
  return 0;
}

uint32_t ControlInstruction::setup(InstructionCommand i_command, uint32_t i_flags, const char * i_commandToRun, uint32_t i_fileStart, uint32_t i_fileChunkSize) {
  version = 0x1;
  command = i_command;
  flags = i_flags;
  fileStart = i_fileStart;
  fileChunkSize = i_fileChunkSize;
  if(i_commandToRun != NULL) {
    commandToRun = i_commandToRun;
  }
  return 0;
}

uint32_t ControlInstruction::setup(InstructionCommand i_command, uint32_t i_key, uint32_t i_flags, const char * i_contactInfo) {
  command = i_command;
  flags = i_flags;
  key = i_key;
  if(i_contactInfo != NULL) {
    contactInfo = i_contactInfo;
  }
  version = 0x1;
  return 0;
}

uint32_t ControlInstruction::setup(InstructionCommand i_command, uint32_t i_arg1 , uint32_t i_arg2, uint32_t i_arg3, uint32_t i_flags) {
  command = i_command;
  flags = i_flags;
  if (i_command == SNDISTEPMSG) {
    majorIstepNum = i_arg1;
    minorIstepNum = i_arg2;
    timeout = i_arg3;
    version = 0x4;
  }
  return 0;
}

uint32_t ControlInstruction::execute(ecmdDataBuffer & o_data, InstructionStatus & o_status, Handle ** io_handle) {
  uint32_t rc = 0;

  /* set the version of this instruction in the status */
  o_status.instructionVersion = version;

  /* check for any previous errors to report back */
  if (error) {
    rc = o_status.rc = error;
    return rc;
  }

  switch(command) {
    case INFO:
      {
        /* They want to know who we are */
        o_data.setWordLength(7);
        o_data.setWord(0, 0x1);
        o_data.setWord(1, SERVER_TYPE);
        o_data.setWord(2, 0x1FFFFFFF);
        o_data.setWord(3, 0x000000FF);
        o_data.setWord(4, 0x000000FF);
        o_data.setWord(5, 0x000000FF);
        uint32_t myFlags = 0x0;
        // FSP1 should support HW CRC properly now, so enable it
        myFlags |= SERVER_INFO_HW_CRC_SUPPORT;
        if ((controls != NULL) && (controls->global_multi_client_pointer != NULL) && (*(controls->global_multi_client_pointer)) == true ) {
          myFlags |= SERVER_INFO_MULTI_CLIENT;
        }
        if ((controls != NULL) && (controls->global_auth_pointer != NULL) && (controls->global_auth_pointer->enabled)) {
          myFlags |= SERVER_INFO_AUTH_NEEDED;
        }
        o_data.setWord(6, myFlags);
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case RUN_CMD:

      {
        if (commandToRun.size() == 0) {
          rc = o_status.rc = SERVER_INVALID_COMMAND_BLOCK_FIELD_COMMPTR;
          break;
        }
        const int maxbuf = 1024 * 1024;
        char buf[maxbuf];
        char buferr[maxbuf];
        memset(buf, 0x0, maxbuf);
        memset(buferr, 0x0, maxbuf);

        // create a pipe for child process to communicate with
        int std_out[2];
        int std_err[2];
        if( (pipe(std_out) == -1) || (pipe(std_err) == -1) )
        {
          rc = o_status.rc = SERVER_COMMAND_PIPE_ERROR;
          break;
        }

        // fork a new process for the command
        pid_t pid = fork();
        if( pid == -1 )
        {
          // error, couldn't create thread
        }
        else if( pid == 0 )
        {
          // CHILD process
          // close unused read side of pipe
          close( std_out[0] );
          close( std_err[0] );
          // dup to stderr/stdout
          errno = 0;
          while( (dup2(std_out[1], STDOUT_FILENO) == -1) && (errno == EINTR) ) {}
          errno = 0;
          while( (dup2(std_err[1], STDERR_FILENO) == -1) && (errno == EINTR) ) {}
          // close unused descriptor
          close( std_out[1] );
          close( std_err[1] );

          execl( "/bin/sh", "/bin/sh", "-c", commandToRun.c_str(), (char*) NULL );
          // should never get here
          _exit(-1);
        }
        else
        {
          // close unused write side of pipe
          close( std_out[1] );
          close( std_err[1] );
        
          struct timeval timeout;
          int found = 0;
          fd_set pipes;
          size_t bytes_readStdout = 0;
          size_t bytes_readStderr = 0;
          size_t lBufferSize = 1024;
          char lBuffer[lBufferSize];
          int idx = 0;
          int idxerr = 0;
          int exit_status = 0;

          FD_ZERO( &pipes );
          FD_SET( std_out[0], &pipes );
          FD_SET( std_err[0], &pipes );
          int max_nfds = ( std_out[0] > std_err[0] ) ? std_out[0] : std_err[0];

          while( 1 )
          {
            bytes_readStdout = bytes_readStderr = 0;
            // read from pipes file descriptors
            // poll for output and check for child pid ending
            timeout.tv_sec = 2;    // Timeout in 2s
            timeout.tv_usec = 0;

            fd_set l_pipes = pipes;

            errno = 0;
            found = select( max_nfds + 1, &l_pipes, NULL, NULL, &timeout );
            if( found < 0 )
            {
              if( errno == EINTR)
              {
                continue;
              }
              break;
            }
            else if( found == 0 )
            {
              // We timed out waiting for data 
              // check if process is still running
              pid_t wpid = waitpid( pid, &exit_status, WNOHANG );
              if( (wpid == pid) && WIFEXITED( exit_status ) )
              {
                // process has exited, so break out now
                break;
              }
              continue;
            }

            bool l_breakStdoutNeeded = false;
            if( FD_ISSET( std_out[0], &l_pipes ) )
            {
              //stdout has data, or returned noop/error
              memset( lBuffer, 0x0, lBufferSize );
              bytes_readStdout = read( std_out[0], lBuffer, lBufferSize );
              // only handle cases where no error or nooop is returned, error handling is later to be able to capture both stderr/stdout
              if( bytes_readStdout == (size_t) -1 )
              {
                l_breakStdoutNeeded = true;  // ERROR
              }
              else if( bytes_readStdout == 0 )
              {
                l_breakStdoutNeeded = true;  // EOF
              }
              else
              {
                // size to copy into buf
                size_t lCopySize = ((idx + bytes_readStdout) > (maxbuf - 2)) ? (maxbuf - 2 - idx) : bytes_readStdout;
                std::copy( lBuffer, lBuffer + lCopySize, buf + idx );
                if( (idx + bytes_readStdout) > (maxbuf - 2) )
                {
                  rc = o_status.rc = SERVER_COMMAND_BUFFER_OVERFLOW;
                  l_breakStdoutNeeded = true;
                }
                idx += lCopySize;
              }
            }

            bool l_breakStderrNeeded = false;
            if( FD_ISSET( std_err[0], &l_pipes ) )
            {
              //stderr has data, or returned noop/error
              memset( lBuffer, 0x0, lBufferSize );
              bytes_readStderr = read( std_err[0], lBuffer, lBufferSize );
              // only handle cases where no error or nooop is returned, error handling is later to be able to capture both stderr/stdout
              if( bytes_readStderr == (size_t) -1 )
              {
                l_breakStderrNeeded = true;  // ERROR
              }
              else if( bytes_readStderr == 0 )
              {
                l_breakStderrNeeded = true;  // EOF
              }
              else
              {
                // size to copy into buf
                size_t lCopySize = ((idxerr + bytes_readStderr) > (maxbuf - 2)) ? (maxbuf - 2 - idxerr) : bytes_readStderr;
                std::copy( lBuffer, lBuffer + lCopySize, buferr + idxerr );
                if( (idxerr + bytes_readStderr) > (maxbuf - 2) )
                {
                  rc = o_status.rc = SERVER_COMMAND_BUFFER_OVERFLOW;
                  l_breakStderrNeeded = true;
                }
                idxerr += lCopySize;
              }
            }

            // stdout/stderr processing generated a condition where a break out of the while(1) is needed
            if (found == 1) {
                if (l_breakStdoutNeeded || l_breakStderrNeeded) break;
            } else {
                if (l_breakStdoutNeeded && l_breakStdoutNeeded) break;
            }
          }
          waitpid( pid, &exit_status, 0 );
          close( std_err[0] );
          close( std_out[0] );
          exit_status = WEXITSTATUS( exit_status );
          o_status.data.setBitLength( 32 );
          o_status.data.insert( (uint32_t) exit_status, 0, 32 );
       
          /* Now let's copy this into our return structure */
          o_data.setWordLength( (((strlen(buf) + 1) / sizeof(uint32_t)) + 1) + (((strlen(buferr) + 1) / sizeof(uint32_t)) + 1) );
          if( strlen( buferr ) > 0 ) 
          {
            o_data.insert( (uint8_t *) buferr, 0, (strlen(buferr) + 1) * 8, 0 );
          } 
          if( strlen( buf ) > 0 ) 
          {
            o_data.insert( (uint8_t *) buf, (strlen(buferr)) * 8, (strlen(buf) + 1) * 8, 0 );
          }
        }

        if(!rc) 
        {
          rc = o_status.rc = SERVER_COMMAND_COMPLETE;
        }
      }
      break;

    case SNDISTEPMSG:

      // FIXME implement
      rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      break;

    case AUTH:
      {
        if ((controls != NULL) && (controls->global_auth_pointer != NULL) && (controls->global_auth_pointer->enabled)) {
          if (controls->global_auth_pointer->keyMap.count(key) != 0) {
            /* authorization valid */
            *(controls->threadKeyValid_pointer) = true;
            *(controls->threadKey_pointer) = key;
            rc = o_status.rc = SERVER_COMMAND_COMPLETE;
          } else {
            /* authorization invalid */
            rc = o_status.rc = SERVER_AUTHORIZATION_INVALID;
            o_status.errorMessage = std::string("Server is locked. Invalid authorization. Contact : ") + controls->global_auth_pointer->keyMap[controls->global_auth_pointer->firstKey];
          }
        } else {
          /* authorization was not needed */
          rc = o_status.rc = SERVER_COMMAND_COMPLETE;
        }
      }
      break;

    case ADDAUTH:
      {
        if ((controls != NULL) && (controls->global_auth_pointer != NULL)) {
          if (controls->global_auth_pointer->enabled == false) {
            controls->global_auth_pointer->firstKey = key;
          }
          controls->global_auth_pointer->enabled = true;
          controls->global_auth_pointer->keyMap[key] = contactInfo;
          *(controls->threadKeyValid_pointer) = true;
          *(controls->threadKey_pointer) = key;
          rc = o_status.rc = SERVER_COMMAND_COMPLETE;
        }
      }
      break;

    case CLEARAUTH:
      {
        if ((controls != NULL) && (controls->global_auth_pointer != NULL)) {
          controls->global_auth_pointer->enabled = false;
          controls->global_auth_pointer->keyMap.clear();
          controls->global_auth_pointer->firstKey = 0;
          *(controls->threadKeyValid_pointer) = false;
          *(controls->threadKey_pointer) = 0x0;
          rc = o_status.rc = SERVER_COMMAND_COMPLETE;
        }
      }
      break;

    case VERSION:
      {
        std::ostringstream oss;
        oss << "Server Version" << std::endl;
        oss << "Date: " << __DATE__ << " " << __TIME__ << std::endl;
        oss << "Git Version: " << git_version << std::endl;
        std::map<std::string, uint32_t>::iterator versionIterator = controls->global_version_map_pointer->begin();
        while (versionIterator != controls->global_version_map_pointer->end()) {
          oss << std::left << std::setw(24) << versionIterator->first << " : " << versionIterator->second << std::endl;
          versionIterator++;
        }
        o_data.setWordLength(((oss.str().length() + 1) / sizeof(uint32_t)) + 1);
        o_data.memCopyIn((uint8_t *) oss.str().c_str(), oss.str().length() + 1);
        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case FLIGHTRECORDER:

      {
        std::ostringstream oss;
        oss << "Flight Recorder" << std::endl;

        std::list<FlightRecorderEntry >::iterator frIterator = controls->global_flight_recorder_pointer->begin();
        while (frIterator != controls->global_flight_recorder_pointer->end()) {
          oss << std::left << std::setw(16) << InstructionTypeToString(frIterator->type) << " : "
              << std::left << std::setw(16) << InstructionCommandToString(frIterator->command) << " : " << frIterator->vars << std::endl;
          frIterator++;
        }

        o_data.setWordLength(((oss.str().length() + 1) / sizeof(uint32_t)) + 1);
        o_data.memCopyIn((uint8_t *) oss.str().c_str(), oss.str().length() + 1);

        rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      }
      break;

    case EXIT:
      if ((controls != NULL) && (controls->global_exit_pointer != NULL)) {
        *(controls->global_exit_pointer) = true;
      }
      rc = o_status.rc = SERVER_COMMAND_COMPLETE;
      break;

    case QUERYSP:
      {
        if (commandToRun.size() == 0) {
          rc = o_status.rc = SERVER_INVALID_COMMAND_BLOCK_FIELD_COMMPTR;
          break;
        }
        if (commandToRun == "LIST") {
          o_data.insertFromAsciiAndResize("LIST SPTYPE");
          rc = o_status.rc = SERVER_COMMAND_COMPLETE;
          break;
        } else if (commandToRun == "SPTYPE") {
          o_data.insertFromAsciiAndResize(serverTypeToString(SERVER_TYPE).c_str());
          rc = o_status.rc = SERVER_COMMAND_COMPLETE;
          break;
        } else {
          rc = o_status.rc = SERVER_CONTROL_UNKNOWN_QUERYSP;
        }
      }
      break;

    case GETFILE:
      {
        char errstr[200];
        if ( commandToRun.size() == 0 )
        {
          rc = o_status.rc = SERVER_INVALID_COMMAND_BLOCK_FIELD_COMMAND;
          break;
        }

        // test that commandToRun contains a valid file
        // error out if the file does not exist
        struct stat buffer;
        if( stat(commandToRun.c_str(), &buffer) != 0 )
        {
          snprintf(errstr, 200, "ControlInstruction::execute(GETFILE) Failed finding %s\n", commandToRun.c_str() );
          o_status.errorMessage.append(errstr);
          rc = o_status.rc = SERVER_CONTROL_INVALID_FILE_NAME;
        }
        else
        {
            std::ifstream l_file;
            l_file.open( commandToRun.c_str() );

            if ( l_file.fail() ) 
            {
                snprintf(errstr, 200, "ControlInstruction::execute(GETFILE) Error opening file (%d) %s\n", errno, commandToRun.c_str() );
                o_status.errorMessage.append(errstr);
                rc = o_status.rc = SERVER_CONTROL_OPEN_FILE_FAILURE;
                break;
            }

            // seek to the end to determine file size
            l_file.seekg(0, l_file.end);
            uint32_t l_fileSize = l_file.tellg();

            // seek to where the user wants us to start
            if ( fileStart < l_fileSize )
            {
                l_file.seekg(fileStart);
            }
            else
            {
                snprintf(errstr, 200, "ControlInstruction::execute(GETFILE) fileStart(%d) starts beyond size of file (%d)\n", fileStart, l_fileSize);
                o_status.errorMessage.append(errstr);
                rc = o_status.rc = SERVER_CONTROL_READ_FILE_FAILURE;
                break;
            }

            // set fileChunkSize to file size if the default of 0 is set
            if ( fileChunkSize == 0 ) fileChunkSize = l_fileSize;

            o_data.setByteLength(fileChunkSize);
 
            rc = o_data.readFileStream( l_file, fileChunkSize*8 );
            
          if ( rc ) 
          {
            snprintf(errstr, 200, "ControlInstruction::execute(GETFILE) Error reading file %s\n", commandToRun.c_str() );
            o_status.errorMessage.append(errstr);
            rc = o_status.rc = SERVER_CONTROL_READ_FILE_FAILURE;
          }
            else if ( (fileStart + fileChunkSize) < l_fileSize )
            {
                rc = o_status.rc = SERVER_CONTROL_MORE_DATA_AVAILABLE;
            }
          else
          {
            rc = o_status.rc = SERVER_COMMAND_COMPLETE;
          }
          
          l_file.close();
        }
      }
      break;  

    default:
      rc = o_status.rc = SERVER_COMMAND_NOT_SUPPORTED;
      break;
  }
  return rc;
}

uint32_t ControlInstruction::flatten(uint8_t * o_data, uint32_t i_len) const {
  uint32_t rc = 0;
  uint32_t * o_ptr = (uint32_t *) o_data;
  
  if (i_len < flattenSize()) {
    out.error("ControlInstruction::flatten", "i_len %d bytes is too small to flatten\n", i_len);
    rc = 1;
  } else {
    // clear memory
    memset(o_data, 0, flattenSize());
    o_ptr[0] = htonl(version);
    o_ptr[1] = htonl(command);
    o_ptr[2] = htonl(flags);
    if (command == RUN_CMD || command == QUERYSP) {
      if (commandToRun.size() > 0) {
        strcpy(((char *)(o_ptr + 3)), commandToRun.c_str());
      } else {
        memset(((char *)(o_ptr + 3)), 0x0, 1);
      }
    } else if (command == GETFILE) {
      o_ptr[3] = htonl(fileStart);
      o_ptr[4] = htonl(fileChunkSize);
      strcpy(((char *)(o_ptr + 5)), commandToRun.c_str());
    } else if (command == AUTH) {
      o_ptr[3] = htonl(key);
    } else if (command == ADDAUTH) {
      o_ptr[3] = htonl(key);
      if (contactInfo.size() > 0) {
        strcpy(((char *)(o_ptr + 4)), contactInfo.c_str());
      } else {
        memset(((char *)(o_ptr + 4)), 0x0, 1);
      }
    } else if (command == SNDISTEPMSG && version >= 0x4) {
      o_ptr[3] = htonl(majorIstepNum);
      o_ptr[4] = htonl(minorIstepNum);
      o_ptr[5] = htonl(timeout);
    }
  }
  return rc;
}

uint32_t ControlInstruction::unflatten(const uint8_t * i_data, uint32_t i_len) {
  uint32_t rc = 0;
  uint32_t * i_ptr = (uint32_t *) i_data;

  version = ntohl(i_ptr[0]);
  if(version == 0x1 || version == 0x2 || version == 0x3 || version == 0x4) {
    command = (InstructionCommand) ntohl(i_ptr[1]);
    flags = ntohl(i_ptr[2]);
    if (command == RUN_CMD || command == QUERYSP) {
      commandToRun = ((char *)(i_ptr + 3));
    } else if (command == GETFILE) {
      fileStart = ntohl(i_ptr[3]);
      fileChunkSize = ntohl(i_ptr[4]);
      commandToRun = ((char *)(i_ptr + 5));
    } else if (command == AUTH) {
      key = ntohl(i_ptr[3]);
    } else if (command == ADDAUTH) {
      key = ntohl(i_ptr[3]);
      contactInfo = ((char *)(i_ptr + 4));
    } else if (command == SNDISTEPMSG && version >= 0x4) {
      majorIstepNum = ntohl(i_ptr[3]);
      minorIstepNum = ntohl(i_ptr[4]);
      timeout = ntohl(i_ptr[5]);
    }
  } else {
    error = rc = SERVER_UNKNOWN_INSTRUCTION_VERSION;
  }
  return rc;
}

uint32_t ControlInstruction::flattenSize(void) const {
  uint32_t size = 0;
  if (command == RUN_CMD || command == QUERYSP) {
    size = (3 * sizeof(uint32_t)) + commandToRun.size() + 1;
  } else if (command == GETFILE) {
    size = (5 * sizeof(uint32_t)) + commandToRun.size() + 1;
  } else if (command == AUTH) {
    size = (4 * sizeof(uint32_t));
  } else if (command == ADDAUTH) {
    size = (4 * sizeof(uint32_t)) + contactInfo.size() + 1;
  } else if (command == SNDISTEPMSG && version >= 0x4) {
    size = (6 * sizeof(uint32_t));
  } else {
    size = (3 * sizeof(uint32_t));
  }
  return size;
}

std::string ControlInstruction::dumpInstruction(void) const {
  std::ostringstream oss;
  oss << "ControlInstruction" << std::endl;
  oss << "version       : " << version << std::endl;
  oss << "command       : " << InstructionCommandToString(command) << std::endl;
  oss << "type          : " << InstructionTypeToString(type) << std::endl;
  oss << "flags         : " << InstructionFlagToString(flags) << std::endl;
  if (command == RUN_CMD || command == QUERYSP || command == GETFILE) {
    oss << "commandToRun  : " << commandToRun << std::endl;
    if ( command == GETFILE ) {
      oss << "fileStart     : " << fileStart << std::endl;
      oss << "fileChunkSize : " << fileChunkSize << std::endl;
    }
  } else if (command == AUTH) {
    oss << "key           : " << std::hex << std::setw(8) << std::setfill('0') << key << std::dec << std::endl;
  } else if (command == ADDAUTH) {
    oss << "key           : " << std::hex << std::setw(8) << std::setfill('0') << key << std::dec << std::endl;
    oss << "contactInfo   : " << contactInfo << std::endl;
  } else if (command == SNDISTEPMSG && version >= 0x4) {
    oss << "majorIstepNum : " << std::dec << majorIstepNum << std::endl;
    oss << "minorIstepNum : " << std::dec << minorIstepNum << std::endl;
    oss << "timeout       : " << std::dec << timeout << std::endl;
  }

  return oss.str();
}

std::string ControlInstruction::dumpInstructionShort(void) const {
  std::ostringstream oss;

  if (command == RUN_CMD || command == QUERYSP || command == GETFILE) {
    oss << commandToRun;
    if ( command == GETFILE ) {
      oss << ", " << fileStart << ", " << fileChunkSize;
    }
  } else if (command == AUTH) {
    oss << std::hex << std::setw(8) << std::setfill('0') << key;
  } else if (command == ADDAUTH) {
    oss << std::hex << std::setw(8) << std::setfill('0') << key << ", ";
    oss << contactInfo;
  } else if (command == SNDISTEPMSG && version >= 0x4) {
    oss << std::dec << majorIstepNum << ", ";
    oss << std::dec << minorIstepNum << ", ";
    oss << std::dec << timeout;
  }

  return oss.str();
}

uint64_t ControlInstruction::getHash(void) const {
  uint32_t hash = 0x0;
  switch (command) {
    case INFO:
    case GETFILE:
    case RUN_CMD:
    case QUERYSP:
    case SNDISTEPMSG:
    case VERSION:
    case FLIGHTRECORDER:
      hash = 0x0;
      break;
    default:
      hash |= ((0x0000000F & type) << 28);
      break;
  }
  return hash;
}
std::string ControlInstruction::getInstructionVars(const InstructionStatus & i_status) const {
  std::ostringstream oss;

  oss << std::hex << std::setfill('0');
  if (command == RUN_CMD || command == QUERYSP || command == GETFILE) {
    oss << "rc: " << std::setw(8) << i_status.rc;
    oss << " commandToRun: " << commandToRun;
    if ( command == GETFILE ) {
      oss << " fileStart: " << fileStart;
      oss << " fileChunkSize: " << fileChunkSize;
    }
  } else if (command == AUTH || command == ADDAUTH) {
    oss << "rc: " << std::setw(8) << i_status.rc;
    oss << " key: " << std::setw(8) << key;
  } else if (command == SNDISTEPMSG) {
    oss << "rc: " << std::setw(8) << i_status.rc;
    oss << " major: " << majorIstepNum << " minor: " << minorIstepNum << " timeout: " << timeout;
  } else {
    return Instruction::getInstructionVars(i_status);
  }

  return oss.str();
}

uint32_t ControlInstruction::populateTypeInfo(server_type_info & o_typeInfo, ecmdDataBuffer & i_data) {
  /* check version */
  if(i_data.getWord(0) == 0x1) {
    /* They gave us real data, let's get going */
    o_typeInfo.type = (SERVER_MACHINE_TYPE) i_data.getWord(1);
    o_typeInfo.tms_mask = i_data.getWord(2); o_typeInfo.tck_mask = i_data.getWord(3);
    o_typeInfo.tdi_mask = i_data.getWord(4); o_typeInfo.tdo_mask = i_data.getWord(5);
    o_typeInfo.flags = i_data.getWord(6);
  } else {
    out.error("ControlInstruction::populateTypeInfo","Unknown version of hw info\n");
    return 1;
  }
  return 0;
}

std::string ControlInstruction::serverTypeToString( SERVER_MACHINE_TYPE i_type )
{
    switch( i_type )
    {
        case(SERVER_CSP):
            return std::string("CSP");
        case(SERVER_BPC):
            return std::string("BPC");
        case(SERVER_FSP):
            return std::string("FSP");
        case(SERVER_SJM):
            return std::string("SJM");
        case(SERVER_PROXY):
            return std::string("PROXY");
        case(SERVER_SIM):
            return std::string("SIM");
        case(SERVER_LOFT):
            return std::string("LOFT");
        case(SERVER_6682TESTER):
            return std::string("6682TESTER");
        case(SERVER_SIMDISPATCHER):
            return std::string("SIMDISPATCHER");
        case(SERVER_UNDEFINED):
            return std::string("UNDEFINED");
        case(SERVER_ICON):
            return std::string("ICON");
        case(SERVER_FTDI):
            return std::string("FTDI");
        case(SERVER_GSD2PIB):
            return std::string("GSD2PIB");
        case(SERVER_D2C):
            return std::string("D2C");
        case(SERVER_BMC):
            return std::string("BMC");
        default:
            return std::string("UNKNOWN");
    }
    return std::string("UNKNOWN");
}
