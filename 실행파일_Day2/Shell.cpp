#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <process.h>
#include <filesystem>
using namespace std;
string DIR = "SSD.exe", NAND = "nand.txt", RESULT = "result.txt", LOG = "latest.log";
int SIZE = 100;
bool LOG_DISABLED = false;

vector<string> tokenize(string s);
vector<string> tokenize(string s, string delim);
bool validateLba(string s);
bool validateValue(string s);
int execute(vector<string>& args);

int write(int start, int end, string value);
int write(int pos, string value);
int write(string pos, string value);
string read(int pos);
string read(string pos);
int erase(int pos, int size);
int erase(string pos, string size);
int erase_range(int start, int end);
int erase_range(string start, string end);
int flush();
string getResult();
string msgMaker(string funcName, string msg);
string fileName();
void print(string msg);
void logger(string funcName, string msg);


int main(int argc, char* argv[])
{
    string line;
    vector<string> args;
    if (argc == 1) {
        while (true) {
            print(">> ");
            getline(cin, line);
            args = tokenize(line);
            if (execute(args) < 0) break;
        }
    }
    else if (argc == 2) {
        LOG_DISABLED = true;
        string script = argv[1];
        vector<string> line = tokenize(script, ";");
        for (int i = 0; i < line.size() - 1; i++) {
            args = tokenize(line[i],"_");
            int ret = execute(args);
            if (ret != 0) return ret;
        }
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
    if (s.size() != 10 || s[0] != '0' || s[1] != 'x') return false;
    for (int i = 2; i < 10; i++) {
        if (('0' <= s[i] && s[i] <= '9') || ('A' <= s[i] && s[i] <= 'F')) continue;
        return false;
    }
    return true;
}

bool validateSize(string s) {
    int num;
    try {
        num = stoi(s);
    }
    catch (invalid_argument) {
        return false;
    }
    return 1 <= num && num <= 10;
}

bool validatePos(string s) {
    int num;
    try {
        num = stoi(s);
    }
    catch (invalid_argument) {
        return false;
    }
    return 0 <= num && num < 100;
}


int execute(vector<string>& args) {
    // 한 줄 입력받아 실행
    // 정상 동작시 return 0, exit 시 return -1, 예외 발생 시 return 1

    // 빈 줄 입력시 계속 입력 대기
    if (args[0] == "") return 0;
    if (args[0] == "exit") {
        print("   프로그램을 종료합니다.\n");
        return -1;
    }

    else if (args[0] == "help") {
        if (args.size() == 1) {
            string size = to_string(SIZE - 1);
            print("\n   사용 가능한 명령어는 다음과 같습니다\n\n\n");
            print( "   write {LBA} {VALUE}\t\t\t: LBA(0~" + size + ") 영역의 값을 VALUE(0x00000000~0xFFFFFFFF)로 바꿉니다.\n\n");
            print( "   read {LBA}\t\t\t\t: LBA(0~" + size + ") 영역의 값을 읽어 출력합니다.\n\n");
            print( "   fullwrite {VALUE}\t\t\t: 모든 LBA(0~" + size + ") 영역의 값을 VALUE(0x00000000~0xFFFFFFFF)로 바꿉니다.\n\n");
            print( "   fullread\t\t\t\t: 모든 LBA(0~" + size + ") 영역의 값을 읽어 출력합니다.\n\n");
            print( "   erase {LBA} {SIZE}\t\t\t: LBA(0~" + size + " - SIZE) 영역부터 SIZE개 영역의 값을 지웁니다.\n\n");
            print( "   erase_range {START_LBA} {END_LBA}\t: START_LBA(0~END_LBA)부터 END_LBA(START_LBA~" + size + ") 영역의 값을 지웁니다.\n\n");
            print( "   flush\t\t\t\t: SSD Write Buffer의 내용을 쓰고 Buffer를 비웁니다.\n\n");
            print( "   help\t\t\t\t\t: 이 도움말을 표시합니다.\n\n");
            print( "   exit\t\t\t\t\t: 프로그램을 종료합니다.\n\n");
        }
        else {
            print("   " + args[1] + "에 해당하는 도움말이 없습니다. \n");
        }
        return 0;
    }

    else if (args[0] == "write") {
        if (args.size() != 3) {
            print("   write에는 3개의 인수가 필요합니다. ");
            print("help를 입력해 도움말을 확인하세요.\n");
            return 1;
        }
        if (!validateLba(args[1])) {
            string size = to_string(SIZE - 1);
            print("   LBA 값은 0 이상 " + size + "이하의 값이어야 합니다.\n");
            return 1;
        }
        if (!validateValue(args[2])) {
            print("   저장할 값은 0x00000000 이상 0xFFFFFFFF 이하의 값이어야 합니다.\n");
            return 1;
        }
        write(args[1], args[2]);
        return 0;
    }
    else if (args[0] == "read") {
        if (args.size() != 2) {
            print("   read에는 2개의 인수가 필요합니다. ");
            print("help를 입력해 상세한 내용을 확인하세요.\n");
            return 1;
        }
        if (!validateLba(args[1])) {
            string size = to_string(SIZE - 1);
            print("   LBA 값은 0 이상 " + size + "이하의 값이어야 합니다.\n");
            return 1;
        }
        read(args[1]);
        return 0;
    }
    else if (args[0] == "fullwrite") {
        if (args.size() != 2) {
            print("   fullwrite에는 2개의 인수가 필요합니다. ");
            print("help를 입력해 상세한 내용을 확인하세요.\n");
            return 1;
        }
        if (!validateValue(args[1])) {
            print("   저장할 값은 0x00000000 이상 0xFFFFFFFF 이하의 값이어야 합니다.\n");
            return 1;
        }
        write(0, SIZE-1, args[1]);
        return 0;
    }
    else if (args[0] == "fullread") {
        if (args.size() != 1) {
            print("   fullread에는 1개의 인수가 필요합니다. ");
            print("help를 입력해 상세한 내용을 확인하세요.\n");
            return 1;
        }
        for (int i = 0; i < SIZE; i++) read(i);
        return 0;
    }
    else if (args[0] == "erase") {
        if (args.size() != 3) {
            print("   erase에는 3개의 인수가 필요합니다. ");
            print("help를 입력해 도움말을 확인하세요.\n");
            return 1;
        }
        if (!validateLba(args[1])) {
            string size = to_string(SIZE - 1);
            print("   LBA 값은 0 이상 " + size + "이하의 값이어야 합니다.\n");
            return 1;
        }
        if (!validateSize(args[2])) {
            print("   SIZE 값은 1 이상 10이하의 값이어야 합니다.\n");
            return 1;
        }
        erase(args[1], args[2]);

        return 0;
    }
    else if (args[0] == "erase_range") {
        if (args.size() != 3) {
            print("   erase_range에는 3개의 인수가 필요합니다. ");
            print("help를 입력해 도움말을 확인하세요.\n");
            return 1;
        }
        if (!validateLba(args[1]) || !validateLba(args[2])) {
            string size = to_string(SIZE - 1);
            print("   LBA 값은 0 이상 " + size + "이하의 값이어야 합니다.\n");
            return 1;
        }
        erase_range(args[1], args[2]);

        return 0;
    }
    else if (args[0] == "flush") {
        if (args.size() != 1) {
            print("   flush에는 1개의 인수가 필요합니다. ");
            print("help를 입력해 상세한 내용을 확인하세요.\n");
            return 1;
        }
        flush();
        return 0;
    }
    else {
        print("   " + args[0] + "에 해당하는 명령어를 찾을 수 없습니다. ");
        print("help를 입력해 도움말을 확인하세요.\n");
        return 1;
    }
}


int write(int start, int end, string value) {
    logger("write("+to_string(start)+","+to_string(end)+","+value + ")", to_string(start)+"~"+to_string(end)+" 범위의 값에 "+value+"값 write 시도 ...");
    int ret = 0;
    for (int i = start; i <= end; i++) {
        ret += write(to_string(i), value);
    }

    logger("write(" + to_string(start) + "," + to_string(end) + "," + value + ")", to_string(start) + "~" + to_string(end) + " 범위의 값에 " + value + "값 write "+(ret==0?"성공":"실패!!!"));
    return ret;
}


int write(int pos, string value) {
    return write(to_string(pos), value);
}


int write(string pos, string value) {
    // value : 0x00000000 ~ 0xFFFFFFFF
    int res = system((DIR + " W " + pos + " " + value.substr(2)).c_str());

    logger("write(" + pos + "," + value + ")", pos + " 영역의 값에 " + value + "값 write " + (res == 0?"성공":"실패!!!"));

    return res;
}


string read(int pos) {
    return read(to_string(pos));
}


string read(string pos) {
    int res = system((DIR + " R " + pos).c_str());
    string ret = (res == 0 ? getResult() : "-1");
    logger("read(" + pos + ")", pos + " 영역에 저장된 값 read" + (res == 0 ? "성공 ("+ret+")" : "실패!!!"));
    return ret;
}


string getResult() {
    ifstream readFile;
    readFile.open(RESULT);
    string tmp;
    getline(readFile, tmp);
    readFile.close();
    return tmp;
}


int erase(int pos, int size) {
    return erase(to_string(pos), (to_string(size)));
}


int erase(string pos, string size) {
    int res = system((DIR + " E " + pos + " " + size).c_str());

    logger("erase(" + pos + "," + size + ")", pos + " 영역부터 "+size+"개의 영역 erase" + (res == 0 ? "성공" : "실패!!!"));
    return res;
}


int erase_range(int start, int end) {
    return erase_range(to_string(start), (to_string(end)));
}


int erase_range(string start, string end) {
    string size = to_string(stoi(end) - stoi(start));
    int res = system((DIR + " E " + start + " " + size).c_str());

    logger("erase(" + start + "," + end + ")", start + " 영역부터 " + end + " 영역까지 erase " + (res == 0 ? "성공" : "실패!!!"));
    return res;
}


int flush() {
    int ret = system((DIR + " F").c_str());
    string res = (ret == 0 ? "성공" : "실패");
    logger("flush()", "SSD Write Buffer flush "+res);
    return ret;
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
    string fileName = "until_" + to_string(t.tm_year + 1900) + (t.tm_mon>=10?"":"0")+to_string(t.tm_mon + 1) + (t.tm_mday >= 10 ?"":"0") + to_string(t.tm_mday) + "_"
        + (t.tm_hour >= 10 ? "" : "0") + to_string(t.tm_hour) + "h_"
        + (t.tm_min >= 10 ? "" : "0") + to_string(t.tm_min) + "m_"
        + (t.tm_sec >= 10 ? "" : "0") + to_string(t.tm_sec) + "s.log";
    return fileName;
}


void print(string msg) {
    if (LOG_DISABLED) return;
    cout << msg;
}


void logger(string funcName, string msg) {
    if (LOG_DISABLED) return;
    string logMsg = msgMaker(funcName, msg);
    fstream log(LOG, ios::app);
    log.seekg(0, ios::end);
    int size = log.tellg();

    if (size + msg.size() >= 10000) {
        log.close();

        for (const auto& entry : filesystem::directory_iterator("./")) {
            string path{ entry.path().u8string() };
            if (path.size() <= 10 || path.substr(2, 5) != "until" || path.substr(path.size() - 3, 3) != "log") continue;
            string logfile = path.substr(2), zipfile = path.substr(0, path.size() - 3) + "zip";

            rename(logfile.c_str(), zipfile.c_str());
        }


        if (rename(LOG.c_str(), fileName().c_str()) != 0) print("log 파일 생성 오류 : "+ fileName()+'\n');
        fstream log(LOG, ios::app);
        log.seekg(0, ios::end);
        for (char ch : logMsg) log.put(ch);
    }
    else {
        for (char ch : logMsg) log.put(ch);
    }
}