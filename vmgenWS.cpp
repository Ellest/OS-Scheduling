/*
    WUSTL - FL2014 - CSE 422S - Lab 3
    Name: SeungJu Lee and Elliot Yun
    seungjulee@wustl.edu

    Lab 3
 */
#include <iostream>
#include<fstream>
#include <sstream>

using namespace std;

int randULBound(int LB, int UB){

    int size = UB - LB + 1;
    return rand() % size + LB;
}

int main(int argc, char* argv[]) {

    if (argc != 7 || atoi(argv[1]) > 100 || atoi(argv[1]) < 0){
        cout << "Usage" << endl;
        cout << "-----" << endl;
        cout << "vmgenWS [ws_size] [LB] [UB] [range] [length] [input_filename]" << endl;
    } else {
        int ws_size = atoi(argv[1]);
        int LB = atoi(argv[2]);
        int UB = atoi(argv[3]);
        int range = atoi(argv[4]);
        int length = atoi(argv[5]);
        char* filename = argv[6];

        int sumLength = 0;

        int sets[ws_size];
        int newSets[ws_size];
        int finalSet[length];

        int count;
        int idx=0;
        int ranValUL = 0;

        for (int i=0; i<length; i++){
            if (i == idx && i !=0){
                for (int j = 0; j< ws_size; j++){
                    sets[j] = rand() % range;
                }
                idx += randULBound(LB, UB);
            } else {
                if (i==0){
                    for (int i = 0; i < ws_size; i++){
                        sets[i] = rand() % range;
                    }
                    idx += randULBound(LB, UB);
                }
            }
            finalSet[i] = sets[rand() % ws_size];
        }

        ofstream result(filename);
        if (result.is_open())
        {
            for (int i = 0; i < length; i++){

                result << finalSet[i] << " ";
            }
            result << endl;
            result.close();
        }
    }

    return 0;
}