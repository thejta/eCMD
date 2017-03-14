//IBM_PROLOG_BEGIN_TAG
//IBM_PROLOG_END_TAG

//----------------------------------------------------------------------
//  Includes
//----------------------------------------------------------------------
#define eth_transfer_C

#undef eth_transfer_C

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#ifdef AIX
extern "C"
{
#include "/usr/include/arpa/inet.h"
}
#else
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>


#ifndef OTHER_USE
# include <bluebox.h>
# define W_GLOBAL extern
# include <CronusData.H>
#else
# include <OutputLite.H>
  extern OutputLite out;
#endif

#include <eth_transfer1.h>

#ifdef HW
#include <fd_impl.H>
#endif

#define ETH_DEBUG 0


//----------------------------------------------------------------------
//  User Types
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Macros
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//  Internal Function Prototypes
//----------------------------------------------------------------------
uint32_t ethgenbyteswap(uint32_t data);

//----------------------------------------------------------------------
//  Global Variables
//----------------------------------------------------------------------

//---------------------------------------------------------------------
// Member Function Specifications
//---------------------------------------------------------------------
eth_transfer::eth_transfer() {
  clientVersion = 0x1;
  maxSocks = 4;

  pthread_mutex_init(&socksMutex, NULL);
  pthread_cond_init(&socksCondition, NULL);

  seedCounter = 0;
  pthread_mutex_init(&seedsMutex, NULL);
}

eth_transfer::~eth_transfer() {

  pthread_mutex_destroy(&socksMutex);
  pthread_cond_destroy(&socksCondition);

  pthread_mutex_destroy(&seedsMutex);
  while (!seeds.empty()) {
    delete seeds.front();
    seeds.pop_front();
  }
}


