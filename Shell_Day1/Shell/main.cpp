#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <process.h>

using namespace std;

string DIR = "D:\\SSD\\x64\\Debug\\SSD.exe", NAND = "nand.txt", RESULT = "result.txt";
int SIZE = 100;


vector<string> tokenize(string s);
vector<string> tokenize(string s, string delim);
bool validateLba(string s);
bool validateValue(string s);
bool validateWrite(int pos, string value);
int execute(string s);

int write(int pos, string value);
int write(string pos, string value);
string read(int pos);
string read(string pos);
string getResult();


int main(int argc, char* argv[])
{
	string line;
	vector<string> args;

	while (true) {
		cout << ">> ";
		getline(cin, line);
		if (execute(line) < 0) break;
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
	for (int p; (p = s.find(" ")) != -1;) {
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
	catch(invalid_argument){
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


int execute(string s) {
	vector<string> args = tokenize(s);
	// �� �� �Է¹޾� ����
	// ���� ���۽� return 0, exit �� return -1, ���� �߻� �� return 1

	// �� �� �Է½� ��� �Է� ���
	if (args[0]=="") return 0;

	if (args[0] == "exit") {
		cout << "   ���α׷��� �����մϴ�.\n";
		return -1;
	}

	else if (args[0] == "help") {
		if (args.size() == 1) {
			cout << "\n   ��� ������ ��ɾ�� ������ �����ϴ�\n\n\n"
				<< "   write {LBA} {VALUE}\t: LBA(0~" << SIZE - 1 << ") ������ ���� VALUE(0x00000000~0xFFFFFFFF)�� �ٲߴϴ�.\n\n"
				<< "   read {LBA}\t\t: LBA(0~" << SIZE - 1 << ") ������ ���� �о� ����մϴ�.\n\n"
				<< "   fullwrite {VALUE}\t: ��� LBA(0~" << SIZE - 1 << ") ������ ���� VALUE(0x00000000~0xFFFFFFFF)�� �ٲߴϴ�.\n\n"
				<< "   fullread\t\t: ��� LBA(0~" << SIZE - 1 << ") ������ ���� �о� ����մϴ�.\n\n"
				<< "   help\t\t\t: �� ������ ǥ���մϴ�.\n\n"
				<< "   exit\t\t\t: ���α׷��� �����մϴ�.\n\n";
		}
		else {
			cout << "   " << args[1] << "�� �ش��ϴ� ������ �����ϴ�. \n";
		}
		return 0;
	}

	else if (args[0] == "write") {
		if (args.size() != 3) {
			cout << "   write���� 3���� �μ��� �ʿ��մϴ�. "
				<< "help�� �Է��� ������ Ȯ���ϼ���.\n";
			return 1;
		}
		if (!validateLba(args[1])) {
			cout << "   LBA ���� 0 �̻� " << SIZE - 1 << "������ ���̾�� �մϴ�.\n";
			return 1;
		}
		if (!validateValue(args[2])) {
			cout << "   ������ ���� 0x00000000 �̻� 0xFFFFFFFF ������ ���̾�� �մϴ�.\n";
			return 1;
		}
		write(args[1], args[2]);
		return 0;
	}
	else if (args[0] == "read" || args[0] == "R") {
		if (args.size() != 2) {
			cout << "   read���� 2���� �μ��� �ʿ��մϴ�. "
				<< "help�� �Է��� ���� ������ Ȯ���ϼ���.\n";
			return 1;
		}
		if (!validateLba(args[1])) {
			cout << "   LBA ���� 0 �̻� " << SIZE - 1 << "������ ���̾�� �մϴ�.\n";
			return 1;
		}
		read(args[1]);
		return 0;
	}
	else if (args[0] == "fullwrite") {
		if (args.size() != 2) {
			cout << "   fullwrite���� 2���� �μ��� �ʿ��մϴ�. "
				<< "help�� �Է��� ���� ������ Ȯ���ϼ���.\n";
			return 1;
		}
		if (!validateValue(args[1])) {
			cout << "   ������ ���� 0x00000000 �̻� 0xFFFFFFFF ������ ���̾�� �մϴ�.\n";
			return 1;
		}
		for (int i = 0; i < SIZE; i++)  write(i, args[1]);
		return 0;
	}
	else if (args[0] == "fullread") {
		if (args.size() != 1) {
			cout << "   fullread���� 1���� �μ��� �ʿ��մϴ�. "
				<< "help�� �Է��� ���� ������ Ȯ���ϼ���.\n";
			return 1;
		}
		for (int i = 0; i < SIZE; i++) read(i);
		return 0;
	}
	else if (args[0] == "testapp1") {
		if (args.size() != 2) {
			cout << "   testapp1���� 2���� �μ��� �ʿ��մϴ�.\n";
			return 1;
		}
		if (!validateValue(args[1])) {
			cout << "   ������ ���� 0x00000000 �̻� 0xFFFFFFFF ������ ���̾�� �մϴ�.\n";
			return 1;
		}

		for (int i = 0; i < SIZE; i++) {
			validateWrite(i, args[1]);
		}

		return 0;
	}
	else if (args[0] == "testapp2") {
		if (args.size() != 1) {
			cout << "   testapp2���� 1���� �μ��� �ʿ��մϴ�.\n";
			return 1;
		}
		for (int t = 0; t < 30; t++) {
			for (int i = 0; i <= 5; i++) {
				write(i, "0xAAAABBBB");
			}
		}
		for (int i = 0; i <= 5; i++) {
			validateWrite(i, "0x12345678");
		}
		return 0;
	}
	else{
		cout << "   " << args[0] << "�� �ش��ϴ� ��ɾ ã�� �� �����ϴ�. "
			 << "help�� �Է��� ������ Ȯ���ϼ���.\n";
		return 1;
	}
}



bool validateWrite(int pos, string value) {
	write(pos, value);
	string res = read(pos);
	bool ret = (value == read(pos));

	// TODO : logger �и�
	cout << "   " << pos << (pos<10? "  �� ������ " :" �� ������ ") << (ret ? "����" : "����") << " (��� : " << value << ", ����� �� : " << res << ")\n";

	return ret;
}


int write(int pos, string value) {
	return write(to_string(pos), value);
}


int write(string pos, string value) {
	// value : 0x00000000 ~ 0xFFFFFFFF
	int res = system((DIR + " write " + pos + " " + value.substr(2)).c_str());
	if (res != 0) cout << "   write " + pos + " : ���� �߻�\n";
	return res;
}


string read(int pos) {
	return read(to_string(pos));
}


string read(string pos) {
	int res = system((DIR + " read " + pos).c_str());
	if (res != 0) cout << "   read " + pos + " : ���� �߻�\n";
	return res == 0 ? getResult() : "-1";
}


string getResult() {
	ifstream readFile;
	readFile.open(RESULT);
	string tmp;
	getline(readFile, tmp);
	readFile.close();
	return tmp;
}