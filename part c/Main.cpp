#include "Node.cpp"


int main() {
    char buf[20];

    cout << "Welcome! Now build a Node on the Network" << endl;
    cout << "Please Enter Node ID: " << endl;
    fgets(buf, 20, stdin);
    int id = stoi(buf);

    cout << "Please Enter Node IP: " << endl;
    fgets(buf, 20, stdin);
    char* ip = buf;
    // string ip(buf);

    cout << "Please Enter Node Port: " << endl;
    fgets(buf, 20, stdin);
    int port = stoi(buf);

    Node n(id, ip, port);  
    return 0;
}