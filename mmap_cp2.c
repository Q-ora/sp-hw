#define _GNU_SOURCE

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
 
int main(int argc, char *argv[])
{
    int inputFd, outputFd;
    char *inputPtr, *outputPtr;
    char *inputCur, *outputCur;
    off_t begin=0, end=0;
    ssize_t numIn, numOut;
    ssize_t fileSize=0;
    ssize_t dataSize=0;

    //只讀
    inputFd = open (argv [1], O_RDONLY);
    if (inputFd == -1) {
        perror ("cannot open the file for read"); exit(1); }

    //可讀可寫
    outputFd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR| S_IWUSR);
    if(outputFd == -1){
        perror("canot open the file for write"); exit(1); }

    //fileSize
    fileSize = lseek(inputFd, 0, SEEK_END);
    //printf("file size = %ld\n", fileSize);

    inputPtr = mmap(NULL, fileSize, PROT_READ, MAP_SHARED , inputFd , 0);
    //perror("mmap");
    //printf("inputPtr = %p\n", inputPtr);

    //設定檔案大小
    ftruncate(outputFd, fileSize);
    outputPtr = mmap(NULL, fileSize, PROT_WRITE, MAP_SHARED , outputFd , 0);
    //perror("mmap, output");
    //printf("outputPtr = %p\n", outputPtr);

    //printf("memory copy\n");
    //time_t timer1, timer2;
    //timer1 = time(NULL);

    //=============================
    //      以下是主要修改的部份      
    //=============================
    
    lseek(inputFd, 0, SEEK_SET);
    inputCur = inputPtr;
    outputCur = outputPtr;
    //用lseek跳過file hole, 找到data block
    //用memcpy複製data block
    while( lseek(inputFd, 0, SEEK_CUR) < fileSize ){
        begin = lseek(inputFd, end, SEEK_DATA);
        end = lseek(inputFd, begin, SEEK_HOLE);
        inputCur = begin + inputPtr;
        outputCur = begin + outputPtr;
        dataSize = end - begin;
        memcpy(outputCur, inputCur, dataSize);
        lseek(inputFd, end, SEEK_SET);
    }

    //timer2 = time(NULL);

    //printf("time(memcpy) = %ld sec \n", timer2 - timer1);

    assert(munmap(inputPtr, fileSize) == 0);
    assert(munmap(outputPtr, fileSize) == 0);
    
    assert(close (inputFd) == 0);
    assert(close (outputFd) == 0);

    return (EXIT_SUCCESS);
}