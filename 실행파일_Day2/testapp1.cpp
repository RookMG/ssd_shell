/* testapp1 */

#include <iostream>
#include <string>
#include <process.h>
#include <fstream>
using namespace std;
string SHELL = "Shell.exe", RESULT = "result.txt";

string getResult();

int main() {
	string script = "";
	for (int i = 0; i < 100; i++) script += "write_" + to_string(i) + "_0x1111AFAF;";
	int ret = system((SHELL+" "+script).c_str());
	if (ret != 0) return ret;
    for (int i = 0; i < 100; i++) {
        ret = system((SHELL + " read_" + to_string(i)+";").c_str());
        if (ret != 0||getResult()!="0x1111AFAF") return ret;
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