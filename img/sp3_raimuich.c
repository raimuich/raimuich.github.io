/*
sp3_raimuich.c
Semester project part3
Raimu Ichikawa
5/5/2023
*/

#include <stdio.h>

#define MAX_SIZE 1048576

//global int for on/off printing
// on(1), off(0)
int printFileSize = 0;
int printBuffer = 0;
int printBlockCount = 0;
int printMessage = 0;
int printBitCount = 0;
int printHbefore = 1;
int printHafter = 1;
int printDigest = 1;

//global int for initializing {Hi}
unsigned int H[] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};

//function prototype
unsigned int readFile(unsigned char buffer[]);
unsigned int calculateBlocks(unsigned int sizeOfFileInBytes);
void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes);
void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount);
void computeMessageDigest(unsigned int message[], unsigned int blockCount);
unsigned int f(unsigned int t, unsigned int B, unsigned int C, unsigned int D);
unsigned int K(unsigned int t);
unsigned int circleLeft(unsigned int number, unsigned int rotation);


int main(){

    //initialize variables
    int i;
    unsigned char buffer[MAX_SIZE];
    
    

    /*   Reading file with readFile function   */

    //calculate file size
    unsigned int fileSize = readFile(buffer);

    //printing for checking fileSize
    if(printFileSize == 1){
        printf("file size is: %d\n\n\n", fileSize);
    }//end printFileSize

    //printing for checking buffer
    if(printBuffer == 1){
        printf("Check the buffer array:\n\n");
        for(i=0; i<=fileSize; i++){
            printf("buffer[%d] = 0x%x\n", i, buffer[i]);
        }
        printf("\n\n");
    }//end printBuffer



    /*   Calculating the number of blocks   */

    //calculate block count
    unsigned int blockCount = calculateBlocks(fileSize);

    //printing for checking blockCount
    if(printBlockCount == 1){
        printf("blockCount is = %d\n\n\n", blockCount);
    }//end printBlockCount



    /*   Converting char array to int array   */

    /* 
    "MAX_SIZE / 4" does not work.
    It can be compiled but does not print anything,
    so I declared 250020, which is larger than Mi and smaller than (MAX_SIZE/4)
    */
    //int mMax = MAX_SIZE / 4;
    //unsigned int message[mMax];
    
    unsigned int message[250020];

    //converting buffer to message array
    convertCharArrayToIntArray(buffer, message, fileSize);

    //printing for checking convertCharArrayToIntArray
    if(printMessage == 1){
        printf("Check the message array:\n\n");
        for(i=0; i<(16 * blockCount); i++){
            printf("message[%d] = 0x%x\n", i, message[i]);
        }
        printf("\n");
    }//end printMessage



    /*   Adding bit count to last block   */

    //adding bit count to the last block
    addBitCountToLastBlock(message, fileSize, blockCount);

    //printing for checking addBitCountToLastBlock
    if(printBitCount == 1){
        printf("Check the message array added bit count:\n\n");
        for(i=0; i<(16 * blockCount); i++){
            printf("message[%d] = 0x%x\n", i, message[i]);
        }
        printf("\n");
    }//end printBitCount



    /*   Computing message digest   */

    //printing H value before processing
    if(printHbefore == 1){
        printf("H values before processing:\n\n");
        for(i=0; i<5; i++){
            printf("H[%d] = 0x%x\n", i, H[i]);
        }
        printf("\n");
    }//end printHbefore

    //computing message digest
    computeMessageDigest(message, blockCount);

    //printing H value after processing
    if(printHafter == 1){
        printf("H values after processing:\n\n");
        for(i=0; i<5; i++){
            printf("H[%d] = 0x%x\n", i, H[i]);
        }
        printf("\n");
    }//end printHafter



    /*   Final Output   */

    //printing message digest
    if(printDigest == 1){
        puts("Message digest:\n");
        for(i=0; i<5; i++){
        printf("%x ", H[i]);
    }
    printf("\n\n");
    }//end printDigest

    return 0;

}//end main


unsigned int readFile(unsigned char buffer[]){

    //initialize variables
    unsigned int fileSize = 0;
    char letter;

    //from Session 7 "counting.c"
    letter = getchar();
    while(letter != EOF){

        //letting buffer store a character
        buffer[fileSize] = letter;
        
        //counting file size
        fileSize++;

        //error checking for too big file
        if(fileSize > MAX_SIZE){
            puts("File size is too big.");
            return 0;
        }

        //getting next char
        letter = getchar();

    }//end while loop

    //append 1 bit at the end of the message
    buffer[fileSize] = 0x80;

    //return file size in Bytes
    return fileSize;

}//end readFile function


