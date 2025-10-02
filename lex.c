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
   - Implement a lexical analyzer for the PL/0 language.
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
#define strmax 500  //maximum length of strings


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
   int token;              //token type of respective lexeme
   char lexeme[strmax];    //stores lexeme in string  
   char list[strmax];      //stores stream of token in 'token lexeme' format
   char error[strmax];     //stores error message
   int error_flag;         //flags error detected during lexical analysis
}TokenResult;


char * fileScanner(FILE * source_program);
TokenResult * lexicalAnalyzer(TokenResult *result, char *scanner, int *cursize, int *maxsize);
TokenResult * createTokenArray(int maxsize);
TokenResult * doubleTokenArray(TokenResult *result, int *maxsize);


int main(int argc, char * argv[]){
   FILE * source_program = fopen(argv[1], "r"); //reads input file from console


   //checks if file was not located
   if(source_program == NULL){
       printf("Failed to locate file...\n");
       return 1;
   }


   printf("Source Program:\n\n");


   char ch;


   //displays entire content of input file
   while((ch = fgetc(source_program)) != EOF){
       putchar(ch);
   }


   rewind(source_program); //returns file pointer to start of input file


   char * scanner = fileScanner(source_program);


   int maxsize = 500;
   int cursize = 0;


   TokenResult *result = createTokenArray(maxsize);
   result = lexicalAnalyzer(result, scanner, &cursize, &maxsize);


   printf("\n\nLexeme Table:\n\n");
   printf("lexeme\t\ttoken type\n");
  
   for(int i = 0; i < cursize; i++){
       if(result[i].error_flag == 0){
           printf("%-10s\t%d\n", result[i].lexeme, result[i].token);
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


       //allocates more memory to TokenResult struct array
       if(*cursize >= *maxsize){
           result = doubleTokenArray(result, maxsize);
       }


       //checks alphabetic characters for reserved words and identifiers
       if(isalpha(scanner[index])){


           //tokenizes found reserved word
           for(int i = 0; i < norw; i++){
               int len = strlen(reserved_word[i]); //declares length of each reserved word


               if(index + len <= scanner_size && strncmp(reserved_word[i], &scanner[index], len) == 0){
                   word_flag = 1; //flags reserved word match


                   //stores lexeme, token type, and token list in result struct array
                   strcpy(result[*cursize].lexeme, reserved_word[i]);
                   result[*cursize].token = wsym[i];
                   sprintf(result[*cursize].list, "%d", result[*cursize].token);


                   //flags and stores no error found
                   result[*cursize].error_flag = 0;
                   strcpy(result[*cursize].error, "");


                   *cursize = *cursize + 1; //increments struct array index for next element
                   index = index + len;     //increments index to next char after reserved word


                   break; //exits loop if a reserved word match is found
               }
           }


           //checks for valid and invalid identifiers
           if(word_flag != 1 && isalpha(scanner[index])){
               int id_start = index;
               int id_end = index;
               int i = index;


               //traverses scanner to track the length of identifier
               while(i <= scanner_size && isalnum(scanner[i])){
                   word_flag = 0;


                   //checks for reserved word in between identifiers
                   for(int x = 0; x < norw; x++){
                       int len = strlen(reserved_word[x]);


                       if(i + len <= scanner_size && strncmp(reserved_word[x], &scanner[i], len) == 0){
                           word_flag = 1;
                           break; //exits for loop if a reserved word match is found
                       }
                   }


                   //exits while loop to tokenize identifier before the reserved word match
                   if(word_flag == 1){
                       break;
                   }
                   i++;
               }


               id_end = i;
               int id_len = id_end - id_start; //calculates length of identifier


               //checks if identifier follows length constraints
               if(id_len <= cmax){


                   //declares a string variable and stores identifier
                   char identifier[cmax + 1];
                   strncpy(identifier, &scanner[id_start], id_len);
                   identifier[id_len] = '\0';


                   //stores valid identifier and its token in struct array
                   strcpy(result[*cursize].lexeme, identifier);
                   result[*cursize].token = identsym;
                   sprintf(result[*cursize].list, "%d", result[*cursize].token);
                   strcat(result[*cursize].list, " ");
                   strcat(result[*cursize].list, result[*cursize].lexeme);


                   result[*cursize].error_flag = 0;
                   strcpy(result[*cursize].error, "");


                   *cursize = *cursize + 1;
                   index = id_end;
               }
               else{
                   char identifier[strmax];
                   strncpy(identifier, &scanner[id_start], id_len);
                   identifier[id_len] = '\0';


                   //stores lexical error for identifier in struct array
                   strcpy(result[*cursize].lexeme, identifier);
                   result[*cursize].token = skipsym;
                   sprintf(result[*cursize].list, "%d", result[*cursize].token);


                   //flags error and stores error type
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
           int num_end = index;
           int i = index;


           //traverses scanner to track length of number
           while(i <= scanner_size && isdigit(scanner[i])){
               i++;
           }


           num_end = i;
           int num_len = num_end - num_start;


           //checks if number follows length constraints
           if(num_len <= imax){


               //declares a string variable and stores number
               char number[imax + 1];
               strncpy(number, &scanner[num_start], num_len);
               number[num_len] = '\0';


               //stores valid number and its token in struct array
               strcpy(result[*cursize].lexeme, number);
               result[*cursize].token = numbersym;
               sprintf(result[*cursize].list, "%d", result[*cursize].token);
               strcat(result[*cursize].list, " ");
               strcat(result[*cursize].list, result[*cursize].lexeme);


               result[*cursize].error_flag = 0;
               strcpy(result[*cursize].error, "");


               *cursize = *cursize + 1;
               index = num_end;
           }
           else{
               char number[strmax];
               strncpy(number, &scanner[num_start], num_len);
               number[num_len] = '\0';


               //stores lexical error for number in struct array
               strcpy(result[*cursize].lexeme, number);
               result[*cursize].token = skipsym;
               sprintf(result[*cursize].list, "%d", result[*cursize].token);


               //flags error and stores error type
               result[*cursize].error_flag = 1;
               strcpy(result[*cursize].error, "Number too long");


               *cursize = *cursize + 1;
               index = num_end;
           }
       }


       //checks for comments, special symbols, and invalid symbols
       if(ispunct(scanner[index])){
          
           //checks for comment delimeters and ignores comment if found
           if(scanner[index] == '/'){
               if(index + 1 <= scanner_size && scanner[index + 1] == '*'){


                   //traverses comment until end comment symbols "*/" are found
                   for(int i = index + 2; i < scanner_size; i++){
                       if(i + 1 <= scanner_size && scanner[i] == '*' && scanner[i + 1] == '/'){
                           index = i + 2; //increments index to ignore entire comment
                           break;         //exits for loop after entire comment is skipped
                       }
                   }
               }
           }


           //checks for special double symbols
           for(int i = 0; i < noss2; i++){
               int len = 2;


               if(index + len <= scanner_size && strncmp(double_symbols[i], &scanner[index], len) == 0){
                   symbol_flag = 1;


                   //stores lexeme and token type of special symbol in struct
                   strcpy(result[*cursize].lexeme, double_symbols[i]);
                   result[*cursize].token = ssym_double[i];
                   sprintf(result[*cursize].list, "%d", result[*cursize].token);


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
                   result[*cursize].token = ssym[i];
                   sprintf(result[*cursize].list, "%d", result[*cursize].token);


                   result[*cursize].error_flag = 0;


                   *cursize = *cursize + 1;
                   index = index + 1;
                   break;
               }
           }


           if(symbol_flag != 1 && ispunct(scanner[index])){


               //stores lexical error of invalid symbols
               sprintf(result[*cursize].lexeme, "%c", scanner[index]);
               result[*cursize].token = skipsym;
               sprintf(result[*cursize].list, "%d", result[*cursize].token);


               //flags and store error type
               result[*cursize].error_flag = 1;
               strcpy(result[*cursize].error, "Invalid Symbol");


               *cursize = *cursize + 1;
               index = index + 1;
           }
       }
   }
   return result;
}


  
//Function that reads input file and stores it in scanner character by character
char * fileScanner(FILE * source_program){
   int maxsize = 500;  //declares initial max size of scanner
   int cursize = 0;    //declares the total number of elements in scanner


   //dynamically allocates memory to scanner
   char *scanner = malloc(sizeof(char) * maxsize);


   char ch;


   while((ch = fgetc(source_program)) != EOF){


       //allocates more memory to scanner array
       if(cursize >= maxsize){
           maxsize *= 2;
           char * temp = realloc(scanner, sizeof(char) * maxsize);


           //checks if realloc() was unsucessful
           if(temp == NULL){
               printf("realloc() error in fileScanner()...\n");
               exit(1);
           }
           scanner = temp;
       }


       //scanner skips through invisible characters
       if(!isspace(ch) && !iscntrl(ch)){
           scanner[cursize++] = ch; //stores input file
       }
   }


   if(cursize >= maxsize){
       maxsize *= 2;
       char * temp = realloc(scanner, sizeof(char) * maxsize);


       if(temp == NULL){
           printf("realloc() error in fileScanner()...\n");
           exit(1);
       }
       scanner = temp;
   }
   scanner[cursize++] = '\0';


   return scanner;
}


//Function creates an empty TokenResult struct array
TokenResult * createTokenArray(int maxsize){
   TokenResult *result = malloc(sizeof(TokenResult) * maxsize);


   if(result == NULL){
       printf("malloc() error in createTokenArray()...\n");
       exit(1);
   }
   return result;
}




//Function allocates more memory to TokenResult struct array
TokenResult * doubleTokenArray(TokenResult *result, int *maxsize){
   *maxsize = *maxsize * 2;


   TokenResult *temp = realloc(result, sizeof(TokenResult) * (*maxsize));


   if(temp == NULL){
       printf("realloc() error in doubleTokenArray()...\n");
       exit(1);
   }
   result = temp;


   return result;
}


