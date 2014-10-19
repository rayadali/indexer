/*
 * Copyright 2012 Steven Gribble
 *
 *  This file is part of the UW CSE 333 course project sequence
 *  (333proj).
 *
 *  333proj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  333proj is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./HashTable.h"

// What we are storing as the value in our hashtable.
typedef struct word_count_st {
  char         *filename;
  unsigned int  count;
} WordCount;

// Print the usage message to STDERR and exit(EXIT_FAILURE);
void Usage(void);

// Reads the file into memory, allocating sufficient space
// for the file as it goes.  Returns NULL on failure, non-NULL
// on success.
char *ReadFileIntoMemory(char *filename);

// Insert a word into the hashtable.  Returns 0 on failure,
// 1 on success.
int InsertWordIntoHashTable(HashTable ht, char *word);

int main(int argc, char **argv) {
  if (argc != 2)
    Usage();

  // Read in the file content.
  char *fdata = ReadFileIntoMemory(argv[1]);
  if (fdata == NULL)
    Usage();

  // Create a hashtable.
  HashTable ht = AllocateHashTable(1);

  // Split the file up into tokens.
  char *tok = strtok(fdata, " \r\n\t");
  while (tok != NULL) {
    // Convert the word to lower-case.
    for (int i=0; i<strlen(tok); i++) {
      tok[i] = tolower(tok[i]);
    }

    // Insert into HT
    if (!InsertWordIntoHashTable(ht, tok)) {
      Usage();
    }

    tok = strtok(NULL, " \r\n\t");
  }

  // Print out the words / counts.
  HTIter it = HashTableMakeIterator(ht);
  if (it == NULL) {
    fprintf(stderr, "iterator make failed.\n");
    exit(EXIT_FAILURE);
  }

  while (!HTIteratorPastEnd(it)) {
    HTKeyValue kv;
    if (HTIteratorGet(it, &kv) == 0) {
      fprintf(stderr, "goofy return value from iterator get.\n");
      exit(EXIT_FAILURE);
    }
    WordCount *wc = (WordCount *) kv.value;
    printf("%u %s\n", wc->count, wc->filename);

    HTIteratorNext(it);
  }

  // Clean up and exit.
  HTIteratorFree(it);
  free(fdata);
  FreeHashTable(ht, &free);
  return EXIT_SUCCESS;
}

void Usage(void) {
  fprintf(stderr, "usage: word_count filename\n");
  exit(EXIT_FAILURE);
}

char *ReadFileIntoMemory(char *filename) {
  // Try to open the file.
  FILE *f = fopen(filename, "rb");
  if (f == NULL)
    return NULL;

  // Figure out how big the file is.
  long fsize;
  if (fseek(f, 0, SEEK_END) == -1) {
    fclose(f);
    return NULL;
  }
  fsize = ftell(f);
  if (fsize < 0) {
    fclose(f);
    return NULL;
  }
  if (fseek(f, 0, SEEK_SET) == -1) {
    fclose(f);
    return NULL;
  }

  // Allocate space for the file.
  char *retval = (char *) malloc(sizeof(char) * (fsize + 1));
  if (retval == NULL) {
    fclose(f);
    return NULL;
  }

  // Read the file into memory.
  size_t readval = fread(retval, fsize, 1, f);
  if (readval != 1) {
    fclose(f);
    free(retval);
    return NULL;
  }

  // NULL-terminate the file data.
  retval[fsize] = '\0';

  // Clean up.
  fclose(f);
  return retval;
}

int InsertWordIntoHashTable(HashTable ht, char *word) {
  HTKeyValue kv, oldkv;

  // Test to see if the word is already in the HT.
  if (LookupHashTable(ht,
		      FNVHash64((unsigned char *) word, strlen(word)),
		      &kv) == 1) {
    WordCount *wc = (WordCount *) kv.value;
    wc->count += 1;
    return 1;
  }

  // Allocate a WordCount to store the word.
  WordCount *wc = (WordCount *) malloc(sizeof(WordCount));
  if (wc == NULL) {
    return 0;
  }

  // Prep the WordCount and insert it.
  wc->filename = word;
  wc->count = 1;
  kv.key = FNVHash64((unsigned char *) word, strlen(word));
  kv.value = (void *) wc;
  if (InsertHashTable(ht, kv, &oldkv) != 1) {
    free(wc);
    return 0;
  }

  // All done!
  return 1;
}
