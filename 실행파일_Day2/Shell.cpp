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
    // delim�� �����ڷ� string ��ūȭ
    // delim�� ���Խ��� �ƴ� find ������� �����Կ� ����

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
    // �� �� �Է¹޾� ����
    // ���� ���۽� return 0, exit �� return -1, ���� �߻� �� return 1

    // �� �� �Է½� ��� �Է� ���
    if (args[0] == "") return 0;
    if (args[0] == "exit") {
        print("   ���α׷��� �����մϴ�.\n");
        return -1;
    }

    else if (args[0] == "help") {
        if (args.size() == 1) {
            string size = to_string(SIZE - 1);
            print("\n   ��� ������ ��ɾ�� ������ �����ϴ�\n\n\n");
            print( "   write {LBA} {VALUE}\t\t\t: LBA(0~" + size + ") ������ ���� VALUE(0x00000000~0xFFFFFFFF)�� �ٲߴϴ�.\n\n");
            print( "   read {LBA}\t\t\t\t: LBA(0~" + size + ") ������ ���� �о� ����մϴ�.\n\n");
            print( "   fullwrite {VALUE}\t\t\t: ��� LBA(0~" + size + ") ������ ���� VALUE(0x00000000~0xFFFFFFFF)�� �ٲߴϴ�.\n\n");
            print( "   fullread\t\t\t\t: ��� LBA(0~" + size + ") ������ ���� �о� ����մϴ�.\n\n");
            print( "   erase {LBA} {SIZE}\t\t\t: LBA(0~" + size + " - SIZE) �������� SIZE�� ������ ���� ����ϴ�.\n\n");
            print( "   erase_range {START_LBA} {END_LBA}\t: START_LBA(0~END_LBA)���� END_LBA(START_LBA~" + size + ") ������ ���� ����ϴ�.\n\n");
            print( "   flush\t\t\t\t: SSD Write Buffer�� ������ ���� Buffer�� ���ϴ�.\n\n");
            print( "   help\t\t\t\t\t: �� ������ ǥ���մϴ�.\n\n");
            print( "   exit\t\t\t\t\t: ���α׷��� �����մϴ�.\n\n");
        }
        else {
            print("   " + args[1] + "�� �ش��ϴ� ������ �����ϴ�. \n");
        }
        return 0;
    }

    else if (args[0] == "write") {
        if (args.size() != 3) {
            print("   write���� 3���� �μ��� �ʿ��մϴ�. ");
            print("help�� �Է��� ������ Ȯ���ϼ���.\n");
            return 1;
        }
        if (!validateLba(args[1])) {
            string size = to_string(SIZE - 1);
            print("   LBA ���� 0 �̻� " + size + "������ ���̾�� �մϴ�.\n");
            return 1;
        }
        if (!validateValue(args[2])) {
            print("   ������ ���� 0x00000000 �̻� 0xFFFFFFFF ������ ���̾�� �մϴ�.\n");
            return 1;
        }
        write(args[1], args[2]);
        return 0;
    }
    else if (args[0] == "read") {
        if (args.size() != 2) {
            print("   read���� 2���� �μ��� �ʿ��մϴ�. ");
            print("help�� �Է��� ���� ������ Ȯ���ϼ���.\n");
            return 1;
        }
        if (!validateLba(args[1])) {
            string size = to_string(SIZE - 1);
            print("   LBA ���� 0 �̻� " + size + "������ ���̾�� �մϴ�.\n");
            return 1;
        }
        read(args[1]);
        return 0;
    }
    else if (args[0] == "fullwrite") {
        if (args.size() != 2) {
            print("   fullwrite���� 2���� �μ��� �ʿ��մϴ�. ");
            print("help�� �Է��� ���� ������ Ȯ���ϼ���.\n");
            return 1;
        }
        if (!validateValue(args[1])) {
            print("   ������ ���� 0x00000000 �̻� 0xFFFFFFFF ������ ���̾�� �մϴ�.\n");
            return 1;
        }
        write(0, SIZE-1, args[1]);
        return 0;
    }
    else if (args[0] == "fullread") {
        if (args.size() != 1) {
            print("   fullread���� 1���� �μ��� �ʿ��մϴ�. ");
            print("help�� �Է��� ���� ������ Ȯ���ϼ���.\n");
            return 1;
        }
        for (int i = 0; i < SIZE; i++) read(i);
        return 0;
    }
    else if (args[0] == "erase") {
        if (args.size() != 3) {
            print("   erase���� 3���� �μ��� �ʿ��մϴ�. ");
            print("help�� �Է��� ������ Ȯ���ϼ���.\n");
            return 1;
        }
        if (!validateLba(args[1])) {
            string size = to_string(SIZE - 1);
            print("   LBA ���� 0 �̻� " + size + "������ ���̾�� �մϴ�.\n");
            return 1;
        }
        if (!validateSize(args[2])) {
            print("   SIZE ���� 1 �̻� 10������ ���̾�� �մϴ�.\n");
            return 1;
        }
        erase(args[1], args[2]);

        return 0;
    }
    else if (args[0] == "erase_range") {
        if (args.size() != 3) {
            print("   erase_range���� 3���� �μ��� �ʿ��մϴ�. ");
            print("help�� �Է��� ������ Ȯ���ϼ���.\n");
            return 1;
        }
        if (!validateLba(args[1]) || !validateLba(args[2])) {
            string size = to_string(SIZE - 1);
            print("   LBA ���� 0 �̻� " + size + "������ ���̾�� �մϴ�.\n");
            return 1;
        }
        erase_range(args[1], args[2]);

        return 0;
    }
    else if (args[0] == "flush") {
        if (args.size() != 1) {
            print("   flush���� 1���� �μ��� �ʿ��մϴ�. ");
            print("help�� �Է��� ���� ������ Ȯ���ϼ���.\n");
            return 1;
        }
        flush();
        return 0;
    }
    else {
        print("   " + args[0] + "�� �ش��ϴ� ��ɾ ã�� �� �����ϴ�. ");
        print("help�� �Է��� ������ Ȯ���ϼ���.\n");
        return 1;
    }
}


