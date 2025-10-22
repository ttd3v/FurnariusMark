#include <alloca.h>
#include<argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* HTML_PRESET = "<html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>%s</title><link rel=\"stylesheet\" href=\"%s\"></head><body>%s</body></html>";

struct Parameters{
    FILE* file;
    char* title;
    char* css_href;
    unsigned int title_len;
    unsigned int css_href_len;
};
size_t read_line(char *buffer, size_t size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0'; 
        return 0;
    }

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0'; 
        len--;
    }

    return len;
}
static inline unsigned long get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);      
    unsigned long size = ftell(file);        
    fseek(file, 0, SEEK_SET);      
    return size;
}

#define MAX_MD_STACK 3145728
enum HtmlTag {
    HTML_Bold = 0,
    HTML_Italic = 1,
    HTML_Bold_Italic = 2,
    HTML_Paragraph = 3,
    HTML_H1 = 4,
    HTML_H2 = 5,
    HTML_H3 = 6,
    HTML_Code = 7,
    HTML_Pre_Code = 8,
    HTML_Ul = 9,
};

const unsigned long OCO[10] = {3, 3, 6, 3, 4, 4, 4, 6, 11, 6};
const unsigned long CCO[10] = {4, 4, 8, 4, 5, 5, 5, 7, 12, 7};
const char* OTAGS[10] = {"<b>", "<i>", "<b><i>", "<p>", "<h1>", "<h2>", "<h3>", "<code>", "<pre><code>", "<ul>"};
const char* CTAGS[10] = {"</b>", "</i>", "</i></b>", "</p>", "</h1>", "</h2>", "</h3>", "</code>", "</code></pre>", "</ul>"};
static inline unsigned long get_cost(int tag, int close){
    return close?CCO[tag]:OCO[tag];
}

static inline int push_html_tag(char ** restrict html, unsigned long* cursor, unsigned long* capacity, int close, int tag){
    unsigned long cost = get_cost(tag, close);
    if (*capacity - *cursor < cost){
        void* ptr = realloc(*html, (cost*2)+*capacity*2);
        if (!ptr){
            return -1;
        }
        *html = ptr;
        *capacity*=2;
        *capacity+=(cost*2);
    }
    memcpy(*html+*cursor, close?CTAGS[tag]:OTAGS[tag], cost);
    *cursor+=cost;
    
    return 0;
}
#define PARSER_START() \
    printf("🚀 Starting markdown parser...\n"); \
    fflush(stdout);

#define PARSER_COMPLETE() \
    printf("✅ Parsing completed successfully!\n"); \
    fflush(stdout);

#define PARSER_PROGRESS(percent) \
    printf("⏳ Progress: %d%%\r", percent); \
    fflush(stdout);

// Context-specific parsing notifications
#define PARSE_HEADER(level) \
    printf("📋 Parsing H%d header\n", level); \
    fflush(stdout);

#define PARSE_BOLD() \
    printf("💪 Parsing bold text\n"); \
    fflush(stdout);

#define PARSE_ITALIC() \
    printf("📝 Parsing italic text\n"); \
    fflush(stdout);

#define PARSE_CODE() \
    printf("💻 Parsing inline code\n"); \
    fflush(stdout);

#define PARSE_CODE_BLOCK() \
    printf("🖥️  Parsing code block\n"); \
    fflush(stdout);

#define PARSE_PARAGRAPH() \
    printf("📄 Parsing paragraph\n"); \
    fflush(stdout);

#define PARSE_LIST() \
    printf("📝 Parsing list\n"); \
    fflush(stdout);

#define PARSE_HR() \
    printf("➖ Parsing horizontal rule\n"); \
    fflush(stdout);

// File operations
#define FILE_READING(filename) \
    printf("📖 Reading file: %s\n", filename); \
    fflush(stdout);

#define FILE_WRITING(filename) \
    printf("💾 Writing output to: %s\n", filename); \
    fflush(stdout);

