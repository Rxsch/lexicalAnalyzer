/*
Assignment:
lex - Lexical Analyzer for PL/0


Author: Jarielys Cruz Gomez, Daniel Rangosch Montero


Language: C(only)


To Compile:
   gcc -O2 -std=c11 -o lex lex.c


To Execute (on Eustis):
   ./lex <input file>


where:
   <input file> is the path to the PL/0 source program


Notes:
   - Implement a lexical analyser for the PL/0 language.
   - The program must detect errors such as
       - numbers longer than five digits
       - identifiers longer than eleven characters
       - invalid characters.
   - The output format must exactly match the specification.
   - Tested on Eustis.


Class: COP 3402 - System Software - Fall 2025


Instructor: Dr. Jie Lin


Due Date: Friday, October 3, 2025 at 11:59 PM ET
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define norw 15     //total number of reserved words
#define noss1 12    //total number of special single symbols
#define noss2 4     //total number of special double symbols
#define imax 5      //maximum number of digits for numbers
#define cmax 11     //maximum number of chars for identifies
#define strmax 256  //maximum length of strings


typedef enum {
   skipsym = 1,    // skip/ignore token   
   identsym,       // identifier
   numbersym,      // number
   plussym,        // +
   minussym,       // -
   multsym,        // *
   slashsym,       // /
   eqsym,          // =
   neqsym,         // <>
   lessym,         // <
   leqsym,         // <=
   gtrsym,         // >
   geqsym,         // >=
   lparentsym,     // (
   rparentsym,     // )
   commasym,       // ,
   semicolonsym,   // ;
   periodsym,      // .
   becomessym,     // :=
   beginsym,       // being
   endsym,         // end
   ifsym,          // if
   fisym,          // fi
   thensym,        // then
   whilesym,       // while
   dosym,          // do
   callsym,        // call
   constsym,       // const
   varsym,         // var
   procsym,        // procedure
   writesym,       // write
   readsym,        // read
   elsesym,        // else
   evensym         // even
}TokenType;


typedef struct {
   char lexeme[strmax];    //recognized element of respective token type
   char type[strmax];      //token type in string, to display error detection
   char list[strmax];      //stores token list of current token type
   char error[strmax];     //stores respective error message
   int error_flag;         //flags any error detection
}TokenResult;


TokenResult * createTokenArray(int maxsize);
TokenResult * lexicalAnalyzer(TokenResult *result, char *scanner, int *cursize, int *maxsize);


int main(int argc, char * argv[]){
   FILE * source_program = fopen(argv[1], "r"); //reads input file from console


   //error handling for locating file
   if(source_program == NULL) {
       printf("Failed to locate file...\n");
       return 1;
   }


   printf("Source Program:\n\n");


   char ch;


   //displays entire content of input file
   while((ch = fgetc(source_program)) != EOF){
       putchar(ch);
   }
  
   rewind(source_program); //returns file point to the beginning of input file


   int scanner_maxsize = 100;
   int scanner_cursize = 0;


   //initializes string array to store all non-invisible characters from input file
   char *scanner = malloc(sizeof(char) * scanner_maxsize);


   //error handling memory allocation
   if(scanner == NULL) {
       printf("malloc() error in main()...\n");
       exit(1);
   }


   while((ch = fgetc(source_program)) != EOF){


       //allocates more memory to struct array
       if(scanner_cursize + 1 >= scanner_maxsize){
           scanner_maxsize = scanner_maxsize * 2;
           char *temp_scanner = realloc(scanner, sizeof(char) * scanner_maxsize);


           //error handling memory re-allocation
           if(temp_scanner == NULL) {
               printf("realloc() error in main()...\n");
               exit(1);
           }
           scanner = temp_scanner;
       }


       //scanner skips through invisible characters
       if(!isspace(ch) && !iscntrl(ch)){
           scanner[scanner_cursize++] = ch;
       }
   }


   if(scanner_cursize + 1 >= scanner_maxsize){
       scanner_maxsize = scanner_maxsize * 2;
       char *temp_scanner = realloc(scanner, sizeof(char) * scanner_maxsize);


       if(temp_scanner == NULL){
           printf("realloc() in main() failed...\n");
           exit(1);
       }
       scanner = temp_scanner;
   }
   scanner[scanner_cursize++] = '\0';


   int maxsize = 100;  //declares initial maximum size of struct array
   int cursize = 0;    //declares initial current size of struct array


   //initializes array struct to store all tokens to their respective lexeme
   TokenResult *result = createTokenArray(maxsize);
   result = lexicalAnalyzer(result, scanner, &cursize, &maxsize);


   printf("\n\nLexeme Table:\n\n");
   printf("lexeme\t\ttoken type\n");
  
   for(int i = 0; i < cursize; i++){
       if(result[i].error_flag != 1){
           printf("%-10s\t%s\n", result[i].lexeme, result[i].type);
       }
       else{
           printf("%-10s\t%s\n", result[i].lexeme, result[i].error);
       }
   }


   printf("\nToken List:\n\n");
  
   for(int i = 0; i < cursize; i++){
       printf("%s ", result[i].list);
   }
   printf("\n");


   free(scanner);
   free(result);
   fclose(source_program);
  
   return 0;
}


TokenResult * lexicalAnalyzer(TokenResult *result, char *scanner, int *cursize, int *maxsize){
   char *reserved_word[] = {"begin", "end", "if", "fi", "then", "while", "do",
                            "call", "const", "var", "procedure", "write", "read",
                            "else", "even"};
                           
   int wsym[] = {beginsym, endsym, ifsym, fisym, thensym, whilesym, dosym, callsym,
                 constsym, varsym, procsym, writesym, readsym, elsesym, evensym};
  
   char special_symbol[] = {'+', '-', '*', '/', '=', '<', '>', '(', ')', ',', ';', '.'};
  
   int ssym[] = {plussym, minussym, multsym, slashsym, eqsym, lessym, gtrsym,
                 lparentsym, rparentsym, commasym, semicolonsym, periodsym};


   char *double_symbols[] = {"<>", "<=", ">=", ":="};


   int ssym_double[] = {neqsym, leqsym, geqsym, becomessym};


   int scanner_size = strlen(scanner);
   int index = 0;
   int word_flag = 0;
   int symbol_flag = 0;


   while(index < scanner_size){
       word_flag = 0;
       symbol_flag = 0;


       //allocates more memory to struct array
       if(*cursize + 1 >= *maxsize){
           *maxsize = *maxsize * 2;
           TokenResult * temp_result = realloc(result, sizeof(TokenResult) * *maxsize);


           //error handling memory re-allocation
           if(temp_result == NULL){
               printf("realloc() error in lexicalAnalyzer()...\n");
               exit(1);
           }


           result = temp_result;
          
       }


       if(isalpha(scanner[index])){


           //checks and tokenizes reserved words
           for(int i = 0; i < norw; i++){
               int len = strlen(reserved_word[i]);


               if(index + len <= scanner_size && strncmp(reserved_word[i], &scanner[index], len) == 0){
                   word_flag = 1;
  
                   strcpy(result[*cursize].lexeme, reserved_word[i]);
                   sprintf(result[*cursize].type, "%d", wsym[i]);
                   strcpy(result[*cursize].list, result[*cursize].type);


                   result[*cursize].error_flag = 0;


                   *cursize = *cursize + 1;
                   index = index + len;
                   break; //exits loop if a reserved word match was found
               }
           }
          
           //checks for valid and invalid identifiers
           if(word_flag != 1){
               int id_start = index;
               int id_end = 0;
               int i = index;


               //traverses scanner to track the entire identifier
               while(i <= scanner_size && isalnum(scanner[i])){
                  
                   int word_match = 0;
                  
                   //checks to see if there is a reserved word between identifiers
                   for(int x = 0; x < norw; x++) {
                       int len = strlen(reserved_word[x]);


                       if(i + len <= scanner_size && strncmp(reserved_word[x], &scanner[i], len) == 0){
                           word_match = 1;
                           break; //exits for loop if match is found
                       }
                   }


                   //exits while loop to tokenize reserved word during the next iteration of outermost while loop
                   if(word_match == 1) {
                       break;
                   }
                  
                   i++;
               }


               id_end = i;
               int id_len = id_end - id_start; //calculates the length of identifier


               //checks if identifier follows length constraints
               if(id_len <= cmax){
                   char identifier[cmax + 1];
                   strncpy(identifier, &scanner[id_start], id_len);
                   identifier[id_len] = '\0';


                   strcpy(result[*cursize].lexeme, identifier);
                   sprintf(result[*cursize].type, "%d", identsym);
                   strcpy(result[*cursize].list, result[*cursize].type);
                   strcat(result[*cursize].list, " ");
                   strcat(result[*cursize].list, result[*cursize].lexeme);


                   result[*cursize].error_flag = 0;


                   *cursize = *cursize + 1;
                   index = id_end;
               }
               //detects and stores lexical error for identifier
               else{
                   char identifier[strmax];
                   strncpy(identifier, &scanner[id_start], id_len);
                   identifier[id_len] = '\0';


                   strcpy(result[*cursize].lexeme, identifier);
                   sprintf(result[*cursize].type, "%d", skipsym);
                   strcpy(result[*cursize].list, result[*cursize].type);


                   result[*cursize].error_flag = 1;
                   strcpy(result[*cursize].error, "Identifier too long");


                   *cursize = *cursize + 1;
                   index = id_end;
               }
           }
       }


       //checks for valid and invalid numbers
       if(isdigit(scanner[index])){
           int num_start = index;
           int num_end = 0;
           int i = index;


           //traverses scanner to track the entire number
           while(i <= scanner_size && isdigit(scanner[i])){
               i++;
           }


           num_end = i;
           int num_len = num_end - num_start;


           //checks if number follows length constraints
           if(num_len <= imax){
               char number[imax + 1];
               strncpy(number, &scanner[num_start], num_len);
               number[num_len] = '\0';


               strcpy(result[*cursize].lexeme, number);
               sprintf(result[*cursize].type, "%d", numbersym);
               strcpy(result[*cursize].list, result[*cursize].type);
               strcat(result[*cursize].list, " ");
               strcat(result[*cursize].list, result[*cursize].lexeme);


               result[*cursize].error_flag = 0;


               *cursize = *cursize + 1;
               index = num_end;
           }
           //detects and stores lexical error for number
           else{
               char number[strmax];
               strncpy(number, &scanner[num_start], num_len);
               number[num_len] = '\0';


               strcpy(result[*cursize].lexeme, number);
               sprintf(result[*cursize].type, "%d", skipsym);
               strcpy(result[*cursize].list, result[*cursize].type);


               result[*cursize].error_flag = 1;
               strcpy(result[*cursize].error, "Number too long");


               *cursize = *cursize + 1;
               index = num_end;  
           }
       }


       if(ispunct(scanner[index])){


           //checks for block comment and ignores comments if found
           if(scanner[index] == '/'){ 
               if(index + 1 <= scanner_size && scanner[index + 1] == '*'){
                   for(int i = index + 2; i < scanner_size; i++){
                       if(i + 1 <= scanner_size && scanner[i] == '*' && scanner[i + 1] == '/'){
                           index = i + 2;
                           break;
                       }
                   }
               }
           }


           //checks for special double symbols
           for(int i = 0; i < noss2; i++){
               int len = strlen(double_symbols[i]);


               if(index + len <= scanner_size && strncmp(double_symbols[i], &scanner[index], len) == 0){
                   symbol_flag = 1;


                   strcpy(result[*cursize].lexeme, double_symbols[i]);
                   sprintf(result[*cursize].type, "%d", ssym_double[i]);
                   strcpy(result[*cursize].list, result[*cursize].type);


                   result[*cursize].error_flag = 0;


                   *cursize = *cursize + 1;
                   index = index + len;
                   break;
               }
           }


           //checks for special single symbols
           for(int i = 0; i < noss1; i++){ 
               if(special_symbol[i] == scanner[index]){
                   symbol_flag = 1;


                   sprintf(result[*cursize].lexeme, "%c", special_symbol[i]);
                   sprintf(result[*cursize].type, "%d", ssym[i]);
                   strcpy(result[*cursize].list, result[*cursize].type);


                   result[*cursize].error_flag = 0;


                   *cursize = *cursize + 1;
                   index = index + 1;
                   break;
               }
           }


           if(ispunct(scanner[index])) {
              
               //checks for and stores lexical error of invalid symbols
               if(symbol_flag != 1) {
                   sprintf(result[*cursize].lexeme, "%c", scanner[index]);
                   sprintf(result[*cursize].type, "%d", skipsym);
                   strcpy(result[*cursize].list, result[*cursize].type);


                   result[*cursize].error_flag = 1;
                   strcpy(result[*cursize].error, "Invalid Symbol");


                   *cursize = *cursize + 1;
                   index = index + 1;
               }
           }
       }
   }
   return result;
}


//Function that creates an empty array of structs with dynamic memory allocation
TokenResult * createTokenArray(int maxsize){
   TokenResult * arr = malloc(sizeof(TokenResult) * maxsize);


   if(arr == NULL) {
       printf("malloc() error in createTokenArray()...\n");
       exit(1);
   }
   return arr;
}
