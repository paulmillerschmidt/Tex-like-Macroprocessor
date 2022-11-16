

//POTENTIAL # replacer 

if (c == '#') {
                    //if there is a arg for udm
                    // printf("THESE ARE THE UDM ARGS %s\n", UDM_arg->str);
                    if (UDM_arg->length == 0) {
                        add(out_string, c);
                    }
                    else {
                        // printf("THESE ARE THE UDM ARGS %s\n", UDM_arg->str);
                        // add_string(out_string, UDM_arg);
                        //create 
                        string_t *temp = create_string(2);
                        add_string(temp, UDM_arg); 

                        for (int i = 0; i < temp->length; i++) {
                        // //tick through this shit bruv  
                           tick(state, buffer, buffer_2, temp, buffer_flag, out_string, temp->str[i], brace_count, if_args_s, user_defined_macros, first_brace_flag);
                        // // printf("%c", buffer_2->str[i]);
                        }   
                        free_string(temp); 

                        
                    }
                    //might not need this
                    ignore_flag = 1;

                    //ignore the last brace of a macro
                    
                    // *state = state_ignore_char;
                    break;
                }


//POTENTIAL EXPAND FUNCTION 


// void expand(string_t *in_string, string_t *out_string, string_t *UDM_argument, int *brace_count, if_args_t *if_args_s, user_macro_dict *user_defined_macros, int *first_brace_flag) {
                    

//                     state_t current_state = state_plaintext;
//                     string_t *new_buffer = create_string(2);
//                     string_t *new_buffer_2 = create_string(2);
//                     int buffer_flag = 0;

//                     //make the hashtag flag one so it looks at it like a macro 
//                     // printf("Ticking through %s with arg %s\n", buffer_2->str, arg->str);
                    
//                     // printf("Arguments: %s\n Length: %ld", buffer_2->str, buffer_2->length);

//                     for (int i = 0; i < in_string->length; i++) {    
//                         printf("%c\n", in_string->str[i]);

//                         tick(&current_state, new_buffer, new_buffer_2, UDM_argument, &buffer_flag, out_string, in_string->str[i], brace_count, if_args_s, user_defined_macros, first_brace_flag);

//                     }

//                     //ignore this brace 
//                     free_string(new_buffer);
//                     free_string(new_buffer_2);
// }
//make an function that replaces the # with the args 