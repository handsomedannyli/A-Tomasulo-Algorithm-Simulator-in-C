#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MAX_INSTR_LENGTH 25  // Maximum length of each instruction
#define MAX_INSTRUCTIONS 100 // Maximum number of instructions
#define REG_AMOUNT 32        // Number of FP registers
#define ADDR_REG_AMOUNT 16   // Number of address registers
#define STATIONS_N 7         // Number of stations


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

typedef struct
{
    int busy;//unbusy:0;busy:1
    int type;//1:Load;2:Store;3:Adder;4:Multiplier
    Instruction instruction;
    int debugInstructionLine;
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
    reservationStation[stationIndex].debugInstructionLine = instructionPosInLine + 1;

    // ���ر���վ������
    return stationIndex;
}

typedef struct
{
    int busyBy; // ����ָʾ��ǰռ�øüĴ����ı���վ������
    int value;  // �Ĵ����д洢����ֵ
} Register;

int isRegisterFree(Register reg, short stationIndex)
{
    if (reg.busyBy == -1 || reg.busyBy == stationIndex){
        return 1;
    }
    return 0;
}
//����1��ʾ�Ĵ����ǿ��еģ����߱�ָ���ı���վռ��,����0��ʾ�Ĵ�������������վռ��



void main (){
    char* inst = (char*)malloc(sizeof(char)*30);

    Register registers[REG_AMOUNT];
    for (short i = 0; i < REG_AMOUNT; i++){
        registers[i].busyBy = -1;
        registers[i].value = i;
    }
    //��ʼ���Ĵ�����

    Station reservationStations[STATIONS_N] = {
        {0, 3, 0, 0},
        {0, 3, 0, 0},
        {0, 4, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 2, 0, 0},
        {0, 2, 0, 0}};
    //��ʼ������վ

    int currentPos = 0;

    while(1){

        Instruction currentInst = instructionsList[currentPos];


        if (currentPos < instructionAmount){

            int stationIndex = dispatchInstruction(reservationStations, currentInstruction, clock, currentPos);//�����ø���ָ������

            if (stationIndex != -1){
                currentInstructionPos += 1;
                logInstructionStep(0, currentInstructionPos);//���� logInstructionStep ������¼ָ��衣
            }
            else{
                logDependecy(currentInstructionPos + 1);//���� logDependecy ������¼ָ��������
            }
        }
        //����ǰָ����ɵ�һ�����еı���վ,�����ø���ָ������

        for (int i = 0; i < STATIONS_N; i++){
            if (reservationStations[i].busy == 0){
                continue;
            }
            //�������վ��æ��busy Ϊ 0���������ñ���վ�����������һ����

            if (reservationStations[i].instruction.issuedT == clock){
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
                targetReg = reservationStations.instruction.threeReg.targetReg;
                srcReg1 = reservationStations[i].instruction.threeReg.srcReg1;
                srcReg2 = reservationStations[i].instruction.threeReg.srcReg2;
            }
            //���ָ����ؼĴ������

            if (reservationStations[i].instruction.startedT == -1){

                if (isRegisterFree(registers[targetReg], i) == 0){
                    logDependencyRegister(associatedRegisters[0], reservationStations[i].debugInstructionLine);
                    continue;
                }
                if (isRegisterFree(registers[srcReg1], i) == 0){
                    logDependencyRegister(associatedRegisters[1], reservationStations[i].debugInstructionLine);
                    continue;
                }

                if (instType == 1 && isRegisterFree(registers[srcReg2], i) == 0){
                    logDependencyRegister(associatedRegisters[2], reservationStations[i].debugInstructionLine);
                    continue;
                }

                registers[targetReg].busyBy = i;
                logInstructionStep(1, reservationStations[i].debugInstructionLine);
                reservationStations[i].instruction.startedT = clock;
                continue;
            }
            //������վ����׼����ִ�е�ָ��,���ָ���״̬����ؼĴ�����ʹ������������Ƿ���Կ�ʼִ��ָ��

            if (reservationStations[i].instruction.writtenT == clock){



                logInstructionStep(3, reservationStations[i].debugInstructionLine);

                registers[targetReg].busyBy = -1;
                reservationStations[i].busy = 0;
                continue;
            }
            //��targetReg����Ϊ����,�ͷű���վ

            if ((reservationStations[i].instruction.startedAt + getOperationTime(reservationStations[i].instruction.operation)) <= clock){
                //��ʼʱ�� + ִ��ʱ�� <= ��ǰʱ�����ڣ�˵��ָ���Ѿ����
                getAssociatedRegisters(associatedRegisters, reservationStations[i].instruction);
                logInstructionStep(2, reservationStations[i].debugInstructionLine);
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

                reservationStations[i].instruction.finishedT = clock;
                reservationStations[i].instruction.writtenT = clock + 1;
                continue;

            }

        }

        logRegisters(registers, reservationStations);
        logStations(reservationStations);
        clock += 1;

        if (clock >= 10)
        {
            int allDone = 1;
            for (int i = 0; i < STATIONS_AMOUNT; i++)
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