int write(int start, int end, string value) {
    logger("write("+to_string(start)+","+to_string(end)+","+value + ")", to_string(start)+"~"+to_string(end)+" ������ ���� "+value+"�� write �õ� ...");
    int ret = 0;
    for (int i = start; i <= end; i++) {
        ret += write(to_string(i), value);
    }

    logger("write(" + to_string(start) + "," + to_string(end) + "," + value + ")", to_string(start) + "~" + to_string(end) + " ������ ���� " + value + "�� write "+(ret==0?"����":"����!!!"));
    return ret;
}


int write(int pos, string value) {
    return write(to_string(pos), value);
}


int write(string pos, string value) {
    // value : 0x00000000 ~ 0xFFFFFFFF
    int res = system((DIR + " W " + pos + " " + value.substr(2)).c_str());

    logger("write(" + pos + "," + value + ")", pos + " ������ ���� " + value + "�� write " + (res == 0?"����":"����!!!"));

    return res;
}


string read(int pos) {
    return read(to_string(pos));
}


string read(string pos) {
    int res = system((DIR + " R " + pos).c_str());
    string ret = (res == 0 ? getResult() : "-1");
    logger("read(" + pos + ")", pos + " ������ ����� �� read" + (res == 0 ? "���� ("+ret+")" : "����!!!"));
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

    logger("erase(" + pos + "," + size + ")", pos + " �������� "+size+"���� ���� erase" + (res == 0 ? "����" : "����!!!"));
    return res;
}


int erase_range(int start, int end) {
    return erase_range(to_string(start), (to_string(end)));
}


int erase_range(string start, string end) {
    string size = to_string(stoi(end) - stoi(start));
    int res = system((DIR + " E " + start + " " + size).c_str());

    logger("erase(" + start + "," + end + ")", start + " �������� " + end + " �������� erase " + (res == 0 ? "����" : "����!!!"));
    return res;
}


int flush() {
    int ret = system((DIR + " F").c_str());
    string res = (ret == 0 ? "����" : "����");
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


        if (rename(LOG.c_str(), fileName().c_str()) != 0) print("log ���� ���� ���� : "+ fileName()+'\n');
        fstream log(LOG, ios::app);
        log.seekg(0, ios::end);
        for (char ch : logMsg) log.put(ch);
    }
    else {
        for (char ch : logMsg) log.put(ch);
    }
}