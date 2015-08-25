/*
    WUSTL - FL2014 - CSE 422S - Lab 3
    Name: SeungJu Lee and Elliot Yun
    seungjulee@wustl.edu

    Lab 3
 */
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4 || atoi(argv[1]) > 100 || atoi(argv[1]) < 0){
        cout << "Usage" << endl;
        cout << "-----" << endl;
        cout << "vmgenU [range] [length] [filename]" << endl;
    }
    else {
        int range = atoi(argv[1]);
        int length = atoi(argv[2]);

        char* filename = argv[3];

        ofstream result(filename);

        if (result.is_open())
        {
            int random;
            for (int i = 0; i < length; i++){
                random = rand() % range;
                result << random << " ";
            }
            result << endl;
            result.close();
        }
    }

    return 0;
}