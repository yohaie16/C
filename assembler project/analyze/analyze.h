#ifndef __ANALYZE_H_
#define __ANALYZE_H_

#define MAX_NUMBER_DATA 81
#define MAX_LABEL_LEN 31

/*this struct desribing all the possible shapes of any line in assembly.*/
struct analyze_ast{
    char label_name[MAX_LABEL_LEN];
    char syntax_error[256];
    enum {
        ast_dir,
        ast_inst
    }ast_option;
    union{
        struct{
            enum{
                dir_extern,
                dir_entry,
                dir_string,
                dir_data
            }dir_options;
            union{
                char * label_name; /*entry and extern*/
                char * string; /*string*/
                struct {
                    int data[MAX_NUMBER_DATA];
                    int data_count;
                }data; /*data*/
            }dir_operand;
        }ast_dir;
        struct{
            enum{
                /*Two operands*/
                inst_mov,
                inst_cmp,
                inst_add,
                inst_sub,
                inst_lea,

                /*One operand*/
                inst_not,
                inst_clr,
                inst_inc,
                inst_dec,
                inst_jmp,
                inst_bne,
                inst_red,
                inst_prn,
                inst_jsr,

                /*No operand*/
                inst_rts,
                inst_stop
            }inst_options;
            enum {
                inst_operand_opt_none = 0,
                inst_operand_opt_const_number = 1,
                inst_operand_opt_label = 3,
                inst_operand_opt_register = 5
            }inst_operand_option[2];
            union {
                int const_number;
                int register_number;
                char * label;
            }inst_operands[2];
        }ast_inst;
    }dir_or_inst;
};

typedef struct analyze_ast analyze_ast;

void deallocate_mem();

analyze_ast lexer_get_info(char * logical_line);

#endif