#define FILE_READ_COMPLETE(bytes) \
    printf("✓ Read %lu bytes\n", (unsigned long)bytes); \
    fflush(stdout);

#define PARSE_BOLD_ITALIC() \
    printf("📝 Parsing bold and italic text\n"); \
    fflush(stdout);
// Memory operations
#define MEMORY_ALLOC(size) \
    printf("🧠 Allocating %lu bytes\n", (unsigned long)size); \
    fflush(stdout);

#define MEMORY_REALLOC(new_size) \
    printf("🔄 Reallocating to %lu bytes\n", (unsigned long)new_size); \
    fflush(stdout);

// Error notifications
#define PARSER_ERROR(msg) \
    printf("❌ Error: %s\n", msg); \
    fflush(stdout);

#define PARSER_WARNING(msg) \
    printf("⚠️  Warning: %s\n", msg); \
    fflush(stdout);

int all_right(char input, char* memo, int len){
    for (int i = 0; i < len; i ++ ){
        if (*(memo+i) == input){
            return 0;
        }
    }
    return 1;
}

unsigned int dump_line(const unsigned int start, const char* text,const unsigned int length, char**  html,unsigned long * html_capacity,unsigned long * html_cursor, char* until){
    unsigned int seg_size = 0;
    char *pattern = until == NULL?"\0":until;
    int len = strlen(pattern);
    while(start+seg_size < length){
        if(text[start+seg_size] != '\0' && text[start+seg_size] != '\n' && all_right(text[start+seg_size], pattern, len)){
            seg_size += 1;
            continue;
        }
        break;
    }
    if(*html_capacity-*html_cursor < seg_size){
        void *ptr = realloc(*html, 1+*html_capacity*2);
        if(ptr == NULL){
            printf("Critical, finishing the program.\n[ENOMEM] Failed to rallocate on dump_line\n");
            exit(-1);
        }
        *html = ptr;
        *html_capacity*=2;
        *html_capacity+=1;
    }
    memcpy(*html+*html_cursor, text+start, seg_size);
    *html_cursor+=seg_size;
    return seg_size;
}

