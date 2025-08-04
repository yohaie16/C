#include "assembler.h"
#include "../analyze/analyze.h"
#include "../preprocessor/preprocessor.h"
#include "../share/share.h"
#include "../output_unit/output_unit.h"
#include "../trie/trie.h"
#include "../vector/vector.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#define START_ADDRESS 100
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

const char* symbol_type_strings[6] = {
    "external_symbol",
    "entry_symbol",
    "code_symbol",
    "data_symbol",
    "entry_code_symbol",
    "entry_data_symbol"
};

/**
 * @brief building a format to write all warnings
 * 
 * @param file_name - the name of the file we check
 * @param line - what line is the warning comes from
 * @param format - the warning message we want to print
 * @param ... - in case we have %s, %d etc. , we give the virables of them
 */
static void warning_design(const char * file_name, int line, const char * format, ...){
    va_list va1;
    va_start(va1, format);
    printf("%s:%d"YELLOW" waring: "RESET,file_name, line);
    vprintf(format,va1);
    printf("\n");
    va_end(va1);
}

/**
 * @brief building a format to write all errors.
 * 
 * @param file_name - the name of the file we check.
 * @param line - what line is the errors comes from.
 * @param format - the warning message we want to print.
 * @param ... - in case we have %s, %d etc. , we give the virables of them.
 */
static void error_design(const char * file_name, int line, const char * format, ...){
    va_list va1;
    va_start(va1, format);
    printf("%s:%d"RED" error: "RESET,file_name, line);
    vprintf(format,va1);
    printf("\n");
    va_end(va1);
}

/**
 * @brief like it's name- compiling the assembler code there. sending every line to the 
 * analyze and check every label if was it declared as an entry or as an extern. also the function
 * checking for labels if they are legal or not, and printing all errors or warnings if it need too.
 * 
 * @param am_file - the 'am' file we would like to check
 * @param obj - a data structure that holds all the externs and entries and printing what lines we called them.
 * @param am_file_name - the name of the 'am' file.
 * @return int - returns '1' if the line compiled well, and '0' if not.
 */
