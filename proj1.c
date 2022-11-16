
#include "helper.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


//functions 
string_t *expand(string_t *in_string, user_macro_dict *user_defined_macros, string_t *macro_name_buffer);
//tick should return a state 

state_t tick(state_t *state, string_t *buffer, string_t *buffer_2, string_t *macro_name_buffer, int *buffer_flag, string_t *out_string, char c, int *brace_count, if_args_t *if_args_s, user_macro_dict *user_defined_macros, int *first_brace_flag);
state_t tick_over(state_t *state, string_t *buffer, string_t *buffer_2, string_t *macro_name_buffer, int *buffer_flag, string_t *out_string, string_t *input, int *brace_count, if_args_t *if_args_s, user_macro_dict *user_defined_macros, int *first_brace_flag);
void remove_comments(state_t *state, string_t *out_string, char c);
void remove_comments_file(FILE *f, string_t *out);
void replace_args(string_t *macro_def, string_t *args, string_t *out);

//make new buffers, initilalize buffer flag, first first brace flag, use same udms, make new if args 
string_t *expand(string_t *in_string, user_macro_dict *user_defined_macros, string_t *macro_name_buffer) {
    state_t state = state_plaintext;
    string_t *new_buffer = create_string(2);
    string_t *new_buffer_2 = create_string(2);
    string_t *output = create_string(2);
    if_args_t *if_args_a = create_if_args();
    int new_brace_count = 0;
    int buffer_flag = 0;
    int first_brace_flag = 0;

    state = tick_over(&state, new_buffer, new_buffer_2, macro_name_buffer, &buffer_flag, output, in_string, &new_brace_count, if_args_a, user_defined_macros, &first_brace_flag);

    free(new_buffer);
    free(new_buffer_2);

    if (new_brace_count != 0) {
        fprintf(stderr, "Braces are not balanced in expand. Braces are: %d\n", new_brace_count);
        exit(1);
    }

    return output;


}

void remove_comments(state_t *state, string_t *out_string, char c) {
    //state change
    switch(*state) {
        case (state_plaintext): {
            //switch states

            //if there is a escape
            if (c == '\\') {
                *state = state_escape;
            }
            

            else if (c == '%') {
                *state = state_comment;
            }
            break;
        }

        //escape out of comments
        case (state_escape): {
            *state = state_plaintext;
            break;

        
        case state_comment:
            //if its a new line, remove white-space
            if (c == '\n') {
                *state = state_whitespace;
                break;
            }
            break;
        }
        
        case (state_whitespace): {
            if (c == '%') {
                *state = state_comment;
            }
            //if its not space, stop and go back to plain text 
            else if (c != ' ' && c != '\t') {
                *state = state_plaintext;
            }
            break;
        }

        //dont worry about the other states here 
        default: 
            break;
    }

    
    //once you are out of state change, do things 
    if ((*state != state_comment) && (*state != state_whitespace)) {
        add(out_string, c);
        }
    

}

void remove_comments_file(FILE *f, string_t *out){
    state_t current_state = state_plaintext;
    char ch;
    if (f) {
                // printf("argv: %s\n", argv[i]);
                while ((ch = fgetc(f)) != EOF ){  
                    //is that fine or do i need a curr_state  
                    remove_comments(&current_state, out, ch);
                }   

            } else fprintf(stderr, "failed to open file.\n");
}


void replace_args(string_t *macro_def, string_t *args, string_t *out) {

    int ignore_next = 0;

    for (int i = 0; i < macro_def->length; i++) {    
        // printf("%c\n", buffer_2->str[i]);
        if (macro_def->str[i] == '\\') {
            add(out, macro_def->str[i]);
            ignore_next = 1 - ignore_next;
        }


        else if (macro_def->str[i] == '#') {
            //if the last character was escaped, don't add 
            if (ignore_next == 1) {
                add(out, '#');
            } else {

            
                add_string(out, args);
                //if its just a backslash add another, is this ok or will it fuck shit up???? 
                if (args->length == 1 && args->str[0] == '\\') {
                    add(out, '\\');
                }
            }
            ignore_next = 0;

        }
        else {
            ignore_next = 0;
            add(out, macro_def->str[i]);
        }
    }
    

}

