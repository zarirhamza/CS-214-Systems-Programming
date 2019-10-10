#include <stdio.h>
#include <stdlib.h>

int compare (const char *str1, const char *str2){ //essentially strcmp, copied from source code
	while(*str1){
		//printf("%c", *str2);
		if(*str1 != *str2)
            		break;
       		 str1++;
		 str2++;
    	}
	//printf("\n");
    	return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int isDigit(const char *str1){//self explanatory
	while(*str1){
		if(*str1 != '0' && *str1 != '1' && *str1 != '2' && *str1 != '3' && *str1 != '4' && *str1 != '5' && *str1 != '6' && *str1 != '7' && *str1 != '8' && *str1 != '9'){
			return 0;
		}
		*str1++;
	}
	return 1;
}

int isBoolean(const char *str1){
	if(compare(str1,"true") == 0 || compare(str1,"false") == 0){
		return 1;
	}
	return 0;
}

int isArithmetic (const char *str1){ // self explanatory
	if(compare(str1,"+") == 0 || compare(str1,"-") == 0 || compare(str1,"*") == 0 || compare(str1,"/") == 0)
		return 1;
	return 0;
}

int isLogical (const char *str1){// self explanatory
	if(compare(str1,"AND") == 0 || compare(str1,"OR") == 0 || compare(str1,"NOT") == 0)
		return 1;
	return 0;
}

char** tokenize(const char *str1, const char str2){ //seprate string str1 by any character str2
	int curr = 0;
	int prev = 0; // indexes of beginning and end of each token
	int ct = 0; //number of tokens

	int hold = 0;

	int length;
	for(length = 0; str1[length] != '\0'; ++length); //length of string
	
	char **tokens = (char**)calloc(length,sizeof(char*));
	int init = length;
	while(init > 0){
		tokens[length - init] = (char*) calloc((length+1),sizeof(char));
		init--;	
	} // initialize char array
	//loop through string to split tokens by space and semicolon
	while(str1[curr] != '\0'){
		if(str1[curr] == str2){
			while(prev < curr){
				tokens[ct][hold] = str1[prev]; //add char by char to string array of tokens
				prev++;
				hold++;
			}
			
			if(hold == 0){
				if(str2 == ' ' && curr == 0 && str1[curr] != '\0')
					ct--;
				else
					tokens[ct] = "EMPTYEXPRESSION";	//place holder for empty expressions
			}
			/*if(str1[curr] != '\0' && str1[curr + 1] == str2){
				tokens[ct + 1][0] = str2;
				tokens[ct + 1][1] = '\0';
				ct++;
			}*/
			ct++;
			prev++;
			hold = 0;
		}
		curr++;
	}
	
	//add final token to array of tokens
	hold = 0;
	while(prev < curr){
		tokens[ct][hold] = str1[prev];
		prev++;
		hold++;
	}
	//printf("%s\n",tokens[ct]);
	tokens[ct+1] = "\0"; //end of array
	return tokens;
}

enum State {start = 0, num1 = 1, arith = 2, bool1 = 3, logic = 4}; //current state of equation

int main (int argc, char **argv){
	if(argc != 2){
		printf("Wrong number of arguments\n");
		return 0;
	}
	else if(argv[1][0] == '\0'){
		printf("Found 1 expressions: 0 logical and 0 arithmetic.\nError: Scan error in expression 0: incomplete expression\n");
		return 0;
	}
		
	char **listOfTokens = tokenize(argv[1],';'); 
		
	int numOfExpressions = 0;
	int numOfArithExpressions = 0;
	int numOfLogicExpressions = 0; // counters for expressions

	char **listStart = listOfTokens; // first loop to count number of expressions
	while(**listOfTokens){
		char **expression = tokenize(*listOfTokens,' ');
		while(**expression){
			if(isDigit(*expression)){
				numOfArithExpressions++;
				break;
			}
			else if(isBoolean(*expression)){
				numOfLogicExpressions++;
				break;
			}
			**expression++;
		}
		numOfExpressions++;
		**listOfTokens++;
	}
	printf("Found %d expressions: %d logical and %d arithmetic\n", numOfExpressions, numOfLogicExpressions, numOfArithExpressions);	
	
	listOfTokens = listStart;
	int OK = 0;
	int iter = 0; // expr number
	int tkn = 0; // number of tkns
	int NOT = 0;

	enum State st = start;
	int currentOK = 0;
	while(**listOfTokens){ //second loop to identify individuals errors
		char **expression = tokenize(*listOfTokens,' ');
		st = start;
		while(**expression){ //ALL POSSIBLE CASES
			currentOK = OK;
			if(tkn == 3){
				printf("Error: Parse error in expression %d: Expression not ended\n", iter);
				//printf("Error: Parse error in expression %d: Unexpected operand %s\n", iter, *expression);
				st = 0;
				OK++;
				//currentOK++; 
			}
			if(st == 0){
                                if(isDigit(*expression)){
                                        if(tkn >= 3)
						OK++;
					st = num1;
				}
                                else if(isBoolean(*expression)){
                                        if(tkn >= 3)
						OK++;
					st = bool1;
				}
                                else if(compare(*expression,"NOT") == 0){
                                        if(tkn >= 3)
						OK++;
					st = logic;
				}
                                else if(compare(*expression,"EMPTYEXPRESSION") == 0){
                                        	printf("Error: Scan error in expression %d: empty expression - ""\n",iter);
                                        	OK++;
                                }
				else if(isArithmetic(*expression) || isLogical(*expression)){
					printf("Error: Parse error in expression %d: unexpected operator - %s\n",iter,*expression);                                                                                                     OK++;
                                        st = start;
				}
                                else if(tkn < 3){
                                        printf("Error: Parse error in expression %d: unknown identifier - %s\n",iter,*expression);
					st = start;
				}
			}
			else if (st == 1){
				if(isArithmetic(*expression)){
						st = arith;
				}
				else if (isLogical(*expression)){
					if(tkn >= 3)
						st = 1;
					else{
						st = arith;
						printf("Error: Parse error in expression %d: Operand mismatch - %s\n",iter,*expression);
                        		}
					OK++;
				}
				else{
					if(tkn >= 3)
                                                st = 1;
                                        else{
                                                st = arith;
						printf("Error: Parse error in expression %d: unknown operator - %s\n",iter,*expression);   
					} 
                                        OK++;
				}	
			}
			else if (st == 2){
					if(isDigit(*expression))
                                        	st = start;
                                	else{
                                        	printf("Error: Parse error in expression %d: unknown operand - %s\n",iter,*expression);                                                                                                         OK++;
						st = start;
					}

			}
			else if(st == 3){
					if(isLogical(*expression)){
						st = logic;
						if (compare(*expression,"NOT") == 0){
							printf("Error: Parse error in expression %d: Too many arguments for unary operator - %s\n",iter,*expression);
							OK++;
						}
					}
		
					else if (isArithmetic(*expression)){
						st = logic;
						if(tkn >= 3)
							st = 3;
						else
							printf("Error: Parse error in expression %d: Operand mismatch - %s\n",iter,*expression);
                        			OK++;
					}
					else{
						st = logic;
						if(tkn >= 3)
                                                        st = 3;
                                                else
						printf("Error: Parse error in expression %d: unknown operator - %s\n",iter,*expression);                                                                                                        OK++;
					}
			}
			else if(st == 4){
				if(isBoolean(*expression)){
					st = start;
					NOT++;
				}
                                else{
                                        printf("Error: Parse error in expression %d: unknown operand - %s\n",iter,*expression);                                                                                                         OK++;
	                   		st = start;
				}
			}
			if (currentOK != OK && tkn >= 3){
				if(isDigit(*expression) || isBoolean(*expression))
					printf("Error: Parse error in expression %d: unknown operand - %s\n",iter,*expression); 
				else if (isArithmetic(*expression) || isLogical(*expression))
					printf("Error: Parse error in expression %d: unknown operator - %s\n",iter,*expression); 
				else if(compare((*expression), "EMPTYEXPRESSION") != 0)
					printf("Error: Parse error in expression %d: unknown identifier - %s\n",iter,*expression); 
			}
			tkn++;
			**expression++;
		}
		if (tkn == 1){
			**expression--;
			printf("Error: Parse error in expression %d: missing operator after %s\n", iter,  *expression);
			OK++;
		}
		else if(tkn == 2 && NOT == 0 && st != 1 && st != 3){
			**expression--;
			//printf("%d\n", st);
			printf("Error: Parse error in expression %d: missing operand after %s\n", iter,  *expression);
                        OK++;
		}
		NOT = 0;
		iter++;
		**listOfTokens++;
		tkn = 0;
	}
	
	
	int length;
	for(length = 0; argv[1][length] != '\0'; ++length); //length of string
	**listOfTokens--;
	if(argv[1][length-1] == ';'){
		printf("Error: Parse error in expression %d: Unfinished expression - %s;\n", --iter,*listOfTokens);
		OK++;
	}
	if(OK == 0)
		printf("OK\n");

	return 0;
}
