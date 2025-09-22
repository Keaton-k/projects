#pragma once
#include <iostream>
using namespace std;
#include <vector>
#include <string>
#include "motorDrive.hpp"

int totalJunctions = 0;
//std::vector<char> listOfTurns;
char listOfTurn[20];
int counter = 0;
//std::vector<char> ogTurns;

class Junction {
public:
    Junction* left;
    Junction* right;
    Junction* forward;
    Junction* parent;
    int id; // Just for identification purposes
    int maxTurns;
    int turnsMade;
    bool visted;
    bool deadend;
    bool end;
    std::string junctionType;

    // Constructor
    Junction(int _id) : id(_id), left(nullptr), right(nullptr), forward(nullptr), parent(nullptr) , maxTurns(0) , turnsMade(0), deadend(false),visted(false), end(false) {}

    /*~Junction(){
        delete left;
        delete right;
        delete forward;
    }*/

    // Function to display the junction's connections
    void displayConnections() {
        std::cout << "Junction " << id << " connections:\n";
        std::cout << "  Left -> " << (left ? std::to_string(left->id) : "None") << "\n";
        std::cout << "  Right -> " << (right ? std::to_string(right->id) : "None") << "\n";
        std::cout << "  Forward -> " << (forward ? std::to_string(forward->id) : "None") << "\n";
    }
};

Junction addJunction(Junction junc,bool paths[3]){

    if(paths[0]==true){
        totalJunctions += 1;    
        Junction* j1 = new Junction(totalJunctions);
        junc.left = j1;
    }
    if(paths[1]==true){
        totalJunctions += 1;    
        Junction* j2 =  new Junction(totalJunctions);
        junc.forward = j2;
    }
    if(paths[2]==true){
        totalJunctions += 1;    
        Junction* j3 = new Junction(totalJunctions);
        junc.right = j3;    }
    return junc;
};

void backTrack(struct Junction* junc){
    if(junc->parent != NULL){
        Junction pr = *junc->parent;

        if(pr.right != NULL){
            //input left
            if(pr.right->id == junc->id){
                //listOfTurns.push_back('l');
                //ogTurns.push_back('r');
                listOfTurn[counter] = 'l';
                counter += 1;
            }
        }
        if(pr.left != NULL){
            //input right
            if(pr.left->id == junc->id){
                //listOfTurns.push_back('r');
                //ogTurns.push_back('l');
                listOfTurn[counter] = 'r';
                counter += 1;
            }
        }
        if(pr.forward != NULL){
            //input forward
            if(pr.forward->id == junc->id){
                //listOfTurns.push_back('f');
                //ogTurns.push_back('f');
                listOfTurn[counter] = 'f';
                counter += 1;
            }
        }
        junc = &pr;
        backTrack(junc);
    }
    return;
}


void dfs(struct Junction* junc){
    // will have to keep track of junctions I have already visted because this will show which junction to go to next
    // will be recrusive but can't fully run entire time
    if (junc == NULL)
        return;
    if (junc->end == true){
        backTrack(junc);
        return;
    }
    /* pre order traversalthen print the data of node */
    //cout << junc->id << " ";

    /* first recur on left child */
    dfs(junc->right);

    dfs(junc->forward);

    /* now recur on right child */
    dfs(junc->left);
}

void printInorder(struct Junction* junc)
{
    if (junc == NULL)
        return;
    
    /* pre order traversalthen print the data of node */
    cout << junc->id << " ";

    /* first recur on left child */
    printInorder(junc->left);

    printInorder(junc->forward);

    /* now recur on right child */
    printInorder(junc->right);
}

