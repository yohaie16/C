#include "analyze.h"
#include "../trie/trie.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define SKIP_SPACE(s) while(*s && isspace(*s)) s++
#define MAX_REGISTER 7
#define MIN_REGISTER 0
#define MAX_CONSET_NUMBER 511
#define MIN_CONST_NUMBER -512
#define SPACE_CHARS "\t\n\f\r\v "

static int is_trie_inited = 0;
Trie instruction_lookup = NULL;
Trie directive_lookup = NULL;

/*I- imidiate, L - label (direct addressing), R - register */
static struct ast_inst_building{
    const char * inst_name;
    int key;
    const char * src;
    const char * dest;
}ast_inst_building[16]= {
    {"mov", inst_mov, "ILR", "LR"},
    {"cmp", inst_cmp, "ILR", "ILR"},
    {"add", inst_add, "LR", "ILR"},
    {"sub", inst_sub, "LR", "ILR"},
    {"lea", inst_lea, "L", "LR"},

    {"not", inst_not, NULL, "LR"},
    {"clr", inst_clr, NULL, "LR"},
    {"inc", inst_inc, NULL, "LR"},
    {"dec", inst_dec, NULL, "LR"},
    {"jmp", inst_jmp, NULL, "LR"},
    {"bne", inst_bne, NULL, "LR"},
    {"red", inst_red, NULL, "LR"},
    {"prn", inst_prn, NULL, "ILR"},
    {"jst", inst_jsr, NULL, "LR"},

    {"rts", inst_rts, NULL, NULL},
    {"stop", inst_stop, NULL, NULL}
};

static struct ast_dir_building{
    const char * dir_name;
    int key;
}ast_dir_building[4] = {
    {".extern", dir_extern},
    {".entry", dir_entry},
    {".string", dir_string},
    {".data", dir_data}
};

enum label_analyze{
    label_ok,
    starts_without_alpha,
    contains_none_alpha_numeric,
    label_too_long
};

/**
 * @brief This function filling the trie with all of our instructions and directives.
 * 
 */
static void trie_initial(){
    int i;
    instruction_lookup = trie();
    directive_lookup = trie();
    for(i = 0; i < 16; i++)
        trie_insert(instruction_lookup,ast_inst_building[i].inst_name, &ast_inst_building[i]);
    for(i = 0; i < 4; i++)
        trie_insert(directive_lookup, ast_dir_building[i].dir_name,&ast_dir_building[i]);
    is_trie_inited = 1;
}

/**
 * @brief gets label and returns its status- is ok or not, and what the promblem is.
 * 
 * @param label the label from the start of the line or as an operand
 * @return enum label_analyze - if the label is okay or not, and if not so why, by enum.
 */
enum label_analyze label_analyzing(const char * label){
    int char_count = 0;
    if(!isalpha(*label))
        return starts_without_alpha;
    label++;
    char_count++;  
    while(*label && isalnum(*label)){
        char_count++;
        label++;
    }
    if(*label != '\0')
        return contains_none_alpha_numeric;
    if(char_count > MAX_LABEL_LEN)
        return label_too_long;
    return label_ok;
}

/*parse_number: get a number as string and analyzing it if it's a number or not good number (overflow)
for example.*/
/**
 * @brief get a number as string and analyzing it if it's a number or not good number (overflow)
          for example.
 * 
 * @param num_str - the number we would like to check as a string
 * @param num - holding the number as a long intiger
 * @param min - the minimum limit of the number
 * @param max  - the maximum kimit of the number
 * @return int 
 */
static int parse_number(const char* num_str, long num, long min, long max){
    char * my_end;
    num = strtol(num_str,(&my_end),10);
    errno = 0;
    while(isspace(*my_end))
        my_end++;
    if(*my_end)
        return -1; /*if after the token ',' there's nothing*/
    if(errno == ERANGE)
        return -2; /*it's not a number*/
    if (num > max || num < min)
        return -3; /*Out of range*/
    return 0; /*Everything alright*/
}

/*dictionary:
I -> number
R -> register
L -> label
N -> unknown
E -> empty
F -> overflow
*/
/**
 * @brief analyzing the operand- is it register, label, number or something unfamiliar, which is
a problem and raise an error.
 * 
 * @param operand_string - the line we're about to analyze it's operands
 * @param label - the label operand we want to check if correct
 * @param const_number - the const number we want to check if it's in range -512 - 511
 * @param register_number - the register number we want to check if it's in range 0-7
 * @return char be the dictionary below
 */
