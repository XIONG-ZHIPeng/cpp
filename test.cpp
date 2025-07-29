#include <iostream>
#include <vector>
using namespace std;

int my_atoi(const char* str) {
    int result = 0;
    int sign = 1;

    // 跳过前导空格
    while (*str == ' ') str++;

    // 处理正负号
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // 遍历每一个字符
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0'); // 将字符转为对应的数字
        str++;
    }

    return result * sign;
}


int main() {
    
    vector<string> test_cases = {
        "123",
        "-123",
        "   456",
        "+789",
        "0",
        "   -42",
        "42abc", // 测试非数字字符
        "abc42", // 测试前面有非数字字符
        "2147483647", // 测试最大整数
        "-2147483648" // 测试最小整数
    };

    for (const auto& test : test_cases) {
        int result = my_atoi(test.c_str());
        cout << "my_atoi(\"" << test << "\") = " << result << endl;
    }

    return 0;
}