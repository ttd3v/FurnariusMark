#include <alloca.h>
#include<argp.h>
#include <ctype.h>
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
    if (capacity - cursor < cost){
        void* ptr = realloc(*html, *capacity*2);
        if (!ptr){
            return -1;
        }
        *html = ptr;
    }
    *capacity*=2;
    memcpy(*html+*cursor, close?CTAGS[tag]:OTAGS[tag], cost);
    *cursor+=cost;
    
    return 0;
}

int parse(char const* input, char ** restrict html, unsigned long size, struct Parameters param){
    unsigned long html_cursor=0;
    unsigned long html_capacity = size*2;
    *html = calloc(html_capacity, 1);
    int context = 0;
    unsigned long start = 0;
    unsigned int idx = 0;
    unsigned int rel = 0;
    int flush = 0;
    int wflush = 0;
    while (start < size){
        
        if (input[start] == '*'){
            int stack = 0;
            int segment = 1;
            int len = 0;
            int target = 0;
            for (idx = start; idx < size && input[idx] != '\n'; idx ++){
                if(input[idx] == '*' && !segment && stack == target){break;};
                if(input[idx] == '*') {stack++;continue;};
                if(isalnum(input[idx])){
                    target = stack*2;
                    segment = 0;
                    len++;
                }
            }
            stack/=2;
            if (push_html_tag(html, &html_cursor, &html_capacity, 0, stack) < 0){
                printf("Failed to allocate memory :(\nFinishing the program.\n");
            }
            if(html_capacity-(html_cursor+len) < len){
                void* bu = realloc(*html,  len+(html_capacity * 2));
                html_capacity*=2;
                *html = bu;
            }
            memcpy(*html+html_cursor, &input[start+stack], len);
            html_cursor+=len;
            if (push_html_tag(html, &html_cursor, &html_capacity, 1, stack) < 0){
                printf("Failed to allocate memory :(\nFinishing the program.\n");
            }
            start+=idx+stack;
        }
        if(input[start] == '\n'){
            if(flush){
                if (push_html_tag(html, &html_cursor, &html_capacity, 1, wflush)<0){
                    printf("Failed to allocate memory :( \n");
                    return -1;
                }
                flush = 0;
                wflush = flush;
            }
            rel=0;
        }
        if(input[start] == '#' && rel == 1){
            int stack = 0;
            for (idx = start+1; idx < size && stack < 3 && input[idx] != '\n'; idx++){
                if(input[idx] == '#') {stack++;continue;};
                break;
            }

            if (push_html_tag(html, &html_cursor, &html_capacity, 0, 4+stack)<0){
                printf("Failed to allocate memory :( \n");
                return -1;
            }
            start+=stack;
            if(html_capacity-html_cursor < size-start){
                void* n = realloc(*html, html_capacity+1+size-start);
                if(!n){
                    printf("Failed to allocate memory :( \n");
                    return -1;
                }
                *html = n;
            }
            while(input[start] != '\n' && input[start] != '\0' && input[start] != EOF){   
                (*html)[html_cursor] = input[start]; 
                html_cursor++;
                start++;
            }
            
            if (push_html_tag(html, &html_cursor, &html_capacity, 1, 4+stack)<0){
                printf("Failed to allocate memory :( \n");
                return -1;
            }; 
        }
        if(input[start] == '-' && rel == 1 && !flush){
            int stack = 0;
            for (idx = start; idx < size && stack < 3 && input[idx] != '\n'; idx++){
                if(input[idx] == '-') {stack++;continue;};
                break;
            }
            if(stack == 1){
                if (push_html_tag(html, &html_cursor, &html_capacity, 0, 8)<0){
                    printf("Failed to allocate memory :( \n");
                    return -1;
                }
                start+=stack; 
                flush=1;
                wflush=8;
            }
            if(stack>1){
                if(html_capacity - html_cursor < 5){
                    void* n = realloc(*html, 5 + html_capacity*2);
                    html_capacity*=2;
                    if(!n){
                        printf("Failed to allocate memory :(");
                        exit(-1);
                    }
                } 
                memcpy(*html + html_cursor, "<hr/>", 5);
                start+=stack;
                html_cursor+=5;
            }
        }
        if (input[start] == '`' && rel == 1) {
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
            while (idx < size) {
                if (input[idx] == '`' && (idx + stack <= size) && strncmp(&input[idx], "```", stack) == 0) {
                    closed = 1;
                    idx += stack;
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
                rel = 0;
            }
            start++;
        }
        if (rel == 1 && input[start] != '#' && input[start] != '-' && input[start] != '*' && input[start] != '`' && !flush) {
            if (push_html_tag(html, &html_cursor, &html_capacity, 0, HTML_Paragraph) < 0) {
                printf("Failed to allocate memory :(\n");
                return -1;
            }
            flush = 1;
            wflush = HTML_Paragraph;
            memcpy(*html + html_cursor, &input[start], 1);
            html_cursor++;
            start++;
            rel++;
        }
        start ++;rel++;
    }

    
    char n[255] = {0};
    snprintf(n, 255, "./%s.html",param.title);

    FILE* output = fopen(n, "a");
    if(!output){
        printf("Failed to create output file.\n");
        return -1;
    }
    
    size_t css_len = param.css_href ? strlen(param.css_href) : 0;
    char *final_html = malloc(strlen(*html) + strlen(param.title) + css_len + 300);

    snprintf(final_html, strlen(*html) + strlen(param.title) + strlen(param.css_href) + 300, 
    HTML_PRESET, param.title, param.css_href ? param.css_href : "", *html);

    if (fwrite(final_html, strlen(final_html), 1, output) <= 0) {
        printf("Failed to write into the file.\n");
        free(final_html);
        return -1;
    }

    free(final_html);

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
    {
        void* n = realloc(param->title, param->title_len);
        if(n != NULL){
            param->title = n;
        }
    }
    {
        void* n = realloc(param->css_href, param->css_href_len);
        if(n != NULL){
            param->css_href = n;
        }
    }
    return process(param);
}
int main (){
    struct Parameters param = {0};
    prompt(&param);
    return 0;
}
