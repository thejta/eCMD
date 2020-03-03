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

#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <netdb.h>
#include <poll.h>
#include <limits.h>

#include <pthread.h>

#include <sys/sem.h>	/* semaphore ops */
#include <signal.h>	/* signal() */

#include <vector>
#include <list>
#include <map>

#include <Instruction.H>
#include <ControlInstruction.H>
#include <ServerFSIInstruction.H>
#include <ServerGPIOInstruction.H>
#include <ServerI2CInstruction.H>
#include <ServerSPIInstruction.H>
#include <PNORInstruction.H>
#include <ServerSBEFIFOInstruction.H>
#include <ServerBrkptInstruction.H>

#include <OutputLite.H>
OutputLite out;

#include <fd_impl.H>


bool global_exit = false;
bool global_multi_client = true;
bool global_server_debug = false;
std::list<FlightRecorderEntry> global_flight_recorder;
std::map<std::string, uint32_t> global_version_map;
std::map<uint64_t, uint32_t> global_resource_map;

Authorization global_auth;

// TODO list
// Do we want to handle Resend of data?

/****************************************************************************/
/* Socket stuff                                                             */
/****************************************************************************/

enum socketState { SOCKET_RUNNING, SOCKET_ENDING};

/****************************************************************************/
/* clientThread thread function                                             */
/****************************************************************************/

