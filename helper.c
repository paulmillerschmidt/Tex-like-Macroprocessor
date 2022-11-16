#include "proj1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//STRING TYPE:

typedef struct {
        char *str;
        size_t capacity;
        size_t length;
    } string_t;

string_t *create_string(size_t capacity) {
    string_t* result = malloc(sizeof(string_t));
    result->str = (char*) calloc(capacity, sizeof(char));
    result->length = 0;
    result->capacity = capacity;
    return result;
}

void add(string_t *s, char c) {
    //if theres space add it 
    if (s->length + 1 >= s->capacity) {
        s->str = DOUBLE(s->str, s->capacity);
    }
    s->str[s->length++] = c;
    s->str[s->length] = '\0';
}

void add_string(string_t *s, string_t *t) {
    int i = 0;
    while (i < t->length) {        
        add(s, t->str[i]);
        i++;    
    }
}

void clear(string_t *s) {
    memset(s->str,0,s->capacity);
    s->length = 0;
    s->str[0] = '\0';
}

//method for freeing the buffer
void free_string(string_t *b) {
    free(b->str);
    free(b);
}


//MACRO TYPE 

//make macro data type 
typedef struct user_macro_t {
    string_t *name;
    string_t *definition;
    size_t pos;
} user_macro_t;

user_macro_t create_user_macro() {
    user_macro_t result;
    result.name = create_string(2);
    result.definition = create_string(2);
    result.pos = 0;
    return result;
}

void set_user_macro(user_macro_t user_macro, string_t *name, string_t *definition) {
    add_string(user_macro.name, name);
    add_string(user_macro.definition, definition);


}
size_t get_macro_pos(user_macro_t user_macro) {
    return user_macro.pos;
}
//maybe I dont need this???
// void set_user_macro_name(user_macro_t *user_macro, string_t *name, string_t *args) {
//     add_string(user_macro->name, name->str);
//     user_macro->args = strdup(args);
// }

//MACRO DICT TYPE 
//store macros in a dictionary like thing 
typedef struct user_macro_dict {
    size_t len;
    size_t capacity;
    user_macro_t *user_macros;
} user_macro_dict;

//make makcro dict
user_macro_dict *create_macro_dict(size_t size) {
    user_macro_dict * result = (user_macro_dict *) malloc(sizeof(user_macro_dict));
    result->len = 0;
    result->capacity = size;
    result->user_macros = (user_macro_t *) malloc(sizeof(user_macro_t) * size);
    return result;
}

void print_char_array(string_t *s) {
    for (int i = 0; i < s->length; i++) {
        printf("%c ", s->str[i]);
    }
    printf("\n");
    printf("Length: %zu\n", s->length);
}

bool str_equals(string_t *a, string_t *b) {
    if (a->length != b->length) {
        return false;
    }

    for (int i = 0; i < a->length; i++) {
        if (a->str[i] != b->str[i]) return false;
    }

    return true;
}

void add_macro(user_macro_dict *dict, string_t *name, string_t *definition) {
    //do some stuff to check if its in the array already
    //ERROR CHECKING IS NOT WORKING 
    for (int i = 0; i < dict->len; i++) {
        
        if (str_equals(dict->user_macros[i].name, name)) {
            fprintf(stderr, "ERROR: name is already defined\n");
            exit(0);
        } 
    }
    // user_macro_t result;
    // result.name = name;
    // result.definition = definition;
    
    user_macro_t result = create_user_macro();

    //make space if needed
    if (dict->len + 1 > dict->capacity) {
        dict->user_macros = (user_macro_t *) DOUBLE(dict->user_macros, dict->capacity);
    
    }
    
    set_user_macro(result, name, definition);
    // add_string(result->name, name);
    // add_string(result->definition, definition);
    dict->user_macros[dict->len++] = result;

}

int macro_exists(user_macro_dict *dict, string_t *name) {
    for (int i = 0; i < dict->len; i++) {        
        if (str_equals(dict->user_macros[i].name, name)) {
            return 1;
            }
    }
    return 0;
}

void delete_macro(user_macro_dict *dict, string_t *name) {
    for (int i = 0; i < dict->len; i++) {        
        if (str_equals(dict->user_macros[i].name, name)) {
            //get rid of the macro 
            clear(dict->user_macros[i].name);
            clear(dict->user_macros[i].definition);
        } 
    }

}

void clear_macro(user_macro_t *macro) {
    clear(macro->name);
    clear(macro->definition);
    macro->pos = 0;
}

void free_macro(user_macro_t m){
    free_string(m.definition);
    free_string(m.name);

}
    
void free_macro_dict(user_macro_dict *dict) {
    //idk if i need these ampersands
    if (dict == NULL || dict->user_macros == NULL) {
        return;
    }

    for (int i = dict->len - 1; i >= 0; i--) {
        free_macro(dict->user_macros[i]);
    }
    free(dict->user_macros);
    free(dict);

}
string_t *get_macro_definition(user_macro_dict *dict, string_t *macro_name) {
    for (int i = 0; i < dict->len; i++) {
        
        if (str_equals(dict->user_macros[i].name, macro_name)) {
            return dict->user_macros[i].definition;
        } 
    }
    //throw error, macro not found 
    fprintf(stderr, "ERROR: Macro %s not found.\n", macro_name->str);
    exit(1);
    
}
//NEXT STEP: GET AMACRO DEFINITION
// string_t *get_macro_definition(string_t macro_name) {
//     string_t *result
// }





// char* get_macro_def(macro_t macro, macro_dict *dict) {
//     int i;
//     for (i = 0; i < dict->len; i++) {
//         if (strcmp(macro[0].macro))
//     }
// }

//IF ARGS TYPE
typedef struct {
    int full;
    int *args_arr;
} if_args_t;

if_args_t *create_if_args() {
    if_args_t * result = malloc(sizeof(if_args_t));
    result->args_arr = (int *) calloc (2, sizeof(int));
    result->full = 0;
    return result;
    
}

void set_if_args(if_args_t *if_args, int first, int second) {
    if_args->args_arr[0] = first;
    if_args->args_arr[1] = second;
}

void free_if_args(if_args_t *if_args) {
    free(if_args->args_arr);
    free(if_args);
}

//STATES
//different states 
typedef enum { 
        state_plaintext, 
        state_ignore_char, 
        state_escape, 
        state_comment,
        state_whitespace, 
        state_handle_UDM_brace,
        state_def_escaped_char,
        state_UDM_escaped_char, 
        state_handle_def_macro_brace,
        state_reading_macro, 
        state_defining_macro, 
        state_calling_macro, 
        state_if_conditional, 
        state_handle_if_args,
        state_reading_else,
        state_reading_then,
        state_def_macro,
        state_undef_macro,
        state_ifdef_conditional, 
        state_expandafter_macro,
        state_include_macro,
        state_expand_macro, 
        state_user_defined_macro, 
        state_reading_before, 
        state_reading_after

    } state_t;

    // state_t;

