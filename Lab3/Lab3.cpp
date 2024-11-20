#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

enum class TokenType {
    ReservedWord,
    Keyword,
    Operator,
    Punctuation,
    Number,
    StringLiteral,
    CharLiteral,
    PreprocessorDirective,
    Comment,
    Identifier,
    Unknown
};

// Клас Token
class Token {
public:
    string lexeme;  
    TokenType type;    

    Token(const string& lexeme, TokenType type) : lexeme(lexeme), type(type) {}
};

vector<string> reserved = {
    "abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char",
    "checked", "class", "const", "continue", "decimal", "default", "delegate",
    "do", "double", "else", "enum", "event", "explicit", "extern", "false",
    "finally", "fixed", "float", "for", "foreach", "goto", "if", "implicit",
    "in", "int", "interface", "DateTime", "internal", "is", "lock", "long", "namespace",
    "new", "null", "object", "operator", "out", "override", "params", "private", "partial",
    "protected", "public", "readonly", "ref", "return", "sbyte", "sealed",
    "short", "sizeof", "stackalloc", "static", "string", "struct", "switch",
    "this", "throw", "true", "try", "typeof", "uint", "ulong", "unchecked",
    "unsafe", "ushort", "using", "var", "virtual", "void", "volatile", "while", "get", "set", "Guid"
};


vector<string> operators = {
    "=", "+", "-", "*", "/", "%", "&&", "||", "!", "==", "!=", "<", ">", "<= ", ">=",
    "+=", "-=", "*=", "/=", "%=", "&&=", "||=", "!", "++", "--"
};

vector<string> punctuation = {
    "(", ")", "{", "}", "[", "]", ",", ";", ":", "=>", "."
};

//vector<string> dataTypes = {
//    "int", "float", "double", "decimal", "short", "long", "byte",
//    "string", "char", "bool", "Guid"
//};

vector<char> digits = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
vector<char> letters = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '?', '.'
};

vector<string> identifiers;

vector<string> divideToPhrases(string code) {
    vector<string> result;
    string phrase;
    size_t i = 0;

    while (i < code.size()) {
        char ch = code[i];
        string punct(1, ch); // Створюємо рядок з одного символу

        if (ch == '#') {
            // Директива препроцесора
            if (!phrase.empty()) {
                result.push_back(phrase);
                phrase.clear();
            }
            phrase += ch;
            i++;
            while (i < code.size() && code[i] != '\n') {
                phrase += code[i++];
            }
            result.push_back(phrase);
            phrase.clear();
        }
        else if (ch == '/' && i + 1 < code.size()) {
            // Перевірка на коментарі
            if (code[i + 1] == '/') {
                // Однорядковий коментар
                if (!phrase.empty()) {
                    result.push_back(phrase);
                    phrase.clear();
                }
                phrase += "//";
                i += 2;
                while (i < code.size() && code[i] != '\n') {
                    phrase += code[i++];
                }
                result.push_back(phrase);
                phrase.clear();
            }
            else if (code[i + 1] == '*') {
                // Багаторядковий коментар
                if (!phrase.empty()) {
                    result.push_back(phrase);
                    phrase.clear();
                }
                phrase += "/*";
                i += 2;
                while (i < code.size() && !(code[i] == '*' && i + 1 < code.size() && code[i + 1] == '/')) {
                    phrase += code[i++];
                }
                if (i < code.size() && code[i] == '*' && code[i + 1] == '/') {
                    phrase += "*/";
                    i += 2;
                }
                result.push_back(phrase);
                phrase.clear();
            }
            else {
                // Звичайний символ `/`
                phrase += ch;
                i++;
            }
        }
        else if (ch == '"') {
            // Рядок у лапках
            if (!phrase.empty()) {
                result.push_back(phrase);
                phrase.clear();
            }
            phrase += ch;
            i++;
            while (i < code.size() && code[i] != '"') {
                phrase += code[i++];
            }
            if (i < code.size() && code[i] == '"') {
                phrase += '"'; // Додаємо закриваючу лапку
                i++;
            }
            result.push_back(phrase);
            phrase.clear();
        }
        else if (find(punctuation.begin(), punctuation.end(), punct) != punctuation.end()) {
            // Знак пунктуації
            if (!phrase.empty()) {
                result.push_back(phrase);
                phrase.clear();
            }
            result.push_back(punct);
            i++;
        }
        else if (ch == ';') {
            // Завершення фрази
            if (!phrase.empty()) {
                result.push_back(phrase);
                phrase.clear();
            }
            i++;
        }
        else if (ch != '\n') {
            // Інші символи
            phrase += ch;
            i++;
        }
        else {
            // Новий рядок
            i++;
        }
    }

    // Додаємо останню фразу, якщо вона не порожня
    if (!phrase.empty()) {
        result.push_back(phrase);
    }

    return result;
}


