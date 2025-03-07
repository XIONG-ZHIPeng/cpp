## Procedural versus Object-oriented programming
 - 面向对象和面向过程语言的区别
    - 面向过程就是分析出解决问题所需要的步骤，然后用函数把这些步骤一步一步实现，使用的时候一个一个依次调用就可以了。

    - 面向对象是把构成问题事务分解成各个对象，建立对象的目的不是为了完成一个步骤，而是为了描叙某个事物在整个解决问题的步骤中的行为。

## Encapsulation, Polymorphism and Inheritance
 - 封装
   - 封装就是将抽象得到的数据和行为相结合，形成一个有机的整体，也就是将数据与操作数据的源代码进行有机的结合，形成类，其中数据和函数都是类的成员，目的在于将对象的使用者和设计者分开，可以隐藏实现细节包括包含私有成员，使得代码模块增加安全指数，同时提高软件的可维护性和可修改性。
   - 特点:
     - 结合性，即是将属性和方法结合
     - 信息隐蔽性，利用接口机制隐蔽内部实现细节，只留下接口给外界调用
     - 实现代码重用
 - 继承
   - 类的派生指的是从已有类产生新类的过程。原有的类成为基类或父类，产生的新类称为派生类或子类，子类继承基类后，可以创建子类对象来调用基类的函数，变量等。
   - 继承方式:
     - 单一继承：继承一个父类，这种继承称为单一继承
     - 多重继承：一个派生类继承多个基类，类与类之间要用逗号隔开，类名之前要有继承权限，假使两个或两个基类都有某变量或函数，在子类中调用时需要加类名限定符如obj.classA::i = 1
     - 菱形继承：多重继承掺杂隔代继承1-n-1模式，此时需要用到虚继承，例如 B，C虚拟继承于A，D再多重继承B，C，否则会出错
    ![Inheritance](Images/ch1/inheritance.png)
    - 多态
      - 多态是以封装和继承为基础实现的性质，一个形态的多种表现方式。硬要解释的话可以说是一个接口，多个功能，在用父类指针调用函数时，实际调用的是指针指向的实际类型（子类）的成员函数。
        - 静态多态。静态多态的设计思想：对于相关的对象类型，直接实现它们各自的定义，不需要共有基类，甚至可以没有任何关系。只需要各个具体类的实现中要求相同的接口声明，静态多态本质上就是模板的具现化。
        - 动态多态。对于相关的对象类型，确定它们之间的一个共同功能集，然后在基类中，把这些共同的功能声明为多个公共的虚函数接口。各个子类重写这些虚函数，以完成具体的功能。具体实现就是c++的虚函数。
      - c++多态有以下几种：
        - 重载。函数重载和运算符重载，编译期-静态绑定
        - 虚函数。子类的多态性，运行期-动态绑定
        - 模板，类模板，函数模板。编译期

## c++98, 11, 17的区别

| C++ Version | Release Year | Key Features |
|-------------|--------------|--------------|
| C++98       | 1998         | STL, Exception Handling, Namespaces, OOP, iostream, bool type |
| C++11       | 2011         | Auto, nullptr, decltype, Lambda, Smart Pointers, Move Semantics, Concurrency, for-each, constexpr |
| C++17       | 2017         | Structured Binding, std::optional, std::variant, if constexpr, Filesystem Library, Fold Expressions |

- C++98
  - C++98 是 C++ 语言的第一个国际标准
  - 特性：
	1.	标准模板库（STL）
        • 容器：vector, list, map, set, queue, stack, priority_queue
        • 迭代器：begin(), end()
        • 算法库：sort(), find(), copy(), accumulate()
        • 适配器：bind1st, bind2nd, function object
	2.	异常处理
        • try-catch 机制，throw 关键字用于抛出异常
    3.	命名空间（namespace）
        • 解决全局命名冲突
    4.	面向对象编程（OOP）
        •	抽象类、继承、多态、虚函数等 OOP 机制
    5.	输入输出流（iostream）
        •	cin, cout, cerr, ifstream, ofstream
    6.	bool 类型
        •	C++98 明确引入了 bool 类型（C 语言使用 int）
- C++11
  - C++11 是对 C++98 的重大升级，引入了很多现代 C++ 特性，提高了可读性、可维护性、性能和并发能力
  - 主要新特性
    1.	自动类型推导（auto）
        •	让编译器推断变量类型
    2.	nullptr 代替 NULL
        •	解决 NULL 可能被解析为 int 的问题
    3.	decltype 关键字
        •	用于获取表达式的类型
    4.	Lambda 表达式
        •	允许在代码中定义匿名函数
    5.	智能指针（std::unique_ptr, std::shared_ptr）
        •	解决手动 new/delete 导致的内存泄漏问题
    6.	移动语义（Move Semantics）和 rvalue 引用
        •	允许高效移动资源，而不是拷贝
    7.	并发库（<thread>）
        •	允许创建和管理线程：
    8.	for-each 语法
        •	让循环更加简洁
    9.	constexpr 关键字
        • 允许在编译期计算常量
- C++17（2017年）
  - C++17 在 C++11 的基础上进行了优化，主要目标是简化代码，提高可读性和性能
    - 新特性：
      - 结构化绑定（Structured Binding）
        - 允许解构 tuple 或 pair
      - std::optional
        - 解决返回值可能为空的问题
      - std::variant
        - 允许存储不同类型的数据
      - if constexpr
        - 允许在编译期进行分支优化
      - 文件系统库（<filesystem>）
        - 允许操作文件和目录
      - 折叠表达式（Fold Expressions）
        - 让 ... 变参展开更简单

