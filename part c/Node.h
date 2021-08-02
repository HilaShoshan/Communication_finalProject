#pragma once

#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
// #include </usr/local/lib/boost/algorithm/string.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
// #include "Path.h"
#define SIZE 512


// global variables
int MSG_ID = 1;  // a global counter to the number of messages
static char buff[SIZE] = {0};  // max size of a message
struct sockaddr_in server_addr;


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
    _peers
};


class Node {

    private:
    int ID; 
    std::vector<std::vector<Node>> paths = {};  // saves all the paths from the current node to other nodes on the network

    public:

    Function do_command(std::string command);

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
    // Path getPath(int destID) {}  // returns the path from paths vector if exists, or search one

    //constructor
    Node(int id) : ID(id) {}

    //destructor
    ~Node() {}
};