int eth_transfer::initialize(const char * opt) {
  int rc = 0;
  int sock = 0;

  int connected = 0;
  int retry_count = 0;

  std::string data = opt;

  int ip_port = 8192;   /* Default Cronus Port */
  char* tempptr;
  char l_host[50];

  while (!connected) {

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      out.error("eth_transfer::initialize","Unable to allocate socket for connection\n");

      return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
    }

    struct hostent host;
    struct hostent *hostp = &host;
#ifdef AIX
    struct hostent_data hostdata;
    memset((void *) &hostdata, 0, sizeof hostdata);
    //example rc = gethostbyname_r(name, &host, &hostdata);
#else
    int host_errno = 0;
    char hostdata[1024 * 10];
    memset((void *) &hostdata, 0, sizeof hostdata);
    //example rc = gethostbyname_r(name, &host, hostdata, sizeof hostdata, &hostp, &host_errno);
#endif

    struct sockaddr_in server;
    memset((void*) &server, 0, sizeof server);

    /* Look to see if they provided us some data to  initialize with */
    if (data.size()) {
        /* They provided me a Hostname, they want me to use */

        strncpy(l_host, data.c_str(), 50);

        if ( (gethostbyname_r(data.c_str(), &host, hostdata, sizeof hostdata, &hostp, &host_errno) != 0) || (host_errno !=0) ) {
           if (host_errno !=0) {
              out.error("eth_transfer::initialize","host_errno error %d\n",host_errno);
           }
          out.error("eth_transfer::initialize","Unable to lookup hostname %s\n",data.c_str());
          return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
        }

        memcpy((void*) &server.sin_addr, hostp->h_addr, hostp->h_length);
    } else {

      /* Let's see if they specifed a host through an environment variable */
      tempptr = getenv("SERVER_HOST");
      if (tempptr != NULL) {
        strncpy(l_host, tempptr, 50);

        if ((gethostbyname_r(tempptr, &host, hostdata, sizeof hostdata, &hostp, &host_errno)) != 0) {
          out.error("eth_transfer::initialize","Unable to lookup hostname %s\n",tempptr);
          return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
        }

      } else {
        /* They didn't provide us any data, we will use "localhost" for now */

        strncpy(l_host, "localhost", 50);

        if ((gethostbyname_r("localhost", &host, hostdata, sizeof hostdata, &hostp, &host_errno)) != 0) {
          out.error("eth_transfer::initialize","Unable to lookup hostname localhost\n");
          return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
        }
      }

      memcpy((void*) &server.sin_addr, hostp->h_addr, hostp->h_length);
    }

    /* Set the port */
    static struct servent s;
    struct servent *servp;
    servp = &s;
    s.s_port = htons(ip_port);
    server.sin_family = AF_INET;
    server.sin_port = servp->s_port;


    if (connect(sock, (struct sockaddr*)&server, sizeof server) < 0) {
      out.error("eth_transfer::initialize","Unable to Connect to Service Processor on : %s\n",l_host);
      return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
    }


    /* Ok, now we need to figure out if we are going to need to byte swap, the server is going to send us a word which should be 0xFEEDBOBO */
    uint32_t t_buf[3];
    int t_buf_len;

    /* Let's wrap this handshake read in timeout code so we can at least fail if the hang happens again */
    fd_set rset;
    struct timeval tv;

    FD_ZERO(&rset);
    FD_SET(sock, &rset);

    // Init
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    // 60 seconds is a huge amount of time to wait for this, but we'll be safe   
    tv.tv_sec = 60;
    //tv.tv_usec = 60000000;

    do {
      errno = 0;
      int sockFromSelect = select(sock+1, &rset, NULL, NULL, &tv);

      /* Error check our cases for <= 0 */
      if (sockFromSelect < 0 && errno == EINTR) {
        continue;
      } else if (sockFromSelect < 0) {
        out.error("eth_transfer::initialize","Error calling select: %d\n",errno);
        return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
      } else if (!sockFromSelect) {
#ifndef OTHER_USE
        ecmdChipTarget target;
        out.stelaEvent(target,"NETWORK_TIMEOUT","Timed out in initialize waiting to hear back from the FSP\n");
        out.stela("eth_transfer::initialize","service processor: %s\n",l_host);
#endif
        out.error("eth_transfer::initialize","Timed out waiting to hear back from the FSP!\n");
        out.error("eth_transfer::initialize","For now, we are going to fail out instead of hanging.\n");
        return TRANSFER_ACK_TIMEOUT;
      }
    } while (0);

    /* This is from the server */
    t_buf_len = read(sock, t_buf, 2 * sizeof(uint32_t));
    if (t_buf_len != (2 * sizeof(uint32_t))) {
      out.error("eth_transfer::initialize","Service Processor didn't send back byte swap data\n");
      ::close(sock);
      return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
    }
    if (ETH_DEBUG) {
      out.print("Read %d bytes ...\n",t_buf_len);
    }

    if (rc) {
      out.error("eth_transfer::initialize","Didn't Recieve Word from Service Processor\n");
      return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
    } else {
      if (ETH_DEBUG) printf("TBUF[1] : %.8X\n",t_buf[1]);
      uint32_t serverVersion = 0x00000000;
      if ((t_buf[1] & 0xFFFFFFF0) == 0xFEEDB0B0) {
        serverVersion = (t_buf[1] & 0x0000000F);
        connected = 1;
      } else if ((ethgenbyteswap(t_buf[1]) & 0xFFFFFFF0) == 0xFEEDB0B0) {
        serverVersion = (ethgenbyteswap(t_buf[1]) & 0x0000000F);
        connected = 1;
        /* Now let's see if the FSP was busy */
      } else if ((t_buf[1] == 0xB0B0DEAD) || (ethgenbyteswap(t_buf[1]) == 0xB0B0DEAD)) {
        /* FSP is busy, we need to wait and retry to connect later */
        if (retry_count++ < 5) {
          connected = 0;
          ::close(sock);
          sleep(1);
          continue;
        } else {
#ifndef OTHER_USE
          ecmdChipTarget target;
          out.stelaEvent(target,"FSPBUSY","FSPServer is busy, if this persists you may need to kill it and restart\n");
#endif
          out.error("eth_transfer::initialize","FSPServer is busy, if this persists you may need to kill it and restart\n");
          return TRANSFER_FSP_BUSY;
        }
      } else {
        out.error("eth_transfer::initialize","Unable to determine Byte Swap from Service Processor\n");
        return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
      }

      /* check if ther versions match */
      if(serverVersion != clientVersion) {
        /* we found an old server 
         * tell the user to upgrade the server */
        if(serverVersion < clientVersion) {
          out.error("eth_transfer::initialize","The fsp server with which you are attempting to connect only works with older clients.\n");
          out.error("eth_transfer::initialize","Please update the fsp server on the target system.\n");
        }
        /* we found a new server version
         * tell the user to upgrade their client
         * or go to an older fsp server level */
        else {
          out.error("eth_transfer::initialize","The fsp server with which you are attempting to connect only works with newer clients.\n");
          out.error("eth_transfer::initialize","Please switch to the latest dev level of Cronus or if you are attempting to use an archived version\n");
          out.error("eth_transfer::initialize","revert to an older fsp server on your system fsp.\n");
        }
        return TRANSFER_DEVICE_DRIVER_INIT_FAIL;
      }

    }


  } /* while !connected */

  // add sock to sock list
  // WARNING trusting that this code is called before threads are started or inside of another lock WARNING
  socks.push_back(sock);
  availableSocks.push_front(sock);

  return rc;
}