static char parse_operand(char * operand_string, char ** label, int * const_number, int * register_number){
    long num = 0;
    char *temp;
    char *temp2;
    int num_rec;
    char * my_end;
    SKIP_SPACE(operand_string);
    if(*operand_string == '\0') /*Available for 'stop' and 'rts'.*/
        return 'E';
    if(*operand_string == '@'){ /*checking registers*/
        if(*(operand_string + 1) == 'r'){
            if(*(operand_string + 2) == '+' || *(operand_string + 2) == '-')
                return 'N';
            if(parse_number((operand_string+2), num, MIN_REGISTER, MAX_REGISTER) != 0)
                return 'N'; /*It returns '0' when it's okay*/
            if(register_number)
                *register_number = (int)(*(operand_string + 2) - '0');
            if(*(operand_string+3) == ' ' || *(operand_string+3) == '\0')
                return 'R';
        }
        return 'N';
    }
    if(isalpha(*operand_string)){/*checking label*/
        temp2 = temp = strpbrk(operand_string,SPACE_CHARS);
        if(temp){
            *temp = '\0';
            temp++;
            SKIP_SPACE(temp);
            if(*temp != '\0'){
                *temp2 = ' ';
                return 'N';
            }
        }
        if(label_analyzing(operand_string) != label_ok) 
            return 'N';
        if(label)
            (*label) = operand_string;
        return 'L';
    }
    /*checking const numbers*/
    if((num_rec = parse_number(operand_string, num, MIN_CONST_NUMBER,MAX_CONSET_NUMBER)) < -2)
        return 'F';
    else if(num_rec == 0){
        if(const_number)
            (*const_number) = strtol(operand_string, &my_end, 10);
        return 'I';
    }
    return 'N';
}

/**
 * @brief checking all the instructions (mov, rts, etc...) and analyze what operands can they take and if their operands are correct or not.
 * 
 * @param ast - holding the data of the line
 * @param operand_string - holding the line we want to check it's instruction.
 * @param aib - holding the all types of instructions we can have.
 */
