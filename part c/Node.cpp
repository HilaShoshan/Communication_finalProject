#include "Node.h" 
#include "Message.h"

using namespace std;


Function open_tcp_sock(const char* ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        return Nack;
    }
    server_addr.sin_family = AF_INET;  // IPV4
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
            if (open_tcp_sock(char_arr, stoi(port)) == Nack) {
                return Nack; 
            }
            return connect();  
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
        return route(stoi(info));  // info contains the id only
            
    case _peers:
        return peers();
    
    default:
        return Nack;
    }
}


Function Node::connect() {

    // act as client and send a connect message

    if (connect(sockfd, (const struct sockaddr*) &serveraddr, sizeof(serveraddr)) < 0) {
        return Nack;
    }
    struct Message msg = {MSG_ID, this->ID, 0, 0, Function::Connect, ""};  // connect message
    MSG_ID++;
    send(sockfd, &msg, strlen(msg), 0);  

    // act as server and listen to the return message of the other node

    if(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return Nack;
    }
    if (listen(sockfd, LINSTENPORT) == 0) {
	    cout << "Node " << this->ID << " Listening ..." << endl;
    } else {
        return Nack;
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockfd, (struct sockaddr*) &new_addr, &addr_size);
    len = sizeof(buff); 
    if (getsockopt(sockfd, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) { 
        return Nack;
    } 
    valread = read(new_sock, buff, 512);  // read the message from buffer
    cout << "valread: " << valread << endl;
    
}