#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

// 全局结果字符串
string result;

// 关键字列表
const vector<string> keywords = {
    "asm", "else", "new", "this", "auto", "enum", "operator", "throw", "bool", "explicit", "private", "true",
    "break", "export", "protected", "try", "case", "extern", "public", "typedef", "catch", "false", "register", "typeid",
    "char", "float", "reinterpret_cast", "typename", "class", "for", "return", "union", "const", "friend", "short", "unsigned",
    "const_cast", "goto", "signed", "using", "continue", "if", "sizeof", "virtual", "default", "inline", "static", "void",
    "delete", "int", "static_cast", "volatile", "do", "long", "struct", "wchar_t", "double", "mutable", "switch", "while",
    "dynamic_cast", "namespace", "template", "main", "cout", "cin", "using", "namespace", "std"
};

// 运算符列表
const vector<char> operators = {
    '+', '-', '*', '/', '=', '!', '>', '<', '|', '&', '^', '~', '?', '%'
};

// 判断是否为数字
bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

// 判断是否为字母或下划线
bool isAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

// 判断是否为运算符
bool isOperator(char ch) {
    for (char op : operators) {
        if (ch == op) return true;
    }
    return false;
}

// 判断是否为分隔符
bool isDelimiter(char ch) {
    static const vector<char> delimiters = {'(', ')', ',', ';', '{', '}', '#', '.', ':', '[', ']'};
    for (char d : delimiters) {
        if (ch == d) return true;
    }
    return false;
}

// 判断是否为关键字
bool isKeyword(const string& token) {
    for (const auto& kw : keywords) {
        if (token == kw) return true;
    }
    return false;
}

