//
// Created by Akshat Bajpai on 7/15/21.
//

#include <string>
#include <ctime>
#include<iostream>
using namespace std;
string get(){
    time_t now = time(0);
    string dt = ctime(&now);
    return dt;
}