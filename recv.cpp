
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<iostream>
#include "msg.h"    /* For the message struct */
using namespace std;

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";

message sndMsg, rcvMsg;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	/* TODO: 1. Create a file called keyfile.txt containing string "Hello world" (you may do
 		    so manually or from the code).
	         2. Use ftok("keyfile.txt", 'a') in order to generate the key.
		 3. Use the key in the TODO's below. Use the same key for the queue
		    and the shared memory segment. This also serves to illustrate the difference
		    between the key and the id used in message queues and shared memory. The id
		    for any System V object (i.e. message queues, shared memory, and sempahores)
		    is unique system-wide among all System V objects. Two objects, on the other hand,
		    may have the same key.
	 */
	 key_t key = ftok("keyfile.txt",'a');
	 cout<<"Creating ftok key."<<endl;
	 if(key  == -1)
	 {
			 cerr<<"Fail to create ftok key"<<endl;
			 exit(1);
	 }
	 else {
		 cout<<"ftok key is successfully created."<<endl;
	 }


	/* TODO: Allocate a piece of shared memory. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */
	cout<<"Create the id of shared memory segment."<<endl;
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666|IPC_CREAT);
	if(shmid == -1)
	{
		cerr << "Fail to create the shared memory segment id." << endl;
		exit(1);
	}
	else
	{
		cout<<"The id of shared memory segment is successfully created. "<<endl;
	}

	/* TODO: Attach to the shared memory */
	cout<<"Attach to the shared memory"<<endl;
	sharedMemPtr = shmat(shmid,(void *) 0, 0);
	if(sharedMemPtr == (void*) -1)
	{
		cerr<<"Fail to attach to the shared memory"<<endl;
		exit(1);
	}
	else
	{
		cout<<"Successfully attach to the shared memory."<<endl;
	}
	/* TODO: Create a message queue */
	cout<<"Attach to the message queue."<<endl;
	msqid = msgget(key, 0666 | IPC_CREAT);
	if(msqid == -1)
	{
		cerr<<"Fail to attach to the message queue."<<endl;
	}
	else
	{
		cout<<"Successfully attach to the message queue."<<endl;
	}
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */

}


/**
 * The main loop
 */
void mainLoop()
{
	/* The size of the mesage */
	int msgSize = 0;

	/* Open the file for writing */
	FILE* fp = fopen(recvFileName, "w");

	/* Error checks */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}

    /* TODO: Receive the message and get the message size. The message will
     * contain regular information. The message will be of SENDER_DATA_TYPE
     * (the macro SENDER_DATA_TYPE is defined in msg.h).  If the size field
     * of the message is not 0, then we copy that many bytes from the shared
     * memory region to the file. Otherwise, if 0, then we close the file and
     * exit.
     *
     * NOTE: the received file will always be saved into the file called
     * "recvfile"
     */

	/* Keep receiving until the sender set the size to 0, indicating that
 	 * there is no more data to send
 	 */
	 msgSize++;

	while(msgSize != 0)
	{

		cout<<"Read new message."<<endl;
		if(msgrcv(msqid, &rcvMsg, sizeof(rcvMsg), SENDER_DATA_TYPE, 0) == -1)
		{
			cerr<<"Fail to read new message."<<endl;
			exit(1);
		}else{
			cout<<"Successfully read a new message."<<endl;
		}

		msgSize = rcvMsg.size;


		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}

			/* TODO: Tell the sender that we are ready for the next file chunk.
 			 * I.e. send a message of type RECV_DONE_TYPE (the value of size field
 			 * does not matter in this case).
 			 */
 			cout<<"Ready to read the next file chunk."<<endl;
 			sndMsg.mtype = RECV_DONE_TYPE;
			sndMsg.size = 0;

			cout<<"Send an empty message back."<<endl;
			if(msgsnd(msqid, &sndMsg, 0, 0) == -1)
			{
				cerr<<"Fail to send empty message back."<<endl;
			}else{
				cout<<"Successfully send empty message back."<<endl;
			}
		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
	}
}



/**
 * Perfoms the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */

void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* TODO: Detach from shared memory */
	cout<<"Detach from shared memory."<<endl;
	if(shmdt(sharedMemPtr) == -1)
	{
		cerr << "Fail to detach from shared memory."<<endl;
		exit(1);
	}
	else
	{
		cout<<"Successfully detach from shared memory."<<endl;
	}
	/* TODO: Deallocate the shared memory chunk */
	cout<<"Deallocate the shared memory chunk."<<endl;
	if(shmctl(shmid, IPC_RMID, NULL) == -1)
	{
		cerr<<"Fail to deallocate the shared memory chunk."<<endl;
		exit(1);
	}
	else
	{
		cout<<"Successfully deallocate the shared memory chunk."<<endl;
	}
	/* TODO: Deallocate the message queue */
	cout<<"Deallocate the mesage queue."<<endl;
	if(msgctl( msqid, IPC_RMID, NULL) == -1)
	{
		cerr<<"Fail to deallocate the message queue."<<endl;
		exit(1);
	}
	else
	{
		cout<<"Successfully deallocate the message queue."<<endl;
	}

}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */

void ctrlCSignal(int signal)
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{

	/* TODO: Install a singnal handler (see signaldemo.cpp sample file).
 	 * In a case user presses Ctrl-c your program should delete message
 	 * queues and shared memory before exiting. You may add the cleaning functionality
 	 * in ctrlCSignal().
 	 */
	 signal(SIGINT,ctrlCSignal);
	/* Initialize */
	init(shmid, msqid, sharedMemPtr);

	/* Go to the main loop */
	mainLoop();

	/** TODO: Detach from shared memory segment, and deallocate shared memory and message queue (i.e. call cleanup) **/
	cleanUp(shmid, msqid, sharedMemPtr);

	cout<<"***********Successfully Inter Process Communication***************"<<endl;
	return 0;
}
