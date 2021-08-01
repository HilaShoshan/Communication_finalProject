#pragma once

#include <stdio.h>
#include <vector>
#include "Node.h"
using namespace std;


class Path {

    private:
    vector<Node*> path;

    public:
    
    //getters:

    Node getDest() {
        return this -> path.at(path.size()-1);  // the last Node on the path
    }

    vector<Node> getPath() {
        return this -> path; 
    }

    int getSize() {
        return path.size();
    }
};