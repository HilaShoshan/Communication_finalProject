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
    char* payload = {nullptr};
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

char* make_str_msg (Message msg) {
    char* bytes = new char[SIZE];
    char *c=bytes;
    int arr[5] = {msg.msg_id, msg.src_id, msg.dest_id, msg.num_trailing_msg, msg.func_id};
    for(int i = 0; i < 5; i++) {
        int field = arr[i];
        const char* bytes_int = (to_string(field)).c_str();  // make a 4-bytes char* 
        const char *d=bytes_int;
        while(*c++ = *d++);  // copy the int to the bytes array
    }
    printf("bytes %s\n", bytes); 
    /*for (char c = *msg.payload; c; c=*++msg.payload) {
        bytes[index] = c; 
        index++;
    }*/
    return bytes;
}
