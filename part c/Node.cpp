#include "Node.h" 
#include "Message.h"

using namespace std;


/* open a socket, listen to inputs */
void Node::listen_to_inputs() {
    int ret, opt = 1;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);  
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port = htons(this->Port);
    // Forcefully attaching socket to the port
    if (bind(listenfd, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);
    if (listen(listenfd, 10) < 0) {  // 10 = the max length to which the queue of pending connections for sockfd may grow
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while(1) {
        Function response;
        printf("waiting for input...\n");
	    ret = wait_for_input();
	    printf("fd: %d is ready. reading...\n", ret);
	    read(ret, buff, 512);
        if (ret == 0) {  // is a command from the keyboard
            // cout << "keyboard, buff = " << buff << endl;
            response = do_command(buff);
            memset(buff, 0, sizeof buff);
        }
        else if (ret == listenfd) {  // someone wants to connect me  
            // cout << "listenfd, buff = " << buff << endl;
            int addrlen = sizeof(my_addr);
            if ((new_sock = accept(listenfd, (struct sockaddr*)&my_addr, (socklen_t*)&addrlen)) < 0) {
                response = Nack;
            }
            printf("adding fd1(%d) to monitoring\n", new_sock);
            add_fd_to_monitoring(new_sock);
            response = Ack;
        }
        else {  // another message (in the given form, start with id)
            if (strlen(buff) != 0) {  // check if the node has disconnected
                // cout << "else, buff = " << buff << endl;
                response = check_msg(buff, ret);
                memset(buff, 0, sizeof buff);
            }
        }
        if(response == Ack) printf("Ack\n");
        else printf("Nack\n");  
    }
}


Function Node::open_tcp_socket(const char* ip, int port) {
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        return Nack;
    }
    printf("Successfully open a TCP socket.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    // server_addr.sin_addr.s_addr = inet_addr(ip);

    if(inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return Nack;
    }
    return Ack; 
}


/* for the switch-case on do_command */
Command hashit (std::string const& inString) {
    if (inString == "setid") return _setid;
    if (inString == "connect") return _connect;
    if (inString == "send") return _send;
    if (inString == "route") return _route;
    if (inString == "peers") return _peers;
    return _illegle_command;
}


Function Node::do_command(string command) {

    size_t pos = command.find(",");
    size_t pos2;
    string command_name = command.substr(0, pos);
    string info = command.substr(pos+1);  // after the ","
    char* char_arr;

    string err_msg = "Wrong command syntax, please try again";

    switch (hashit(command_name)) {

    case _setid:
        try {
            setID(stoi(info));  // info contains the id only
            return Ack;
        }
        catch(const std::exception& e) {
            cout << err_msg << endl;
            return Nack;
        }

    case _connect:
        {
            try {
                pos2 = info.find(":");
                string ip = info.substr(0, pos2);
                char_arr = &ip[0];
                string port = info.substr(pos2+1); 
                if (open_tcp_socket(char_arr, stoi(port)) == Nack) {
                    return Nack; 
                }
                return myconnect();  
            }
            catch(const std::exception& e) {
                cout << err_msg << endl;
                return Nack;
            }
        }

    case _send:
        {
            int id = stoi(info.substr(0, pos));
            info = info.substr(pos+1);  // override info
            int len = stoi(info.substr(0, pos));
            string message = info.substr(pos+1);
            if (len != message.length()) {
                return Nack;
            } 
            cout << "params: id = " << id << " len = " << len << " message = " << message << endl;
            std::vector<int> path = getPath(id); 
            if(path.empty())  // no path has found
                return Nack;
        }
        /*
        int num_msgs = 0;  // number of the following msgs to be relayed .......
        Node current = *this;
        for (i = 1; i < path.getSize()-2; i++) {  // send relay messages until i-2
            int next = path.at(i); 
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
        return peers();
    
    default:
        cout << err_msg << endl;
        return Nack;
    }
}


Function Node::check_msg(string msg, int ret) {
    int src_id = bytesToInt(buff[4], buff[5], buff[6], buff[7]); 
    int dest_id = bytesToInt(buff[8], buff[9], buff[10], buff[11]);  
    int func_id = bytesToInt(buff[16], buff[17], buff[18], buff[19]);  
    if (func_id == Function::Connect) {  // check if its a connect message
        char payload[4];
        for (int i = 0; i < 4; i++) {  // copy msg_id (of the connect msg) to the payload 
            payload[i] = buff[i];      // check it .... &*(%#)
        }
        Function response = Ack;
        if (dest_id != this->ID)  // the message is not for me
            response = Nack;
        struct Message msg = {MSG_ID, this->ID, src_id, 0, response, payload};  // ack ot nack message
        MSG_ID++;
        string str_msg = make_str_msg(msg);
        const char* chars_msg = str_msg.c_str();
        if (send(ret, chars_msg, str_msg.length(), 0) == -1) {
            return Nack;
        }
        // save the id, ip and port of the node (?)
        return Ack; 
    } 
    return Nack;
}


Function Node::myconnect() {
    int e = connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        return Nack;
    }
    printf("adding fd1(%d) to monitoring\n", server_sock);
    add_fd_to_monitoring(server_sock);
    const char* payload = "";
    struct Message msg = {MSG_ID, this->ID, 0, 0, Function::Connect, payload};  
    MSG_ID++;
    string str_msg = make_str_msg(msg);
    const char* chars_msg = str_msg.c_str();
    if (send(server_sock, chars_msg, str_msg.length(), 0) == -1) {
        perror("send");
    }
    int valread = read(server_sock, buff, 512);
    // cout << "the message is : " << buff << endl;
    int func_id = bytesToInt(buff[16], buff[17], buff[18], buff[19]); 
    if (func_id == Function::Ack) {  // check if its an Ack message
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &server_addr.sin_addr, ip, sizeof(ip));
        int port = ntohs(server_addr.sin_port);
        int src_id = bytesToInt(buff[4], buff[5], buff[6], buff[7]);
        // cast all to string and insert to the list
        std::string str_ip(ip);
        list<string> l = {to_string(src_id), str_ip, to_string(port)};
        this->neighbors.push_back(l);
        this->sockets.push_back(server_sock);
        cout << "Connected to Node with ID = " << src_id << endl;
        return Ack;
    }
    return Nack;
}


vector<int> Node::getPath(int destID) {
    for(vector<int> &path : paths) {
        if(path.back() == destID)  // this is a path to the destination node
            return path;
    }
    // a path to destination does not exist yet
    if(discover(destID) == Ack)
        return paths.back();  // the last path is the last one that added 
    else 
        return vector<int>();
}


/* only the first node (the one who want to sent the message) uses this */
Function Node::discover(int destID) {
    for(auto &list : neighbors) {
        if(list.front() == to_string(destID)) {  // the destination node is neighbor of this node
            vector<int> path = { this->ID, destID };
            paths.push_back(path);
            return Ack;
        }
    }
    string bytes; 
    addZero(bytes, this->ID);
    bytes = bytes + to_string(this->ID) + " ";

    const char* payload = bytes.c_str();  // add the id of the current node to payload
    return recursive_discover(destID, payload);   // sendind discover message to neighbors
}


Function Node::recursive_discover(int destID, const char* payload) {
    // send a discover message to all the neighbors (that are not in the payload)
    for(int i = 0; i < neighbors.size(); i++) {  // neighbor is a list {id,ip,port} all strings
        auto neighbor = neighbors[i];
        int neig_id = stoi(neighbor.front());  // the id is the first one on the list
        // if(std::count(got_msg.begin(), got_msg.end(), neig_id) == 0) {  // the neighbor didn't get the message 
        //     const char* payload = "";
        //     struct Message msg = {MSG_ID, this->ID, neig_id, 0, Function::Discover, payload};
        //     MSG_ID++;
        //     string str_msg = make_str_msg(msg);
        //     const char* chars_msg = str_msg.c_str();
        //     if (send(server_sock, chars_msg, str_msg.length(), 0) == -1) {
        //         perror("send");
        //     } 
        //     continue;
        // }
        // if we got here, all the neighbors already got the message
        return Nack; 
    }
   return Nack;

}


Function Node::peers() {
//     std::copy(neighbors[0].begin(),
//       neighbors[0].end(),
//       std::ostream_iterator<std::string>(std::cout + "--> "));
//       return Ack;
return Nack;
}