state_t tick(state_t *state, string_t *buffer, string_t *buffer_2, string_t *macro_name_buffer, int *buffer_flag, string_t *out_string, char c, int *brace_count, if_args_t *if_args_s, user_macro_dict *user_defined_macros, int *first_brace_flag) {

    //for wrong spaces
    if (*state != state_plaintext && *brace_count == 0 && c == ' ') {
        fprintf(stderr, "Space in incorrect place.\n");
        exit(1);
    }
    //ignore flag for # and shit
    int ignore_flag = 0;
    // printf("%c", c);

    switch(*state) {
        case (state_plaintext): {
            //if we are expanding a macro
            // printf("buffer at this point are: %s\n", buffer->str);
            //if the args are full, expand 


            //switch states 
        if (c == '\\') {
            *state = state_escape;
                

            }
        break;
        }

        case (state_ignore_char): {
            //ignore one char

            *state = state_plaintext; 
            break;
        }

        case (state_escape): {

            if ((c == '\\') || (c == '}') || (c == '{') || (c == '#') || (c == '%')) {
                *state = state_plaintext;
            }
            
            else if (isalnum(c)) {
                *state = state_reading_macro;
                //add that char to buffer
                add(macro_name_buffer, c);
            }

            else {
                add(out_string, '\\');
                *state = state_plaintext;
            }
            break;
        }
            

        case (state_whitespace): {
            printf("ws"); 
            break;
        }

        case (state_reading_macro): {
            //read the macro name
            

            // printf("Macro name here: %s with current character: %c\n", buffer->str, c);

            if (c == '{') {

                //incrememnt brace count
                *brace_count += 1;
                

                if ((strcmp(macro_name_buffer->str, "if")) == 0) {
                    
                    *state = state_if_conditional;
                }

                else if ((strcmp(macro_name_buffer->str, "def")) == 0) {
                    *state = state_def_macro;
                    *first_brace_flag = 1;
                    //if its a def macro, create the new macro
                    
                }

                else if ((strcmp(macro_name_buffer->str, "undef")) == 0) {
                    *state = state_undef_macro;
                }

                else if ((strcmp(macro_name_buffer->str, "ifdef")) == 0) {
                    *state = state_ifdef_conditional;
                }

                else if ((strcmp(macro_name_buffer->str, "expandafter")) == 0) {
                    
                    *state = state_reading_before;

                }

                else if ((strcmp(macro_name_buffer->str, "include")) == 0) {
                    *state = state_include_macro;
                }

                else {
                    *state = state_user_defined_macro;
                    *first_brace_flag = 1;
                    //load the definition into the second buffer 
                    //make sure buffer_2 is clear
                    // printf("UDM Name: %s\n", buffer->str);
                    add_string(buffer_2, get_macro_definition(user_defined_macros, macro_name_buffer));
                    // printf("macro def: %s", buffer_2->str);
                    

                }
                //do i need this clear buffer, YES and break, so we dont add the brace
                                            
                clear(macro_name_buffer);
                break;
            }
            //add to the buffer here, because we are already starting with the first letter
            add(macro_name_buffer, c);
            // printf("Macro buffer after add: %s", buffer->str);
            break;
        }

        //case for defining macro, handle first brace, there may be errors here

      


        case (state_def_macro): {

            // if (*brace_count == 0) {
            //     if(c != '{') {
            //         fprintf(stderr, "Spaces between def braces\n");
            //         exit(1);
            //     }
            // }

            if (c == '{') {
                
                *brace_count += 1;
                //if its the first brace, ignore it 
                if (*first_brace_flag == 1) {
                    *first_brace_flag = 0;
                    break;
                }
                
            }
            if (c == '}') {                
                *brace_count -=1;
                
                if (*brace_count == 0) {
                    if (*buffer_flag == 0) {
                        //switch to the other buffer
                        *buffer_flag = 1;
                        break;
                    }
                    else if (*buffer_flag == 1) {
                        //add the macro to the dictionary 
                        //ADDD MACRO TO DICT
                        // printf("buffer_2: %s\n", buffer_2->str);
                        add_macro(user_defined_macros, buffer, buffer_2); 
                        clear(buffer);
                        clear(buffer_2);
                        *buffer_flag = 0;
                        *state = state_plaintext;
                        ignore_flag = 1;                                       
                        break;
                        
                    }
                    
                }
                
            }
        

            if (*buffer_flag == 0) {
                add(buffer, c);
            }
            else {
                add(buffer_2, c);
            }
            
            break;
        }
        
        case (state_undef_macro): {

            if (c == '{') {
                *brace_count += 1;
                break;
                
            }
            if (c == '}') {                
                *brace_count -=1;
                if (*brace_count == 0) {
                    delete_macro(user_defined_macros, buffer);
                    clear(buffer);
                    *state = state_plaintext;
                    ignore_flag = 1;
                    break;
                }
            }
            add(buffer, c);
            break;
        }

        //MACRO STATES!

        case (state_UDM_escaped_char): {
            // printf("UDM escape char %c\n", c);
            add(buffer, c);
            *state = state_user_defined_macro;
            break;
        }

        case (state_user_defined_macro): {
            // printf("UDM char is %c\n", c);
            if (c == '\\') {
                // *escape_flag = 1 - 0;
                
                *state = state_UDM_escaped_char;
                break;
            }

            if (c == '{') {
                //do this, so we can add braces to the end  
                if (*brace_count != 0) {
                    add(buffer, c);
                }               
                *brace_count += 1;
                break;
            }
            else if (c == '}') {  

                if (*brace_count != 1) {
                    add(buffer, c);
                }    

                *brace_count -=1;
            
                if (*brace_count == 0) {
                    //if braces are  balanced, expand this thing, copy the buffer to UDM args
                    //make new things 
                    //does buffer have UDM arg?
                    *state = state_plaintext;
                    string_t *new_buffer = create_string(2);
                    string_t *new_buffer_2 = create_string(2);
                    string_t *arg = create_string(2);
                    string_t *replaced_arg = create_string(2);
                    add_string(arg, buffer);
                    replace_args(buffer_2, arg, replaced_arg);
                    // printf("Expanding: %s with Args: %s\n", buffer_2->str, arg->str);
                    // printf("Expanded as: %s\n", replaced_arg->str);
                    // printf("Expanding in UDM\n");

                    *state = tick_over(state, new_buffer, new_buffer_2, macro_name_buffer, buffer_flag, out_string, replaced_arg, brace_count, if_args_s, user_defined_macros, first_brace_flag);
                    //return the current state of that level to this level? And make that levels state this levels state 
                    //glo

                    //ignore this brace 
                    free_string(new_buffer);
                    free_string(new_buffer_2);
                    free_string(arg);
                    free_string(replaced_arg);
                    //ignore this brace 
                    ignore_flag = 1;
                    clear(buffer_2);
                    clear(buffer);
                    // *state = state_plaintext;

                    break;
                }
                break;

            } else {
                add(buffer, c);
                break;
            }
        }
        

            

        case (state_if_conditional): {
            //start by clearing the buffer
            // clear(buffer);
            if (*brace_count == 0) {
                if(c != '{') {
                    fprintf(stderr, "Spaces between braces.\n");
                    exit(1);
                }
            }

            if (c == '}') {
                //create an if args               

                //if there is something in the buffer, do then 
                if (buffer->length > 0) {
                    set_if_args(if_args_s, 1, 0);
                    
                } 
                else {
                    set_if_args(if_args_s, 0, 1);
                    
                }
                //switch states, clear buffer and break!
                if_args_s->full = 1;
                *state = state_reading_then;
                // printf("%d", brace_count);
                *brace_count -= 1;
                clear(buffer);
                break;
                
            }

            add(buffer,c);                                   
            break;
        }

        case (state_ifdef_conditional): {
            

            // if (*brace_count == 0) {
            //     if(c != '{') {
            //         fprintf(stderr, "Spaces between braces\n");
            //         exit(1);
            //     }
            // }

            if (c == '}') {
                //create an if args
                // printf("FINAL COND: %s\n", buffer->str);
                //if there is something in the buffer, do then 
                if (macro_exists(user_defined_macros, buffer) == 1) {
                    
                    
                    set_if_args(if_args_s, 1, 0);
                    
                } 
                else {
                    set_if_args(if_args_s, 0, 1);
                    
                }
                //switch states, clear buffer and break!
                
                if_args_s->full = 1;
                *state = state_reading_then;
                // printf("%d", brace_count);
                *brace_count -= 1;
                clear(buffer);
                break;
                
            } else {
                add(buffer,c);                                   
                break;
            }
        }  

        case (state_reading_then): {

            if (c == '{') {

                if (*brace_count != 0) {
                    add(buffer, c);
                }              
                    *brace_count += 1; 
                    break;      

            } else if (c == '}') {

                if (*brace_count != 1) {
                    add(buffer, c);
                }    
                    *brace_count -= 1;

                    if (*brace_count == 0) {
                        // printf("Then: %s\n", buffer->str);
                        *state = state_reading_else;
                        break;
                    }
            } else {
                add(buffer, c);
                break;
            }
        }

        case (state_reading_else): {

            if (c == '{') {

                if (*brace_count != 0) {
                    add(buffer_2, c);
                } 
                    *brace_count += 1; 
                    break;       

            } else if (c == '}') {

                if (*brace_count != 1) {
                    add(buffer_2, c);
                }  

                *brace_count -= 1;

                if (*brace_count == 0) {
                   
                    string_t *arg = create_string(2);                  
                    string_t *new_buffer = create_string(2);
                    string_t *new_buffer_2 = create_string(2);
                    // printf("Then: %s\n", buffer->str);
                    // printf("Else: %s\n", buffer_2->str);

                    *state = state_plaintext;

                    if (if_args_s->args_arr[0] == 1) {               
                        add_string(arg, buffer);

                    } else if (if_args_s->args_arr[1] == 1) {
                        add_string(arg, buffer_2);
                    }                         

                    *state = tick_over(state, new_buffer, new_buffer_2, macro_name_buffer, buffer_flag, out_string, arg, brace_count, if_args_s, user_defined_macros, first_brace_flag);


                free_string(arg);
                free_string(new_buffer);
                free_string(new_buffer_2);
                clear(buffer);
                clear(buffer_2);
                //ignore this one
                ignore_flag = 1;
                // *state = state_plaintext;
                break;

                }
                        
            } else {
                add(buffer_2, c);
                break;
                }
        }
            

        case (state_reading_before): {
            if (c == '{') {
                if (*brace_count != 0) {
                    add(buffer, c);
                }
                *brace_count += 1;
                break;
            }
            else if (c == '}') {
                //do i need this
                if (*brace_count != 1) {
                    add(buffer, c);
                }
                
                *brace_count -= 1;
                
                if (*brace_count == 0) {
                    *state = state_reading_after;
                }
  
                break;
            }
            else {
                add(buffer, c);
                break;
                }
        }

        case(state_reading_after): {
            if (c == '{') {
                if (*brace_count != 0) {
                    add(buffer_2, c);
                }
                *brace_count += 1;
                break;
            }
            if (c == '}') {


                if (*brace_count != 1) {
                    add(buffer_2, c);
                }
                *brace_count -= 1;


                // printf("buffer 2: %s and *brace count: %d\n", buffer_2->str, *brace_count);

                if (*brace_count == 0) {

                    // printf("buffer2: %s\n", buffer_2->str);
                    // printf("buffer: %s\n", buffer->str);

                    ////EXPAND SHIT HERE, MAYBE WRITE NEW METHOD FOR EXPAND 
                    *state = state_plaintext;
                    string_t *temp_out = create_string(2);
                    string_t *expanded_after = create_string(2);
                    temp_out = expand(buffer_2, user_defined_macros, macro_name_buffer);
                    //append the temp to after, is THIS RIGHT? 
                    add_string(expanded_after, buffer);
                    add_string(expanded_after, temp_out);
                    //clear buffers
                    clear(buffer);
                    clear(buffer_2);

                    // printf("Expanded after: %s\n", expanded_after->str);

                    //do you need to clear buffers
                    *state = tick_over(state, buffer, buffer_2, macro_name_buffer, buffer_flag, out_string, expanded_after, brace_count, if_args_s, user_defined_macros, first_brace_flag);
                    free_string(temp_out);
                    free_string(expanded_after);
                    // printf("OUT: %s", out_string->str);
                }
                ignore_flag = 1;

                break;
            }
            else {
                add(buffer_2, c);
                break;
                }
        }



        case (state_include_macro): {
            //dont expand  include "path" 

            if (c == '{') {
                *brace_count += 1;
            }
            if (c == '}') {
                *brace_count -= 1;

                if (*brace_count == 0) {
                

                // printf("This is the include string: %s\n", buffer->str);

                FILE *f = fopen(buffer->str, "r");
                if (f == NULL) {
                    fprintf(stderr, "Incorrect file.\n");
                    exit(1);
                }
                // printf("file contents: %s\n", buffer->str);
                
                string_t *file_out = create_string(2);
                *state = state_plaintext;

                // printf("got here");
                clear(buffer);
                clear(buffer_2);
                remove_comments_file(f, file_out); 

                // printf("%s", file_out->str);
                tick_over(state, buffer, buffer_2, macro_name_buffer, buffer_flag, out_string, file_out, brace_count, if_args_s, user_defined_macros, first_brace_flag);

                fclose(f);
                free_string(file_out);
                ignore_flag = 1;
                break;

                }
            }

            add(buffer, c);
            break;
                     
        }
        
        default: {
            *state = state_plaintext;
            break;
        }
            
    }

    //add things to out 
    if (*state == state_plaintext) {
        //if its not an arg?? 
        if (ignore_flag == 1) {
            ignore_flag = 0;
        } else {

            add(out_string, c);          
            
        }

    }

    return *state;
}

