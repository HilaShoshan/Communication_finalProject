#pragma once

#include "Node.hpp"


class Path {

    private:
    Node dest;
    vector<Node> path;

    public:
    
    //getters:

    Node getDest() {
        return this -> dest; 
    }

    vector<Node> getPath() {
        return this -> path; 
    }

    int getSize() {
        return path.size();
    }
};