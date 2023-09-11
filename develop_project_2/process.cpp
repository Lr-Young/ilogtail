#include<iostream>
#include<string>
#include<vector>
#include "ctype.h"
#include "Log.h"

using namespace std;


void test();
std::string LogToString(Log& log);
void Process(std::string foramt, vector<std::string>& input, vector<Log>& output);
Content getContent(std::string key, std::string value);
std::string jumpStr(int step, char **strPtrPtr);
int realStep(int min, int max, int step);
std::string removeSpace(std::string str, int n, bool leftAligned);


int main() {
    test();
    return 0;
}

void test() {
    string format = "%caller*%caller%n%date";
    string str = "Caller+0	 at com.logback.test.Logback.logPattern(Logback.java:61)\nCaller+1	 at com.logback.test.Logback.main(Logback.java:16)\n*Caller+0	 at com.logback.test.Logback.logPattern(Logback.java:61)\n\n2021-02-02 11:49:49,003";
    vector<string> input;
    input.push_back(str);
    vector<Log> output;
    Process(format, input, output);
    
    for (auto it : output) {
        cout << "Log :" << endl;
        cout << LogToString(it);
    }
}

std::string LogToString(Log& log) {
    std::string res = "";
    for (vector<Content>::iterator it = log.contents.begin(); it != log.contents.end(); it++) {
        res.append("\"" + (*it).key + "\": ");
        res.append("\"" + (*it).value + "\"\n");
    }
    return res;
}

void Process(std::string format, vector<std::string>& input, vector<Log>& output) {
    char *formatPtr;
    char *strPtr;
    for(vector<std::string>::iterator it = input.begin(); it != input.end(); it++) {
        std::string str = *it;
        formatPtr = &format[0];
        strPtr = &str[0];
        Log log;
        while (*formatPtr != '\0') {
            if (*formatPtr == '%') {
                bool leftAligned = false;
                int min = -1;
                int max = -1;
                int step = 0;
                std::string value;
                formatPtr++;
                if (*(formatPtr) == '-') {
                    leftAligned = true;
                    formatPtr++;
                }
                if (isdigit(*formatPtr)) {
                    min = *formatPtr - '0';
                    formatPtr++;
                    while (isdigit(*formatPtr)) {
                        min = 10 * min + *formatPtr - '0';
                        formatPtr++;
                    } 
                }
                if (*formatPtr == '.') {
                    formatPtr++;
                    if (*formatPtr == '-') {
                        // .-{max}
                        formatPtr++;
                    }
                    max = *formatPtr - '0';
                    formatPtr++;
                    while (isdigit(*formatPtr)) {
                        max = 10 * max + *formatPtr - '0';
                        formatPtr++;
                    }
                }
                if (isalpha(*formatPtr)) {
                    int conversionLength;
                    for (conversionLength = 1; isalpha(*(formatPtr + conversionLength)); conversionLength++);
                    std::string conversion = formatPtr;
                    conversion = conversion.substr(0, conversionLength);
                    formatPtr += conversionLength;
                    if (conversion == "c" | conversion == "lo" | conversion == "logger") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("logger", value));
                    } else if (conversion == "C" | conversion == "class") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("class", value));
                    } else if (conversion == "cn" | conversion == "contextName") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("contextName", value));
                    }  else if (conversion == "d" | conversion == "date") {
                        step = 23;
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("date", value));
                    } else if (conversion == "ms" | conversion == "micros") {
                        for (; isdigit(*(strPtr + step)) ; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("micros", value));
                    } else if (conversion == "F" | conversion == "file") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("file", value));
                    } else if (conversion == "caller") {
                        step = 8; // Caller+0
                        while (true) {
                            if (*(strPtr + step) == '\n') {
                                std:string tmp = strPtr + step;
                                if (tmp.length() < 8 || tmp.substr(1, 7) != "Caller+") {
                                    step++;
                                    break;
                                } else {
                                    step += 9;
                                }
                            } else {
                                step++;
                            }
                        }
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("caller", value));
                    } else if (conversion == "kvp") {
                        // TODO
                    } else if (conversion == "L" | conversion == "line") {
                        for (; isdigit(*(strPtr + step)); step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("line", value));
                    } else if (conversion == "message" | conversion == "msg" | conversion == "m") {
                        for (; *(strPtr + step) != '\n' && *(strPtr + step) != '\0'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("message", value));
                    } else if (conversion == "M" | conversion == "method") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)); step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("method", value));
                    } else if (conversion == "n") {
                        strPtr++;
                    } else if (conversion == "p" | conversion == "level" | conversion == "le") {
                        if (*strPtr == 'W' || *strPtr == 'I') {
                            step = 4;
                        } else {
                            step = 5;
                        }
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("level", value));
                    } else if (conversion == "r" | conversion == "relative") {
                        for (; isdigit(*(strPtr + step)); step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("relative", value));
                    } else if (conversion == "t" | conversion == "thread") {
                        for (; isalpha(*(strPtr + step)); step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("thread", value));
                    } else if (conversion == "X" | conversion == "mdc") {
                        // TODO
                    } else if (conversion == "ex" | conversion == "throwable" | conversion == "exception") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("exception", value));
                    } else if (conversion == "xEx" | conversion == "xThrowable" | conversion == "xException") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("eException", value));
                    } else if (conversion == "nopex" | conversion == "nopexception") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("nopexception", value));
                    } else if (conversion == "marker") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("marker", value));
                    } else if (conversion == "property") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("property", value));
                    } else if (conversion == "replace") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("replace", value));
                    } else if (conversion == "prefix") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("prefix", value));
                    } else if (conversion == "rEx" | conversion == "rootException") {
                        for (; isalpha(*(strPtr + step)) | isdigit(*(strPtr + step)) | *(strPtr + step) == '.'; step++);
                        value = jumpStr(realStep(min, max, step), &strPtr);
                        value = removeSpace(value, min - step, leftAligned);
                        log.contents.push_back(getContent("rootException", value));
                    } 
                } else {
                    // no chance to get here
                }
            } else if (*formatPtr == '\\') {
                if (*(formatPtr + 1) == '%') {
                    formatPtr += 2;
                    strPtr++;
                } else {
                    // TODO: other '\' chars
                }
            } else {
                formatPtr++;
                strPtr++;
            }
        }
        output.push_back(log);
    }
}

std::string jumpStr(int step, char **strPtrPtr) {
    std::string res = *strPtrPtr;
    res = res.substr(0, step);
    *strPtrPtr += step;
    return res;
}

Content getContent(std::string key, std::string value) {
    Content content;
    content.key = key;
    content.value = value;
    return content;
}

std::string removeSpace(std::string str, int n, bool leftAligned) {
    if (n <= 0) {
        return str;
    }
    if (leftAligned) {
        return str.substr(0, str.length() - n);
    } else {
        return str.substr(n);
    }
}

int realStep(int min, int max, int step) {
    if (min > 0 && step < min) return min;
    if (max > 0 && step > max) return max;
    return step;
}