// 判断是否为十六进制字符 (a-f, A-F)
bool is0X(char ch) {
    return (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
}

// 处理数字（整数、浮点数、科学计数法、十六进制）
void numToken(char ch, ifstream& inputFile) {
    string numberToken;
    numberToken += ch;
    char c;
    while (inputFile.get(c)) {
        if (isDigit(c)) {
            numberToken += c;
            continue;
        } else if (c == '.') { // 浮点数
            numberToken += c;
            while (inputFile.get(c)) {
                if (isDigit(c)) {
                    numberToken += c;
                    continue;
                } else if (c == 'e' || c == 'E') { // 科学计数法
                    numberToken += c;
                    inputFile.get(c);
                    if (c == '+' || c == '-') {
                        numberToken += c;
                    }
                    while (inputFile.get(c)) {
                        if (isDigit(c)) {
                            numberToken += c;
                            continue;
                        }
                        break;
                    }
                }
                break;
            }
        } else if (c == 'e' || c == 'E') { // 整型科学计数法
            numberToken += c;
            inputFile.get(c);
            if (c == '+' || c == '-') {
                numberToken += c;
            }
            while (inputFile.get(c)) {
                if (isDigit(c)) {
                    numberToken += c;
                    continue;
                }
                break;
            }
        } else if (c == 'x' || c == 'X') { // 十六进制
            numberToken += c;
            while (inputFile.get(c)) {
                if (isDigit(c) || is0X(c)) {
                    numberToken += c;
                    continue;
                }
                break;
            }
        }
        break;
    }
    inputFile.unget();
    cout << numberToken << " 数字" << endl;
    result += numberToken + " 数字\n";
}

// 处理字母（关键字或标识符）
void alphaToken(char ch, ifstream& inputFile) {
    string alphaToken;
    alphaToken += ch;
    char c = inputFile.peek();
    while (isAlpha(c) || isDigit(c)) {
        alphaToken += c;
        inputFile.ignore();
        c = inputFile.peek();
    }
    if (isKeyword(alphaToken)) {
        cout << alphaToken << " 关键字" << endl;
        result += alphaToken + " 关键字\n";
    } else {
        cout << alphaToken << " 标识符" << endl;
        result += alphaToken + " 标识符\n";
    }
}

// 处理运算符
void operatorToken(char ch, ifstream& inputFile) {
    string operatorToken;
    operatorToken += ch;
    char nextChar = inputFile.peek();
    if (nextChar == '=') { // 各种 X=
        operatorToken += "=";
        inputFile.ignore();
    } else if (ch == '>' && nextChar == '>') {
        operatorToken = ">>";
        inputFile.ignore();
        if (inputFile.peek() == '=') {
            operatorToken = ">>=";
            inputFile.ignore();
        }
    } else if (ch == '<' && nextChar == '<') {
        operatorToken = "<<";
        inputFile.ignore();
        if (inputFile.peek() == '=') {
            operatorToken = "<<=";
            inputFile.ignore();
        }
    } else if (ch == '&' && nextChar == '&') {
        operatorToken = "&&";
        inputFile.ignore();
    } else if (ch == '|' && nextChar == '|') {
        operatorToken = "||";
        inputFile.ignore();
    } else if (ch == '-' && nextChar == '>') {
        operatorToken = "->";
        inputFile.ignore();
    } else if (ch == '+' && nextChar == '+') {
        operatorToken = "++";
        inputFile.ignore();
    } else if (ch == '-' && nextChar == '-') {
        operatorToken = "--";
        inputFile.ignore();
    }
    cout << operatorToken << " 运算符" << endl;
    result += operatorToken + " 运算符\n";
}

// 判断字符类型
int startCharType(char ch) {
    if (isDigit(ch)) return 1;
    else if (isAlpha(ch)) return 2;
    else if (isDelimiter(ch)) return 3;
    else if (isOperator(ch)) return 4;
    else if (ch == '\n') return 6;
    else if (ch == '"') return 7;
    else if (ch == '\'') return 8;
    else return 5;
}

// 词法分析主逻辑
void analyzeFile(const string& inputFilePath, const string& outputFilePath = "") {
    ifstream inputFile(inputFilePath);
    if (!inputFile) {
        cerr << "错误：无法打开输入文件 " << inputFilePath << endl;
        return;
    }

    result.clear();
    string currentWord;
    char c;

    while (inputFile.get(c)) {
        if (c == '/') { // 处理注释
            currentWord += c;
            if (inputFile.peek() == '/') { // 单行注释
                string currentLine;
                getline(inputFile, currentLine);
                currentWord += currentLine;
                cout << currentWord << " 注释" << endl;
                result += currentWord + " 注释\n";
                currentWord.clear();
                continue;
            } else if (inputFile.peek() == '*') { // 多行注释
                inputFile.ignore();
                currentWord += '*';
                bool commentClosed = false;
                while (inputFile.get(c)) {
                    currentWord += c;
                    if (c == '*' && inputFile.peek() == '/') {
                        inputFile.ignore();
                        currentWord += '/';
                        commentClosed = true;
                        break;
                    }
                }
                if (!commentClosed) {
                    cerr << "错误：多行注释未闭合" << endl;
                } else {
                    cout << currentWord << " 注释" << endl;
                    result += currentWord + " 注释\n";
                }
                currentWord.clear();
                continue;
            } else {
                operatorToken(c, inputFile);
                currentWord.clear();
            }
        } else if (currentWord.empty() && c == '#') { // 头文件
            cout << "#" << "\t特殊符号" << endl;
            result += "# 特殊符号\n";
            while (inputFile.get(c) && (c != '<' && c != '"')) {
                currentWord += c;
            }
            cout << currentWord << "\t关键字" << endl;
            result += currentWord + " 关键字\n";
            currentWord.clear();
            currentWord += c;
            while (inputFile.get(c) && (c != '>' && c != '"')) {
                currentWord += c;
            }
            currentWord += c;
            cout << currentWord << "\t特殊符号" << endl;
            result += currentWord + " 特殊符号\n";
            currentWord.clear();
        } else {
            switch (startCharType(c)) {
                case 1: // 数字
                    numToken(c, inputFile);
                    break;
                case 2: // 字母
                    alphaToken(c, inputFile);
                    break;
                case 3: // 分隔符
                    currentWord += c;
                    cout << c << " 特殊字符" << endl;
                    result += currentWord + " 特殊字符\n";
                    currentWord.clear();
                    break;
                case 4: // 运算符
                    operatorToken(c, inputFile);
                    break;
                case 7: // 字符串
                    currentWord += c;
                    while (inputFile.get(c) && c != '"') {
                        currentWord += c;
                    }
                    currentWord += c;
                    cout << currentWord << " 字符串" << endl;
                    result += currentWord + " 字符串\n";
                    currentWord.clear();
                    break;
                case 8: // 单字符
                    currentWord += c;
                    while (inputFile.get(c) && c != '\'') {
                        currentWord += c;
                    }
                    currentWord += c;
                    cout << currentWord << " 单字符" << endl;
                    result += currentWord + " 单字符\n";
                    currentWord.clear();
                    break;
                default:
                    break;
            }
        }
    }
    inputFile.close();

    // 如果指定了输出文件，写入结果
    if (!outputFilePath.empty()) {
        ofstream outputFile(outputFilePath);
        if (outputFile) {
            outputFile << result;
            outputFile.close();
            cout << "结果已保存到 " << outputFilePath << endl;
        } else {
            cerr << "错误：无法打开输出文件 " << outputFilePath << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "用法: " << argv[0] << " <输入文件> [输出文件]" << endl;
        return 1;
    }

    string inputFilePath = argv[1];
    string outputFilePath = (argc > 2) ? argv[2] : "";

    analyzeFile(inputFilePath, outputFilePath);
    return 0;
}