## 一个cpp程序文件到运行的过程

一个 C++ 程序 从 源代码（.cpp 文件） 到 可执行程序 再到 运行，通常经历 四个主要阶段：
	1.	预处理（Preprocessing）
	2.	编译（Compilation）
	3.	汇编（Assembly）
	4.	链接（Linking）
	5.	加载和执行（Loading & Execution）

⸻

1. 预处理（Preprocessing）

预处理器（Preprocessor） 处理 #include、#define、#ifdef 等指令，生成预处理后的代码。

🔹 主要任务
	•	处理 #include 指令，插入头文件内容。
	•	处理 #define 和 #ifdef 预处理宏。
	•	删除注释。

🔹 示例

#include <iostream>
#define PI 3.14

int main() {
    std::cout << "PI: " << PI << std::endl;
    return 0;
}

🔹 预处理后代码

// 头文件展开
#include <iostream>  // 这部分内容会被展开并插入

int main() {
    std::cout << "PI: " << 3.14 << std::endl;
    return 0;
}

🔹 查看预处理结果

g++ -E program.cpp -o program.i

.i 文件是预处理后的代码。

⸻

2. 编译（Compilation）

编译器（Compiler） 负责将 预处理后的代码 转换为 汇编代码（Assembly Code）。

🔹 主要任务
	•	语法分析（Syntax Analysis）
	•	语义分析（Semantic Analysis）
	•	生成中间代码
	•	代码优化
	•	生成汇编代码（.s 文件）

🔹 示例

g++ -S program.i -o program.s

.s 文件是汇编代码，它是与特定 CPU 体系结构相关的低级代码。

⸻

3. 汇编（Assembly）

汇编器（Assembler） 负责将 汇编代码（.s 文件） 转换成 机器码（.o 或 .obj 文件），也叫 目标文件（Object File）。

🔹 主要任务
	•	把汇编代码转换为二进制机器指令
	•	生成目标文件 .o（Linux）或 .obj（Windows）

🔹 示例

g++ -c program.s -o program.o

.o 文件是二进制代码，但还不能单独执行，因为它可能依赖其他库或函数。

⸻

4. 链接（Linking）

链接器（Linker） 负责将 目标文件 与 库文件 进行合并，最终生成 可执行文件（.out 或 .exe）。

🔹 主要任务
	•	解决符号（Symbol Resolution）：把函数调用的地址替换为实际地址。
	•	处理库链接：
	•	静态库（.a 或 .lib）—— 编译时复制到最终可执行文件。
	•	动态库（.so 或 .dll）—— 运行时加载，不包含在可执行文件中。
	•	生成最终可执行文件。

🔹 示例

g++ program.o -o program.out

program.out 是最终可执行文件。

⸻

5. 加载和执行（Loading & Execution）

当我们运行 ./program.out 时，操作系统（OS） 负责加载并执行程序。

🔹 主要任务
	•	加载（Loading）：操作系统将可执行文件加载到内存中。
	•	内存分配（Memory Allocation）：
	•	代码段（Text Segment）：存放程序指令。
	•	数据段（Data Segment）：存放全局变量、常量。
	•	堆（Heap）：动态分配的内存（new/malloc）。
	•	栈（Stack）：局部变量、函数调用帧（stack frame）。
	•	CPU 执行（Execution）：
	•	入口点（Entry Point）：从 main() 函数开始执行。
	•	操作系统管理进程（Process）。

🔹 示例

./program.out



⸻

总结：C++ 程序执行全过程

阶段	工具	输出文件	作用
预处理	g++ -E	.i	处理 #include, #define 等
编译	g++ -S	.s	生成汇编代码
汇编	g++ -c	.o	生成目标文件（机器码）
链接	g++	.out 或 .exe	生成可执行文件
执行	./program.out	运行	加载、执行代码



⸻

附加说明

1. 为什么需要四个步骤？
	•	预处理 解决代码组织问题（宏、头文件）。
	•	编译 使代码适用于不同 CPU 体系结构。
	•	汇编 生成二进制代码，便于计算机理解。
	•	链接 解决跨文件、库的依赖问题。

2. -O2, -O3 编译优化

使用 -O2 或 -O3 进行优化：

g++ -O2 program.cpp -o program.out

	•	-O2：标准优化，平衡性能与编译时间。
	•	-O3：更激进的优化，可能带来更高的性能。

3. 静态链接 vs. 动态链接
	•	静态链接（Static Linking）
	•	库文件被拷贝到最终可执行文件。
	•	运行时不依赖外部库，但文件较大。
	•	示例：

g++ program.cpp -o program.out -static


	•	动态链接（Dynamic Linking）
	•	运行时加载库（.so 或 .dll），不拷贝到可执行文件。
	•	优点：减少可执行文件大小，可共享库。
	•	缺点：库可能更新导致不兼容。
	•	示例：

g++ program.cpp -o program.out -L/usr/lib -lmylib



⸻

结论

一个 C++ 程序的执行过程是：
	1.	预处理 处理 #include 和 #define。
	2.	编译 生成汇编代码。
	3.	汇编 生成机器码（目标文件）。
	4.	链接 生成最终可执行文件。
	5.	加载和执行 由操作系统加载进内存，CPU 执行。

这个过程涉及编译原理、操作系统、计算机体系结构等多个知识点，是 C++ 语言的重要基础。

