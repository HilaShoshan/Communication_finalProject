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
