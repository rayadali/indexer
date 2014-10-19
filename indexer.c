/*
 * indexer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizeran.c"
#include <dirent.h>
//#include "tokenizer.c"

struct linkedlist_{
    char *word;
    struct ht *filecount;
    struct linkedlist_ *next;
};
typedef struct linkedlist_ ll;


int getAllFiles(){
    DIR *dp;
    struct dirent *ep;
    dp = opendir ("./");
    if(dp != NULL){
        while(ep = readdir(dp)){
            puts (ep->d_name);
        }
        (void) closedir(dp);
    }
    else{
        perror ("Couldn't open the directory");
    }
    return 0;
}

int openFile(char* filename){
    FILE *fp = fopen(filename, "r"); //read only
    //empty file
    if(fp == NULL){
        printf("Empty file or NULL error\n");
        return -1;
    }
    char word[100]; // create char array;
    while(fscanf(fp, "%s", word) !=EOF){
        //fscanf(fp, "%s", word);
         fscanf(fp, "%49[a-zA-Z0-9]", word);
        //TokenizerANT* tokenizer = (TokenizerANT*)malloc(sizeof(TokenizerANT));
        //tokenizer = TKANCreate(word);
        TokenizerANT* tokenizer = TKANCreate(word);
        //printf("word is %s\n",word);
	
	if(tokenizer == NULL) {
		printf("Error: unable to create tokenizer\n");
	}
	
	char* token = NULL;
	//printf("here\n");
        //printf("token is %s\n",tokenizer->copied_string);
        token = TKANGetNextToken(tokenizer);
        printf("token is %s\n",token);
         }
    fclose(fp);
    return 0;
}

int main(){
    getAllFiles();
    char *file = "boo";
    openFile(file);
    return 0;
}
 