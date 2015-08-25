/*
    WUSTL - FL2014 - CSE 422S - Lab 3
    Name: SeungJu Lee and Elliot Yun
    seungjulee@wustl.edu

    Lab 3
 */
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
int* pageStream;
int* pageFault;
int pageFaultCount=0;
int** memFrames;
int* viMemory;
int rounds;
int range;

void printResult(){
    bool hasF = false;
    int fCount = 0;
    int initCount = 0;
    for (int i = 0; i<rounds; i++){
        if (pageStream[i] < 10 ){
            cout << pageStream[i] << " :  [";
        } else{
            cout << pageStream[i] << ":  [";
        }

        for (int k = 0; k < range; k++ ){
            int frame = memFrames[i][k];
            if (frame == -1){
                cout << "  ";
            } else if (frame < 10){
                cout << " " << frame;
            } else {
                cout << frame;
            }
            if (k==range-1){
                cout << "]" ;
                hasF = false;
                for (int j=0; j<range; j++){
                    if (i>0) {
                        if(pageStream[i]==memFrames[i-1][j]){
                            hasF = true;
                        }
                        if (memFrames[i-1][j]==-1){
                            hasF = true;
                        }
                    }
                }
                if (memFrames[i][k] == -1){
                    initCount++;
                }
                if (!hasF && i>0){
                    fCount++;
                    cout << " F";
                }
            }
            else
            {
                cout << "|";
            }

        }
        cout << " " << endl;
    }
    cout << " " << endl;

    cout << "Miss rate = " << fCount << " / " << rounds-initCount << " = " << (double)fCount/(double)(rounds-initCount) *100 << "%" << endl;
}
void parse(int argc, char* argv[]){
    range = atoi(argv[1]);
    char* filename = argv[2];

    int a;
    int count=0;

    ifstream input(filename);
    while (input.eof()==0){
        input >> a;
        count++;
    }
    input.close();

    rounds = count -1;
    pageStream = new int[rounds];

    ifstream inp(filename);
    int kount=0;

    while (inp >> pageStream[kount] && kount<rounds){
        kount++;
    }
    inp.close();

    pageFault = new int[rounds];
    memFrames= new int*[rounds];
    for(int i = 0; i < rounds; ++i)
        memFrames[i] = new int[range];
    viMemory = new int[range];

    for (int i=0;i<rounds;i++) {
        for (int j = 0; j < range; j++) {
            memFrames[i][j] = -1 ;
            viMemory[j] = -1;
        }
        pageFault[i] = 0;
    }
}
void parseAndExecute(int argc, char* argv[]){

    parse(argc,argv);

    int saveRound;
    bool isDone = false;
    bool isFull = false;
    int firstPageStream = pageStream[0];
    int isFullAt = 0;
    bool isFullSwitch = false;
    int insertedAt[range];
    int clkPointer = 0;
    bool clkFrameBit[rounds][range];

    for (int i=0;i<rounds;i++) {
        for (int j = 0; j < range; j++) {
            clkFrameBit[i][j] = false;
        }
    }

    for (int i=0;i<rounds;i++){
        if (!isFullSwitch){
            for (int j=0;j<range;j++){
                // if the j in virtual memory is empty
                if (viMemory[j] == -1){
                    if (!isDone){
                        // copy saved pages from the previous round
                        if (j > 0){
                            for (int jj = 0; jj < j; jj++){
                                memFrames[i][jj] = memFrames[i-1][jj];
                                if (memFrames[i][jj] == pageStream[i]){
                                    pageFault[i]++;
                                    pageFaultCount++;
                                }
                                clkFrameBit[i][jj] = true;

                            }
                        }
                        // add a new page only when there is no page fault on this round
                        if (pageFault[i] == 0){
                            viMemory[j] = pageStream[i];
                            memFrames[i][j] = viMemory[j];
                            insertedAt[j] = i;
                            clkFrameBit[i][j] = true;
                            if (j != range-1){
                                clkPointer++;
                            } else {
                                clkPointer = 0;
                            }
                        }
                        isDone = true;
                    }
                } else
                {
                    if (j==range-1){
                        isFull = true;
                        bool hasPageFault=0;
                        for (int k=0; k<range; k++){
                            memFrames[i][k] = viMemory[k];

                            if (memFrames[i][k] == pageStream[i]){
                                isFull = false;
                                hasPageFault = true;
                                pageFault[i]++;
                                pageFaultCount++;
                            }
                        }

                        if(!hasPageFault){
                            for (int k=0; k<range;k++){
                                memFrames[i][k] = -1;
                            }
                        }
                        if (!isFullSwitch && !hasPageFault){
                            isFullAt = i;
                            isFullSwitch = true;
                        }
                    }
                }
            }
            isDone = false;
        }
    }


    if(isFull){
        bool isPageFault=false;
        // initialize steps
        int steps[rounds][range];
        for (int i=0; i < rounds; i++){
            for (int j = 0; j < range; j++) {
                steps[i][j] = 0;
            }
        }
        for (int i = isFullAt; i < rounds; i++) {
            isPageFault = false;

            for (int p = 0; p < range; p++) {
                viMemory[p] = memFrames[i - 1][p];
                memFrames[i][p] = viMemory[p];
                if (pageStream[i] == memFrames[i][p]) {
                    isPageFault = true;
                    pageFault[i]++;
                    pageFaultCount++;
                }
            }

            if ((strncmp(argv[3], "OPT", 4) == 0 || strncmp(argv[3], "opt", 4) == 0) && !isPageFault) {
                for (int j = 0; j < range; j++) {
                    for (int z = rounds - 1; z >= i + 1; z--) {
                        if (pageStream[z] == viMemory[j]) {
                            steps[i][j] = z - i;
                        }
                    }
                    if (j == range - 1) {
                        //last and check
                        int largest = -1;
                        int largestIdx = -1;
                        int zeroCount = 0;

                        for (int a = range - 1; a >= 0; a--) {
                            if (a == range - 1) {
                                largest = steps[i][a];
                                largestIdx = a;
                            }
                            if (steps[i][a] >= largest && largest > 0 && a != range - 1) {
                                largest = steps[i][a];
                                largestIdx = a;
                            }
                            if (steps[i][a] == 0) {
                                largest = steps[i][a];
                                largestIdx = a;
                            }
                        }
                        viMemory[largestIdx] = pageStream[i];
                        memFrames[i][largestIdx] = viMemory[largestIdx];
                    }
                }
            }
            if ((strncmp(argv[3], "LRU", 4) == 0 || strncmp(argv[3], "lru", 4) == 0) && !isPageFault) {
                for (int j = 0; j < range; j++) {
                    for (int z = 0; z < i; z++) {

                        if (pageStream[z] == viMemory[j]) {
                            steps[i][j] = z - i;
                        }
                    }
                }

                int farthest = -1;
                int farthestIdx = -1;

                for (int j = 0; j < range; j++) {
                    if (j == 0) {
                        farthest = steps[i][j];
                        farthestIdx = j;
                    } else {
                        if (farthest > steps[i][j]) {
                            farthest = steps[i][j];
                            farthestIdx = j;
                        }
                    }
                }

                viMemory[farthestIdx] = pageStream[i];
                memFrames[i][farthestIdx] = viMemory[farthestIdx];

            }
            if ((strncmp(argv[3], "FIFO", 4) == 0 || strncmp(argv[3], "fifo", 4) == 0) && !isPageFault) {
                int smallest = -1;
                int smallestIdx = -1;

                for (int j = 0; j < range; j++) {
                    if (j == 0) {
                        smallest = insertedAt[j];
                        smallestIdx = j;
                    } else {
                        if (smallest > insertedAt[j]) {
                            smallest = insertedAt[j];
                            smallestIdx = j;
                        }
                    }

                }

                viMemory[smallestIdx] = pageStream[i];
                memFrames[i][smallestIdx] = viMemory[smallestIdx];
                insertedAt[smallestIdx] = i;
            }
            if (strncmp(argv[3], "CLOCK", 4) == 0 || strncmp(argv[3], "clock", 4) == 0) {
                for (int j = 0; j < range; j++) {
                    clkFrameBit[i][j] = clkFrameBit[i - 1][j];
                }

                int knt = 0;
                bool hasPageF = false;
                int hasPageFIdx = -1;
                for (int j = 0; j < range; j++) {
                    if (clkFrameBit[i][j]) {
                        knt++;
                    }
                    if (memFrames[i][j] == pageStream[i]) {
                        hasPageF = true;
                        hasPageFIdx = j;
                    }
                }


                if (knt == range){
                    if (!hasPageF) {
                        for (int j = 0; j < range; j++) {
                            clkFrameBit[i][j] = false;
                        }
                        viMemory[clkPointer] = pageStream[i];
                        memFrames[i][clkPointer] = viMemory[clkPointer];

                        clkFrameBit[i][clkPointer] = true;
                        if (clkPointer != range-1){
                            clkPointer++;
                        } else {
                            clkPointer = 0;
                        }
                    }
                } else {
                    int pnt = clkPointer;
                    // advances pointer if use bit is 0

                    while (clkFrameBit[i][pnt] && !hasPageF){
                        clkFrameBit[i][pnt] = false;
                        if (pnt != range-1){
                            pnt++;
                        } else {
                            pnt = 0;
                        }
                        clkPointer = pnt;
                    }

                    if (memFrames[i][clkPointer]==pageStream[i] || (hasPageF && clkPointer!=hasPageFIdx) ){
                        if (memFrames[i][clkPointer]==pageStream[i] )
                            clkFrameBit[i][clkPointer] = true;
                        else
                            clkFrameBit[i][hasPageFIdx] = true;

                    } else {
                        viMemory[clkPointer] = pageStream[i];
                        memFrames[i][clkPointer] = viMemory[clkPointer];
                        clkFrameBit[i][clkPointer] = true;
                        if (clkPointer != range-1){
                            clkPointer++;
                        } else {
                            clkPointer = 0;
                        }

                    }
                }

            }
        }
    }
}

