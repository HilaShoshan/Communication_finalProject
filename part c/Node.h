#pragma once

#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <ctype.h>
#include <algorithm>
#include "select.h"

#define SIZE 512
#define MAX 100


// global variables
int MSG_ID = 1;  // a global counter to the number of messages
static char buff[SIZE] = {0};  // max size of a message
char input_buff[1025];  // max size to get inputs


/* enum for the function numbers */
enum Function { 
    Ack = 1,        
    Nack = 2,       
    Connect = 4,    
    Discover = 8,   
    Route = 16,     
    Send = 32,      
    Relay = 64      
};


/* enum for the user commands, for switch-case */
enum Command {  
    _setid,
    _connect,
    _send,
    _route,
    _peers,
    _illegle_command
};


class Node {

    private:

    int ID = 0; 
    int Port;
    char* IP;

    std::vector<std::vector<int>> paths = {};  // saves all the paths from the current node to other nodes on the network
    std::vector<std::list<std::string>> neighbors = {};  // each list is: {id, ip, port}
    std::vector<int> sockets = {};  // neighbors sockets, corresponding to neighbors vector
    struct sockaddr_in my_addr, server_addr;
    int listenfd, server_sock, new_sock;

    public:

    void listen_to_inputs();
    Function do_command(std::string command);
    Function check_msg(std::string msg, int ret);

    // getter
    int getID() {
        return ID;
    }

    // setter
    void setID(int id) {
        this -> ID = id; 
    }

    // command & helper functions
    Function myconnect();
    Function discover(int destID);
    Function route(int discover_message_id, int ans_len, std::vector<int> nodes);
    Function mysend(int len, std::string message);
    Function relay(int nextID, int num_msgs);
    Function peers();
    std::vector<int> getPath(int destID);  // returns the path from paths vector if exists, or search one
    Function open_tcp_socket(const char* ip, int port);

    //constructor
    Node(char* ip, int port) : IP(ip), Port(port) {
        printf("Successfuly created a Node with ID = %d, Port = %d, IP = %s\n", 
                                            ID, Port, IP); 
        listen_to_inputs();
    }

    //destructor 
    ~Node() {
        /* swap the contents of the vector into a temporary 
        that will get destroyed and free the memory */
        std::vector<std::vector<int>>().swap(paths);
    } 
};