bool isLetterOrDigit(char ch) {
    return find(letters.begin(), letters.end(), ch) != letters.end() ||
        find(digits.begin(), digits.end(), ch) != digits.end();
}

// Функція для перевірки ідентифікатора
bool isValidIdentifier(const string& identifier) {
    // Якщо ідентифікатор вже є у векторі, повертаємо true
    if (find(identifiers.begin(), identifiers.end(), identifier) != identifiers.end()) {
        return true;
    }

    // Перевірка на '@' на початку, якщо він є, ідентифікатор може бути зарезервованим словом
    int startIndex = 0;
    if (identifier[0] == '@') {
        startIndex = 1;
    }

    // Перевірка, чи не є рядок зарезервованим словом без '@'
    if (startIndex == 0 && find(reserved.begin(), reserved.end(), identifier) != reserved.end()) {
        return false;
    }

    // Перевірка, чи починається ідентифікатор з літери або підкреслення
    if (!(find(letters.begin(), letters.end(), identifier[startIndex]) != letters.end() || identifier[startIndex] == '_')) {
        return false;
    }

    // Перевірка, чи складається рядок тільки з літер, цифр і підкреслень
    for (int i = startIndex; i < identifier.size(); i++) {
        char ch = identifier[i];
        if (!(isLetterOrDigit(ch) || ch == '_')) {
            return false;
        }
    }

    // Якщо ідентифікатор пройшов усі перевірки, додаємо його до вектора
    identifiers.push_back(identifier);

    return true;
}


bool isNumber(const std::string& word) {
    if (word.empty()) return false; // Порожній рядок не є числом

    size_t i = 0;
    bool hasDecimalPoint = false;
    bool hasExponent = false;

    // Перевірка наявності знака +/- на початку
    if (word[i] == '+' || word[i] == '-') {
        ++i;
    }

    // Перевірка на шістнадцяткове число
    if (i + 1 < word.size() && word[i] == '0' && (word[i + 1] == 'x' || word[i + 1] == 'X')) {
        i += 2;
        if (i >= word.size()) return false; // Після "0x" не може бути порожньо
        // Перевіряємо, чи всі наступні символи є шістнадцятковими
        for (; i < word.size(); ++i) {
            if (!std::isxdigit(word[i])) {
                return false;
            }
        }
        return true; // Це шістнадцяткове число
    }

    // Перевірка на десяткові або плаваючі числа
    bool hasDigits = false;

    for (; i < word.size(); ++i) {
        if (std::isdigit(word[i])) {
            hasDigits = true;
        }
        else if (word[i] == '.') {
            if (hasDecimalPoint || hasExponent) {
                return false; // Друга крапка або крапка після експоненти — некоректно
            }
            hasDecimalPoint = true;
        }
        else if (word[i] == 'e' || word[i] == 'E') {
            if (hasExponent || !hasDigits) {
                return false; // Друга експонента або експонента без цифр — некоректно
            }
            hasExponent = true;
            hasDigits = false; // Після 'e' повинні йти цифри
            if (i + 1 < word.size() && (word[i + 1] == '+' || word[i + 1] == '-')) {
                ++i; // Пропускаємо знак після 'e'
            }
        }
        else {
            return false; // Некоректний символ
        }
    }

    return hasDigits; // Повинні бути цифри для коректного числа
}