int main(int argc, char* argv[]) {
    bool syntaxCheck=false;
    if (strncmp(argv[3], "OPT", 4) == 0  || strncmp(argv[3], "opt", 4) == 0
            || strncmp(argv[3], "LRU", 4) == 0  || strncmp(argv[3], "lru", 4) == 0
            || strncmp(argv[3], "FIFO", 5) == 0 || strncmp(argv[3], "fifo", 5) == 0
            || strncmp(argv[3], "CLOCK", 6) == 0 || strncmp(argv[3], "clock", 6) == 0
    )
    {
        syntaxCheck = true;
    }
    if (argc != 4 || atoi(argv[1]) > 100 || atoi(argv[1]) < 0 || !syntaxCheck) {
        cout << "-----" << endl;
        cout << "Usage" << endl;
        cout << "-----" << endl;
        cout << "vmsim [FRAMES] [INPUT_FILE] [ALGORITHM]" << endl;
        cout << "Example: vmsim 5 vmrefs.dat LRU" << endl;
        cout << " " << endl;
        cout << "FRAMES - the total number of physical memory frames (maximum 100)" << endl;
        cout << "INPUT_FILE - an input filename where a sequence of page references is stored" << endl;
        cout << "ALGORITHM - (OPT | LRU | FIFO | CLOCK)" << endl;
        cout << " " << endl;
    }
    else {
        parseAndExecute(argc,argv);
        printResult();
    }

    return 0;
}