static int compiler(FILE * am_file, struct object_file * obj, const char * am_file_name){
    char line_buffer[MAX_LABEL_LEN + 1] = {0};
    const char * data_str;
    analyze_ast ast;
    struct missing_symbol m_symbol = {0};
    struct symbol local_sym = {0};
    struct symbol * find_sym;
    void * const * begin;
    void * const * end;
    unsigned int extern_call_address = 0;
    unsigned int machine_word = 0;
    unsigned int * just_inserted_machine_word;
    int i;
    int line_counter = 1;
    int error_code = 1;
    Vector missing_symbol_table = new_vector(ctor_missing_symbol, dtor_missing_symbol);
    while(fgets(line_buffer, sizeof(line_buffer), am_file)){
        ast = lexer_get_info(line_buffer);
        if(ast.syntax_error[0] != '\0'){ /*checking if there is an error in this line, after analyzing.*/
            error_design(am_file_name, line_counter, "%s", ast.syntax_error);
            line_counter++;
            error_code = 0;
            continue;
        }
        if(ast.label_name[0] != '\0'){ /*checking the label at the start of the line.*/
            strcpy(local_sym.name, ast.label_name);
            find_sym = trie_exists(obj->symbol_table_lookup, ast.label_name);
            if(ast.ast_option == ast_inst){
                if(find_sym){
                    if(find_sym->sym_type != symbol_entry){
                        error_design(am_file_name,line_counter, "label %s was already defined as %s in line '%d'.",find_sym->sym_type,symbol_type_strings[find_sym->sym_type], find_sym->address);
                        error_code = 0;
                    }
                    else{
                        find_sym->sym_type = symbol_entry_code;
                        find_sym->address = (unsigned int)vector_get_item_count(obj->code_section) + START_ADDRESS;
                    }
                }
                else{
                    local_sym.sym_type = symbol_code;
                    local_sym.address = (unsigned int)vector_get_item_count(obj->code_section) + START_ADDRESS;
                    local_sym.decleratoin = line_counter;
                    trie_insert(obj->symbol_table_lookup, local_sym.name, vector_insert(obj->symbol_table, &local_sym));
                }
            }
            else{
                if(ast.dir_or_inst.ast_dir.dir_options <= dir_entry)
                    warning_design(am_file_name,line_counter, "label %s was already defined as %s in line '%d'.",ast.dir_or_inst.ast_dir.dir_operand.label_name,symbol_type_strings[1], line_counter - vector_get_item_count(missing_symbol_table));
                else{
                    if(find_sym){
                        if(find_sym->sym_type != symbol_entry){
                            error_design(am_file_name, line_counter,  "label %s was already defined as %s in line '%d'.",find_sym->sym_type,symbol_type_strings[find_sym->sym_type], find_sym->address);
                            error_code = 0;
                        }
                        else{
                            find_sym->sym_type = symbol_entry_data;
                            find_sym->address = (unsigned int)vector_get_item_count(obj->code_section) + START_ADDRESS;
                            find_sym->decleratoin = line_counter;
                        }
                    }
                    else{
                        local_sym.sym_type = symbol_data;
                        local_sym.address = (unsigned int)vector_get_item_count(obj->code_section) + START_ADDRESS;
                        local_sym.decleratoin = line_counter;
                        trie_insert(obj->symbol_table_lookup, local_sym.name, vector_insert(obj->symbol_table, &local_sym));
                    }
                }
            }
        }
        switch(ast.ast_option){ /*checking the rest of the line.*/
            case ast_inst:
            machine_word = ast.dir_or_inst.ast_inst.inst_operand_option[1] << 2;
            machine_word |= ast.dir_or_inst.ast_inst.inst_operand_option[0] << 9;
            machine_word |= ast.dir_or_inst.ast_inst.inst_options << 5;
            vector_insert(obj -> code_section, &machine_word);
            if(!(ast.dir_or_inst.ast_inst.inst_options >= inst_rts)){
                if(ast.dir_or_inst.ast_inst.inst_operand_option[0] == inst_operand_opt_register && ast.dir_or_inst.ast_inst.inst_operand_option[1] == inst_operand_opt_register){
                    machine_word = ast.dir_or_inst.ast_inst.inst_operands[1].register_number << 2;
                    machine_word |= ast.dir_or_inst.ast_inst.inst_operands[0].register_number << 7;
                    vector_insert(obj -> code_section, &machine_word);
                }
                else{
                    for(i = 0; i < 2; i++){
                        switch(ast.dir_or_inst.ast_inst.inst_operand_option[i]){
                            case inst_operand_opt_register:
                            machine_word = ast.dir_or_inst.ast_inst.inst_operands[i].register_number << (7 - (5*i));
                            vector_insert(obj -> code_section, &machine_word);
                            break;
                            
                            case inst_operand_opt_label:
                            find_sym = trie_exists(obj->symbol_table_lookup, ast.dir_or_inst.ast_inst.inst_operands[i].label);
                            if(find_sym && find_sym->sym_type != symbol_entry){
                                machine_word = find_sym->address << 2;
                                if(find_sym->sym_type == symbol_extern){
                                    machine_word |= 1;
                                    extern_call_address = (unsigned int)vector_get_item_count(obj->code_section) + START_ADDRESS;
                                    add_extern(obj->extern_calls, find_sym->name, extern_call_address);
                                }
                                else
                                    machine_word |= 2;
                            }
                            just_inserted_machine_word = vector_insert(obj->code_section, &machine_word);
                            if(!find_sym || (find_sym && find_sym->sym_type == symbol_entry)){
                                
                                strcpy(m_symbol.name, ast.dir_or_inst.ast_inst.inst_operands[i].label);
                                m_symbol.machine_word = just_inserted_machine_word;
                                m_symbol.call_line = line_counter;
                                m_symbol.call_address = (unsigned int)vector_get_item_count(obj->code_section) + START_ADDRESS;
                                vector_insert(missing_symbol_table, &m_symbol);
                            }
                            break;
                            case inst_operand_opt_const_number:
                            machine_word = ast.dir_or_inst.ast_inst.inst_operands[i].const_number;
                            vector_insert(obj -> code_section, &machine_word);
                            break;
                            default:
                            break;
                        }
                    }
                }
            }
            break;
            case ast_dir:
            switch(ast.dir_or_inst.ast_dir.dir_options){
                case dir_data:
                for(i = 0; i < ast.dir_or_inst.ast_dir.dir_operand.data.data_count; i++)
                    vector_insert(obj->data_section, &ast.dir_or_inst.ast_dir.dir_operand.data.data[i]);
                break;
                case dir_string:
                for(data_str = ast.dir_or_inst.ast_dir.dir_operand.string + 1; *(data_str-1); data_str++){
                    machine_word = *data_str;
                    vector_insert(obj->data_section, &machine_word);
                }
                break;
                case dir_entry: case dir_extern:
                find_sym = trie_exists(obj->symbol_table_lookup, ast.dir_or_inst.ast_dir.dir_operand.label_name);
                if(find_sym){
                    if(ast.dir_or_inst.ast_dir.dir_options == dir_entry){ /*checking entry*/
                        if(find_sym->sym_type == symbol_entry || find_sym->sym_type == symbol_entry_code)
                            warning_design(am_file_name, line_counter,  "label %s was already defined as %s in line '%d'.",find_sym->sym_type,symbol_type_strings[find_sym->sym_type], find_sym->address);
                        else if(find_sym->sym_type == symbol_extern){
                            error_design(am_file_name, line_counter,  "label %s was already defined as %s in line '%d'.",find_sym->sym_type,symbol_type_strings[find_sym->sym_type], find_sym->address);
                            error_code = 0;
                        }
                        else
                            find_sym->sym_type = find_sym->sym_type==symbol_code ? symbol_entry_code : symbol_entry_data;
                    }
                }
                else{
                    strcpy(local_sym.name, ast.dir_or_inst.ast_dir.dir_operand.label_name);
                    local_sym.sym_type = ast.dir_or_inst.ast_dir.dir_options;
                    local_sym.address = 0;
                    local_sym.decleratoin = line_counter;
                    trie_insert(obj->symbol_table_lookup, local_sym.name, vector_insert(obj->symbol_table, &local_sym));
                }
                break;
            }
            break;
            default:
            break;
        }
        line_counter++; /*moving to the next line.*/
    }
    VECTOR_FOR_EACH(begin, end, obj->symbol_table){ /*setting the entry-labels into one vector to print it in ent file in the output unit.*/
        if(*begin){
            if(((struct symbol *)(* begin))->sym_type == symbol_entry){
                error_design(am_file_name, line_counter, "label:'%s' was declared in line:%d but was never defined in this file.",((struct symbol *)(* begin))->name, symbol_type_strings[1], ((struct symbol *)(* begin))->decleratoin);
                error_code = 0;
            }
            else if(((struct symbol *)(* begin))->sym_type == symbol_entry_code)
                obj->entry_counter++;
            if(((struct symbol *)(* begin))->sym_type == symbol_data || ((struct symbol *)(* begin))->sym_type == symbol_entry_data)
                ((struct symbol *)(* begin))->address += vector_get_item_count(obj->code_section) + START_ADDRESS;
        }
    }
    VECTOR_FOR_EACH(begin, end, missing_symbol_table){/*setting the entry-labels that first were declered and than defined in the code into one vector to print it in ent file in the output unit.*/
        if(*begin){
            find_sym = trie_exists(obj->symbol_table_lookup, ((struct missing_symbol *)(* begin))->name);
            if(find_sym && find_sym->sym_type == symbol_extern){
                *((struct missing_symbol *)(* begin)) -> machine_word = find_sym->address << 2;
                if(find_sym->sym_type == symbol_extern){
                    *((struct missing_symbol *)(* begin)) -> machine_word |= 1;
                    add_extern(obj->extern_calls, find_sym->name, ((struct missing_symbol *)(* begin))->call_address);
                }
                else
                    *((struct missing_symbol *)(* begin)) ->machine_word |= 2;
            }
            else{
                error_design(am_file_name, line_counter, "missing label:'%s' was refered in line %d but was never defined in this file.", ((struct missing_symbol *)(* begin))->name, ((struct missing_symbol *)(* begin))->call_line);
                error_code = 0;
            }
        }
    }
    return error_code;
}

/*assembler: doing the whole process- opening macros with preprocessor, compiling the code
and at the end printing with the output unit the summary of the as code.*/
/**
 * @brief doing the whole process- opening macros with preprocessor, compiling the code
 * and at the end printing with the output unit the summary of the as code.
 * 
 * @param file_count - how many files do we have to check
 * @param file_names - what is the name of the current file we check.
 * @return int - return 0 when everything done.
 */
int assembler(int file_count, char ** file_names){
    int i;
    const char * am_file_name;
    FILE * am_file;
    struct object_file current_object;
    for(i = 0; i < file_count; i++){
        if(file_names[i] == NULL)
            continue;
        am_file_name = preprocessor(file_names[i]); /*for opening macros and removing commends and empty lines.*/
        if(am_file_name){
            am_file = fopen(am_file_name, "r");
            if(am_file){
                current_object = create_new_object();
                if(compiler(am_file, &current_object, am_file_name))
                    mmn14_output(file_names[i], &current_object, "S"); /*printing the summary of the code.*/
                fclose(am_file);
                destroy_object(&current_object);
            }
            free((void *)am_file_name);
        }
    }
    return 0;
}
