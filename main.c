#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MAX_INSTR_LENGTH 25  // Maximum length of each instruction
#define MAX_INSTRUCTIONS 100 // Maximum number of instructions
#define REG_AMOUNT 32        // Number of FP registers
#define ADDR_REG_AMOUNT 16   // Number of address registers
#define STATIONS_N 9         // Number of stations
#define INSTRUCTION_AMOUNT 6

typedef struct
{
    int targetReg;
    int srcReg1;
    int offset;
} InstTwoReg;
//˫�Ĵ���ָ�LD��SD

typedef struct
{
    int targetReg;
    int srcReg1;
    int srcReg2;
} InstThreeReg;
//���Ĵ���ָ�ADD��SUB��MUL��DIV

typedef struct
{
    int operation;
    int type;
    InstThreeReg threeReg;
    InstTwoReg twoReg;
    //C����û�нṹ�壬����type��ֵ����ʹ��Instruction_ThreeReg����Instruction_TwoReg
    int issuedT;
    int startedT;
    int finishedT;
    int writtenT;
    //��������ʼ��������д���ʱ���
} Instruction;
//ָ��ṹ��

int getOp(char *op)
{
    if (strcmp(op, "LD") == 0)
        return 1;
    else if (strcmp(op, "SD") == 0)
        return 2;
    else if (strcmp(op, "ADD") == 0)
        return 3;
    else if (strcmp(op, "SUB") == 0)
        return 4;
    else if (strcmp(op, "MUL") == 0)
        return 5;
    else if (strcmp(op, "DIV") == 0)
        return 6;

    return -1; //��ȡʧ��
}
//1-6�ֱ����LD,SD,ADD,SUB,MUL,DIV

int getType(int op)
{
    if (op == 1 || op == 2)
        return 0;
    if (op == 3 || op == 4 || op == 5 || op == 6)
        return 1;
}
//0����˫�Ĵ���ָ�1�������Ĵ���ָ��

int getOperationTime(int op){
    if (op == 1 || op == 2)
        return 1;
    if (op == 3 || op == 4 )
        return 2;
    if (op == 5)
        return 9;
    if (op == 6)
        return 39;
}


Instruction extractInstruction(char *instructionStr)
{
    Instruction rinstruct;
    //remove_newline(instructionStr);
    char *regStr;
    char *opStr = strtok_r(instructionStr, " ", &regStr);

    //��ȡ������Ͳ���������

    int op = getOp(opStr);

    rinstruct.operation = op;
    rinstruct.type = getType(op);

    if(rinstruct.type == 0){
        sscanf(regStr, "F%d,%d(R%d)", &rinstruct.twoReg.targetReg, &rinstruct.twoReg.offset, &rinstruct.twoReg.srcReg1);
    }
    //�����˫�Ĵ���ָ�twoReg���������

    if(rinstruct.type == 1){
        sscanf(regStr, "F%d,F%d,F%d", &rinstruct.threeReg.targetReg, &rinstruct.threeReg.srcReg1, &rinstruct.threeReg.srcReg2);
    }
    //��������Ĵ���ָ�threeReg���������

    rinstruct.issuedT = -1;
    rinstruct.startedT = -1;
    rinstruct.finishedT = -1;
    rinstruct.writtenT = -1;
    //ʱ���ʼ��

    return rinstruct;
}

typedef struct{
    int busy;//unbusy:0;busy:1
    int type;//1:Load;2:Store;3:Adder;4:Multiplier
    Instruction instruction;
    int debugInstructionLine;
    int instIndex;
} Station;

int findNoBusyStation(Station station[STATIONS_N], int type)
{
    for (int i = 0; i < STATIONS_N; i++)
    {
        if (type == station[i].type)
        {
            if (station[i].busy == 0)
                return i;
        }
    }
    return -1;
}

int getStationType(Instruction inst)
{
    if(inst.operation == 1){
        return 1;
    }
    if(inst.operation == 2){
        return 2;
    }
    if(inst.operation == 3 || inst.operation == 4){
        return 3;
    }
    if(inst.operation == 5 || inst.operation == 6){
        return 4;
    }
}

int dispatchInstruction(Station reservationStation[STATIONS_N], Instruction instruction, int clock, int instructionPosInLine)
{
    // ��ȡָ���Ӧ�ı���վ����
    int stationType = getStationType(instruction);

    // ����һ�����еı���վ
    int stationIndex = findNoBusyStation(reservationStation, stationType);

    // ���û���ҵ����еı���վ������ -1
    if (stationIndex == -1)
        return -1;

    // ���±���վ��״̬��ָ����Ϣ
    reservationStation[stationIndex].busy = 1;
    reservationStation[stationIndex].instruction = instruction;
    reservationStation[stationIndex].instruction.issuedT = clock;
    reservationStation[stationIndex].instruction.startedT = -1;
    reservationStation[stationIndex].instruction.finishedT = -1;
    reservationStation[stationIndex].instruction.writtenT = -1;
    reservationStation[stationIndex].instIndex = instructionPosInLine;
    // ���ر���վ������
    return stationIndex;
}