int parse(char const* input, char ** restrict html,const unsigned long size, struct Parameters param){
    unsigned long html_cursor=0;
    unsigned long html_capacity = size*2;
    *html = calloc(html_capacity, 1);
    int context = 0;
    unsigned long start = 0;
    unsigned int idx = 0;
    int flush = 0;
    int wflush = 0;
    #define nl (start == 0 ||input[start-1]=='\n')
    while (start < size){
        printf("bt: roll\n");
        printf("bt: start %lu\n",start);
        if(input[start] == '\n'){
            printf("bt: \\n\n");
            if(flush){
                if (push_html_tag(html, &html_cursor, &html_capacity, 1, wflush)<0){
                    printf("Failed to allocate memory :( \n");
                    return -1;
                }
                flush = 0;
                wflush = flush;
            }
            start++;
        }
        if ( nl && input[start] != ' ' && input[start] != '#' && input[start] != '-' && input[start] != '*' && input[start] != '`' && !flush) {
            PARSE_PARAGRAPH()
            if (push_html_tag(html, &html_cursor, &html_capacity, 0, HTML_Paragraph) < 0) {
                printf("Failed to allocate memory :(\n");
                return -1;
            }
            unsigned long c = dump_line(start, input, size, html, &html_capacity, &html_cursor, "*´-#\0");
            html_cursor+=c;
            start+=c;
        }
        
        if (input[start] == '*'){
            printf("*\n");fflush(stdout); 
            int stack = 0;
            for (; start < size; ){
                if(input[start] == '*'){
                    stack+=1;
                    start++;
                    continue;;
                };
                if(input[start] != '*'){
                    break;
                }
            }
            if (push_html_tag(html, &html_cursor, &html_capacity, 0, stack-1) < 0){
                printf("Failed to allocate memory :(\nFinishing the program.\n");;exit(-1);
            };
            start+=stack;
            start += dump_line(start, input, size, html, &html_capacity, &html_cursor, "*") + stack;
            if (push_html_tag(html, &html_cursor, &html_capacity, 1, stack-1) < 0){
                PARSER_ERROR("Failed to allocate memory :(\nFinishing the program.\n");exit(-1);
            }

            
        }
        
        if(input[start] == '#' && nl){
            int stack = 0;
            for (idx = start; idx < size && stack < 3 && input[idx] != '\n'; idx++){
                if(input[idx] == '#') {stack++;continue;};
                break;
            }
            PARSE_HEADER(stack);
            if (push_html_tag(html, &html_cursor, &html_capacity, 0, 3+stack)<0){
                printf("Failed to allocate memory :( \n");
                return -1;
            }
            start+=stack+1;
            switch (stack) {
                case 0:
                    wflush = HTML_H1;
                    break;
                case 1:
                    wflush = HTML_H2;
                    break;
                case 2:
                    wflush = HTML_H3;
                    break;
                default:
                    wflush = HTML_H1;
                    break;
            };
            start+=dump_line(start, input, size, html, &html_capacity, &html_cursor,NULL);
            if (push_html_tag(html, &html_cursor, &html_capacity, 1, 3+stack) < 0) {
                return -1;
            }
        }
        if(input[start] == '-' && nl && !flush){
            PARSE_LIST();
            int stack = 0;
            for (idx = start; idx < size && stack < 3 && input[idx] != '\n'; idx++){
                if(input[idx] == '-') {stack++;continue;};
                break;
            }
            if(stack == 1){
                if (push_html_tag(html, &html_cursor, &html_capacity, 0, HTML_Ul)<0){
                    printf("Failed to allocate memory :(\n");
                    return -1;
                }
                start+=dump_line(start, input, size, html, &html_capacity, &html_cursor,NULL);
                 if (push_html_tag(html, &html_cursor, &html_capacity, 1, HTML_Ul)<0){
                    printf("Failed to allocate memory :(\n");
                    return -1;
                }
            }
            if(stack>1){
                if(html_capacity - html_cursor < 5){
                    void* n = realloc(*html, 5 + (html_capacity*2));
                    html_capacity= (html_capacity*2) + 5;
                    if(!n){
                        printf("Failed to allocate memory :(\n");
                        exit(-1);
                    }
                }
                memcpy(*html + html_cursor, "<hr/>", 5);
            }
            start+=stack;
        }
        if (input[start] == '`' && nl) {
            int stack = 0;
            unsigned long idx = start;
            while (idx < size && input[idx] == '`' && stack < 3) {
                stack++;
                idx++;
            }   
            int tag = (stack == 1) ? HTML_Code : HTML_Pre_Code;
            unsigned long content_start = idx;
            unsigned long len = 0;
            int closed = 0;
            if (stack != 1) PARSE_CODE_BLOCK();
            if (stack == 1) PARSE_CODE();
            while (idx < size) {
                if (input[idx] == '`' && (idx + stack <= size) && strncmp(&input[idx], "```", stack) == 0) {
                    closed = 1;
                    idx += stack + 1;
                    break;
                }
                len++;
                idx++;
            }
            if (closed && len > 0) {
                if (push_html_tag(html, &html_cursor, &html_capacity, 0, tag) < 0) {
                    printf("Failed to allocate memory :(\n");
                    return -1;
                }
                if (html_capacity - html_cursor < len) {
                    void* bu = realloc(*html, len + html_capacity * 2);
                    if (!bu) {
                        printf("Failed to allocate memory :(\n");
                        return -1;
                    }
                    html_capacity *= 2;
                    *html = bu;
                }
                memcpy(*html + html_cursor, input + content_start, len);
                html_cursor += len;
                if (push_html_tag(html, &html_cursor, &html_capacity, 1, tag) < 0) {
                    printf("Failed to allocate memory :(\n");
                    return -1;
                }
                start = idx;
            }
            start++;
        }
        if (wflush == HTML_Paragraph){
            if (html_capacity - html_cursor == 0 ){
                *html = realloc(*html, html_capacity*=2);
                if (!html){
                    printf("Failed to allocate memory :/\n");
                    exit(-1);
                }
                memcpy(*html + html_cursor,input+start,1);
            }
        }
        start++;
    }

    
    char n[255] = {0};
    snprintf(n, 255, "./%s.html",param.title);

    FILE* output = fopen(n, "w");
    if(!output){
        printf("Failed to create output file.\n");
        return -1;
    }
    (*html)[html_cursor] = '\0'; 
    size_t css_len = param.css_href ? strlen(param.css_href) : 0;
    #define NULL_TERM_SIZE 1
    char *final_html = malloc(strlen(*html)+NULL_TERM_SIZE + param.title_len+ NULL_TERM_SIZE+ css_len+ NULL_TERM_SIZE+ strlen(HTML_PRESET) + NULL_TERM_SIZE);

    snprintf(final_html, strlen(*html) + param.title_len + param.css_href_len + 300, 
    HTML_PRESET, param.title, param.css_href ? param.css_href : "", *html);

    if (fwrite(final_html, strlen(final_html), 1, output) <= 0) {
        printf("Failed to write into the file.\n");
        free(final_html);
        return -1;
    }

    free(final_html);
    if (param.title) free(param.title);
    if (param.css_href) free(param.css_href);
    if (param.file) fclose(param.file);

    return 0;
}