state_t tick_over(state_t *state, string_t *buffer, string_t *buffer_2, string_t *macro_name_buffer, int *buffer_flag, string_t *out_string, string_t *input, int *brace_count, if_args_t *if_args_s, user_macro_dict *user_defined_macros, int *first_brace_flag) {
    state_t this_state = *state;
    for (int i = 0; i < input->length; i++) {
        this_state = tick(state, buffer, buffer_2, macro_name_buffer, buffer_flag, out_string, input->str[i], brace_count, if_args_s, user_defined_macros, first_brace_flag);
    }

    return this_state;
}


int main(int argc, char *argv[]) {
    // set up file and name
    FILE *fp;
    //character being read
    
    //create a buffer and output
    string_t *buffer = create_string(2);
    //name of macro, buffer needs to be shared between recursive calls 

    string_t *macro_name_buffer = create_string(2);
    string_t *buffer2 = create_string(2);
    string_t *uncommented = create_string(2);
    string_t *out = create_string(2);
    int brace_counter = 0;
    int buffer_flag = 0;
    int first_brace_flag = 0;
    //original state 
    // state_t *curr_state = malloc (sizeof(state_t));
    state_t curr_state = state_plaintext;
    if_args_t *if_args_a = create_if_args();
    //character count 
    //make macro dictionary
    user_macro_dict *all_user_macros = create_macro_dict(2);

    
        
    //create a buffer and output

    
    // printf("Argc: %d\n", argc);

    //if there is no file, its stdin
    if (argc < 2){
        fp = stdin;
        remove_comments_file(fp, uncommented);
        
    }
    //if there is a file to be read 
    else {

        for(int i = 1; i < argc; i++) {
            fp = fopen(argv[i], "r");
            remove_comments_file(fp, uncommented);
            fclose(fp);

        }
    }
      


    //if the file is opened, print its contents
        
        for (int i = 0; i < uncommented->length; i++) {

            tick(&curr_state, buffer, buffer2, macro_name_buffer, &buffer_flag, out, uncommented->str[i], &brace_counter, if_args_a, all_user_macros, &first_brace_flag);


        }

        //ERROR CHECKING 
        //cjeck if braces are balanced
        if (brace_counter != 0) {
            fprintf(stderr, "braces are not balanced\n");
            exit(1);
        }

        //we want to exit in plaintext state
        if (curr_state != state_plaintext) {
            fprintf(stderr, "exited in wrong state\n");
            exit(1);
        }
  

        printf("%s", out->str);

        //process everything else 
        
        // printf("%s", all_user_macros->user_macros[1]->definition->str);

        //close the file
        //print the result 
        // printf("%s", out->str);
        //close and free everything
        
        free_string(out);
        free_string(uncommented);
        free_string(macro_name_buffer);
        //free everything
        free_string(buffer);
        free_string(buffer2);
        free_macro_dict(all_user_macros);
        free_if_args(if_args_a);
    
  

    return 0;
    
}
