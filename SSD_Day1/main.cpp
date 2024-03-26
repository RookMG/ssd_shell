#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#define SIZE 100

using namespace std;

string NAND = "nand.txt", RESULT = "result.txt";
vector<string> ssdData(SIZE, "00000000");

bool validateLba(string s);
bool validateValue(string s);
void readNand();
void writeNand(int pos);
void writeResult(int pos);
void printNand(int pos);

int main(int argc, char* argv[])
{
    vector<string> args;
    for (int i = 0; i < argc; i++) args.push_back(argv[i]);

    if (args.size() <= 1) return -1;

    if (args[1] == "write") {
        if (args.size() != 4 || !validateLba(args[2]) || !validateValue(args[3])) return -1;
        readNand();
        int pos = stoi(args[2]);
        ssdData[pos] = args[3];
        writeNand(pos);
    }
    else if (args[1] == "read") {
        if (args.size() != 3 || !validateLba(args[2])) return -1;
        readNand();
        int pos = stoi(args[2]);
        writeResult(pos);
        // printNand(pos);
    }
    else return -1;

    return 0;
}


bool validateLba(string s) {
    int num;
    try {
        num = stoi(s);
    }
    catch (invalid_argument) {
        return false;
    }
    return 0 <= num && num < SIZE;
}


bool validateValue(string s) {
    for (int i = 0; i < 8; i++) {
        if (('0' <= s[i] && s[i] <= '9') || ('A' <= s[i] && s[i] <= 'F')) continue;
        return false;
    }
    return true;
}


void readNand() {
    ifstream readFile;
    readFile.open(NAND);
    int lba = 0;
    string tmp;
    getline(readFile, tmp);
    while (tmp.size() < 8 * SIZE) tmp += "00000000";
    for (int i = 0; i < SIZE; i++) {
        if (!validateValue(ssdData[i] = tmp.substr(i * 8, 8))) {
            cout << "   " << i << " 번 데이터 이상 발생 (" << ssdData[i] << "), 0x00000000으로 초기화\n";
            ssdData[i] = "00000000";
        }
    }
    readFile.close();
}


void writeNand(int pos) {
    ofstream writeFile;
    writeFile.open(NAND);
    for (int i = 0; i < SIZE; i++) {
        writeFile << ssdData[i];
    }
    writeFile.close();
}


void writeResult(int pos) {
    ofstream writeFile;
    writeFile.open(RESULT);
    string ret = "0x" + ssdData[pos];
    writeFile << ret;
    writeFile.close();
}


void printNand(int pos) {
    cout << (pos < 10 ? "    " : "   ") << pos << " : 0x" << ssdData[pos] << '\n';
}