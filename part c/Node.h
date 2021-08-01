#pragma once

#include <stdio.h>
#include <string.h>
#include <vector>
// #include </usr/local/lib/boost/algorithm/string.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Path.h"
using namespace std;


// global variables
int MSG_ID = 1;  // a global counter to the number of messages
static char buff[512] = {0};  // max size of a message


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


class Node {

    private:
    int ID; 
    vector<Path*> paths = {};  // saves all the paths from the current node to other nodes on the network

    public:

    Function do_command(string command);

    // getter
    int getID() {
        return ID;
    }

    // setter
    void setID(int id) {
        this -> ID = id; 
    }

    // functions
    Function connect() {}
    Function discover(int destID) {}
    Function route(int discover_message_id, int ans_len, int[] nodes) {}
    Function send(int len, string message) {}
    Function relay(int nextID, int num_msgs) {}
    Function peers() {}
    Path getPath(int destID) {}  // returns the path from paths vector if exists, or search one

    //constructor
    Node(int id) : ID(id) {}

    //destructor
    ~Node() {}
};