void processClient(int socket, enum socketState & state, bool & keyValid, uint32_t & key)
{
    int rc = 0;

    /* initialize ServerControls structure */
    ServerControls controls;
    controls.global_exit_pointer = &global_exit;
    controls.global_multi_client_pointer = &global_multi_client;
    controls.global_auth_pointer = &global_auth;
    controls.threadKeyValid_pointer = &keyValid;
    controls.threadKey_pointer = &key;
    controls.global_flight_recorder_pointer = &global_flight_recorder;
    //controls.global_client_mutex_pointer = &global_client_mutex;
    controls.global_version_map_pointer = &global_version_map;

    do
    {
        /* Beginning of Process Intructions */
        std::vector<std::pair<DataTransferInfo_t, Instruction *> > instructionList;

        /* Read in instruction stream */
        int bytesread = 0;
        uint32_t numberOfInstructions = 0;

        /* Read number of instructions */
        bytesread = read(socket, &numberOfInstructions, sizeof(uint32_t));
        if (global_server_debug)
        {
            printf("socket %d bytesread = %d\n", socket, bytesread);
        }
        if (bytesread == -1)
        {
            printf("ERROR : socket %d : problem reading data from client. errno = %s\n", socket, strerror(errno));
            continue;
        }
        else if (bytesread == 0)
        {
            state = SOCKET_ENDING;
            break;
        }
        else if (bytesread < (int) sizeof(uint32_t))
        {
            printf("ERROR : socket %d : Unhandled error. attempted to read %zd bytes for number of instructions but received %d\n", socket, sizeof(uint32_t), bytesread);
            state = SOCKET_ENDING;
            break;
        }

        numberOfInstructions = ntohl(numberOfInstructions);
        if (global_server_debug)
        {
            printf("socket %d numberOfInstructions = %d\n", socket, numberOfInstructions);
        }

        for (uint32_t i = 0; i < numberOfInstructions; i++)
        {
            /* Read instruction key */
            /* Read instruction type */
            /* Read instruction data size */
            DataTransferInfo_t instructionInfo;
            bytesread = fd_read(socket, &instructionInfo, sizeof(DataTransferInfo_t));
            if (bytesread == -1)
            {
                printf("ERROR : socket %d : problem reading data from client. errno = %s\n", socket, strerror(errno));
                state = SOCKET_ENDING;
                break;
            }
            else if (bytesread == 0)
            {
                state = SOCKET_ENDING;
                break;
            }
            else if (bytesread < (int) sizeof(DataTransferInfo_t))
            {
                printf("ERROR : socket %d : Unhandled error. attempted to read %zd bytes for number of instructions but received %d\n",
                    socket, sizeof(DataTransferInfo_t), bytesread);
                state = SOCKET_ENDING;
                break;
            }

            instructionInfo.key = ntohl(instructionInfo.key);
            instructionInfo.type = ntohl(instructionInfo.type);
            instructionInfo.size = ntohl(instructionInfo.size);

            if (global_server_debug)
            {
                printf("socket %d instructionInfo.key = %08X\n", socket, instructionInfo.key);
                printf("socket %d instructionInfo.type = %s\n", socket,
                    InstructionTypeToString((Instruction::InstructionType) (instructionInfo.type)).c_str());
                printf("socket %d instructionInfo.size = %u\n", socket, instructionInfo.size);
            }

            /* Read instruction object data */
            uint8_t * instructionData = new (std::nothrow) uint8_t [instructionInfo.size];
            if(instructionData == NULL)
            {
                printf("ERROR : socket %d : problem allocating memory for instruction object data.\n", socket);
                state = SOCKET_ENDING;
            break;
            }

            bytesread = fd_read(socket, instructionData, instructionInfo.size);
            if (bytesread == -1)
            {
                printf("ERROR : socket %d : problem reading data from client. errno = %s\n", socket, strerror(errno));
                state = SOCKET_ENDING;
                break;
            }
            else if (bytesread == 0)
            {
                state = SOCKET_ENDING;
                break;
            }
            else if (bytesread < (int) instructionInfo.size)
            {
                printf("ERROR : socket %d : Unhandled error. attempted to read %d bytes for instruction but received %d\n",
                    socket, instructionInfo.size, bytesread);
                state = SOCKET_ENDING;
                break;
            }

            /* Create the instruction */
            Instruction * newInstruction;
            switch(instructionInfo.type)
            {
                case Instruction::CONTROL:
                    newInstruction = new ControlInstruction(&controls);
                    break;
                case Instruction::FSI:
                    newInstruction = new ServerFSIInstruction();
                    break;
                case Instruction::GPIO:
                    newInstruction = new ServerGPIOInstruction();
                    break;
                case Instruction::I2C:
                    newInstruction = new ServerI2CInstruction();
                    break;
                case Instruction::SPI:
                    newInstruction = new ServerSPIInstruction();
                    break;
                case Instruction::PNOR:
                    newInstruction = new PNORInstruction();
                    break;
                case Instruction::SBEFIFO:
                    newInstruction = new ServerSBEFIFOInstruction();
                    break;
                case Instruction::BRKPT:
                    newInstruction = new ServerBrkptInstruction();
                    break;
                default:
                    printf("ERROR : socket %d : Unknown instruction received %d\n", socket, instructionInfo.type);
                    newInstruction = new Instruction();
                    break;
            }

            if(newInstruction == NULL)
            {
                printf("ERROR : socket %d : problem allocating memory for instruction object.\n", socket);
                state = SOCKET_ENDING;
                delete [] instructionData;
                break;
            }

            /* unflatten the instruction */
            rc = newInstruction->unflatten(instructionData, instructionInfo.size);
            if(rc != 0)
            {
                printf("ERROR : socket %d : problem unflattenting instruction object.\n", socket);
                // unflatten error will be handled in execute
            }

            delete [] instructionData;

            instructionList.push_back(std::pair<DataTransferInfo_t, Instruction *>(instructionInfo, newInstruction));
        } //for (int i = 0; i < numberOfInstructions; i++)


        /* this handle map is used to keep handles open between consequtive instructions */
        std::map<uint64_t, Handle *> handleMap;
        /* these maps will store data and status until we return it to the user */
        std::map<uint32_t, ecmdDataBuffer *> dataMap;
        std::map<uint32_t, InstructionStatus *> statusMap;
        /* this uint32_t is used to store to previous instructions rc, used to determine if next instruction should be executed */
        uint32_t previous_rc = SERVER_COMMAND_COMPLETE;
        /* Execute Instructions */
        std::vector<std::pair<DataTransferInfo_t, Instruction *> >::iterator currentInstruction = instructionList.begin();
        while(currentInstruction != instructionList.end())
        {
            Handle * currentHandle = NULL;
            uint64_t currentHash = currentInstruction->second->getHash();
            /* look if we have saved this handle */
            if(handleMap.count(currentHash) != 0)
            {
                currentHandle = handleMap[currentHash];
            }

            ecmdDataBuffer * data = new ecmdDataBuffer;
            InstructionStatus * status = new InstructionStatus;

            bool resource_available = false;

            if ((currentInstruction->second->getType() == Instruction::FSI) ||
                (currentInstruction->second->getType() == Instruction::FSISTREAM))
            {
                resource_available = true;
                if (global_server_debug)
                {
                    printf("socket %d using FSI resource " UINT64_HEX_VARIN_FORMAT(%016) "\n", socket, currentHash);
                }
                /* do not lock resources that have hash == 0x0 */
            }
            else if (currentHash != 0x0)
            {
                /* lock the resource based upon the handle */
                if (global_resource_map.count(currentHash) != 0)
                {
                    // someone else has the resource
                    rc = status->rc = SERVER_RESOURCE_IN_USE;
                    status->instructionVersion = 0xFFFFFFFF;
                }
                else
                {
                    global_resource_map[currentHash] = socket;
                    resource_available = true;
                    if (global_server_debug)
                    {
                        printf("socket %d locking resource " UINT64_HEX_VARIN_FORMAT(%016) "\n", socket, currentHash);
                    }
                }
            }
            else
            {
                resource_available = true;
            }

            if(resource_available == true)
            {
                /* check if previous command failed */
                if (previous_rc != SERVER_COMMAND_COMPLETE)
                {
                    rc = status->rc = SERVER_PREVIOUS_INSTRUCTION_FAILED;
                    status->instructionVersion = 0xFFFFFFFF;
                }
                /* check if authorization enabled and valid*/
                else if ((global_auth.enabled && keyValid &&
                          (global_auth.keyMap.count(key) != 0)) ||
                         (!global_auth.enabled))
                {
                    rc = currentInstruction->second->execute(*data, *status, &currentHandle);
                }
                else if (currentInstruction->second->getType() == Instruction::CONTROL &&
                         (currentInstruction->second->getCommand() == Instruction::INFO || 
                          currentInstruction->second->getCommand() == Instruction::AUTH || 
                          currentInstruction->second->getCommand() == Instruction::ADDAUTH ||
                          currentInstruction->second->getCommand() == Instruction::CLEARAUTH))
                {
                    /* allow authentication and info commands */
                    rc = currentInstruction->second->execute(*data, *status, &currentHandle);
                }
                else
                {
                    /* do not allow other commands */
                    rc = status->rc = SERVER_AUTHORIZATION_NEEDED;
                    status->errorMessage = std::string("Server is locked. Authorization needed. Contact : ") +
                        global_auth.keyMap[global_auth.firstKey];
                }

                // add instruction info to the flight recorder
                FlightRecorderEntry newEntry;
                newEntry.type = currentInstruction->second->getType();
                newEntry.command = currentInstruction->second->getCommand();
                newEntry.vars = currentInstruction->second->getInstructionVars(*status);
                global_flight_recorder.push_back(newEntry);
                while(global_flight_recorder.size() > 100)
                {
                    global_flight_recorder.pop_front();
                }

                if ((currentInstruction->second->getType() == Instruction::FSI) ||
                    (currentInstruction->second->getType() == Instruction::FSISTREAM))
                {
                    if (global_server_debug)
                    {
                        printf("socket %d not using FSI resource " UINT64_HEX_VARIN_FORMAT(%016) "\n", socket, currentHash);
                    }
                }
                else if (currentHash != 0x0)
                {
                    /* unlock the resource based upon the handle */
                    global_resource_map.erase(currentHash);
                    if (global_server_debug)
                    {
                        printf("socket %d unlocking resource " UINT64_HEX_VARIN_FORMAT(%016) "\n", socket, currentHash);
                    }
                }
            } // end of if (resource_available == true)

            /* keep return code for next instruction */
            previous_rc = rc;

            /* see if handle changed */
            if((currentHandle != NULL) && (handleMap.count(currentHash) == 0))
            {
                // new handle
                handleMap[currentHash] = currentHandle;
                if (global_server_debug)
                {
                    printf("socket %d instruction hash " UINT64_HEX_VARIN_FORMAT(%016) " saving handle \n", socket, currentHash);
                }
            }
            else if(handleMap.count(currentHash) != 0)
            {
                // if we already had this handle
                if(handleMap[currentHash] != currentHandle)
                {
                    // check for changed handle
#if 0
                    // CODE NOT USED
                    if(handleMap[currentHash] != NULL)
                    {
                        // close old handle
                        Handle * tempHandle = handleMap[currentHash];
                        currentInstruction->second->closeHandle(&tempHandle);
                    }
#endif
                    if(currentHandle != NULL)
                    {
                        // if this is a real handle
                        handleMap[currentHash] = currentHandle;
                    }
                    else
                    {
                        // if the handle has been closed
                        handleMap.erase(currentHash);
                    }
                }
            }

            /* save data and status */
            dataMap[currentInstruction->first.key] = data;
            statusMap[currentInstruction->first.key] = status;

            currentInstruction++;
        }

        /* create a buffer to send back all data and status */
        uint32_t resultBufferSize = sizeof(uint32_t);
        uint32_t numberOfResults = 0;

        currentInstruction = instructionList.begin();
        while(currentInstruction != instructionList.end())
        {
            uint32_t resultDataSize = dataMap[currentInstruction->first.key]->flattenSizeMinCap();
            uint32_t resultStatusSize = statusMap[currentInstruction->first.key]->flattenSize();
            resultBufferSize += sizeof(DataTransferInfo_t) * 2 + resultDataSize + resultStatusSize;
            numberOfResults += 2;
            currentInstruction++;
        }

        uint8_t * resultBuffer = new (std::nothrow) uint8_t[resultBufferSize];
        uint32_t * numberOfResults_p = (uint32_t *) resultBuffer;
        *numberOfResults_p = htonl(numberOfResults);

        uint32_t offset = sizeof(uint32_t);

        currentInstruction = instructionList.begin();
        while(currentInstruction != instructionList.end())
        {
            if (global_server_debug)
            {
                printf("socket %d sending back data for instruction key %08X\n", socket, currentInstruction->first.key);
            }

            /* Return results */
            uint32_t resultDataSize = dataMap[currentInstruction->first.key]->flattenSizeMinCap();
            uint32_t resultStatusSize = statusMap[currentInstruction->first.key]->flattenSize();

            DataTransferInfo_t * dataResultInfo = (DataTransferInfo_t *) (resultBuffer + offset);
            dataResultInfo->key = currentInstruction->first.key;
            dataResultInfo->type = ECMD_DBUF;
            uint8_t * resultData = resultBuffer + offset + sizeof(DataTransferInfo_t);
            rc = dataMap[currentInstruction->first.key]->flattenMinCap(resultData, resultDataSize);
            if (rc)
            {
                printf("ERROR : problem flattening data for %s : %s\n",
                    InstructionTypeToString(currentInstruction->second->getType()).c_str(),
                    InstructionCommandToString(currentInstruction->second->getCommand()).c_str());
            }
            offset += sizeof(DataTransferInfo_t) + resultDataSize;

            DataTransferInfo_t * statusResultInfo = (DataTransferInfo_t *) (resultBuffer + offset);
            statusResultInfo->key = currentInstruction->first.key;
            statusResultInfo->type = INSTRUCTION_STATUS;
            uint8_t * resultStatus = resultBuffer + offset + sizeof(DataTransferInfo_t);
            rc = statusMap[currentInstruction->first.key]->flatten(resultStatus, resultStatusSize);
            if (rc)
            {
                printf("ERROR : problem flattening status for %s : %s\n",
                    InstructionTypeToString(currentInstruction->second->getType()).c_str(),
                    InstructionCommandToString(currentInstruction->second->getCommand()).c_str());
            }
            offset += sizeof(DataTransferInfo_t) + resultStatusSize;

            dataResultInfo->key = htonl(dataResultInfo->key);
            dataResultInfo->type = htonl(dataResultInfo->type);
            dataResultInfo->size = htonl(resultDataSize);
            statusResultInfo->key = htonl(statusResultInfo->key);
            statusResultInfo->type = htonl(statusResultInfo->type);
            statusResultInfo->size = htonl(resultStatusSize);

            currentInstruction++;
        }

        /* block SIGPIPE while we send data back to the client */
        sigset_t newSignalMask, oldSignalMask, pendingSignalMask;
        sigemptyset(&newSignalMask);
        sigaddset(&newSignalMask, SIGPIPE);
        pthread_sigmask(SIG_BLOCK, &newSignalMask, &oldSignalMask);

        rc = fd_write(socket, resultBuffer, resultBufferSize);
        if (rc == -1)
        {
            printf("ERROR : socket %d : problem writing data to client. errno = %s\n", socket, strerror(errno));
        }
        else if (rc < (int) resultBufferSize)
        {
            printf("ERROR : socket %d : Unhandled error. attempted to write %zd bytes for instruction but wrote %d\n", socket, sizeof(uint32_t), rc);
        }

        /* check if we have received SIGPIPE while we have blocked it */
        sigpending(&pendingSignalMask);
        while (sigismember(&pendingSignalMask, SIGPIPE))
        {
            /* clear out signals until we have no SIGPIPEs waiting */
            int signalFound = 0;
            sigwait(&pendingSignalMask, &signalFound);
            sigpending(&pendingSignalMask);
        }

        /* put back the original signal mask */
        pthread_sigmask(SIG_SETMASK, &oldSignalMask, NULL);

        delete [] resultBuffer;

        /* Cleanup instructionList */
        currentInstruction = instructionList.begin();
        while(currentInstruction != instructionList.end())
        {
             uint64_t currentHash = currentInstruction->second->getHash();
            if(handleMap.count(currentHash) != 0)
            {
                if(handleMap[currentHash] != NULL)
                {
                    if (global_server_debug)
                    {
                        printf("socket %d instruction hash " UINT64_HEX_VARIN_FORMAT(%016) " closing handle \n", socket, currentHash);
                    }
                    Handle * tempHandle = handleMap[currentHash];
                    currentInstruction->second->closeHandle(&tempHandle);
                }
                handleMap.erase(currentHash);
            }

            /* delete data and status */
            delete dataMap[currentInstruction->first.key];
            delete statusMap[currentInstruction->first.key];
            dataMap.erase(currentInstruction->first.key);
            statusMap.erase(currentInstruction->first.key);

            delete currentInstruction->second;
            currentInstruction->second = NULL;
            currentInstruction++;
        }

    } while(0);

    return;
}