typedef struct
{
    int busyBy; // ����ָʾ��ǰռ�øüĴ����ı���վ������
    int value;  // �Ĵ����д洢����ֵ
} Register;

int isRegisterFree(Register reg, int stationIndex)
{
    if (reg.busyBy == -1 || reg.busyBy == stationIndex){
        return 1;
    }
    return 0;
}
//����1��ʾ�Ĵ����ǿ��еģ����߱�ָ���ı���վռ��,����0��ʾ�Ĵ�������������վռ��


void loginfo(int clock, char *inst, Instruction instList[] ,Station reservationStation[] ,Register reg[]){
    FILE *file = fopen("./out.txt", "w");

    char instName[6][4] = {
        "LD",
        "SD",
        "ADD",
        "SUB",
        "MUL",
        "DIV"
    };

    char stationName[9][8] = {
        "Load1",
        "Load2",
        "Store1",
        "Store2",
        "Add1",
        "Add2",
        "Add3",
        "Mult1",
        "Mult2"
    };



    fprintf("Cycle:%d\n",clock);

    fprintf(file,"\n*Instruction Statue*\n");

    fprintf(file,"Instruction         ");
    fprintf(file,"Issue  ");
    fprintf(file,"Start  ");
    fprintf(file,"finish ");
    fprintf(file,"write  \n");
    for (int i = 0; i < INSTRUCTION_AMOUNT; i++){
        fprintf(file,"%-20s",inst[i]);
        if(clock>=instList[i].issuedT  && instList[i].issuedT != -1){
            fprintf(file,"%-7d",instList[i].issuedT);
        }else{
            fprintf(file,"       ");
        }
        if(clock>=instList[i].startedT  && instList[i].startedT != -1){
            fprintf(file,"%-7d",instList[i].startedT);
        }else{
            fprintf(file,"       ");
        }
        if(clock>=instList[i].finishedT  && instList[i].finishedT != -1){
            fprintf(file,"%-7d",instList[i].finishedT);
        }else{
            fprintf(file,"       ");
        }

        if(clock>=instList[i].writtenT  && instList[i].writtenT != -1){
            fprintf(file,"%-7d\n",instList[i].writtenT);
        }else{
            fprintf(file,"       ");
        }

    }


    fprintf(file,"\n*Reservations Station*\n");
    fprintf(file,"Name    Busy    Op      Vj      Vk      Qj      Qk      \n");
    for(int i = 0; i < STATIONS_N; i++){
        fprintf(file,"%-8s",stationName[i]);
        if(reservationStation[i].busy == 0){
            fprintf(file,"no      ");
        }else{
            fprintf(file,"yes     ");
        }

        fprintf(file,"%-8s",instName[reservationStation[i].instruction.operation-1]);

        Instruction in = instList[reservationStation[i].instIndex];

        int intype = getType(in.operation);

        int targetReg = -1;
        int srcReg1 = -1;
        int srcReg2 = -1;




        if(intype == 0){

            targetReg = reservationStation[i].instruction.twoReg.targetReg;
            srcReg1 = reservationStation[i].instruction.twoReg.srcReg1;

            if (isRegisterFree(reg[srcReg1], i) == 1){
                fprintf(file,"R[F ");
                fprintf(file,"%-2d",srcReg1);
                fprintf(file,"] ");
                fprintf(file,"                        ");
            }
            if (isRegisterFree(reg[srcReg1], i) == 0){
                fprintf(file,"                ");
                fprintf(file,"%-8s",stationName[reg[srcReg1].busyBy]);
                printf(file,"        ");
            }
        }

        if(intype == 1){
            targetReg = reservationStation[i].instruction.threeReg.targetReg;
            srcReg1 = reservationStation[i].instruction.threeReg.srcReg1;
            srcReg2 = reservationStation[i].instruction.threeReg.srcReg2;

            if (isRegisterFree(reg[srcReg1], i) == 1  && isRegisterFree(reg[srcReg2], i) == 1){
                fprintf(file,"R[F ");
                fprintf(file,"%-2d",srcReg1);
                fprintf(file,"] ");
                fprintf(file,"R[F ");
                fprintf(file,"%-2d",srcReg2);
                fprintf(file,"] ");
                fprintf(file,"                ");
            }
            if (isRegisterFree(reg[srcReg1], i) == 1  && isRegisterFree(reg[srcReg2], i) == 0){
                fprintf(file,"R[F ");
                fprintf(file,"%-2d",srcReg1);
                fprintf(file,"] ");
                fprintf(file,"                ");
                fprintf(file,"%-8s",stationName[reg[srcReg2].busyBy]);
            }
            if (isRegisterFree(reg[srcReg1], i) == 0  && isRegisterFree(reg[srcReg2], i) == 1){
                fprintf(file,"        ");
                fprintf(file,"R[F ");
                fprintf(file,"%-2d",srcReg2);
                fprintf(file,"] ");
                fprintf(file,"%-8s",stationName[reg[srcReg1].busyBy]);
                fprintf(file,"        ");
            }
            if (isRegisterFree(reg[srcReg1], i) == 0  && isRegisterFree(reg[srcReg2], i) == 0){
                fprintf(file,"                ");
                fprintf(file,"%-8s",stationName[reg[srcReg1].busyBy]);
                fprintf(file,"%-8s",stationName[reg[srcReg2].busyBy]);
            }
            fprintf(file,"\n");

        }


    }

    fprintf(file,"\n*Register*\n");
    for (int i = 0; i < REG_AMOUNT; i++){
        fprintf(file,"F");
        fprintf(file,"%-2d",i);
        fprintf(file,"   ");
    }
    fprintf(file,"\n");
    for (int i = 0; i < REG_AMOUNT; i++){
        if(reg[i].busyBy == -1){
            fprintf(file,"      ");
        }else{
            fprintf(file,"%-6s",stationName[reg[i].busyBy]);
        }
    }
    fprintf(file,"\n");
    for (int i = 0; i < REG_AMOUNT; i++){
        fprintf(file,"%-6d",reg[i].value);
    }
    fprintf(file,"\n");


    fclose(file);

}

