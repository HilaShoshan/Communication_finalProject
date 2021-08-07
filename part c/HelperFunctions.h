#include <iostream>
#include <vector>

using namespace std;


/* for the switch-case on do_command */
Command hashit (std::string const& inString) {
    if (inString == "setid") return _setid;
    if (inString == "connect") return _connect;
    if (inString == "send") return _send;
    if (inString == "route") return _route;
    if (inString == "peers") return _peers;
    return _illegle_command;
}


/* get the index of an element in vector by the id of the node */
int getIndexByID(vector<list<string>> v, int id) {
    int id2; 
    for (int i = 0; i < v.size(); i++) {
        id2 = stoi(v[i].front());  // the id is the first one on the list
        if (id == id2) {
            return i;
        }
    }
    return -1;
}


int getIndexByVal(vector<int> v, int val) {
    for (int i = 0; i < v.size(); i++) {
        if (v[i] == val) {
            return i;
        }
    }
    return -1;
}


string create_connect_payload(int port, char* IP) {
    string port_bytes, ip_len_bytes;
    addZero(port_bytes, port);
    port_bytes += to_string(port);
    string ip(IP);
    addZero(ip_len_bytes, ip.length());
    ip_len_bytes += to_string(ip.length());
    return port_bytes+ip_len_bytes+ip;
}