int process(struct Parameters* param){
    unsigned long size = get_file_size(param->file);
    if (size == 0) {
        printf("There is nothing left to do.\n");
        exit(0);
    }
    
    char*markdown=malloc(size);
        if (!markdown){
            printf("Failed to allocate memory for the markdown file.\n");
            free(param->css_href);
            free(param->title);
            fclose(param->file);
            exit(-1);
        }

    int result =  fread(markdown, 1, size, param->file);// load all the markdown text into memory
    if(result != size){
        printf("Failed to read the input file.\n");
        free(param->css_href);
        free(param->title);
        fclose(param->file);
        free(markdown);
        exit(-1);
    }
    char* html = NULL;
    if (parse(markdown, &html, size,*param) < 0){
        printf("Failed to parse\n");
        return -1;
    }
    free(html);
    free(markdown);
    return 0;
}

int prompt(struct Parameters* param){
    printf("== FURNARIUSMARK ==\n------ SETUP ------\n📂 File-path: ");
    fflush(stdout);
    while(1){
        char buffer[4096] = {0};
        size_t l = read_line(buffer,4096); 
        FILE *file = fopen(buffer, "r");
        if(!file){
            printf("\n\033[1;31mFailed to open the entered file, verify if it exists.\033[0m\n\n📂 File-path:");
            fflush(stdout);
        }else{
            param->file = file;
            break;
        }
    }
    {
        char buffer[100] = {0};
        printf("📝 title (max length: 100): ");
        fflush(stdout);
        param->title_len = read_line(buffer, 100);
        param->title = malloc(100);
        if(!param->title){
            printf("\n\033[1;31mFailed to allocate memory\033[0m\n");
            fclose(param->file);
            return -1;
        }
        memcpy(param->title, buffer, 100);
    }
    {
        printf("🎨 Css-href (leave as *** for none): ");
        fflush(stdout);
        char buffer[4096] = {0};
        fflush(stdout);
        size_t l = read_line(buffer, 4096); 
        if(buffer[0] == '*' && buffer[1] == '*' && buffer[2] == '*'){
            param->css_href = NULL;
        }else{
            param->css_href = malloc(4096);
            if(!param->css_href){
                printf("\n\033[1;31mFailed to allocate memory\033[0m\n");
                fclose(param->file);
                return -1;
            }
            memcpy(param->css_href, buffer, 4096);
            param->css_href_len = l;
        }
    }
    return process(param);
}

int main (){
    struct Parameters param = {0};
    prompt(&param);
    return 0;
}