unsigned int calculateBlocks(unsigned int sizeOfFileInBytes){

    //from the instruction

    //calculate blockCount with padded 1 bit
    unsigned int blockCount = (((8 * sizeOfFileInBytes) + 1) / 512) + 1;

    //counting for extra block
    if((((8 * sizeOfFileInBytes) + 1) % 512) > (512 - 64)){
        blockCount = blockCount + 1;
    }//end if

    return blockCount;

}//end calculateBlocks function


void convertCharArrayToIntArray(unsigned char buffer[], unsigned int message[], unsigned int sizeOfFileInBytes){

    //from assignment 16 charPacker

    //initialize variables
    unsigned int i;
    unsigned int shifter = 8;
    unsigned int res = 0;
    
    //loop for converting four buffer to one message array
    for(i=0; i<(sizeOfFileInBytes + 1); i += 4){

        //converting using bitwise operating
        res = buffer[i] << shifter;
        res = res | buffer[i+1];
        res = res << shifter;
        res = res | buffer[i+2];
        res = res << shifter;
        res = res | buffer[i+3];

        //storing converted buffers in the message array
        message[i/4] = res;

    }//end loop

}//end convertCharArrayToIntArray function


void addBitCountToLastBlock(unsigned int message[], unsigned int sizeOfFileInBytes, unsigned int blockCount){

    //from the instruction

    int sizeOfFileInBits = sizeOfFileInBytes * 8;
    int indexOfEndOfLastBlock =  (blockCount * 16) - 1;

    //adding bit count to last block
    message[indexOfEndOfLastBlock] = sizeOfFileInBits;

    //printf("message[last] = %x\n\n", message[indexOfEndOfLastBlock-1]);

}//end addBitCountToLastBlock function


void computeMessageDigest(unsigned int message[], unsigned int blockCount){

    //initialize variables
    unsigned int W[80];
    unsigned int A, B, C, D, E;
    unsigned int i, t, temp, fNum, kNum;

    //processing the message
    for(i=0; i<blockCount; i++){

        //dividing Mi into W array
        for(t=0; t<16; t++){
            W[t] = message[t + (16 * i)];
        }
        for(t=16; t<80; t++){
            W[t] = circleLeft((W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]), 1);
        }

        //initialize H value
        A = H[0];
        B = H[1];
        C = H[2];
        D = H[3];
        E = H[4];

        //main loop for processing
        for(t=0; t<80; t++){

            //using f function
            fNum = f(t, B, C, D);

            //using K function
            kNum = K(t);

            temp = circleLeft(A, 5) + fNum + E + W[t] + kNum;
            E = D;
            D = C;
            C = circleLeft(B, 30);
            B = A;
            A = temp;
            
        }

        H[0] = H[0] + A;
        H[1] = H[1] + B;
        H[2] = H[2] + C;
        H[3] = H[3] + D;
        H[4] = H[4] + E;

    }

    //printing ABCDE value after processing
    printf("Hex value for A, B, C, D, E after processing:\n\n");
    printf("A = 0x%x\n", A);
    printf("B = 0x%x\n", B);
    printf("C = 0x%x\n", C);
    printf("D = 0x%x\n", D);
    printf("E = 0x%x\n\n", E);

}//end computeMessageDigest function

unsigned int f(unsigned int t, unsigned int B, unsigned int C, unsigned int D){
    
    //returning calculated value depending on the value of t
    if((t >= 0) && (t <= 19)){
        return (B & C) | ((~B) & D);
    }else if((t >= 20) && (t <= 39)){
        return B ^ C ^ D;
    }else if((t >= 40) && (t <= 59)){
        return (B & C) | (B & D) | (C & D);
    }else if((t >= 60) && (t <= 79)){
        return B ^ C ^ D;
    }else{
        printf("unexpected value in t.");
        return 0;
    }

}//end f function

unsigned int K(unsigned int t){
    
    //returning value depending on the value of t
    if((t >= 0) && (t <= 19)){
        return 0x5A827999;
    }else if((t >= 20) && (t <= 39)){
        return 0x6ED9EBA1;
    }else if((t >= 40) && (t <= 59)){
        return 0x8F1BBCDC;
    }else if((t >= 60) && (t <= 79)){
        return 0xCA62C1D6;
    }else{
        printf("unexpected value in t.");
        return 0;
    }

}//end K function

unsigned int circleLeft(unsigned int number, unsigned int rotation){

    //from assignment 16 circleLeft function
    return (number << rotation) | (number >> (32 - rotation));

}//end circleLeft function