static void prase_instruction(analyze_ast * ast, char * operand_string, struct ast_inst_building * aib){
    char operand_option;
    char * check;
    char * sep = NULL;
    if(operand_string)
        sep = strchr(operand_string,',');
    else{
        if(aib -> dest == NULL && aib -> key < 14){
            sprintf(ast->syntax_error,"instruction: '%s' expects one operand.",aib->inst_name);
            return;
        }
        return;
    }
    if(sep){
        check = strchr(sep+1,',');
        if(check){
            sprintf(ast->syntax_error, "found two or more seperatos ',' tokens.");
            return;
        }
        *sep = '\0';
        operand_option = parse_operand(operand_string, &ast->dir_or_inst.ast_inst.inst_operands[0].label,
                                        &ast->dir_or_inst.ast_inst.inst_operands[0].const_number,
                                        &ast->dir_or_inst.ast_inst.inst_operands[0].register_number);
        switch(operand_option){
            case 'N':
                sprintf(ast->syntax_error,"unknown operand: '%s' for source.",aib->inst_name);
                return;
            break;
            case 'F':
                sprintf(ast->syntax_error,"overflow immidiate operand: '%s' for source.", operand_string);
                return;
            break;
            case 'E':
                sprintf(ast->syntax_error,"got no operand text for source.");
                return;
            break;
            default:
            break;
        }
        if(aib->src != NULL){
            if(strchr(aib->src,operand_option) == NULL){
                sprintf(ast->syntax_error,"instruction: '%s': source operand '%s' is not supported.", aib->inst_name, operand_string);
                return;
            }
        }
        else{
            if(operand_option != 'E'){
                sprintf(ast->syntax_error,"instruction '%s': cannot recieve source.", aib->inst_name);
                return;
            }
        }
        ast->dir_or_inst.ast_inst.inst_operand_option[0] = operand_option == 'I' ? inst_operand_opt_const_number : operand_option == 'R' ? inst_operand_opt_register : inst_operand_opt_label;
        operand_string = sep + 1;
        operand_option = parse_operand(operand_string, &ast->dir_or_inst.ast_inst.inst_operands[1].label,
                                       &ast->dir_or_inst.ast_inst.inst_operands[1].const_number,
                                       &ast->dir_or_inst.ast_inst.inst_operands[1].register_number);
        switch(operand_option){
            case 'N':
                sprintf(ast->syntax_error,"unknown operand: '%s' for destination.",aib->inst_name);
                return;
            break;
            case 'F':
                sprintf(ast->syntax_error,"overflow immidiate operand: '%s' for destination.", operand_string);
                return;
            break;
            case 'E':
                sprintf(ast->syntax_error,"got no operand text for destination.");
                return;
            break;
            default:
            break;
        }
        if(strchr(aib->dest,operand_option) == NULL){
            sprintf(ast->syntax_error,"instruction: '%s': destination operand '%s' is not supported.", aib->inst_name, operand_string);
            return;
        }
        ast->dir_or_inst.ast_inst.inst_operand_option[1] = operand_option == 'I' ? inst_operand_opt_const_number : operand_option == 'R' ? inst_operand_opt_register : inst_operand_opt_label;
    }
    else{ /*No seperator*/
        if(aib -> src != NULL){
            sprintf(ast->syntax_error,"instruction '%s' expects seperator token ','.",aib->inst_name);
            return;
        }
        operand_option = parse_operand(operand_string, &ast->dir_or_inst.ast_inst.inst_operands[1].label,
                                       &ast->dir_or_inst.ast_inst.inst_operands[1].const_number,
                                       &ast->dir_or_inst.ast_inst.inst_operands[1].register_number);
        if(operand_option != 'E' && aib->dest == NULL){
            sprintf(ast->syntax_error,"instruction: '%s' expects no operands.",aib->inst_name);
            return;
        }
        switch(operand_option){
            case 'E':
                sprintf(ast->syntax_error,"instruction: '%s' expects operands.", aib->inst_name);
                return;
            break;
            case 'F':
                sprintf(ast->syntax_error,"overflow immidiate operand: '%s' for destination.",operand_string);
                return;
            break;
            case 'N':
                sprintf(ast->syntax_error,"unknown operand: '%s' for destination.",operand_string);
                return;
            break;
        }
        if(strchr(aib->dest,operand_option) == NULL){
            sprintf(ast->syntax_error, "instruction: '%s': destination operand: '%s' is not supported.", aib->inst_name, operand_string);
            return;
        }
        ast->dir_or_inst.ast_inst.inst_operand_option[1] = operand_option == 'I' ? inst_operand_opt_const_number : operand_option == 'R' ? inst_operand_opt_register : inst_operand_opt_label;
    }
}

/**
 * @brief checking every directive (entry, extrn, data and string) ade look if their operand (or operands) is something they can take. if not- we raise an error.
 * 
 * @param ast - holding the data of the line
 * @param operand_string - holding the line we want to check it's instruction.
 * @param adb - holding the all types of instructions we can have.
 */
static void parse_directive(analyze_ast * ast, char * operand_string, struct ast_dir_building * adb){
    char * open_string;
    char * close_string;
    char * seperator;
    int current_number;
    int count = 0; /*counting how many numbers I have in data, also works as an index.*/
    /*checking .entry and .extern*/
    if(adb->key <= dir_entry){
        /*If entry/extern not have a label with them:*/
        if(parse_operand(operand_string, &ast->dir_or_inst.ast_dir.dir_operand.label_name,NULL,NULL) != 'L'){
            sprintf(ast->syntax_error, "directive: '%s' with invalid operand: '%s'.",adb->dir_name,operand_string);
            return;
        }
    }
    /*checking .string*/
    else if(adb->key == dir_string){
        open_string = strchr(operand_string,'"');
        if(!open_string){ /*No " at the start of the string.*/
            sprintf(ast -> syntax_error, "directive: '%s' has no opening '\"': %s.",adb -> dir_name, operand_string);
            return;
        }
        open_string++;
        close_string = strrchr(open_string,'"');
        if(!close_string){/*No " at the end of the string.*/
            sprintf(ast -> syntax_error, "directive: '%s' has no closing '\"': %s.",adb -> dir_name, operand_string);
            return;
        }
        *close_string = '\0';
        close_string++;
        SKIP_SPACE(close_string);
        if(*close_string != '\0'){ /*Has string also after the closing ".*/
            sprintf(ast -> syntax_error, "directive: '%s' has extra string after the string: '%s'.", adb->dir_name, close_string);
            return;
        }
        ast->dir_or_inst.ast_dir.dir_operand.string = operand_string;
    }
    /*checking .data*/
    else if(adb->key == dir_data){
        do{
            seperator = strchr(operand_string,',');
            if(seperator){
                *seperator = '\0';
            }
            switch(parse_operand(operand_string,NULL,&current_number,NULL)){
                case 'I':
                    ast->dir_or_inst.ast_dir.dir_operand.data.data[count] = current_number;
                    count++;
                    ast->dir_or_inst.ast_dir.dir_operand.data.data_count = count;
                break;
                case 'F':
                    sprintf(ast->syntax_error,"directive: '%s': got overflowed number: '%s'.",adb->dir_name,operand_string);
                    return;
                break;
                case 'E':
                    sprintf(ast->syntax_error,"directive: '%s': got empty string, and expected to get immidiate number.",adb->dir_name);
                    return;
                break;
                default:
                    sprintf(ast->syntax_error,"directive: '%s': got none-number string: '%s'.",adb->dir_name,operand_string);
                    return;
                break;
            }
            if(seperator)
                operand_string = seperator + 1;
            else
                break;
        }while(1);
    }
}

