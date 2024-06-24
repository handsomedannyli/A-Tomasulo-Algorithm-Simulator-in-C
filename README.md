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
