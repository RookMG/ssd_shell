/* Runner */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <process.h>
#include <filesystem>
using namespace std;

string LOG = "[Runner]latest.log";

vector<string> tokenize(string s);
vector<string> tokenize(string s, string delim);

int runTests(string scriptListFile);
int runScript(string scriptFile);
string msgMaker(string funcName, string msg);
string fileName();
void logger(string funcName, string msg);


int main(int argc, char* argv[])
{
    string line;
    vector<string> args;
    if (argc == 2) {
        string scriptFile = argv[1];
        return runTests(scriptFile);
    }

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


int runTests(string scriptListFile) {
    ifstream readFile;
    readFile.open(scriptListFile);
    vector<string> scripts;
    string tmp;
    while (readFile >> tmp) scripts.push_back(tmp);
    readFile.close();
    if (scripts.size() == 0) return -1;
    for (string s : scripts) {
        cout << s << " --- Run...";
        int ret = system(s.c_str());
        cout << (ret == 0 ? "Pass\n" : "FAIL!!!\n");
        logger("runTests(" + scriptListFile + ")", s + " --- Run..." + (ret == 0 ? "Pass" : "FAIL!!!"));
        if (ret != 0) return ret;
    }
    return 0;
}


string msgMaker(string funcName, string msg) {
    string wholeMsg = "";
    time_t timer = time(nullptr);
    struct tm t;
    localtime_s(&t, &timer);
    string timeMsg = "";
    timeMsg += "[" + to_string(t.tm_year + 1900) + "." + (t.tm_mon >= 10 ? "" : "0") + to_string(t.tm_mon + 1) + "." + (t.tm_mday > 10 ? to_string(t.tm_mday) : ("0" + to_string(t.tm_mday))) + " "
        + (t.tm_hour >= 10 ? "" : "0") + to_string(t.tm_hour) + ":" + (t.tm_min >= 10 ? "" : "0") + to_string(t.tm_min) + "]";
    string funcMsg = funcName;
    //while (funcMsg.size() < 31) funcMsg += " ";
    if (funcMsg.size() < 30) funcMsg.resize(30, ' ');
    wholeMsg = timeMsg + funcMsg + ": " + msg + "\n";
    return wholeMsg;
}


string fileName() {
    time_t timer = time(nullptr);
    struct tm t;
    localtime_s(&t, &timer);
    string fileName = "[Runner]until_" + to_string(t.tm_year + 1900) + (t.tm_mon >= 10 ? "" : "0") + to_string(t.tm_mon + 1) + (t.tm_mday >= 10 ? "" : "0") + to_string(t.tm_mday) + "_"
        + (t.tm_hour >= 10 ? "" : "0") + to_string(t.tm_hour) + "h_"
        + (t.tm_min >= 10 ? "" : "0") + to_string(t.tm_min) + "m_"
        + (t.tm_sec >= 10 ? "" : "0") + to_string(t.tm_sec) + "s.log";
    return fileName;
}


void logger(string funcName, string msg) {
    string logMsg = msgMaker(funcName, msg);
    fstream log(LOG, ios::app);
    log.seekg(0, ios::end);
    int size = log.tellg();

    if (size + msg.size() >= 10000) {
        log.close();

        for (const auto& entry : filesystem::directory_iterator("./")) {
            string path{ entry.path().u8string() };
            if (path.size() <= 15 || path.substr(10, 5) != "until" || path.substr(path.size() - 3, 3) != "log") continue;
            string logfile = path.substr(2), zipfile = path.substr(0, path.size() - 3) + "zip";

            rename(logfile.c_str(), zipfile.c_str());
        }


        if (rename(LOG.c_str(), fileName().c_str()) != 0) cout << "log 파일 생성 오류 : " + fileName() + '\n';
        fstream log(LOG, ios::app);
        log.seekg(0, ios::end);
        for (char ch : logMsg) log.put(ch);
    }
    else {
        for (char ch : logMsg) log.put(ch);
    }
}