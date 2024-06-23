#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MAX_INSTR_LENGTH 25  // ָ�����󳤶�
#define MAX_INSTRUCTIONS 100 // ���ָ����
#define REG_AMOUNT 13        // �������Ĵ�������
#define STATIONS_N 9         // ����վ����
#define INSTRUCTION_AMOUNT 6 // ָ������

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
        return 2;
    if (op == 3 || op == 4 )
        return 3;
    if (op == 5)
        return 10;
    if (op == 6)
        return 40;
}


Instruction extractInstruction(char *instructionStr)
{
    Instruction rinstruct;
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
    int src1;
    int src2;
    int depend1;
    int depend2;
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

int compareFiles(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "rb");
    FILE *fp2 = fopen(file2, "rb");

    if (fp1 == NULL || fp2 == NULL) {
        perror("Error opening file");
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        return -1;
    }

    int ch1, ch2;
    int position = 0;

    do {
        ch1 = fgetc(fp1);
        ch2 = fgetc(fp2);
        position++;

        if (ch1 != ch2) {
            printf("Files differ at byte %d\n", position);
            fclose(fp1);
            fclose(fp2);
            return 1;
        }
    } while (ch1 != EOF && ch2 != EOF);

    if (feof(fp1) && feof(fp2)) {
        printf("Files are identical.\n");
        fclose(fp1);
        fclose(fp2);
        return 0;
    } else {
        if (!feof(fp1)) {
            printf("File 1 has extra content after byte %d.\n", position);
        } else if (!feof(fp2)) {
            printf("File 2 has extra content after byte %d.\n", position);
        }
        fclose(fp1);
        fclose(fp2);
        return 1;
    }
}


char test_1[][20] = {
        "LD F6,34(R2)",
        "LD F2,45(R3)",
        "MUL F0,F2,F4",
        "SUB F8,F2,F6",
        "DIV F10,F0,F6",
        "ADD F6,F8,F2"

    };

//RAW

char test_2[][20] = {
        "LD F6,34(R2)",
        "LD F2,25(R1)",
        "MUL F6,F2,F4",
        "SUB F8,F10,F6",
        "DIV F10,F0,F6",
        "ADD F6,F8,F2"
};

//WAW

char test_3[][20] = {
        "LD F6,34(R2)",
        "SD F2,25(R1)",
        "MUL F6,F2,F4",
        "SUB F4,F10,F6",
        "DIV F10,F0,F6",
        "ADD F6,F8,F2"
};

//WAR


//���Ҫ���ָ����������ָ������

void main (){

    char inputInstructions[6][20];
    char inputInstructionscopy[6][20];
    int test_case = 0;

    printf("Press��1��2��3��to select a test case|Press 0 to enter instructions\n");
    scanf("%d",&test_case);

    if(test_case == 1) memcpy(inputInstructions, test_1, sizeof(test_1));

    if(test_case == 2) memcpy(inputInstructions, test_2, sizeof(test_1));

    if(test_case == 3) memcpy(inputInstructions, test_3, sizeof(test_1));

    if(test_case == 0){
        printf("Enter the instruction:\n");
        for(int i = 0 ;i<INSTRUCTION_AMOUNT;i++){
            fgets(inputInstructions[i], sizeof(inputInstructions), stdin);

            // ȥ�����з�
            inputInstructions[i][strcspn(inputInstructions[i], "\n")] = '\0';
        }
        memcpy(inputInstructionscopy, inputInstructions, sizeof(inputInstructions));
    }


    printf("test case:\n");

    for(int i = 0 ; i<INSTRUCTION_AMOUNT ; i++){
        printf("%s\n",inputInstructions[i]);
    }

    printf("press a key to continue  ");
    system("pause");

    Instruction instructionsList[INSTRUCTION_AMOUNT];



    for (int i = 0; i < 6; i++){
        instructionsList[i] = extractInstruction(inputInstructions[i]);
    }

    Register registers[REG_AMOUNT];
    for (int i = 0; i < REG_AMOUNT; i++){
        registers[i].busyBy = -1;
        registers[i].value = -1;
    }
    //��ʼ���Ĵ�����

    Station reservationStations[STATIONS_N] = {
        {0, 1, 0, 0, -1, 1, 1, -1, -1},
        {0, 1, 0, 0, -1, 1, 1, -1, -1},
        {0, 2, 0, 0, -1, 1, 1, -1, -1},
        {0, 2, 0, 0, -1, 1, 1, -1, -1},
        {0, 3, 0, 0, -1, 1, 1, -1, -1},
        {0, 3, 0, 0, -1, 1, 1, -1, -1},
        {0, 3, 0, 0, -1, 1, 1, -1, -1},
        {0, 4, 0, 0, -1, 1, 1, -1, -1},
        {0, 4, 0, 0, -1, 1, 1, -1, -1}};

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

            }

        }
        //����ǰָ����ɵ�һ�����еı���վ,�����ø���ָ������

        for (int i = 0; i < STATIONS_N; i++){

            //printf("station:%s|busy:%d\n",stationName[i],reservationStations[i].busy);

            if (reservationStations[i].busy == 0){
                //printf("����վ��æ��busy Ϊ 0���������ñ���վ�����������һ��\n");
                continue;
            }
            //�������վ��æ��busy Ϊ 0���������ñ���վ�����������һ����

            if (reservationStations[i].instruction.issuedT == clock){
                //printf("������ڸ���������ִ��\n");
                continue;
            }//���������ڸ���������ִ��

            int targetReg = -1;
            int srcReg1 = -1;
            int srcReg2 = -1;
            //��ʼ���뵱ǰָ����صļĴ������

            int instType = getType(reservationStations[i].instruction.operation);

            if(instType == 0){
                targetReg = reservationStations[i].instruction.twoReg.targetReg;
            }
            if(instType == 1){
                targetReg = reservationStations[i].instruction.threeReg.targetReg;
                srcReg1 = reservationStations[i].instruction.threeReg.srcReg1;
                srcReg2 = reservationStations[i].instruction.threeReg.srcReg2;
            }
            //���ָ����ؼĴ������

            if (reservationStations[i].instruction.startedT == -1){

                if (isRegisterFree(registers[targetReg], i) == 0){
                    //printf("target��ռ��\n");
                    continue;
                }


                if(instType == 1){

                    if (reservationStations[i].depend1 != -1 && reservationStations[i].src1 == 0){
                        //printf("src1��ռ��\n");
                        continue;
                    }

                    if (reservationStations[i].depend2 != -1 && reservationStations[i].src2 == 0){
                        //printf("src2��ռ��\n");
                        continue;
                    }

                }

                reservationStations[i].instruction.startedT = clock;
                instructionsList[reservationStations[i].instIndex].startedT = clock;

                //printf("��ʼִ��ָ��\n");
                continue;
            }
            //������վ����׼����ִ�е�ָ��,���ָ���״̬����ؼĴ�����ʹ������������Ƿ���Կ�ʼִ��ָ��



            if ((reservationStations[i].instruction.startedT + getOperationTime(reservationStations[i].instruction.operation)-1) <= clock && reservationStations[i].instruction.writtenT == -1){
                //��ʼʱ�� + ִ��ʱ�� <= ��ǰʱ�����ڣ�˵��ָ���Ѿ����
                reservationStations[i].instruction.finishedT = clock;
                instructionsList[reservationStations[i].instIndex].finishedT = clock;
                reservationStations[i].instruction.writtenT = clock + 1;
                //printf("ָ���Ѿ����\n");
                continue;

            }



        }




        for (int i = 0; i< STATIONS_N; i++){

            if (reservationStations[i].instruction.writtenT == clock){
                int targetReg = -1;

                int instType = getType(reservationStations[i].instruction.operation);

                if(instType == 0){
                    targetReg = reservationStations[i].instruction.twoReg.targetReg;
                }
                if(instType == 1){
                    targetReg = reservationStations[i].instruction.threeReg.targetReg;
                }
                //printf("д�أ��ͷ�targetReg���ͷű���վ\n");

                if(registers[targetReg].busyBy == i){
                    registers[targetReg].busyBy = -1;
                    registers[targetReg].value = reservationStations[i].instIndex+1;
                }
                //�㲥
                for(int j =0; j<STATIONS_N; j++){
                    if(reservationStations[j].src1 == 0 && reservationStations[j].depend1 == i){
                        reservationStations[j].src1 = reservationStations[i].instIndex+1;
                        reservationStations[j].depend1 = -1;
                    }
                    if(reservationStations[j].src2 == 0 && reservationStations[j].depend2 == i){
                        reservationStations[j].src2 = reservationStations[i].instIndex+1;
                        reservationStations[j].depend2 = -1;
                    }

                }

                reservationStations[i].busy = 0;
                reservationStations[i].src1 = 0;
                reservationStations[i].src2 = 0;
                reservationStations[i].depend1 = -1;
                reservationStations[i].depend2 = -1;
                instructionsList[reservationStations[i].instIndex].writtenT = clock;


                continue;

            }
            //��targetReg����Ϊ����,�ͷű���վ


        }


        for(int i = 0;i <STATIONS_N; i++){
            if(reservationStations[i].instruction.issuedT == clock){

                if (reservationStations[i].busy == 0 ){
                    //printf("����վ��æ��busy Ϊ 0���������ñ���վ�����������һ��\n");
                    continue;
                }


                //��ʼ���뵱ǰָ����صļĴ������
                Instruction issued_inst = reservationStations[i].instruction;


                int instType = getType(issued_inst.operation);

                int targetReg = -1;
                int srcReg1 = -1;
                int srcReg2 = -1;

                int issued_inst_type = getType(issued_inst.operation);
                if(issued_inst_type == 0){
                    targetReg = issued_inst.twoReg.targetReg;
                }
                if(issued_inst_type == 1){
                    targetReg = issued_inst.threeReg.targetReg;
                }

                registers[targetReg].busyBy = i;

                if(issued_inst_type == 1){
                    srcReg1 = reservationStations[i].instruction.threeReg.srcReg1;
                    srcReg2 = reservationStations[i].instruction.threeReg.srcReg2;

                if (isRegisterFree(registers[srcReg1], i) == 0 && registers[srcReg1].value == -1){
                    //printf("src1��ռ��\n");
                    reservationStations[i].src1 = 0;
                    reservationStations[i].depend1 = registers[srcReg1].busyBy;
                }else{
                    reservationStations[i].depend1 = -1;
                    reservationStations[i].src1 = registers[srcReg1].value;
                }


                if (isRegisterFree(registers[srcReg2], i) == 0 && registers[srcReg2].value == -1){
                    //printf("src2��ռ��\n");
                    reservationStations[i].src2 = 0;
                    reservationStations[i].depend2 = registers[srcReg2].busyBy;
                }else{
                    reservationStations[i].depend2 = -1;
                    reservationStations[i].src2 = registers[srcReg2].value;
                }
            }

            }
        }

        if(test_case == 1) memcpy(inputInstructions, test_1, sizeof(test_1));

        if(test_case == 2) memcpy(inputInstructions, test_2, sizeof(test_1));

        if(test_case == 3) memcpy(inputInstructions, test_3, sizeof(test_1));

        if(test_case == 0) memcpy(inputInstructions, inputInstructionscopy,sizeof(inputInstructionscopy));

        printf("\n===============================================================================\n");






        printf("Cycle:%d\n",clock);

        printf("\n             Instruction Statue\n");
        printf("Instruction         Issue  Start  finish write  \n");
        for(int i =0; i<INSTRUCTION_AMOUNT; i++){
            printf("%-20s",inputInstructions[i]);
            printTime(instructionsList[i].issuedT);
            printTime(instructionsList[i].startedT);
            printTime(instructionsList[i].finishedT);
            printTime(instructionsList[i].writtenT);
            printf("\n");
        }


        printf("-----------------------------------------------------------------------");

        printf("\n                  Reservations Station\n");
        printf("Name    Busy    Op        Vj        Vk        Qj        Qk        \n");
        for(int i = 0; i < STATIONS_N; i++){
            printf("%-8s",stationName[i]);
        if(reservationStations[i].busy == 0){
            printf("no      \n");
        }else{
            printf("yes     ");
            printf("%-10s",instName[reservationStations[i].instruction.operation-1]);
            //Instruction in = instList[reservationStations[i].instIndex];

            int intype = getType(reservationStations[i].instruction.operation);
            int targetReg = -1;
            int srcReg1 = -1;
            int srcReg2 = -1;

            if(intype == 0){

                srcReg1 = reservationStations[i].instruction.twoReg.srcReg1;
                printf("R[R");
                printf("%-2d",srcReg1);
                printf("]    ");
                printf("                              ");
            }

            if(intype == 1){
                srcReg1 = reservationStations[i].instruction.threeReg.srcReg1;
                srcReg2 = reservationStations[i].instruction.threeReg.srcReg2;

                if (reservationStations[i].src1 != 0  && reservationStations[i].src2 != 0){

                    if(reservationStations[i].src1 == -1){
                        printf("R[F");
                        printf("%-2d",srcReg1);
                        printf("]    ");
                    }else{
                        printf("D");
                        printf("%-9d",reservationStations[i].src1);
                    }
                    if(reservationStations[i].src2 == -1){
                        printf("R[F");
                        printf("%-2d",srcReg2);
                        printf("]    ");
                    }else{
                        printf("D");
                        printf("%-9d",reservationStations[i].src2);
                    }

                    printf("                    ");
                }
                if (reservationStations[i].src1 != 0  && reservationStations[i].src2 == 0){

                    if(reservationStations[i].src1 == -1){
                        printf("R[F");
                        printf("%-2d",srcReg1);
                        printf("]    ");
                    }else{
                        printf("D");
                        printf("%-9d",reservationStations[i].src1);
                    }

                    printf("                    ");
                    printf("%-10s",stationName[reservationStations[i].depend2]);
                }
                if (reservationStations[i].src1 == 0  && reservationStations[i].src2 != 0){
                    printf("          ");

                    if(reservationStations[i].src2 == -1){
                        printf("R[F");
                        printf("%-2d",srcReg2);
                        printf("]    ");
                    }else{
                        printf("D");
                        printf("%-9d",reservationStations[i].src2);
                    }

                    printf("%-10s",stationName[reservationStations[i].depend1]);
                    printf("          ");
                }
                if (reservationStations[i].src1 == 0  && reservationStations[i].src2 == 0){
                    printf("                    ");
                    printf("%-10s",stationName[reservationStations[i].depend1]);
                    printf("%-10s",stationName[reservationStations[i].depend2]);
                }

            }
            printf("\n");
        }



        }

        printf("-----------------------------------------------------------------------");


        printf("\n                     Register\n");
        for (int i = 0; i < REG_AMOUNT; i+= 2){
            printf("F");
            printf("%-2d",i);
            printf("       ");
        }
        printf("\n");

        for (int i = 0; i < REG_AMOUNT; i += 2){
            if(registers[i].value == -1){
                printf("          ");
            }else{
                printf("D");
                printf("%-9d",registers[i].value);
            }
        }

        printf("\n");

        for (int i = 0; i < REG_AMOUNT; i += 2){
            if(registers[i].busyBy == -1){
                printf("          ");
            }else{
                printf("%-10s",stationName[registers[i].busyBy]);
            }
        }
        printf("\n");

        //�������

        clock += 1;


        if (clock >= 10)
        {
            int allDone = 1;
            for (int i = 0; i < STATIONS_N; i++)
            {
                if (reservationStations[i].busy == 1)
                    allDone = 0;
            }
            if (allDone == 1){

                const char *file1 = "out.txt";
                const char *file2 = "ans.txt";

                int result = compareFiles(file1, file2);
                if (result == 0) {
                    printf("The output is correct.\n");
                } else if (result == 1) {
                    printf("The output is incorrect.\n");
                }

                return 0;
            }

        }
        if (clock >= 1000)
            return 0;

    }


}