const char stationName[9][8] = {
        "Load1",
        "Load2",
        "Store1",
        "Store2",
        "Add1",
        "Add2",
        "Add3",
        "Mult1",
        "Mult2"
    };


const char instName[6][4] = {
        "LD",
        "SD",
        "ADD",
        "SUB",
        "MUL",
        "DIV"
    };


void printTime(int inTime){
    if(inTime!=-1){
        printf("%-7d",inTime);
    }else{
        printf("       ");
    }
}


void main (){

    printf("begin");

    char inputInstructions[][20] = {
        "LD F6,34(R2)",
        "LD F2,45(R3)",
        "MUL F0,F2,F4",
        "SUB F8,F6,F2",
        "DIV F10,F0,F6",
        "ADD F6,F8,F2"

    };

    //printf("%s",inputInstructions[5]);


    Instruction instructionsList[INSTRUCTION_AMOUNT];



    for (short i = 0; i < 6; i++){
        instructionsList[i] = extractInstruction(inputInstructions[i]);
    }

    Register registers[REG_AMOUNT];
    for (int i = 0; i < REG_AMOUNT; i++){
        registers[i].busyBy = -1;
        registers[i].value = i;
    }
    //��ʼ���Ĵ�����

    Station reservationStations[STATIONS_N] = {
        {0, 1, 0, 0, -1},
        {0, 1, 0, 0, -1},
        {0, 2, 0, 0, -1},
        {0, 2, 0, 0, -1},
        {0, 3, 0, 0, -1},
        {0, 3, 0, 0, -1},
        {0, 3, 0, 0, -1},
        {0, 4, 0, 0, -1},
        {0, 4, 0, 0, -1}};

    //��ʼ������վ

    int currentPos = 0;
    int clock = 1;


    while(1){

        Instruction currentInstruction = instructionsList[currentPos];


        if (currentPos < INSTRUCTION_AMOUNT){

            int stationIndex = dispatchInstruction(reservationStations, currentInstruction, clock, currentPos);//�����ø���ָ������

            if (stationIndex != -1){
                instructionsList[currentPos].issuedT = clock;
                currentPos += 1;
                //logInstructionStep(0, currentPos);//���� logInstructionStep ������¼ָ��衣
            }
            else{
                //logDependecy(currentPos + 1);//���� logDependecy ������¼ָ��������
            }
        }

        printf("currentPos:%d\n",currentPos);
        //����ǰָ����ɵ�һ�����еı���վ,�����ø���ָ������

        for (int i = 0; i < STATIONS_N; i++){
                printf("station:%s|busy:%d\n",stationName[i],reservationStations[i].busy);
            if (reservationStations[i].busy == 0){
                printf("����վ��æ��busy Ϊ 0���������ñ���վ�����������һ��\n");
                continue;
            }
            //�������վ��æ��busy Ϊ 0���������ñ���վ�����������һ����

            if (reservationStations[i].instruction.issuedT == clock){
                printf("������ڸ���������ִ��\n");
                continue;
            }
            //���������ڸ���������ִ��

            int targetReg = -1;
            int srcReg1 = -1;
            int srcReg2 = -1;
            //��ʼ���뵱ǰָ����صļĴ������

            int instType = getType(reservationStations[i].instruction.operation);

            if(instType == 0){
                targetReg = reservationStations[i].instruction.twoReg.targetReg;
                srcReg1 = reservationStations[i].instruction.twoReg.srcReg1;
                srcReg2 = srcReg1;
            }
            if(instType == 1){
                targetReg = reservationStations[i].instruction.threeReg.targetReg;
                srcReg1 = reservationStations[i].instruction.threeReg.srcReg1;
                srcReg2 = reservationStations[i].instruction.threeReg.srcReg2;
            }
            //���ָ����ؼĴ������

            if (reservationStations[i].instruction.startedT == -1){

                if (isRegisterFree(registers[targetReg], i) == 0){
                    printf("target��ռ��\n");
                    continue;
                }
                if (isRegisterFree(registers[srcReg1], i) == 0){
                    printf("src1��ռ��\n");
                    continue;
                }

                if (instType == 1 && isRegisterFree(registers[srcReg2], i) == 0){
                    printf("src2��ռ��\n");
                    continue;
                }

                registers[targetReg].busyBy = i;
                //logInstructionStep(1, reservationStations[i].debugInstructionLine);
                reservationStations[i].instruction.startedT = clock;
                instructionsList[reservationStations[i].instIndex].startedT = clock;
                printf("��ʼִ��ָ��\n");
                continue;
            }
            //������վ����׼����ִ�е�ָ��,���ָ���״̬����ؼĴ�����ʹ������������Ƿ���Կ�ʼִ��ָ��

            if (reservationStations[i].instruction.writtenT == clock){
                //logInstructionStep(3, reservationStations[i].debugInstructionLine);
                printf("д�أ��ͷ�targetReg���ͷű���վ\n");
                registers[targetReg].busyBy = -1;
                reservationStations[i].busy = 0;
                instructionsList[reservationStations[i].instIndex].writtenT = clock;
                continue;
            }
            //��targetReg����Ϊ����,�ͷű���վ

            if ((reservationStations[i].instruction.startedT + getOperationTime(reservationStations[i].instruction.operation)) <= clock){
                //��ʼʱ�� + ִ��ʱ�� <= ��ǰʱ�����ڣ�˵��ָ���Ѿ����
                //logInstructionStep(2, reservationStations[i].debugInstructionLine);
                /*
                int op  = reservationStations[i].instruction.operation;
                int result = 0;

                if(op == 1 || op == 2){
                    registers[targetReg].value = registers[srcReg1].value;
                    break;
                }

                if(op == 3){
                    result = registers[srcReg1].value + registers[srcReg2].value;
                    registers[targetReg].value = result;
                    break;
                }

                if(op == 4){
                    result = registers[srcReg1].value - registers[srcReg2].value;
                    registers[targetReg].value = result;
                    break;
                }

                if(op == 5){
                    result = registers[srcReg1].value * registers[srcReg2].value;
                    registers[targetReg].value = result;
                    break;
                }

                if(op == 6){
                    if(registers[srcReg2].value == 0){
                        result = registers[srcReg1].value / 1;
                        registers[targetReg].value = result;
                    }else{
                        result = registers[srcReg1].value / registers[srcReg2].value;
                        registers[targetReg].value = result;
                    }
                    break;
                }
                */
                reservationStations[i].instruction.finishedT = clock;
                instructionsList[reservationStations[i].instIndex].finishedT = clock;
                reservationStations[i].instruction.writtenT = clock + 1;
                printf("ָ���Ѿ����\n");
                continue;

            }


        }

        //logRegisters(registers, reservationStations);
        //logStations(reservationStations);
        //loginfo(clock,inputInstructions,instructionsList,reservationStations,registers);
        printf("cycle:%d finished\n===============================================================================\n",clock);


        char inputInstructions[][20] = {
            "LD F6,34(R2)",
            "LD F2,45(R3)",
            "MUL F0,F2,F4",
            "SUB F8,F6,F2",
            "DIV F10,F0,F6",
            "ADD F6,F8,F2"

        };


        printf("Cycle:%d\n",clock);

        printf("\n            Instruction Statue\n");
        printf("Instruction         Issue  Start  finish write  \n");
        for(int i =0; i<INSTRUCTION_AMOUNT; i++){
            printf("%-20s",inputInstructions[i]);
            printTime(instructionsList[i].issuedT);
            printTime(instructionsList[i].startedT);
            printTime(instructionsList[i].finishedT);
            printTime(instructionsList[i].writtenT);
            printf("\n");
        }








        clock += 1;


        if (clock >= 10)
        {
            int allDone = 1;
            for (int i = 0; i < STATIONS_N; i++)
            {
                if (reservationStations[i].busy == 1)
                    allDone = 0;
            }
            if (allDone == 1)
                return 0;
        }


        if (clock >= 1000)
            return 0;

    }


}
