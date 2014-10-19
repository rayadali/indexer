/*
 * indexer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizeran.c"
#include "hashWordCount.c"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include "sl/sortedlist.c"

#
//#include "tokenizer.c"

SortedListPtr wordList;

int compareCounts(void *p1, void *p2)
{
	WordCount s1 = p1;
	WordCount s2 = p2;

	return strcmp(s1->filename, s2->filename);
}

int compareWords(void *p1, void *p2)
{
	ll s1 = p1;
	ll s2 = p2;

	return strcmp(s1->word, s2->word);
}

int addWord(char * word, char * filename){
    //intialisation
    WordCount fileToAdd;
    fileToAdd = (WordCount)malloc(sizeof(WordCount));
    fileToAdd->filename = (char*)malloc((sizeof(char)*100));
    fileToAdd->count = 1;
    fileToAdd->filename = filename;
    ll word = (ll)malloc(sizeof(ll));
    word->word = (char*)malloc((sizeof(char)*100));
    word->word = word;
    
    //need to look at sorted list
    ll wordexists;
    wordexists = (ll) SLFindNode(wordList, word);
    
    //if exists
    
    if(wordexists != NULL){
        printf("word %s exists\n", word);
        SortedListIteratorPtr iter;
        iter =  SLCreateIterator(wordexists->file);
        void *object =  NULL;
        while((object = SLNextItem(iter))){
            WordCount file;
            file = object;
            
            if(compareCounts(file, object) == 0){
                file->count++;
                break;
            }
            //addWord(file->file->filename);
        }
        SLDestroyIterator(iter);
        
        if(object == NULL){ //does not found, add!!
            SLInsert(wordexists->file, fileToAdd); //adding
            //free
            free(word->word);
            free(word);
            return 1;
        }
        else{
            //free everything
            printf("error when adding word\n");
            free(fileToAdd->filename);
            free(fileToAdd);
            free(word->word);
            free(word);
        }
        return 0;
    }
    else{ //file not found
        word->file = SLCreate(compareCounts);
        SLInsert(word->file,fileToAdd);
        SLInsert(wordList, word);
        return 1;
    }
}

int addToHash(char *filename, char* word){
    if(filename == NULL || word == NULL){
        return -1; // errors or empty pointers.
    }
    TokenizerANT tokenizer = TKANCreate(word);
    if(tokenizer == NULL){
        printf("Error creating tokenizer\n");
        return -1;
    }
    
    char* token =  NULL;
    
    while((token =  TKANGetNextToken(tokenizer)) != NULL){
        //got to convert to lowercase
        for(int i = 0; token[i]; i++){
            token[i] = tolower(token[i]);
        }
        //now add to hash
        addWord(filename, token, tokenizer->copied_string);
        // and free created token!
        free(token);
        }
    //check if tokeniser object is freed or not
    if(tokenizer  != NULL){
        TKANDestroy(tokenizer);
    }
}

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
        
        // need to record the token in the file and the count
 
        addWord(token,filename);
        printf("token is %s\n",token);
         }
    fclose(fp);
    return 0;
}

int openDirectory(char *dir){
    char *tempDir;
    tempDir = (char*)malloc((sizeof(char))*100);
    tempDir[0] = '\0'; //null char so we know when we end
    strcat(tempDir, dir);
    //attempting to use FTS for traversing directories
    
    char *directoryArray[2]; //create array of char array
    directoryArray[0] = tempDir;
    directoryArray[1] =  NULL;
    
    FTS *ftsPtr; // create pointer of file hierarchy
    FTSENT *parent, *child; //file structure repreentations
    /* typedef struct _ftsent {
               unsigned short fts_info;     // flags for FTSENT structure 
               char          *fts_accpath;  // access path 
               char          *fts_path;     // root path 
               short          fts_pathlen;  // strlen(fts_path) 
               char          *fts_name;     // filename 
               short          fts_namelen;  // strlen(fts_name) 
               short          fts_level;    // depth (-1 to N) 
               int            fts_errno;    // file errno 
               long           fts_number;   // local numeric value 
               void          *fts_pointer;  // local address value 
               struct ftsent *fts_parent;   // parent directory 
               struct ftsent *fts_link;     // next file structure/
               struct ftsent *fts_cycle;    // cycle structure 
               struct stat   *fts_statp;    // stat(2) information/
           } FTSENT; */
    
    // options for fts_open
    /* There are a number of options, at least one of which (either
       FTS_LOGICAL or FTS_PHYSICAL) must be specified.  The options are
       selected by ORing the following values:

       FTS_COMFOLLOW
                    This option causes any symbolic link specified as a root
                    path to be followed immediately whether or not
                    FTS_LOGICAL is also specified.

       FTS_LOGICAL  This option causes the fts routines to return FTSENT
                    structures for the targets of symbolic links instead of
                    the symbolic links themselves.  If this option is set,
                    the only symbolic links for which FTSENT structures are
                    returned to the application are those referencing
                    nonexistent files.  Either FTS_LOGICAL or FTS_PHYSICAL
                    must be provided to the fts_open() function.

       FTS_NOCHDIR  As a performance optimization, the fts functions change
                    directories as they walk the file hierarchy.  This has
                    the side-effect that an application cannot rely on being
                    in any particular directory during the traversal.  The
                    FTS_NOCHDIR option turns off this optimization, and the
                    fts functions will not change the current directory.
                    Note that applications should not themselves change
                    their current directory and try to access files unless
                    FTS_NOCHDIR is specified and absolute pathnames were
                    provided as arguments to fts_open().

       FTS_NOSTAT   By default, returned FTSENT structures reference file
                    characteristic information (the statp field) for each
                    file visited.  This option relaxes that requirement as a
                    performance optimization, allowing the fts functions to
                    set the fts_info field to FTS_NSOK and leave the
                    contents of the statp field undefined.

       FTS_PHYSICAL This option causes the fts routines to return FTSENT
                    structures for symbolic links themselves instead of the
                    target files they point to.  If this option is set,
                    FTSENT structures for all symbolic links in the
                    hierarchy are returned to the application.  Either
                    FTS_LOGICAL or FTS_PHYSICAL must be provided to the
                    fts_open() function.

       FTS_SEEDOT   By default, unless they are specified as path arguments
                    to fts_open(), any files named "."  or ".."  encountered
                    in the file hierarchy are ignored.  This option causes
                    the fts routines to return FTSENT structures for them.
     */
    int ftsOptions = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR; // | FTS_NOSTAT | FTS_PHYSICAL | FTS_SEEDOT;
    
    printf("we are in DIR %s\n",directoryArray);
    if((ftsPtr = fts_open(directoryArray, ftsOptions, NULL))==NULL){
        printf("error in fts\n"); //error
        return -1;
    }
    
    //no children
    /*fts_children()
       The fts_children() function returns a pointer to an FTSENT structure
       describing the first entry in a NULL-terminated linked list of the
       files in the directory represented by the FTSENT structure most
       recently returned by fts_read().  The list is linked through the
       fts_link field of the FTSENT structure, and is ordered by the user-
       specified comparison function, if any.  Repeated calls to
       fts_children() will re-create this linked list.
     */
    child = fts_children(ftsPtr,0);
    if(child ==  NULL) return 0;
    
    char *fullPathName;
    
    while((parent = fts_read(ftsPtr))!= NULL){
        switch(parent->fts_info){
            //FTS_F       A regular file.
            case FTS_F:
                fullPathName = (char*)malloc((sizeof(char))*100);
                fullPathName = parent->fts_path;
                openFile(fullPathName);
                break;
            default:
                printf("bad file\n");
                break;
        }
    }
    //close and free
    fts_close(ftsPtr);
    free(fullPathName);
    
}

int main(){
    getAllFiles();
    char *file = "boo";
    openFile(file);
    return 0;
}
 
