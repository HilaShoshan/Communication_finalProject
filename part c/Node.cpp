#include "Node.h" 
#include "Message.h"

using namespace std;


/* open a socket, listen to inputs */
void Node::listen_to_inputs() {
    int ret;
    
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&my_addr, '0', sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr(this->IP);
    my_addr.sin_port = htons(this->Port); 

    bind(listenfd, (struct sockaddr*)&my_addr, sizeof(my_addr));

    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);
    listen(listenfd, 10);

    while(1) {
        socklen_t addr_size = sizeof(new_addr);
        new_sock = accept(listenfd, (struct sockaddr*)&new_addr, &addr_size);
        if (new_sock != -1) {
            printf("adding fd1(%d) to monitoring\n", new_sock);
            add_fd_to_monitoring(new_sock);
        }

        printf("waiting for input...\n");
	    ret = wait_for_input();
	    printf("fd: %d is ready. reading...\n", ret);
	    read(ret, buff, 512);
        if (isalpha(buff[0])) {  // is a command
            Function response = do_command(buff);
            if(response == Ack) 
                printf("Ack\n");
            else 
                printf("Nack\n");
        }
        else {  // another message (in the given form, start with id)
            int dest_id = bytesToInt(buff[4], buff[5], buff[6], buff[7]);
            int func_id = bytesToInt(buff[16], buff[17], buff[18], buff[19]);  
            if (func_id == Function::Connect) {  // check if its a connect message
                char payload[4];
                for (int i = 0; i < 4; i++) {  // copy msg_id (of the conect msg) to the payload 
                    payload[i] = buff[i];  
                }
                struct Message msg = {MSG_ID, this->ID, dest_id, 0, Function::Ack, payload};  // connect message
                MSG_ID++;
                char* str_msg = make_str_msg(msg);
                send(new_sock, &str_msg, strlen(str_msg), 0);  
            }  // if something wrong, send Nack ....
        }
        
    }
}


Function Node::open_tcp_socket(const char* ip, int port) {
    socklen_t len;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        return Nack;
    }
    printf("Successfully open TCP socket.\n");

    server_addr.sin_family = AF_INET;
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
    size_t pos2;
    string command_name = command.substr(0, pos);
    string info = command.substr(pos+1);  // after the ","
    char* char_arr;

    switch (hashit(command_name)) {

    case _setid:
        setID(stoi(info));  // info contains the id only
        return Ack;

    case _connect:
        {
            pos2 = info.find(":");
            string ip = info.substr(0, pos2);
            char_arr = &ip[0];
            string port = info.substr(pos2+1); 
            if (open_tcp_socket(char_arr, stoi(port)) == Nack) {
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
        cout << "Wrong command syntax, please try again" << endl;
        return Nack;
    }
}


Function Node::myconnect() {

    int e = connect(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(e == -1) {
        cout << "shit" << endl;
        return Nack;
    }

    char* payload = {nullptr};
    struct Message msg = {MSG_ID, this->ID, 0, 0, Function::Connect, payload};  
    MSG_ID++;
    char* str_msg = make_str_msg(msg);
    send(server_sock, &str_msg, strlen(str_msg), 0);  
    int valread = read(server_sock, buff, 512);
    int func_id = bytesToInt(buff[16], buff[17], buff[18], buff[19]);  
    if (func_id == Function::Ack) {  // check if its an Ack message
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &server_addr.sin_addr, ip, sizeof ip);
        int port = ntohs(server_addr.sin_port);
        int src_id = bytesToInt(buff[4], buff[5], buff[6], buff[7]);
        // cast all to string and insert to the list
        std::string str_ip(ip);
        list<string> l = {to_string(src_id), str_ip, to_string(port)};
        this->neighbors.push_back(l);
        this->sockets.push_back(server_sock);
        cout << "Connected to Node with ID = " << src_id << endl;
    }
    delete[] str_msg;
}


Function Node::discover(int destID) {
    for(auto &list : neighbors) {
        if(list.front() == to_string(destID)) {  // the destination node is neighbor of this node
            vector<int> path = { this->ID, destID };
            paths.push_back(path);
            return Ack;
        }
    }
    // send a discover message to all the neighbors
    vector<int> got_msg = {};  // saves all the nodes that got the discover msg
    for(int i = 0; i < neighbors.size(); i++) {  // neighbor is a list {id,ip,port} all strings
        auto neighbor = neighbors[i];
        int neig_id = stoi(neighbor.front());
        if(std::count(got_msg.begin(), got_msg.end(), neig_id) == 0) {  // the neighbor didn't get the message 
            char* payload = {nullptr};
            struct Message msg = {MSG_ID, this->ID, neig_id, 0, Function::Discover, payload};
            MSG_ID++;
            char* str_msg = make_str_msg(msg);
            int neig_sock = sockets[i]; 
            send(neig_sock, &str_msg, strlen(str_msg), 0);  
            continue;
        }
        // if we got here, all the neighbors already got the message
        return Nack; 
    }

}