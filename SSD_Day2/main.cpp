#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <string>

#define SSD_SIZE 100
#define BUFFER_MAX_SIZE 10

using namespace std;

string NAND = "nand.txt", RESULT = "result.txt", BUFFER = "buffer.txt";
int fileSize = 0, bufferSize = 0;
vector<string> ssdData(SSD_SIZE, "00000000");
unordered_map<string, string> bufferData;

int write(string lba, string value);
int read(string lba);
int erase(string lba, string size);
int flush();

vector<string> tokenize(string s);
vector<string> tokenize(string s, string delim);
bool validateLba(string s);
bool validateValue(string s);
bool validateLbaRange(string lba, string size);

void initNand();
void readNand();
void writeNand(int pos);
void writeResult(int pos);
void readBuffer();
void writeBuffer(string cmd);

int main(int argc, char* argv[])
{
    vector<string> args;
    for (int i = 0; i < argc; i++) args.push_back(argv[i]);

    if (args.size() <= 1) return -1;

    if (args[1] == "W") {
        if (args.size() != 4 || !validateLba(args[2]) || !validateValue(args[3])) return -1;
        return write(args[2], args[3]);
    }
    else if (args[1] == "R") {
        if (args.size() != 3 || !validateLba(args[2])) return -1;
        return read(args[2]);
    }
    else if (args[1] == "E") {
        if (args.size() != 4 || !validateLba(args[2]) || !validateLbaRange(args[2], args[3])) return -1;
        return erase(args[2], args[3]);
    }
    else if (args[1] == "F") {
        return flush();
    }
    else return -1;

    return 0;
}


vector<string> tokenize(string s) {
    return tokenize(s, " ");
}


vector<string> tokenize(string s, string delim) {
    // delim을 구분자로 string 토큰화
    // delim이 정규식이 아닌 find 방식으로 동작함에 주의

    vector<string> tokens;
    for (int p; (p = s.find(delim)) != -1;) {
        tokens.push_back(s.substr(0, p));
        s = s.substr(++p);
    }
    tokens.push_back(s);

    return tokens;
}



int write(string lba, string value) {
    readBuffer();
    if (bufferSize >= BUFFER_MAX_SIZE) flush();
    writeBuffer("W " + lba + " " + value);
    return 0;
}


int read(string lba) {
    int pos = stoi(lba);
    readBuffer();
    auto it = bufferData.find(lba);
    if (it != bufferData.end()) {
        ssdData[pos] = it->second;
    }
    else {
        readNand();
    }
    writeResult(pos);
    return 0;
}


int erase(string lba, string size) {
    readBuffer();
    if (bufferSize >= BUFFER_MAX_SIZE) flush();
    writeBuffer("E " + lba + " " + size);
    return 0;
}


int flush() {
    readNand();
    bufferData.clear();
    readBuffer();
    if (bufferSize == 0) return 0;

    for (auto el : bufferData) {
        int pos = stoi(el.first);
        ssdData[pos] = el.second;
        writeNand(pos);
    }
    bufferData.clear();
    bufferSize = 0;

    ofstream writeFile;
    writeFile.open(BUFFER);
    writeFile << "";
    writeFile.close();
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
    return 0 <= num && num < SSD_SIZE;
}


bool validateValue(string s) {
    for (int i = 0; i < 8; i++) {
        if (('0' <= s[i] && s[i] <= '9') || ('A' <= s[i] && s[i] <= 'F')) continue;
        return false;
    }
    return true;
}


bool validateLbaRange(string lba, string size) {
    int l, s;
    try {
        l = stoi(lba);
        s = stoi(size);
        return 0 < s && s <= 10 && l + s < SSD_SIZE;
    }
    catch (invalid_argument) {
        return false;
    }
}


void initNand() {
    ofstream writeFile;
    writeFile.open(NAND);
    for (int i = 0; i < SSD_SIZE; i++) {
        writeFile << ssdData[i];
    }
    writeFile.close();
}


void readNand() {
    ifstream readFile;
    readFile.open(NAND);
    int lba = 0;
    string tmp;
    getline(readFile, tmp);
    fileSize = tmp.size();
    while (tmp.size() < 8 * SSD_SIZE) tmp += "00000000";
    for (int i = 0; i < SSD_SIZE; i++) {
        if (!validateValue(ssdData[i] = tmp.substr(i * 8, 8))) {
            cout << "   " << i << " 번 데이터 이상 발생 (" << ssdData[i] << "), 0x00000000으로 초기화\n";
            ssdData[i] = "00000000";
        }
    }
    readFile.close();

    if (fileSize != SSD_SIZE * 8) {
        initNand();
    }
}


void writeNand(int pos) {
    fstream writeFile;
    writeFile.open(NAND);
    writeFile.seekg(pos * 8);
    for (char ch : ssdData[pos]) writeFile.put(ch);
    writeFile.close();
}


void writeResult(int pos) {
    ofstream writeFile;
    writeFile.open(RESULT);
    string ret = "0x" + ssdData[pos];
    writeFile << ret;
    writeFile.close();
}


void readBuffer() {
    ifstream readFile;
    readFile.open(BUFFER);
    string tmp;
    getline(readFile, tmp);
    vector<string> commands = tokenize(tmp,";");
    bufferSize = commands.size() - 1;
    for (int i = 0; i < bufferSize; i++) {
        vector<string> command = tokenize(commands[i]);
        if (command[0] == "W") {
            bufferData[command[1]] = command[2];
        }
        else if (command[0] == "E") {
            int start = stoi(command[1]), size = stoi(command[2]);
            for (int i = 0; i < size; i++) bufferData[to_string(start + i)] = "00000000";
        }
    }
    readFile.close();
}


void writeBuffer(string cmd) {
    if (bufferSize == 0) {
        ofstream tmp;
        tmp.open(BUFFER);
        tmp.close();
    }
    fstream writeFile;
    writeFile.open(BUFFER);
    writeFile.seekg(0,ios::end);
    writeFile << cmd << ";";
    writeFile.close();
    bufferSize++;
}