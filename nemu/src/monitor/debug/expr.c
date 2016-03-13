#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256,PLUS,  EQ, NUM, L_PAR, R_PAR, MINUS, MULT, DIV

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", PLUS},					// plus
	{"==", EQ},						// equal
	{"[0-9]+", NUM},
	{"\\(", L_PAR},
	{"\\)", R_PAR},
	{"\\-", MINUS},
	{"\\*", MULT},
	{"\\/", DIV},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;
static bool check_parenthese(int bg, int ed){
	int x = 0;
	int i;
	for ( i = bg; i <= ed; ++i)
	{
		if(tokens[i].type == L_PAR){
			x++;
			continue;
		}
		else if(tokens[i].type == R_PAR){
			x--;
			if(x < 0){
				printf("%d\n", x);
				Assert(0, "Invalid expression\n");
			}
			continue;
		}
		else{
			continue;
		}
	}
	if(x != 0){
		Assert(0, "Invalid expression\n");
	}
	else if(tokens[bg].type == L_PAR &&tokens[ed].type == R_PAR){
		return true;
	}
	else return false;
}
static int eval(int bg, int ed){
	if(bg > ed){
		Assert(0, "Bad expression.\n");
		return -1;
	}
	else if(bg == ed){
		if(tokens[bg].type != NUM){
			Assert(0, "Bad expression.\n");
			return -1;
		}
		else{
			int res = atoi(tokens[bg].str);
			return res;
		}
	}
	else if(check_parenthese(bg, ed)){
		return eval(bg + 1, ed + 1);
	}
	else{
		int dominator = -1;
		int dominator_idx = 0;
		int i = bg;
		while(i<=ed){
			if(tokens[i].type == L_PAR){
				int num_par = 1;
				i++;
				while(i <= ed && num_par){
					if(tokens[i].type == L_PAR){
						num_par++;
						i++;
					}
					else if(tokens[i].type == R_PAR){
						num_par--;
						i++;
					}
					else{
						i++;
					}
				}
			}
			else if(tokens[i].type == NUM){
				i++;
			}
			else{
				if(tokens[i].type >= dominator){
					dominator = tokens[i].type;
					dominator_idx = i;
					i++;
				}
			}
		}
		int left = eval(bg, dominator_idx-1);
		int right = eval(dominator_idx+1, ed);
		switch (dominator){
			case PLUS:
				return left + right;
				break;
			case MINUS:
				return left - right;
				break;
			case MULT:
				return left * right;
				break;
			case DIV:
				return left / right;
			case EQ:
				return left == right;
		}
	}
	return 0;
}
static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NUM:
						tokens[nr_token].type = NUM;
						strncpy(tokens[nr_token].str, e + position - substr_len, substr_len);
						nr_token++;
						break;
					case PLUS:
						tokens[nr_token].type = PLUS;
						nr_token++;
						break;
					case NOTYPE:
						break;
					case L_PAR:
						tokens[nr_token].type = L_PAR;
						nr_token++;
						break;
					case R_PAR:
						tokens[nr_token].type = R_PAR;
						nr_token++;
						break;
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	return eval(0 ,nr_token-1);
	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return 0;
}

