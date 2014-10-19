#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define NHASH 29989 //Use a prime number!
#define MULT 31


struct linkedlist_{
    char *word;
    struct WordCount *file;
    struct linkedlist_ *next;
};
typedef struct linkedlist_ ll;

struct word_count_st {
  char         *filename;
  unsigned int  count;
  struct word_count_st *next;
}; 
typedef struct word_count_st WordCount;

WordCount *bin[NHASH];

unsigned int hash(char *file){
    unsigned int h = 0;
    for(; *file; file++)
        h = MULT * h + *file;
    return h % NHASH;
}

int addFile(char * file){
    WordCount * temp;
    int h =  hash(file);
    for(temp = bin[h]; temp!= NULL; temp->next){
        if(strcmp(file, temp->filename) == 0){ 
            (temp->count)++; // if file already has the word, increase its count
            return 1;
        }
    }
    temp = (WordCount*)malloc(sizeof(WordCount));
    if(!temp)
        return -1;
    temp->count = 1; //else new word, initialise to count  = 1
    temp->filename = (char *)malloc(strlen(file)+1); // creating new word of filename length
    strcpy(temp->filename,file); //copy over the file name and store into nodehash
    temp->next = bin[h];
    bin[h] = temp;
}