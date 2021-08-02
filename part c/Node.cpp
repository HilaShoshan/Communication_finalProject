#include "Node.h" 
#include "Message.h"

using namespace std;


Function init_ip_port(const char* ip, int port) {
    server_addr.sin_family = AF_INET;  // IPv4
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    return Ack;
}


Command hashit (std::string const& inString) {
    if (inString == "setid") return _setid;
    if (inString == "connect") return _connect;
    if (inString == "send") return _send;
    if (inString == "route") return _route;
    if (inString == "peers") return _peers;
}


char* make_str_msg (Message msg) {
    char* bytes = new char[SIZE];
    int arr[] = {msg.msg_id, msg.src_id, msg.dest_id, msg.num_trailing_msg, msg.func_id};
    int index = 0;
    for (int field : arr) {  // convert a int into 4 bytes char*
        bytes[index] = (field >> 24) & 0xFF;
        bytes[index+1] = (field >> 16) & 0xFF;
        bytes[index+2] = (field >> 8) & 0xFF;
        bytes[index+3] = field & 0xFF;
        index+=4;
    }
    for (char c = *msg.payload; c; c=*++msg.payload) {
        bytes[index] = c; 
        index++;
    }
    return bytes;
}


Function Node::do_command(string command) {

    size_t pos = command.find(",");
    string command_name = command.substr(0, pos);
    string info = command.substr(pos);  // after the ","
    char* char_arr;

    switch (hashit(command_name)) {

    case _setid:
        setID(stoi(info));  // info contains the id only
        return Ack;

    case _connect:
        {
            size_t pos2 = command.find(":");
            string ip = info.substr(0, pos2);
            char_arr = &ip[0];
            string port = info.substr(pos2); 
            if (init_ip_port(char_arr, stoi(port)) == Nack) {
                return Nack; 
            }
            return myconnect();  
        }

    case _send:
        return Nack;
        /* 
        vector<string> results;
        boost::algorithm::split(results, info, boost::is_any_of(","));
        int id = stoi(results[0]);
        int len = stoi(results[1]);  // the number of chars on the message
        string message = results[2];
        if (len != message.length()) {
            return Nack;
        } 

        int num_msgs = 0;  // number of the following msgs to be relayed .......

        Path path = getPath(id); 
        Node current = *this;
        for (i = 1; i < path.getSize()-2; i++) {  // send relay messages until i-2
            Node next = path.at(i); 
            current.relay(next.getID(), num_msgs); 
            current = next;
        }
        return current.send(len, message); 
        // the message should contains the id / ip on a header or whatever  (?)
        */
    case _route:
        // return route(stoi(info));  // info contains the id only
        return Nack;
            
    case _peers:
        // return peers();
        return Nack;
    
    default:
        return Nack;
    }
}


Function Node::myconnect() {

    // open tcp socket
    
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        return Nack;
    }

    // act as client and send a connect message

    if (connect(sockfd, (const struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        return Nack;
    }
    char* payload = {nullptr};
    struct Message msg = {MSG_ID, this->ID, 0, 0, Function::Connect, payload};  // connect message
    MSG_ID++;
    char* str_msg = make_str_msg(msg);
    send(sockfd, &str_msg, strlen(str_msg), 0);  
    delete[] str_msg;

    // act as server and listen to the return message of the other node

    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return Nack;
    }
    if (listen(sockfd, 10) == 0) {
	    cout << "Node " << this->ID << " Listening ..." << endl;
    } else {
        return Nack;
    }
    socklen_t addr_size = sizeof(new_addr);
    int new_sock = accept(sockfd, (struct sockaddr*) &new_addr, &addr_size);
    socklen_t len = sizeof(buff); 
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buff, &len) != 0) { 
        return Nack;
    } 
    int valread = read(new_sock, buff, SIZE);  // read the message from buffer
    cout << "valread: " << valread << endl;
    
}