/**
 * @brief the main function the analyzing the logical line from the as text and returns us all information about it- diractive or instruction, what instruction or what directive is it, and there is an error- whar error does it have.
 * 
 * @param logical_line - the line from 'am' file that we would like to analyze and get its data.
 * @return analyze_ast - all the data of the line in one variable
 */
analyze_ast lexer_get_info(char * logical_line){
    analyze_ast ast = {0};
    enum label_analyze label_check;
    struct ast_inst_building * inst_b = NULL;
    struct ast_dir_building * dir_b = NULL;
    char *check1, *check2;
    char *backup = malloc(MAX_NUMBER_DATA);
    memcpy(backup,logical_line,MAX_NUMBER_DATA);
    if(!is_trie_inited)
        trie_initial();
    backup[strcspn(backup,"\n\r")] = 0;
    SKIP_SPACE(backup);
    check1 = strchr(backup, ':'); /*appears when we have label*/
    if(check1 != NULL){
        check2 = strchr(check1+1, ':');
        if(check2){
            sprintf(ast.syntax_error,"Token ':' aprears twice in this line.");
            return ast;
        }
        (*check1) = '\0';
        /*checking the appeared label*/
        switch (label_check = label_analyzing(backup)){
            case starts_without_alpha:
                sprintf(ast.syntax_error,"label '%s' starts without alphabetic character.", backup);
            break;
            case contains_none_alpha_numeric:
                sprintf(ast.syntax_error, "label '%s' contains none-alphanumeric characters.",backup);
            break;
            case label_too_long:
                sprintf(ast.syntax_error, "label '%s' is longer than maximum length - '%d'.",backup, MAX_LABEL_LEN);
            break;
            case label_ok:
                strcpy(ast.label_name,backup);
            break;
        }
        if(label_check != label_ok)
            return ast;
        backup = check1 + 1;
        SKIP_SPACE(backup);
    }
    if(*backup == '\0' && *ast.label_name != '\0')
        sprintf(ast.syntax_error, "Line contains only label:'%s'.", ast.label_name);
    if(check1){ 
        *check1 = '\0';
        check1++;
        SKIP_SPACE(check1);
    }
    check1 = strpbrk(backup,SPACE_CHARS);
    if(check1){
        *check1 = '\0';
        check1++;
        SKIP_SPACE(check1);
    }
    if(*backup == '.'){
        dir_b = trie_exists(directive_lookup, backup);
        if(!dir_b){
            sprintf(ast.syntax_error, "Unknown directive: '%s'.", backup);
            return ast;
        }
        ast.ast_option = ast_dir;
        ast.dir_or_inst.ast_dir.dir_options = dir_b->key;
        parse_directive(&ast,check1,dir_b);
    }
    else{
        inst_b = trie_exists(instruction_lookup,backup);
        if(!inst_b){
            sprintf(ast.syntax_error,"unknown instruction: '%s'.",backup);
            return ast;
        }
        ast.ast_option = ast_inst;
        ast.dir_or_inst.ast_inst.inst_options = inst_b->key;
        prase_instruction(&ast, check1, inst_b);
    }
    return ast;
}

/**
 * @brief destroy used memory
 * 
 */
void deallocate_mem(){
    is_trie_inited = 0;
    trie_destroy(&directive_lookup);
    trie_destroy(&instruction_lookup);
}