/****************************************************************************/
/* main function                                                            */
/****************************************************************************/

int main (int argc, char **argv)
{
    int rc = 0;
    uint16_t ip_port = 8192; /* Default Cronus port number */
    int serverSocket = 0;

    uint32_t serverVersion = 0x1;

    uint32_t hello[3] = { 0x8, 0xFEEDB0B0};
    hello[0] = htonl(hello[0]);
    hello[1] = hello[1] | serverVersion;
    hello[1] = htonl(hello[1]);

    uint32_t busy[3] = { 0x8, 0xB0B0DEAD};
    busy[0] = htonl(busy[0]);
    busy[1] = htonl(busy[1]);

    errno = 0;

    /* Initialize auth */
    global_auth.enabled = false;
    global_auth.keyMap.clear();
    global_auth.firstKey = 0;

    /* Intialize global_version_map */
    global_version_map["server"] = 0x1;
    {
        global_version_map["ControlInstruction"] = ControlInstruction().getVersion();
        global_version_map["FSIInstruction"] = ServerFSIInstruction().getVersion();
        global_version_map["GPIOInstruction"] = ServerGPIOInstruction().getVersion();
        global_version_map["I2CInstruction"] = ServerI2CInstruction().getVersion();
        global_version_map["SPIInstruction"] = ServerSPIInstruction().getVersion();
        global_version_map["PNORInstruction"] = PNORInstruction().getVersion();
        global_version_map["SBEFIFOInstruction"] = ServerSBEFIFOInstruction().getVersion();
        global_version_map["SBEFIFOInstruction"] = ServerSBEFIFOInstruction().getVersion();
        global_version_map["BrkptInstruction"] = ServerBrkptInstruction().getVersion();
    }

    /* parse command line options */
    for (int i = 1; i < argc; i++)
    {
        char * currentArg = argv[i];
        // ip_port
        if (strcmp(currentArg, "-port") == 0)
        {
            i++;
            if (i >= argc)
            {
                printf("ERROR : port number must be specified after -port argument\n");
                return 1;
            }
            currentArg = argv[i];
            int tempPort = 0;
            rc = sscanf(currentArg, "%d", &tempPort);
            if (rc != 1)
            {
                printf("ERROR : port number must be specified after -port argument\n");
                return 1;
            }
            ip_port = (uint16_t) tempPort;

            // debug
        }
        else if (strcmp(currentArg, "-debug") == 0)
        {
            global_server_debug = true;
        }
    }

    /* Lookup Host Address */
    char hostname[100];
    rc = gethostname(hostname, 100);
    if (rc)
    {
        printf("ERROR : Unable to determine hostname!\n");
        exit(1);
    }

    if (global_server_debug)
    {
        printf("Hostname : %s Port: %hu\n",hostname, ip_port);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ip_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* Open up Socket */
    serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, IPPROTO_TCP);
    if(serverSocket == -1)
    {
        printf("ERROR : problem getting socket. errno = %s\n", strerror(errno));
        exit(1);
    }

    int yes = 1;
    rc = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (rc == -1)
    {
        printf("ERROR : problem setting socket options to reuse socket. errno = %s\n", strerror(errno));
        exit(1);
    }

    rc = bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr));
    if (rc == -1)
    {
        printf("ERROR : problem opening socket. errno = %s\n", strerror(errno));
        printf("Wait and try again ... \n");
        exit(1);
    }

    if (global_server_debug)
    {
        printf("Bound Socket\n");
    }

    rc = listen(serverSocket, SOMAXCONN);
    if (rc == -1)
    {
        printf("ERROR : problem listening to socket. errno = %s\n", strerror(errno));
        exit(1);
    }

    if (global_server_debug)
    {
        printf("Listening to Socket\n");
    }

    int OPEN_MAX = 200;
    struct pollfd clients[OPEN_MAX];
    clients[0].fd = serverSocket;
    clients[0].events = POLLRDNORM;
    for (int client = 1; client < OPEN_MAX; client++)
    {
        clients[client].fd = -1;
    }
    std::map<uint32_t, bool> keyValidMap;
    std::map<uint32_t, uint32_t> keyMap;

    int maxClient = 0;

    printf(" --- Cronus Socket Deamon Initialized ...\n");
    printf(" --- Built on : %s %s CST\n", __DATE__, __TIME__);

    while(!global_exit)
    {
        int timeout = 100 * 1000; /* Timeout in 100 secs */

        int clientsReady = poll(clients, maxClient + 1, timeout);
        if (global_server_debug) printf("poll returned %d\n", clientsReady);

        if (clientsReady < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            printf("ERROR : problem calling poll. errno = %s\n", strerror(errno));
            close(serverSocket);
            exit(1);
        }
        else if (clientsReady == 0)
        {
            /* We timed out waiting for data */
            continue;
        }

        if (clients[0].revents & POLLRDNORM)
        {
            /* open new connection from a client */
            struct sockaddr_in remote;
            socklen_t addrlen = sizeof(struct sockaddr_in);
            int new_sock = accept(serverSocket, (struct sockaddr *) &remote, &addrlen);

            if (new_sock < 0)
            {
                printf("ERROR : problem calling accept for new socket. errno = %s\n", strerror(errno));
                close(serverSocket);
                exit(1);
            }
            if (global_server_debug)
            {
                printf("connection from host %s, port %d, socket %d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port), new_sock);
            }

            /* add new client to poll list */
            int client = 0;
            for (client = 1; client < OPEN_MAX; client++)
            {
                if (clients[client].fd == -1)
                {
                    clients[client].fd = new_sock;
                    clients[client].events = POLLRDNORM;
                    keyValidMap[client] = false;
                    keyMap[client] = 0x0;
                    break;
                }
            }

            /* Send a word across so the client to set communication version */
            int write_len = fd_write(new_sock, hello, 2 * sizeof(uint32_t));
            if (write_len < 0)
            {
                printf("**** ERROR : Problems sending hello to new client : errno = %s\n", strerror(errno));
            }
            else if (write_len != (2 * sizeof(uint32_t)))
            {
                printf("**** ERROR : Unable to send entire data to client : exp %zd - act %d\n", 2 * sizeof(uint32_t), write_len);
            }

            if (client > maxClient)
            {
                maxClient = client;
            }

            if (--clientsReady <= 0)
            {
                /* checked all descriptors */
                continue;
            }
        }

        /* process client requests */
        for (int client = 1; client <= maxClient; client++)
        {
            struct pollfd & clientRef = clients[client];
            if (clientRef.fd < 0)
            {
                continue;
            }
            if (clientRef.revents & (POLLRDNORM | POLLERR))
            {
                enum socketState state = SOCKET_RUNNING;
                processClient(clientRef.fd, state, keyValidMap[client], keyMap[client]);

                if (state != SOCKET_RUNNING)
                {
                    close(clientRef.fd);
                    clientRef.fd = -1;
                }
            }

            if (--clientsReady <= 0)
            {
                /* checked all descriptors */
                continue;
            }
        }
    } // while(!global_exit)

    return rc;
}
