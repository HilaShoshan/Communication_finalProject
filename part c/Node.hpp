#pragma once

#include <stdio.h>
#include <string.h>
#include <boost/algorithm/string.hpp>
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

class Node {

    private:
    int ID; 

    public:

    void get_command(string command) {
        size_t pos = command.find(",");
        string command_name = command.substr(0, pos);
        string info = command.substr(pos);  // after the ","
        switch (command_name) {
        case "setid":
            setID(stoi(info));  // info contains the id only
            break;
        case "connect":
            size_t pos2 = command.find(":");
            string ip = info.substr(0, pos2);
            string port = info.substr(pos2); 
            connect(ip, port);  // returns ACK or NACK
            break;
        case "send":
            vector<string> results;
            boost::algorithm::split(results, info, boost::is_any_of(","));
            int id = stoi(results[0]);
            int len = stoi(results[1]);  // the number of chars on the message
            string message = results[2];
            send(id, len, message);
            break;
        case "route":
            route(stoi(info));  // info contains the id only
            break;
        case "peers":
            peers();
        }
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
    Function connect(string ip, string port) {}
    Function discover(int destID) {}
    Function send(int id, int len, string message) {}
    Function route(int id) {}
    Function peers() {}

    //constructor
    Node(int id) : ID(id) {}

    //destructor
    ~Node() {}
};