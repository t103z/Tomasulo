#include "Ins.h"
#include <vector>
#include <string>
#include <fstream>

using namespace std;

int getCycleOfInsOp(InsOp op) {
    switch (op){
        case InsOp::ADDD:
        case InsOp::SUBD:
        case InsOp::LD:
        case InsOp::ST:
            return 2;
        case InsOp::MULTD:
            return 10;
        case InsOp::DIVD:
            return 40;
        default:
            assert(false);
            return 0;
    }
}

string strip(const string& str) {
    return str.substr(str.find_first_not_of(' '), str.find_last_of(' '));
}

void toUpper(string& s){
    std::transform(s.begin(),s.end(),s.begin(), ::toupper);
}

vector<string> split(const string& str){
    vector<string> ss;
    string tmp;
    for(auto&& s:str){
        if (s != ' ' && s != ',') tmp.push_back(s);
        else if (tmp.size()) {
            ss.push_back(tmp);
            tmp.clear();
        }
    }
    if (tmp.size())
        ss.push_back(tmp);
    return ss;
}

pair<bool, Ins> parseInsFromLine(const string& line) {
    Ins ins;
    auto buf = split(line); int n = buf.size();
    if (n != 3 && n != 4) return {false, ins};

    ins.op = getOpFromStr(strip(buf[0]));
    if (ins.op == InsOp::UNINIT) return {false, ins};

    if (n == 3) {
        if (!isMemIns(&ins)) return {false, ins};

        string s = strip(buf[1]);
        toUpper(s);
        int x;
        if (sscanf(s.c_str(), "F%d", &x) != 1){
            return {false, ins};
        } else {
            if ((unsigned)x > REG_SIZE) return {false, ins};
            ins.memReg = x;
        }

        s = strip(buf[2]);
        toUpper(s);
        if (sscanf(s.c_str(), "%d", &x) != 1){
            return {false, ins};
        } else {
            if ((unsigned)x > MEM_SIZE) return {false, ins};
            ins.memAddr = x;
        }
    } else {
        assert(n == 4);
        string s = strip(buf[1]);
        toUpper(s);
        int x;
        if (sscanf(s.c_str(), "F%d", &x) != 1){
            return {false, ins};
        } else {
            if ((unsigned)x > REG_SIZE) return {false, ins};
            ins.des = x;
        }

        s = strip(buf[2]);
        toUpper(s);
        if (sscanf(s.c_str(), "F%d", &x) != 1){
            return {false, ins};
        } else {
            if ((unsigned)x > REG_SIZE) return {false, ins};
            ins.src1 = x;
        }

        s = strip(buf[3]);
        toUpper(s);
        if (sscanf(s.c_str(), "F%d", &x) != 1){
            return {false, ins};
        } else {
            if ((unsigned)x > REG_SIZE) return {false, ins};
            ins.src2 = x;
        }
    }

    return {true, ins};
}

pair<vector<int>, vector<Ins>> Ins::loadInsFromFile(const string& fname) {
    vector<int> errLine;
    vector<Ins> inss;
    ifstream fin(fname);
    if (!fin) return {errLine, inss};

    string line;
    int n = 0;
    while (getline(fin, line)) {
        ++n;
        auto r = parseInsFromLine(line);
        if (r.first) {
            inss.push_back(r.second);
        } else {
            errLine.push_back(n);
        }
    }

    fin.close();
    return {errLine, inss};
}