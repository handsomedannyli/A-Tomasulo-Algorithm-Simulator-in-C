# A-Tomasulo-Algorithm-Simulator-in-C
2024 Spring NENU IST CS Computer Architecture Personal Topic

# How To Use
1. Build main.c from source or get the executable.
2. Run the Program
3. enter 1,2,3 to select the testcase or enter 0 to input the instructions
4. press any key to see the result

# About the work
The number of instructions, instruction length, number of reservation stations, and number of registers supported by Tomasulo simulator can all be modified. If you need to make changes, you can do so in the #define section of the code.

If the number of instructions you input is not 6, please go to the #define section of the source code to modify the number of instructions.

The Tomasulo algorithm simulator support recognizing the following six types of instructions:
1. SD Fx,y(Rz)
2. LD Fx,y(Rz)
3. ADD Fx,Fy,Fz
4. SUB Fx,Fy,Fz
5. DIV Fx,Fy,Fz
6. MUL Fx,Fy,Fz

The program has three built-in test cases. The test cases are as follows:

case1:
"LD F6,34(R2)"
"LD F2,45(R3)"
"MUL F0,F2,F4"
"SUB F8,F2,F6"
"DIV F10,F0,F6"
"ADD F6,F8,F2"

case2:
"LD F6,34(R2)"
"LD F2,25(R1)"
"MUL F6,F2,F4"
"SUB F8,F10,F6"
"DIV F10,F0,F6"
"ADD F6,F8,F2"

case3:
"LD F6,34(R2)"
"SD F2,25(R1)"
"MUL F6,F2,F4"
"SUB F4,F10,F6"
"DIV F10,F0,F6"
"ADD F6,F8,F2"

#C语言实现的Tomasulo算法仿真器
2024年春季 东北师范大学 信息科学与技术学院 计算机体系结构 个人专题

#使用说明
1. 编译 main.c 源文件或获取可执行文件。
2.运行程序。
3. 输入 1、2 或 3 选择测试用例，或输入 0 以手动输入指令。
4. 按任意键查看结果。
#关于本项目
Tomasulo 仿真器支持的指令数、指令长度、保留站数量以及寄存器数量均可修改。如果需要更改，可以在代码的 #define 部分进行修改。

如果你输入的指令数量不是6，请到源代码内的 #define 部分修改指令数量。

Tomasulo 算法仿真器支持识别以下六种类型的指令：

SD Fx,y(Rz)：将寄存器 Fx 的数据存储到地址 Rz + y 的内存位置。
LD Fx,y(Rz)：将地址 Rz + y 的内存数据加载到寄存器 Fx。
ADD Fx,Fy,Fz：计算寄存器 Fy 和 Fz 的和，并将结果存入寄存器 Fx。
SUB Fx,Fy,Fz：计算寄存器 Fy 和 Fz 的差，并将结果存入寄存器 Fx。
DIV Fx,Fy,Fz：将寄存器 Fy 的值除以寄存器 Fz 的值，并将结果存入寄存器 Fx。
MUL Fx,Fy,Fz：计算寄存器 Fy 和 Fz 的乘积，并将结果存入寄存器 Fx。
程序内置了三个测试案例，测试案例如下：

案例1："LD F6,34(R2)" "LD F2,45(R3)" "MUL F0,F2,F4" "SUB F8,F2,F6" "DIV F10,F0,F6" "ADD F6,F8,F2"
案例2："LD F6,34(R2)" "LD F2,25(R1)" "MUL F6,F2,F4" "SUB F8,F10,F6" "DIV F10,F0,F6" "ADD F6,F8,F2"
案例3："LD F6,34(R2)" "SD F2,25(R1)" "MUL F6,F2,F4" "SUB F4,F10,F6" "DIV F10,F0,F6" "ADD F6,F8,F2"
