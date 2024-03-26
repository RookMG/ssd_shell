/* testapp3 */

#include <iostream>
#include <string>
#include <process.h>
#include <fstream>
using namespace std;
string SHELL = "Shell.exe", RESULT = "result.txt";

string getResult();

int main() {
    int ret = 0;
    string script = "";
    for (int i = 0; i <= 5; i++) script += "write_" + to_string(i) + "_0xGGGGGGGG;";
    ret = system((SHELL + " " + script).c_str());
    if (ret != 0) return ret;

    for (int i = 0; i <= 5; i++) {
        ret = system((SHELL + " read_" + to_string(i) + ";").c_str());
        if (ret != 0 || getResult() != "0x12345678") return ret;
    }
    return 0;
}

string getResult() {
    ifstream readFile;
    readFile.open(RESULT);
    string ret;
    getline(readFile, ret);
    readFile.close();
    return ret;
}