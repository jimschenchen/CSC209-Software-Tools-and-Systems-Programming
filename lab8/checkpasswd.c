#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXLINE 256

#define SUCCESS "Password verified\n"
#define INVALID "Invalid password\n"
#define NO_USER "No such user\n"

int main(void) {
  char user_id[MAXLINE];
  char password[MAXLINE];

  /* The user will type in a user name on one line followed by a password 
     on the next.
     DO NOT add any prompts.  The only output of this program will be one 
	 of the messages defined above.
     Please read the comments in validate carefully.
   */

  if(fgets(user_id, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  if(fgets(password, MAXLINE, stdin) == NULL) {
      perror("fgets");
      exit(1);
  }
  
  // TODO
  // display overlength
  if (strlen(user_id) > 11) {
    printf(NO_USER);
    exit(0);
  }
  if (strlen(password) > 11) {
    printf(INVALID);
    exit(0);
  }

  int fd[2], r;
  
  if (pipe(fd) == -1) {
    perror("pipe");
    exit(1);
  }

  if ((r = fork()) > 0) { // Parent
    // close fd[0]
    if (close(fd[0]) == -1) {
      perror("close");
      exit(1);
    };
    // Reset STDOUT to pipe
    // if (dup2(fd[1], STDOUT_FILENO) == -1) {
    //   perror("dup2");
    //   exit(1);
    // }
    // close fd[1]
    if (close(fd[1]) == -1) {
      perror("close");
      exit(1);
    };

    // wait for child
    int status;
    if (wait(&status) == -1) {
      perror("wait");
      exit(1);
    }
    if (WIFEXITED(status)) {
      if (WEXITSTATUS(status) == 0) {
        printf(SUCCESS);
      } else if (WEXITSTATUS(status) == 2) {
        printf(INVALID);
      } else if (WEXITSTATUS(status) == 3) {
        printf(NO_USER);
      } else {
        exit(1);
      }
    }

  } else if (r == 0) {  // Child
    // Reset STDIN to pipe
    if (dup2(fd[0], STDIN_FILENO) == -1) {
      perror("dup2");
      exit(1);
    }

    // close fd[0]
    if (close(fd[0]) == -1) {
      perror("close");
      exit(1);
    }

    // write
    if (write(fd[1], user_id, 10) == -1) {
      perror("write");
    }
    if (write(fd[1], password, 10) == -1) {
      perror("write");
    }

    // close fd[1]
    if (close(fd[1]) == -1) {
      perror("close");
      exit(1);
    };

    if (execl("./validate", "validate", NULL) == -1) {
      perror("execl");
    }
  } else { // fork err
    perror("fork");
    exit(1);
  }
  return 0;
}
