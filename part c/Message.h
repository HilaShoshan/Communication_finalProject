#pragma once

#include <stdio.h>
#include <string.h>
using namespace std;


struct Message {
    int msg_id;
    int src_id;
    int dest_id;
    int num_trailing_msg;
    int func_id;
    char* payload;
};