int eth_transfer::close(int socket) {
  int rc = 0;
  // remove socket from list and close
  rc = pthread_mutex_lock(&socksMutex);
  if (rc) {
    out.error("eth_transfer::close(int)", "pthread_mutex_lock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  socks.remove(socket);
  rc = pthread_cond_signal(&socksCondition);
  if (rc) {
    out.error("eth_transfer::close(int)", "pthread_cond_signal rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  rc = pthread_mutex_unlock(&socksMutex);
  if (rc) {
    out.error("eth_transfer::close(int)", "pthread_mutex_unlock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }

  ::close(socket);

  return 0;
}

int eth_transfer::close() {
  int rc = 0;
  /* get all sockets and close them */
  rc = pthread_mutex_lock(&socksMutex);
  if (rc) {
    out.error("eth_transfer::close()", "pthread_mutex_lock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  int originalMaxSocks = maxSocks;
  maxSocks = 0;
  // clear out any remaining sockets FIXME
  while (socks.empty() == false) {
    // clear out all available sockets
    while (availableSocks.empty() == false) {
      int sock = availableSocks.front();
      availableSocks.pop_front();
      socks.remove(sock);
      ::close(sock);
    }
    if (socks.empty() == false) {
      rc = pthread_cond_wait(&socksCondition, &socksMutex);
      if (rc) {
        out.error("eth_transfer::close()", "pthread_cond_wait rc = %d\n", rc);
        return TRANSFER_PTHREAD_FAIL;
      }
    }
  }

  maxSocks = originalMaxSocks;
  rc = pthread_cond_broadcast(&socksCondition); // wake everyone up
  if (rc) {
    out.error("eth_transfer::close()", "pthread_cond_broadcast rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  rc = pthread_mutex_unlock(&socksMutex);
  if (rc) {
    out.error("eth_transfer::close()", "pthread_mutex_unlock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }

  return 0;
}

int eth_transfer::send(std::list<Instruction *> & i_instruction, std::list<ecmdDataBuffer *> & o_resultData, std::list<InstructionStatus *> & o_resultStatus) {
  int rc = 0;
  // flatten instruction and send
  // check the sizes of the lists
  if (i_instruction.size() != o_resultData.size() || i_instruction.size() != o_resultStatus.size()) {
    out.error("eth_transfer::send", "sizes of input lists do not match");
    return TRANSFER_PIPE_SEND_FAIL;
  }

  /* get a seed for random numbers */
  uint32_t * seed = NULL;
  rc = pthread_mutex_lock(&seedsMutex);
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_lock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  if (seeds.empty()) {
    seed = new uint32_t;
    *seed = time(NULL) + 32000011 * seedCounter; // add mulitple of large prime
    seedCounter++;
  } else {
    seed = seeds.front();
    seeds.pop_front();
  }
  rc = pthread_mutex_unlock(&seedsMutex);
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_unlock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }

  uint32_t messageBufferSize = sizeof(uint32_t);
  for (std::list<Instruction *>::iterator instructionIterator = i_instruction.begin(); instructionIterator != i_instruction.end(); instructionIterator++) {
    messageBufferSize += sizeof(DataTransferInfo_t) + (*instructionIterator)->flattenSize();
  }

  uint8_t * messageBuffer = new (std::nothrow) uint8_t[messageBufferSize];

  uint32_t * numberOfInstructions = (uint32_t *) messageBuffer;

  std::map<uint32_t, ecmdDataBuffer *> instructionDataMap;
  std::map<uint32_t, InstructionStatus *> instructionStatusMap;

  *numberOfInstructions = htonl(i_instruction.size());

  uint32_t offset = sizeof(uint32_t);

  std::list<ecmdDataBuffer *>::iterator resultDataIterator = o_resultData.begin();
  std::list<InstructionStatus *>::iterator resultStatusIterator = o_resultStatus.begin();
  for (std::list<Instruction *>::iterator instructionIterator = i_instruction.begin(); instructionIterator != i_instruction.end(); instructionIterator++) {
    uint32_t instructionDataSize = (*instructionIterator)->flattenSize();
    uint32_t instructionBufferSize = sizeof(DataTransferInfo_t) + instructionDataSize;
    uint8_t * instructionBuffer = messageBuffer + offset;
    offset += instructionBufferSize;

    DataTransferInfo_t * instructionInfo = (DataTransferInfo_t *) instructionBuffer;
    instructionInfo->key = rand_r(seed);
    instructionInfo->type = (*instructionIterator)->getType();
    uint8_t * instructionData = instructionBuffer + sizeof(DataTransferInfo_t);
    rc = (*instructionIterator)->flatten(instructionData, instructionDataSize);
    if (rc) {
      out.error("eth_transfer::send", "problem flattening data for %s : %s\n",
             InstructionTypeToString((*instructionIterator)->getType()).c_str(),
             InstructionCommandToString((*instructionIterator)->getCommand()).c_str());
      delete [] messageBuffer;
      pthread_mutex_lock(&seedsMutex);
      seeds.push_front(seed);
      pthread_mutex_unlock(&seedsMutex);
      return TRANSFER_PIPE_SEND_FAIL;
    }

    /* make sure we don't have a duplicate key */
    while (instructionDataMap.count(instructionInfo->key) != 0) {
      instructionInfo->key = rand_r(seed);
    }

    instructionDataMap[instructionInfo->key] = *resultDataIterator;
    instructionStatusMap[instructionInfo->key] = *resultStatusIterator;
    resultDataIterator++;
    resultStatusIterator++;

    instructionInfo->key = htonl(instructionInfo->key);
    instructionInfo->type = htonl(instructionInfo->type);
    instructionInfo->size = htonl(instructionDataSize);

    if (record_performance) { commands[(*instructionIterator)->getCommand()]++; }
  }

  rc = pthread_mutex_lock(&seedsMutex);
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_lock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  seeds.push_front(seed);
  rc = pthread_mutex_unlock(&seedsMutex);
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_unlock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  seed = NULL;

  // get a socket to use
  int sock = 0;
  rc = pthread_mutex_lock(&socksMutex);
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_lock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  while (availableSocks.empty() == true) {
    if (socks.size() >= maxSocks){ // check if the max number of sockets have been created
      rc = pthread_cond_wait(&socksCondition, &socksMutex); if (rc) return rc;
      if (rc) {
        out.error("eth_transfer::send", "pthread_cond_wait rc = %d\n", rc);
        pthread_mutex_unlock(&socksMutex);
        return TRANSFER_PTHREAD_FAIL;
      }
    } else { // if not create another socket
      rc = initialize(NULL);
      if (rc) {
        pthread_mutex_unlock(&socksMutex);
        return rc;
      }
    }
  }
  sock = availableSocks.front();
  availableSocks.pop_front();
  rc = pthread_mutex_unlock(&socksMutex); if (rc) return rc;
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_unlock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }

  do {
    if (record_performance) { perf_num_writes++; perf_write_len += messageBufferSize; }
    rc = fd_write(sock, messageBuffer, messageBufferSize);
    if (rc == 0) { // EINTR seen
      continue;
    } else if (rc == -1) {
      out.error("eth_transfer::send", "problem writing data to server. errno = %s\n", strerror(errno));
      delete [] messageBuffer;
      pthread_mutex_lock(&socksMutex);
      availableSocks.push_front(sock);
      pthread_cond_signal(&socksCondition);
      pthread_mutex_unlock(&socksMutex);
      return TRANSFER_PIPE_SEND_FAIL;
    } else if (rc < (int) messageBufferSize) {
      out.error("eth_transfer::send", "Unhandled error. attempted to write %d bytes for instruction but wrote %d\n", sizeof(uint32_t), rc);
      delete [] messageBuffer;
      pthread_mutex_lock(&socksMutex);
      availableSocks.push_front(sock);
      pthread_cond_signal(&socksCondition);
      pthread_mutex_unlock(&socksMutex);
      return TRANSFER_PIPE_SEND_FAIL;
    }
  } while (0);

  delete [] messageBuffer;

  while (!instructionDataMap.empty() && !instructionStatusMap.empty()) {

    /* Let's wrap this buffer size read in timeout code so we can at least fail if the hang happens again */
    fd_set rset;
    struct timeval tv;

    FD_ZERO(&rset);
    FD_SET(sock, &rset);

    // Init
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    // 60 seconds is a huge amount of time to wait for this, but we'll be safe
    // Changed to 1200, or 20 minutes, since this is code actually waiting to hear back from the fsp server on a command
    // So if the command take a long time, like systempower on, we could have timed out with just the 60 seconds
    // Changed by JTA 05/07/09
    tv.tv_sec = 1200;
    //tv.tv_usec = 60000000;

    do {
      errno = 0;
      int sockFromSelect = select(sock+1, &rset, NULL, NULL, &tv);

      /* Error check our cases for <= 0 */
      if (sockFromSelect < 0 && errno == EINTR) {
        continue;
      } else if (sockFromSelect < 0) {
        out.error("eth_transfer::send","Error calling select: %d\n",errno);
        return TRANSFER_RECEIVE_FAIL;
      } else if (!sockFromSelect) {
#ifndef OTHER_USE
        ecmdChipTarget target;
        out.stelaEvent(target,"NETWORK_TIMEOUT","Timed out in drv_receive_data waiting to hear back from the FSP\n");
#endif
        out.error("eth_transfer::send","Timed out waiting to hear back from the FSP!\n");
        out.error("eth_transfer::send","For now, we are going to fail out instead of hanging.\n");
        return TRANSFER_RECEIVE_TIMEOUT;
      }
    } while (0);

    int bytesread = 0;
    uint32_t numberOfResults = 0;

    /* Read number of results */
    bytesread = read(sock, &numberOfResults, sizeof(uint32_t));
    if (bytesread != sizeof(uint32_t)) {
      out.error("eth_transfer::send","Service Processor didn't send back data length\n");
      close(sock);
      return TRANSFER_RECEIVE_FAIL;
    }
    if (record_performance) { perf_num_reads++; perf_read_len += bytesread; }

    numberOfResults = ntohl(numberOfResults);

    for(uint32_t j = 0; j < numberOfResults; j++) {
      /* Read result key */
      /* Read result type */
      /* Read result data size */
      DataTransferInfo_t resultInfo;
      do {
        bytesread = fd_read(sock, &resultInfo, sizeof(DataTransferInfo_t));
        if (bytesread == 0) { // EINTR seen
          continue;
        } else if (bytesread < (int) sizeof(DataTransferInfo_t)) {
          out.error("eth_transfer::send","Unhandled error. attempted to read %d bytes for number of results but received %d\n",
                  sizeof(DataTransferInfo_t), bytesread);
          close(sock);
          return TRANSFER_RECEIVE_FAIL;
        }
      } while (0);
      if (record_performance) { perf_num_reads++; perf_read_len += bytesread; }
      resultInfo.key = ntohl(resultInfo.key);
      resultInfo.type = ntohl(resultInfo.type);
      resultInfo.size = ntohl(resultInfo.size);

      if(instructionDataMap.count(resultInfo.key) == 0 && instructionStatusMap.count(resultInfo.key) == 0) {
        out.error("eth_transfer::send","data key (%08X) does not match any known instruction.\n", resultInfo.key);
        close(sock);
        return TRANSFER_RECEIVE_KEY_MISMATCH;
      }

      /* Read result object data */
      uint8_t * resultData = new (std::nothrow) uint8_t [resultInfo.size];
      if(resultData == NULL) {
        out.error("eth_transfer::send","problem allocating memory for result object data.\n");
        close();
        return TRANSFER_MEMALLOC_FAIL;
      }

      do {
        bytesread = fd_read(sock, resultData, resultInfo.size);
        if (bytesread == 0) { // EINTR seen
          continue;
        } else if (bytesread < (int) resultInfo.size) {
          out.error("eth_transfer::send","Unhandled error. attempted to read %d bytes for result but received %d\n",
                  resultInfo.size, bytesread);
          delete [] resultData;
          close(sock);
          return TRANSFER_RECEIVE_FAIL;
        }
      } while (0);
      if (record_performance) { perf_num_reads++; perf_read_len += bytesread; }

      if (resultInfo.type == ECMD_DBUF && instructionDataMap.count(resultInfo.key) != 0) {
        rc = instructionDataMap[resultInfo.key]->unflattenTryKeepCapacity(resultData, resultInfo.size);
        instructionDataMap.erase(resultInfo.key);
        if(rc != 0) {
          if (rc == ECMD_DBUF_NOT_OWNER) {
            out.error("eth_transfer::send","problem unflattening result object. Wrong size expected for unflatten.\n");
          } else {
            out.error("eth_transfer::send","problem unflattening result object.\n");
            delete [] resultData;
            close(sock);
            return TRANSFER_RECEIVE_FAIL;
          }
        }
      } else if (resultInfo.type == INSTRUCTION_STATUS && instructionStatusMap.count(resultInfo.key) != 0) {
        rc = instructionStatusMap[resultInfo.key]->unflatten(resultData, resultInfo.size);
        instructionStatusMap.erase(resultInfo.key);
        if(rc != 0) {
          if (rc == ECMD_DBUF_NOT_OWNER) {
            out.error("eth_transfer::send","problem unflattening result object. Wrong size expected for unflatten.\n");
          } else {
            out.error("eth_transfer::send","problem unflattening result object.\n");
            delete [] resultData;
            close(sock);
            return TRANSFER_RECEIVE_FAIL;
          }
        }
      } else {
        out.error("eth_transfer::send","Unknown result type.\n");
        delete [] resultData;
        close(sock);
        return TRANSFER_RECEIVE_FAIL;
      }
      delete [] resultData;
    }
  }

  rc = pthread_mutex_lock(&socksMutex);
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_lock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  availableSocks.push_front(sock);
  rc = pthread_cond_signal(&socksCondition);
  if (rc) {
    out.error("eth_transfer::send", "pthread_cond_signal rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  rc = pthread_mutex_unlock(&socksMutex);
  if (rc) {
    out.error("eth_transfer::send", "pthread_mutex_unlock rc = %d\n", rc);
    return TRANSFER_PTHREAD_FAIL;
  }
  return rc;
}

int eth_transfer::reset() {
  int rc = 0;

  close();
  sleep(1);
  rc = initialize(NULL);

  return rc;
}

/* ----------------------------------------------------- */
/* swaps bytes from 0x01234567 to 0x67452301             */ 
/* ----------------------------------------------------- */
uint32_t ethgenbyteswap(uint32_t data) {

  uint32_t dataout = 0;

  dataout  = ((data & 0x000000FF) << 24) |
             ((data & 0x0000FF00) <<  8) | 
             ((data & 0x00FF0000) >>  8) |
             ((data & 0xFF000000) >> 24);

  return dataout;

}