bool isStringLiteral(const std::string& word) {
    if (word.size() < 2) return false; // Мінімальна довжина рядка - 2 символи (наприклад, `""`)

    // Перевірка першого та останнього символа
    if (word.front() == '"' && word.back() == '"') {
        // Перевірка на правильність екранування
        for (size_t i = 1; i < word.size() - 1; ++i) {
            if (word[i] == '\\' && i + 1 < word.size()) {
                ++i; // Пропустити екранований символ
            }
        }
        return true;
    }
    return false;
}

bool isCharType(const string& word) {
    if (word.size() == 3 && word[0] == '\'' && word[2] == '\'') {
        char middleChar = word[1]; // Символ посередині
        // Перевірка, чи символ належить до digits або letters
        if (find(digits.begin(), digits.end(), middleChar) != digits.end() ||
            find(letters.begin(), letters.end(), middleChar) != letters.end()) {
            return true;
        }
    }
    return false;
}

vector<Token> analyzePhrases(const vector<string>& phrases) {
    vector<Token> tokens;

    for (const string& phrase : phrases) {
        TokenType type = TokenType::Unknown;

        if (!phrase.empty() && phrase[0] == '#') {
            type = TokenType::PreprocessorDirective;
        }

        else if (!phrase.empty() && phrase[0] == '/') {
            if (phrase.size() > 1 && (phrase[1] == '/' || phrase[1] == '*')) {
                type = TokenType::Comment;
            }
        }
        else if (isStringLiteral(phrase)) {
                type = TokenType::StringLiteral;
        }
        else {
            istringstream stream(phrase);
            string word;

            while (stream >> word) {
                if (find(reserved.begin(), reserved.end(), word) != reserved.end()) {
                    type = TokenType::Keyword;
                }
                else if (find(operators.begin(), operators.end(), word) != operators.end()) {
                    type = TokenType::Operator;
                }
                else if (find(punctuation.begin(), punctuation.end(), word) != punctuation.end()) {
                    type = TokenType::Punctuation;
                }
                else if (isNumber(word)) {
                    type = TokenType::Number;
                }
                else if (isCharType(word)) {
                    type = TokenType::CharLiteral;
                }
                else if (isValidIdentifier(word)) {
                    type = TokenType::Identifier;
                }
                else {
                    type = TokenType::Unknown;
                }

                // Додаємо токен до масиву
                tokens.emplace_back(word, type);
            }
            continue;
        }

        tokens.emplace_back(phrase, type);
    }

    return tokens;
}


int main() {
    vector<string> phrases;
    string code;
    string filename;

    cout << "Enter the file name:" << endl;
    cin >> filename;
    cin.ignore();

    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    string line;
    while (getline(file, line)) {
        code += line + "\n";
    }

    file.close();

    phrases = divideToPhrases(code);

    //vector<string> ::iterator iter = phrases.begin();
    //for (; iter != phrases.end(); iter++) {
    //    cout << *iter << endl;
    //}


    vector<Token> tokens = analyzePhrases(phrases);

    for (const Token& token : tokens) {
        cout << "<" << token.lexeme << " , ";
        switch (token.type) {
        case TokenType::ReservedWord: cout << "ReservedWord>"; break;
        case TokenType::Keyword: cout << "Keyword>"; break;
        case TokenType::Operator: cout << "Operator>"; break;
        case TokenType::Punctuation: cout << "Punctuation>"; break;
        case TokenType::Number: cout << "Number>"; break;
        case TokenType::StringLiteral: cout << "StringLiteral>"; break;
        case TokenType::CharLiteral: cout << "CharLiteral>"; break;
        case TokenType::PreprocessorDirective: cout << "PreprocessorDirective>"; break;
        case TokenType::Comment: cout << "Comment>"; break;
        case TokenType::Identifier: cout << "Identifier>"; break;
        case TokenType::Unknown: cout << "Unknown>"; break;
        }
        cout << endl;
    }

    return 0;
}