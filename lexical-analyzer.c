#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Enum for lexer states
typedef enum {
    START, BUILD_ID, BUILD_NUM, IDENTIFIER, NUMBER, STOP
} State;

// Token information structure
typedef struct {
    const char *lexeme;
    const char *token;
} TokenInfo;

#define NUM_KEYWORDS (sizeof(keywords) / sizeof(TokenInfo))
#define NUM_OPERATORS (sizeof(operators) / sizeof(TokenInfo))

TokenInfo keywords[] = {
    {"auto", "KEYWORD"},
    {"break", "KEYWORD"},
    {"case", "KEYWORD"},
    {"char", "KEYWORD"},
    {"const", "KEYWORD"},
    {"continue", "KEYWORD"},
    {"default", "KEYWORD"},
    {"do", "KEYWORD"},
    {"double", "KEYWORD"},
    {"else", "KEYWORD"},
    {"enum", "KEYWORD"},
    {"extern", "KEYWORD"},
    {"float", "KEYWORD"},
    {"for", "KEYWORD"},
    {"goto", "KEYWORD"},
    {"if", "KEYWORD"},
    {"int", "KEYWORD"},
    {"long", "KEYWORD"},
    {"register", "KEYWORD"},
    {"return", "KEYWORD"},
    {"short", "KEYWORD"},
    {"signed", "KEYWORD"},
    {"sizeof", "KEYWORD"},
    {"static", "KEYWORD"},
    {"struct", "KEYWORD"},
    {"switch", "KEYWORD"},
    {"typedef", "KEYWORD"},
    {"union", "KEYWORD"},
    {"unsigned", "KEYWORD"},
    {"void", "KEYWORD"},
    {"volatile", "KEYWORD"},
    {"while", "KEYWORD"}
};

TokenInfo operators[] = {
    {"+", "OPERATOR"},
    {"-", "OPERATOR"},
    {"*", "OPERATOR"},
    {"/", "OPERATOR"},
    {"%", "OPERATOR"},
    {"++", "OPERATOR"},
    {"--", "OPERATOR"},
    {"==", "OPERATOR"},
    {"!=", "OPERATOR"},
    {"<", "OPERATOR"},
    {">", "OPERATOR"},
    {"<=", "OPERATOR"},
    {">=", "OPERATOR"},
    {"&&", "OPERATOR"},
    {"||", "OPERATOR"},
    {"!", "OPERATOR"},
    {"&", "OPERATOR"},
    {"|", "OPERATOR"},
    {"^", "OPERATOR"},
    {"~", "OPERATOR"},
    {"=", "OPERATOR"},
    {"+=", "OPERATOR"},
    {"-=", "OPERATOR"},
    {"*=", "OPERATOR"},
    {"/=", "OPERATOR"},
    {"%=", "OPERATOR"},
    {"<<", "OPERATOR"},
    {">>", "OPERATOR"},
    {"<<=", "OPERATOR"},
    {">>=", "OPERATOR"},
    {"&=", "OPERATOR"},
    {"|=", "OPERATOR"},
    {"^=", "OPERATOR"},
    {":", "OPERATOR"},
    {";", "OPERATOR"},
    {",", "OPERATOR"},
    {".", "OPERATOR"},
    {"->", "OPERATOR"},
    {"(", "OPERATOR"},
    {")", "OPERATOR"},
    {"[", "OPERATOR"},
    {"]", "OPERATOR"},
    {"{", "OPERATOR"},
    {"}", "OPERATOR"},
};


// Hash table size
#define HASH_TABLE_SIZE 101

// Hash table node
typedef struct HashNode {
    const char *lexeme;
    const char *token;
    struct HashNode *next;
} HashNode;

// Hash table arrays for keywords and operators
HashNode *keywords_hash_table[HASH_TABLE_SIZE];
HashNode *operators_hash_table[HASH_TABLE_SIZE];

