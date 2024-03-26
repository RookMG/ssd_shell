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
	// delim을 구분자로 string 토큰화
	// delim이 정규식이 아닌 find 방식으로 동작함에 주의

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
	// 한 줄 입력받아 실행
	// 정상 동작시 return 0, exit 시 return -1, 예외 발생 시 return 1

	// 빈 줄 입력시 계속 입력 대기
	if (args[0]=="") return 0;

	if (args[0] == "exit") {
		cout << "   프로그램을 종료합니다.\n";
		return -1;
	}

	else if (args[0] == "help") {
		if (args.size() == 1) {
			cout << "\n   사용 가능한 명령어는 다음과 같습니다\n\n\n"
				<< "   write {LBA} {VALUE}\t: LBA(0~" << SIZE - 1 << ") 영역의 값을 VALUE(0x00000000~0xFFFFFFFF)로 바꿉니다.\n\n"
				<< "   read {LBA}\t\t: LBA(0~" << SIZE - 1 << ") 영역의 값을 읽어 출력합니다.\n\n"
				<< "   fullwrite {VALUE}\t: 모든 LBA(0~" << SIZE - 1 << ") 영역의 값을 VALUE(0x00000000~0xFFFFFFFF)로 바꿉니다.\n\n"
				<< "   fullread\t\t: 모든 LBA(0~" << SIZE - 1 << ") 영역의 값을 읽어 출력합니다.\n\n"
				<< "   help\t\t\t: 이 도움말을 표시합니다.\n\n"
				<< "   exit\t\t\t: 프로그램을 종료합니다.\n\n";
		}
		else {
			cout << "   " << args[1] << "에 해당하는 도움말이 없습니다. \n";
		}
		return 0;
	}

	else if (args[0] == "write") {
		if (args.size() != 3) {
			cout << "   write에는 3개의 인수가 필요합니다. "
				<< "help를 입력해 도움말을 확인하세요.\n";
			return 1;
		}
		if (!validateLba(args[1])) {
			cout << "   LBA 값은 0 이상 " << SIZE - 1 << "이하의 값이어야 합니다.\n";
			return 1;
		}
		if (!validateValue(args[2])) {
			cout << "   저장할 값은 0x00000000 이상 0xFFFFFFFF 이하의 값이어야 합니다.\n";
			return 1;
		}
		write(args[1], args[2]);
		return 0;
	}
	else if (args[0] == "read" || args[0] == "R") {
		if (args.size() != 2) {
			cout << "   read에는 2개의 인수가 필요합니다. "
				<< "help를 입력해 상세한 내용을 확인하세요.\n";
			return 1;
		}
		if (!validateLba(args[1])) {
			cout << "   LBA 값은 0 이상 " << SIZE - 1 << "이하의 값이어야 합니다.\n";
			return 1;
		}
		read(args[1]);
		return 0;
	}
	else if (args[0] == "fullwrite") {
		if (args.size() != 2) {
			cout << "   fullwrite에는 2개의 인수가 필요합니다. "
				<< "help를 입력해 상세한 내용을 확인하세요.\n";
			return 1;
		}
		if (!validateValue(args[1])) {
			cout << "   저장할 값은 0x00000000 이상 0xFFFFFFFF 이하의 값이어야 합니다.\n";
			return 1;
		}
		for (int i = 0; i < SIZE; i++)  write(i, args[1]);
		return 0;
	}
	else if (args[0] == "fullread") {
		if (args.size() != 1) {
			cout << "   fullread에는 1개의 인수가 필요합니다. "
				<< "help를 입력해 상세한 내용을 확인하세요.\n";
			return 1;
		}
		for (int i = 0; i < SIZE; i++) read(i);
		return 0;
	}
	else if (args[0] == "testapp1") {
		if (args.size() != 2) {
			cout << "   testapp1에는 2개의 인수가 필요합니다.\n";
			return 1;
		}
		if (!validateValue(args[1])) {
			cout << "   저장할 값은 0x00000000 이상 0xFFFFFFFF 이하의 값이어야 합니다.\n";
			return 1;
		}

		for (int i = 0; i < SIZE; i++) {
			validateWrite(i, args[1]);
		}

		return 0;
	}
	else if (args[0] == "testapp2") {
		if (args.size() != 1) {
			cout << "   testapp2에는 1개의 인수가 필요합니다.\n";
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
		cout << "   " << args[0] << "에 해당하는 명령어를 찾을 수 없습니다. "
			 << "help를 입력해 도움말을 확인하세요.\n";
		return 1;
	}
}



bool validateWrite(int pos, string value) {
	write(pos, value);
	string res = read(pos);
	bool ret = (value == read(pos));

	// TODO : logger 분리
	cout << "   " << pos << (pos<10? "  번 데이터 " :" 번 데이터 ") << (ret ? "정상" : "오류") << " (기댓값 : " << value << ", 저장된 값 : " << res << ")\n";

	return ret;
}


int write(int pos, string value) {
	return write(to_string(pos), value);
}


int write(string pos, string value) {
	// value : 0x00000000 ~ 0xFFFFFFFF
	int res = system((DIR + " write " + pos + " " + value.substr(2)).c_str());
	if (res != 0) cout << "   write " + pos + " : 오류 발생\n";
	return res;
}


string read(int pos) {
	return read(to_string(pos));
}


string read(string pos) {
	int res = system((DIR + " read " + pos).c_str());
	if (res != 0) cout << "   read " + pos + " : 오류 발생\n";
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