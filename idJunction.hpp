#include <_Teensy.h>


#include <Wire.h>

#include "motorDrive.hpp"

#include "tree3.hpp"

void determineTurnUnvisted(){
    if(totalJunctions == 0){
      junc0.maxTurns = calcNumTurns();
      currJunc = junc0;
      totalJunctions +=1;
    }
    int totJuncOffset = totalJunctions - 1;
    if(currJunc.maxTurns > 0){
      Junction tempJunc(totalJunctions);
      tempJunc.parent = &currJunc;
      tempJunc.maxTurns = calcNumTurns();
      currJunc.turnsMade += 1;
      totalJunctions +=1;
      if(typeOfJunction[2]==true){
        currJunc.right = &tempJunc;
        currJunc = *(currJunc.right);
        _90DegreeTurn('r');
        //currJunc = *(currJunc.right);
      }
      else if(typeOfJunction[1]==true){
        currJunc.forward = &tempJunc;
        currJunc = *currJunc.forward;
        moveForward();
        //currJunc = *(currJunc.forward);
  
      }
      else if(typeOfJunction[0]==true){
        currJunc.left = &tempJunc;
        currJunc = *currJunc.left;
        _90DegreeTurn('l');
        //currJunc = *(currJunc.left);
      }
      else{
        currJunc = *currJunc.parent;
        turnAround();
      }
      currJunc.maxTurns = currJunc.maxTurns - 1;
    }
    else{
      currJunc = *currJunc.parent;
      if(typeOfJunction[2]==true){
        _90DegreeTurn('r');
      }
      else if(typeOfJunction[1]==true){
        moveForward();
      }
      else if(typeOfJunction[0]==true){
        _90DegreeTurn('l');
      }
      else{
        turnAround();
      }
    }
  }
  void makeNewJuncAndTurn(){
    if(totalJunctions == 0){
      junc0.maxTurns = calcNumTurns();
      junc0.junctionType = idTypeJunc();
      currJunc = junc0;
      totalJunctions +=1;
    }
    else{
      currJunc.maxTurns = calcNumTurns();
      currJunc.junctionType = idTypeJunc();
    }
    //else if(currJunc.id )
    if(currJunc.turnsMade < currJunc.maxTurns){
        Junction tempJunc(totalJunctions);
        tempJunc.parent = &currJunc;
        //tempJunc.maxTurns = calcNumTurns();
        currJunc.turnsMade += 1;
        totalJunctions +=1;
        if(currJunc.maxTurns == 1){
            if(currJunc.junctionType == "rl"){
                currJunc.right = &tempJunc;
                currJunc = *(currJunc.right);
                _90DegreeTurn('r');
            }
            else if(currJunc.junctionType == "ll"){
                currJunc.left = &tempJunc;
                currJunc = *(currJunc.left);
                _90DegreeTurn('l');
            }
          }
        // types of junctions this identifies --| or |-- or _|_
        else if(currJunc.maxTurns == 2){
            // this one is |--
            if(currJunc.junctionType == "rt"){
                if(currJunc.turnsMade == 0){
                    currJunc.right = &tempJunc;
                    currJunc = *(currJunc.right);
                    _90DegreeTurn('r');
                }
                if(currJunc.turnsMade == 1){
                    currJunc.forward = &tempJunc;
                    currJunc = *(currJunc.forward);
                    _90DegreeTurn('r');
                }
            }
            // this is --|
            else if(currJunc.junctionType == "lt"){
                if(currJunc.turnsMade == 0){
                    currJunc.forward = &tempJunc;
                    currJunc = *(currJunc.forward);
                    moveForward();
                }
                if(currJunc.turnsMade == 1){
                    currJunc.left = &tempJunc;
                    currJunc = *(currJunc.left);
                    _90DegreeTurn('r');
                }
              }
            // this is __|__
            else if(currJunc.junctionType == "t"){
                if(currJunc.turnsMade == 0){
                    currJunc.right = &tempJunc;
                    currJunc = *(currJunc.right);
                    _90DegreeTurn('r');
                }
                if(currJunc.turnsMade == 1){
                    currJunc.left = &tempJunc;
                    currJunc = *(currJunc.left);
                    moveForward();
                }
              }
        }
        // this is checking for T intersection  so like -> + type it has three turns to make
        // and below is what it needs to do for each turn
        else if(currJunc.maxTurns == 3){
            if(currJunc.turnsMade == 0){
                currJunc.right = &tempJunc;
                currJunc = *(currJunc.right);
                _90DegreeTurn('r');
            }
            else if(currJunc.turnsMade == 1){
                currJunc.forward = &tempJunc;
                currJunc = *(currJunc.forward);
                _90DegreeTurn('r');
            }
            else if(currJunc.turnsMade == 2){
                currJunc.left = &tempJunc;
                currJunc = *(currJunc.left);
                _90DegreeTurn('r');
            }
          }
        else{
          currJunc = &currJunc.parent;
          turnAround();
        }
        }
    
    else{
        currJunc = *currJunc.parent;
        if(typeOfJunction[2]==true){
          _90DegreeTurn('r');
        }
        else if(typeOfJunction[1]==true){
          moveForward();
        }
        else if(typeOfJunction[0]==true){
          _90DegreeTurn('l');
        }
        else{
          currJunc = &currJunc.parent;
          turnAround();
        }
      }
  }