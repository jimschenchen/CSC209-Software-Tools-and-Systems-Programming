#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


#define BUF_SIZE 128

#define MAX_AUCTIONS 5
#ifndef VERBOSE
#define VERBOSE 0
#endif

#define ADD 0
#define SHOW 1
#define BID 2
#define QUIT 3

/* Auction struct - this is different than the struct in the server program
 */
struct auction_data {
    int sock_fd;
    char item[BUF_SIZE];
    int current_bid;
};

/* Displays the command options available for the user.
 * The user will type these commands on stdin.
 */

void print_menu() {
    printf("The following operations are available:\n");
    printf("    show\n");
    printf("    add <server address> <port number>\n");
    printf("    bid <item index> <bid value>\n");
    printf("    quit\n");
}

/* Prompt the user for the next command 
 */
void print_prompt() {
    printf("Enter new command: ");
    fflush(stdout);
}


/* Unpack buf which contains the input entered by the user.
 * Return the command that is found as the first word in the line, or -1
 * for an invalid command.
 * If the command has arguments (add and bid), then copy these values to
 * arg1 and arg2.
 */
int parse_command(char *buf, int size, char *arg1, char *arg2) {
    int result = -1;
    char *ptr = NULL;
    if(strncmp(buf, "show", strlen("show")) == 0) {
        return SHOW;
    } else if(strncmp(buf, "quit", strlen("quit")) == 0) {
        return QUIT;
    } else if(strncmp(buf, "add", strlen("add")) == 0) {
        result = ADD;
    } else if(strncmp(buf, "bid", strlen("bid")) == 0) {
        result = BID;
    } 
    ptr = strtok(buf, " "); // first word in buf

    ptr = strtok(NULL, " "); // second word in buf
    if(ptr != NULL) {
        strncpy(arg1, ptr, BUF_SIZE);
    } else {
        return -1;
    }
    ptr = strtok(NULL, " "); // third word in buf
    if(ptr != NULL) {
        strncpy(arg2, ptr, BUF_SIZE);
        return result;
    } else {
        return -1;
    }
    return -1;
}

/* Connect to a server given a hostname and port number.
 * Return the socket for this server
 */
int add_server(char *hostname, int port) {
        // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("client: socket");
        exit(1);
    }
    
    // Set the IP and port of the server to connect to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    struct addrinfo *ai;
    
    /* this call declares memory and populates ailist */
    if(getaddrinfo(hostname, NULL, NULL, &ai) != 0) {
        close(sock_fd);
        return -1;
    }
    /* we only make use of the first element in the list */
    server.sin_addr = ((struct sockaddr_in *) ai->ai_addr)->sin_addr;

    // free the memory that was allocated by getaddrinfo for this list
    freeaddrinfo(ai);

    // Connect to the server.
    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("client: connect");
        close(sock_fd);
        return -1;
    }
    if(VERBOSE){
        fprintf(stderr, "\nDebug: New server connected on socket %d.  Awaiting item\n", sock_fd);
    }
    return sock_fd;
}
/* ========================= Add helper functions below ========================
 * Please add helper functions below to make it easier for the TAs to find the 
 * work that you have done.  Helper functions that you need to complete are also
 * given below.
 */

/* Print to standard output information about the auction
 */
void print_auctions(struct auction_data *a, int size) {
    printf("Current Auctions:\n");

    /* TODO Print the auction data for each currently connected 
     * server.  Use the follosing format string:
     *     "(%d) %s bid = %d\n", index, item, current bid
     * The array may have some elements where the auction has closed and
     * should not be printed.
     */
    for (int i = 0; i < MAX_AUCTIONS; i++) {
        if (a[i].sock_fd > -1) {
            printf("(%d) %s bid = %d\n", i, a[i].item, a[i].current_bid);
        }
    }
    
}

/* Process the input that was sent from the auction server at a[index].
 * If it is the first message from the server, then copy the item name
 * to the item field.  (Note that an item cannot have a space character in it.)
 */
void update_auction(char *buf, int size, struct auction_data *a, int index) {
    
    // TODO: Complete this function
    // server : "%s %d %ld", a->item, a->highest_bid, t->tv_sec
    char *ptr = NULL;
    int seconds;

    ptr = strtok(buf, " "); // first word in buf
    if(ptr != NULL) {
        if (a[index].item[0] == '\0') {
            strncpy(a[index].item, ptr, BUF_SIZE);
        }
    } else {
        fprintf(stderr, "ERROR malformed bid: %s", buf);
        return;
    }

    ptr = strtok(NULL, " "); // second word in buf
    if(ptr != NULL) {
        a[index].current_bid = strtol(ptr, NULL, 10);
    } else {
        fprintf(stderr, "ERROR malformed bid: %s", buf);
        return;
    }

    ptr = strtok(NULL, " "); // third word in buf
    if(ptr != NULL) {
        seconds = strtol(ptr, NULL, 10);
    } else {
        fprintf(stderr, "ERROR malformed bid: %s", buf);
        return;
    }

    printf("\nNew bid for %s [%d] is %d (%d seconds left)\n", a[index].item, a[index].sock_fd, a[index].current_bid, seconds);

    // fprintf(stderr, "ERROR malformed bid: %s", buf);
    // printf("\nNew bid for %s [%d] is %d (%d seconds left)\n",           );
}

