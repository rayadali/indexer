/*
 * tokenizeran.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_HEX_CHARS 2
#define MAX_OCT_CHARS 3

struct TokenizerAlphaNumeric_{
    char* copied_string;
    char* current_position;
};
typedef struct TokenizerAlphaNumeric_ TokenizerANT;

char is_escape_character(char character) {
	
	/*
	 * Description: determines if a character is a member of the set of escaped characters
	 * Parameters: character to be evaluated
	 * Modifies: nothing
	 * Returns: appropriate escape character sequence if it is a member, 0 if it is not
	 * 
	 */

	char* escape_sequence = "ntvbrfa\\?'\"";
	char* escape_characters = "\n\t\v\b\r\f\a\\\?\'\"";
	int offset = 0;
	
	for(offset = 0; offset < strlen(escape_sequence); offset++) {
		if(*(escape_sequence + offset) == character) {
			return *(escape_characters + offset);
		}
	}
	
	return 0;
}
 
int char_to_hex(char character) {
	
	/*
	 * Description: converts a hex digit represented as a character into its actual integer value
	 * Parameters: character to be converted
	 * Modifies: nothing
	 * Returns: value of the hex digit character as an integer
	 * 
	 */
	
	if(isdigit(character)){
		return character - '0';
	} else if (islower(character)){
		return character - 'a' + 10;
	} else {
		return character - 'A' + 10;
	}
} 
 
int char_to_oct(char character) {

	/*
	 * Description: converts a octal digit represented as a character into its actual integer value
	 * Parameters: character to be converted
	 * Modifies: nothing
	 * Returns: value of the octal digit character as an integer
	 * 
	 */

	return character - '0';
} 
 
int is_oct_digit(char oct_digit) {
	
	/*
	 * Description: determines if a character represents a octal digit
	 * Parameters: character to be evaluated
	 * Modifies: nothing
	 * Returns: 1 if it is an octal character, 0 if it is not
	 * 
	 */
	
	if(oct_digit >= '0' && oct_digit <= '7') {
		return 1;
	} else {
		return 0;
	}
}
 

char* unescape_stringan(char* string) {
	
	/*
	 * Description: given an input stream converts escaped characters into their appropriate char representation and returns it as a new string
	 * Parameters: string to be converted
	 * Modifies: nothing
	 * Returns: resulting string with all the escaped characters with their appropriate values
	 * 
	 */

	char* unescaped_string = (char*)malloc(strlen(string) * sizeof(char) + 1);
	int current_position = 0;
	int unescaped_string_position = 0;
	unsigned char escape_character = 0;	
	
	for(current_position = 0; current_position < strlen(string); current_position++) {	
			escape_character = *(string + current_position);
			if(*(string + current_position) == '\\') {
				if(*(string + current_position + 1) == 'x') {
					current_position++;
					unsigned int hex_count;
					escape_character = 0;
					for(hex_count = 1; hex_count <= MAX_HEX_CHARS; hex_count++) {
						if(!isxdigit(*(string + current_position + hex_count))) {
							break;
						}
						escape_character = escape_character * 16 + char_to_hex(*(string + current_position + hex_count));
					}
					hex_count--;
					current_position += hex_count;
				} else if(is_oct_digit((*(string + current_position + 1))) == 1) {
					int oct_count;
					escape_character = 0;
					for(oct_count = 1; oct_count <= MAX_OCT_CHARS; oct_count++) {
						if(is_oct_digit(*(string + current_position + oct_count)) == 0) {
							break;
						}
						escape_character = escape_character * 8 + char_to_oct(*(string + current_position + oct_count));
					}
					current_position += oct_count;
				} else {
					escape_character = is_escape_character(*(string + current_position + 1));
					
					if(escape_character == 0) {
						escape_character = *(string + current_position);
					} else {
						current_position++;
					}
				}
			}
			*(unescaped_string + unescaped_string_position) = escape_character;
			unescaped_string_position++;
			escape_character = 0;
	}
	
	*(unescaped_string + unescaped_string_position) = '\0';
	
	return unescaped_string;
}


TokenizerANT *TKANCreate(char *ts) {
	
	/*
	 * Description: creates a new tokenizer struct from the token stream and delimiters
	 * Parameters: set of delimiters, token stream
	 * Modifies: nothing
	 * Returns: a pointer to a tokenizer struct on success, a null pointer on failure
	 * 
	 */
	 
	if(ts == NULL){
		return NULL;
	}
	
	TokenizerANT* tokenizer = (TokenizerANT*)malloc(sizeof(TokenizerANT));
	
	if(tokenizer == NULL){
		return NULL;
	}
	
	tokenizer->copied_string = unescape_stringan(ts);
	
	tokenizer->current_position = tokenizer->copied_string;
	
	return tokenizer;
}

/*
 * TKANDestroy destroys a TokenizerAlphaNumT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKANDestroy(TokenizerANT *tk) {	
	
	/*
	 * Description: destroys tokenizer struct and deallocates all memory
	 * Parameters: tokenizer to be destroyed
	 * Modifies: tokenizer struct: deallocates memory
	 * Returns: nothing 
	 */
	
	if (tk->copied_string != NULL) free(tk->copied_string);

	free(tk);
	
	return;
}


char is_delimiteran(char character) {
	
	/*
	 * Description: determines if a particular character is a member of the set of delimiters
	 * Parameters: character to be compared, string of delimiters
	 * Modifies: Nothing
	 * Returns: 1 if character is a delimiter, 0 if it is not
	 */
	
	//accept alphanumeric only
        //0-9
    if(character >= 48 && character <=57){
        return 0;
    }
    //A-Z
    if(character >= 65 && character <=90){
        return 0;
    }
    //a-z
    if(character >= 97 && character <= 122){
        return 0;
    }
    else{ // not AN
        return 1;
    }
}



char *TKANGetNextToken(TokenizerANT *tk) {

	/*
	 * Description: returns the next token from the token stream specified within the tokenizer
	 * Parameters: tokenizer from which to extract token
	 * Modifies: tokenizer->current_position: identifies starting point of next token; creates a new string with 
	 * Returns: token extracted as a char* on success, null on failure/end of string;
	 */
	
	char* token = NULL;
	char* token_start = NULL;

	while(tk->current_position - tk->copied_string < strlen(tk->copied_string)) {
		if(!is_delimiteran(*tk->current_position)) {
		
			token_start = tk->current_position;
			break;
		}

			tk->current_position++;
	}
	
	if(token_start == NULL) {
		return NULL;
	}
	
	while(tk->current_position - tk->copied_string < strlen(tk->copied_string)) {
		if(is_delimiteran(*tk->current_position)) {
			break;
		}

			tk->current_position++;
	}	

	token = (char*)malloc(sizeof(char) * (tk->current_position - token_start + 1));
	strncpy(token, token_start, tk->current_position - token_start);
	token[(tk->current_position - token_start)] = '\0';

	/*convert token to lower-case*/
        
	
	return token;
}
/*
int main() {
	

	
	TokenizerANT* tokenizer = TKANCreate("Hello how are you? mail@rutgers12");
	
	if(tokenizer == NULL) {
		printf("Error: unable to create tokenizer\n");
	}
	
	char* token = NULL;
	
	while((token = TKANGetNextToken(tokenizer)) != NULL) {
		printf("%s\n", token);
		free(token);
	}
	
	TKANDestroy(tokenizer);

	return 0;
}
 * 
 * */