// Hash function
unsigned int hash(const char *lexeme) {
    unsigned int hash_value = 0;
    while (*lexeme) {
        hash_value = (hash_value << 5) + *lexeme++;
    }
    return hash_value % HASH_TABLE_SIZE;
}

// Insert into hash table
void insert_into_hash(HashNode **hash_table, const char *lexeme, const char *token) {
    unsigned int index = hash(lexeme);
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    new_node->lexeme = lexeme;
    new_node->token = token;
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
}

// Find token from hash table
const char *find_token(HashNode **hash_table, const char *lexeme) {
    unsigned int index = hash(lexeme);
    HashNode *current = hash_table[index];
    while (current) {
        if (strcmp(current->lexeme, lexeme) == 0) {
            return current->token;
        }
        current = current->next;
    }
    return "IDENTIFIER";
}

State transition(State current_state, char transition_char) {
    State next_state;
    switch (current_state) {
        case START:
            if (isalpha(transition_char))
                next_state = BUILD_ID;
            else if (isdigit(transition_char))
                next_state = BUILD_NUM;
            else if (transition_char == ' ')
                next_state = START;
            else if (transition_char == '.')
                next_state = STOP;
            break;
        case BUILD_ID:
            if (isalpha(transition_char) || isdigit(transition_char) || transition_char == '_')
                next_state = BUILD_ID;
            else if (transition_char == ' ')
                next_state = IDENTIFIER;
            break;
        case BUILD_NUM:
            if (isdigit(transition_char))
                next_state = BUILD_NUM;
            else if (transition_char == ' ')
                next_state = NUMBER;
            break;
        default:
            next_state = current_state;
    }
    return next_state;
}

// ... (Rest of the code) ...

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error opening input file");
        return 1;
    }

    // ... (Populate keywords and operators hash tables) ...

    State current_state = START;
    char transition_char;
    char lexeme[100];
    int lexeme_length = 0;

    int lexeme_count = 0; // To keep track of lexeme count
    int max_lexeme_length = 0; // To determine table column width

    while (fscanf(input_file, "%c", &transition_char) != EOF) {
        if (transition_char == ' ' || transition_char == '\n') {
            if (lexeme_length > 0) {
                lexeme[lexeme_length] = '\0';
                const char *token = find_token(keywords_hash_table, lexeme);
                if (strcmp(token, "IDENTIFIER") == 0) {
                    token = find_token(operators_hash_table, lexeme);
                }
                if (lexeme_length > max_lexeme_length) {
                    max_lexeme_length = lexeme_length; // Update max length
                }
                lexeme_length = 0;

                lexeme_count++; // Increment lexeme count
            }
        } else {
            lexeme[lexeme_length++] = transition_char;
        }

        current_state = transition(current_state, transition_char);
    }

    fclose(input_file);

    // Print table header
    int table_width = max_lexeme_length + 2; // Adjust for padding and borders
    print_table_header(table_width);

    input_file = fopen(argv[1], "r"); // Re-open input file
    current_state = START;

    clock_t start_time = clock(); // Record start time

    while (fscanf(input_file, "%c", &transition_char) != EOF) {
        if (transition_char == ' ' || transition_char == '\n') {
            if (lexeme_length > 0) {
                lexeme[lexeme_length] = '\0';
                const char *token = find_token(keywords_hash_table, lexeme);
                if (strcmp(token, "IDENTIFIER") == 0) {
                    token = find_token(operators_hash_table, lexeme);
                }
                print_table_row(lexeme, token, table_width);
                lexeme_length = 0;
            }
        } else {
            lexeme[lexeme_length++] = transition_char;
        }

        current_state = transition(current_state, transition_char);
    }

    fclose(input_file);

    printf("Total lexemes: %d\n", lexeme_count); // Print lexeme count

    clock_t end_time = clock(); // Record end time
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", execution_time); // Print execution time

    return 0;
}
