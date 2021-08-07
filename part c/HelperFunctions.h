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


/* get the index of an element in vector */
int getIndex(vector<list<string>> v, int id) {
    int id2; 
    for (int i = 0; i < v.size(); i++) {
        id2 = stoi(v[i].front());  // the id is the first one on the list
        if (id == id2) {
            return i;
        }
    }
    return -1;
}