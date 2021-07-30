#pragma once

#include <stdio.h>
#include <string.h>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;


/* enum for the function numbers */
enum Function { 
    Ack = 1,     // 1
    Nack,        // 2
    Connect,     // 3
    Discover,    // 4
    Route,       // 5
    Send,        // 6
    Relay        // 7
};


Function open_tcp_sock(string ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        return Nack;
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    return Ack;
}


class Node {

    private:
    int ID; 
    vector<Path> paths;  // saves all the paths from the current node to other nodes on the network

    public:

    Function do_command(string command) {
        size_t pos = command.find(",");
        string command_name = command.substr(0, pos);
        string info = command.substr(pos);  // after the ","
        switch (command_name) {
        case "setid":
            return setID(stoi(info));  // info contains the id only
        case "connect":
            size_t pos2 = command.find(":");
            string ip = info.substr(0, pos2);
            string port = info.substr(pos2); 
            if (open_tcp_sock(ip, stoi(port)) == Nack) {
                return Nack; 
            }
            return connect();  
        case "send":
            vector<string> results;
            boost::algorithm::split(results, info, boost::is_any_of(","));
            int id = stoi(results[0]);
            int len = stoi(results[1]);  // the number of chars on the message
            string message = results[2];
            int num_msgs = 0;  // number of the following msgs to be relayed .......

            Path path = getPath(id); 
            Node current = *this;
            for (i = 1; i < path.getSize()-2; i++) {
                Node next = path.at(i); 
                current.relay(next.getID(), num_msgs); 
                current = next;
            }
            return current.send(len, message); 
            // the message should contains the id / ip on a header or whatever  (?)
        case "route":
            return route(stoi(info));  // info contains the id only
        case "peers":
            return peers();
        }
        default:

    }

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