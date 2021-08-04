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
