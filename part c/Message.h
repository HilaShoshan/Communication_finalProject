#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;


struct Message {
    int msg_id = 0;
    int src_id = 0;
    int dest_id = 0;
    int num_trailing_msg = 0;
    int func_id = 0;
    char* payload = "";
};


/* get 4 chars (bytes) and return the int */
int bytesToInt(char a, char b, char c, char d) {
    int n = 0;
    n = n + (a & 0x000000ff);
    n = n + ((b & 0x000000ff) << 8);
    n = n + ((c & 0x000000ff) << 16);
    n = n + ((d & 0x000000ff) << 24);
    return n;
}

void addZero(string& s, int i) {
    if (i < 10) {
        s += "000";
    }
    else if (i < 100) {
        s += "00";
    }
    else if (i < 1000) {
        s += "0";
    }
}


string make_str_msg (Message msg) {
    string bytes;
    int int_fields[5] = {msg.msg_id, msg.src_id, msg.dest_id, msg.num_trailing_msg, msg.func_id};
    for(int i = 0; i < 5; i++) {
        addZero(bytes, int_fields[i]);
        bytes += to_string(int_fields[i]);
        cout << "bytes: " << bytes << endl;
    }
   if ((msg.payload != NULL) && (msg.payload[0] == '\0')) {
   printf("payload is empty and the message is : \n");
   return bytes;
   }
   else{
    bytes += msg.payload;
    return bytes;
   }
}