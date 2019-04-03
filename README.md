CSUF-CPSC-351-SP19
Assignment 1
Section 2

 Alex Ho hho114@csu.fullerton.edu

– The programming language: C++

– Execute in Linux terminal.

– How to execute the program.

  Go into directory and run "make" command.
  Then execute "./sender keyfile.txt" to send file to receiver.
  Until it notices "Wait until the receiver send us a message.", press Ctrl + Z to pause sender.
  Now we could execute the receiver "./receiver" to read the message.
  Until it notices "Read the message", press Ctrl + Z to pause receiver.
  Resume the job of sender by type "fg %1", it will notices finished detach from memory.
  Finally, resume the job of receiver "fg %2", it will notices finished IPC.