// helper funciton for handling add
// return the fd
int handle_add (struct auction_data *a, char *hostname, int port, char *name) {
    // search the available auctions
    int auction_index = 0;
    while (auction_index < MAX_AUCTIONS && a[auction_index].sock_fd != -1) {
        auction_index++;
    }
    if (auction_index == MAX_AUCTIONS) {
        fprintf(stderr, "client: max auction connections\n");
        return -1;
    }

    // do not need to check the bad inputs
    int fd = add_server(hostname, port);
    if (fd < 0) {
        fprintf(stderr, "client: add_server");
        close(fd);
        exit(1);
    }
    // add sock fd to a
    a[auction_index].sock_fd = fd;

    // write name to server
    write(a[auction_index].sock_fd, name, sizeof(name));

    return fd;
}


int main(void) {

    char name[BUF_SIZE];

    // Declare and initialize necessary variables
    // TODO
    struct auction_data a[MAX_AUCTIONS];
    for (int i = 0; i < MAX_AUCTIONS; i++) {
        a[i].current_bid = -1;
        a[i].item[0] = '\0';
        a[i].sock_fd = -1;
    }

    // Get the user to provide a name.
    printf("Please enter a username: ");
    fflush(stdout);
    int num_read = read(STDIN_FILENO, name, BUF_SIZE);
    if(num_read <= 0){
        fprintf(stderr, "ERROR: read from stdin failed\n");
        exit(1);
    }
    print_menu();

    // TODO
    // init select
    int max_fd = STDIN_FILENO;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(STDIN_FILENO, &all_fds);

    while(1) {
        print_prompt();
        // TODO

        // copy all fds
        fd_set listen_fds = all_fds;
        int nready;
        if ((nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL)) == -1) {
            perror("client: select");
            exit(1);
        }

        // read from STDIN_FILENO
        if (FD_ISSET(STDIN_FILENO, &listen_fds)) {
            char command[BUF_SIZE];
            char arg1[BUF_SIZE];
            char arg2[BUF_SIZE];
            int num_read = read(STDIN_FILENO, command, BUF_SIZE);
            if(num_read <= 0){
                fprintf(stderr, "ERROR: read from stdin failed\n");
                exit(1);
            }
            int res = parse_command(command, sizeof(command), arg1, arg2);
            switch(res) {
                case SHOW: 
                    print_auctions(a, sizeof(a));
                    break;
                case QUIT:
                    for (int i = 0; i < MAX_AUCTIONS; i++) {
                        // close all open fd
                        if (a[i].sock_fd != -1) {   
                            close(a[i].sock_fd);
                            a[i].sock_fd = -1;
                        }
                    }
                    exit(0);
                    // break;
                case ADD: {
                    int fd = handle_add(a, arg1, strtol(arg2, NULL, 10), name);
                    if (fd > -1) {
                        FD_SET(fd, &all_fds);
                        max_fd = fd > max_fd ? fd : max_fd;
                    }     
                    break;  
                }
                case BID: {
                    int item_index = strtol(arg1, NULL, 10);
                    int bid_value = strtol(arg2, NULL, 10);
                    char buf[BUF_SIZE];
                    sprintf(buf, "%d", bid_value);
                    write(a[item_index].sock_fd, buf, sizeof(buf));
                    break;
                }
                default:
                    fprintf(stderr, "Usage: auction_server [-v] [-t timeout] [-p port] item\n");
                    exit(1);
            }
        }
        
        // all auctions
        for (int i = 0; i < MAX_AUCTIONS; i++) {
            int server_status = -1;
            if (a[i].sock_fd > -1 && FD_ISSET(a[i].sock_fd, &listen_fds)) {
                char buf[BUF_SIZE];
                server_status = read(a[i].sock_fd, buf, sizeof(buf));
                if (server_status < 0) {
                    perror("read");
                    exit(0);
                }
                update_auction(buf, sizeof(buf), a, i);
            }

            // server disconnect
            if (server_status == 0) {
                FD_CLR(a[i].sock_fd, &all_fds);
            }
        }

    }
    return 0; // Shoud never get here
}
