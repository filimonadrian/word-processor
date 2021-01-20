#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <iostream> 
#include <cstring>
#include <vector>
#include <bits/stdc++.h>

#define MASTER 0
#define HORROR 1
#define COMEDY 2
#define FANTASY 3
#define SCIFI 4
#define MAX_THREADS sysconf(_SC_NPROCESSORS_CONF)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define BUFMAX 500

const char COMEDY_NAME[7] = "comedy";
const char HORROR_NAME[7] = "horror";
const char FANTASY_NAME[8] = "fantasy";
const char SCIFI_NAME[16] = "science-fiction";
const char newline[2] = "\n";

using namespace std; 

int P = 4;
pthread_barrier_t barrier;

typedef struct line {
    int NO;
    string data;
} line;

typedef struct read_arguments {
    int id;
    /* vector of paragraph lines and the size */
    line *lines;
    int size;
    char genre[20];

} read_arguments;

// http://www.joshbarczak.com/blog/?p=970
// manual solution works faster
static bool isDelim(char c) {
    const char* DELIMS = " \n\t";

    do {
        if (c == *DELIMS) {
            return true;
        }
        ++DELIMS;
    } while (*DELIMS);

    return false;
}

vector<string> tokenize(std::string &str, vector<string> &result) {
    char *mutableString = (char*) malloc (str.size() + 1);
    strcpy(mutableString, str.c_str());

    char *p = mutableString;
    
    while (*p && isDelim(*p)) {
        ++p;
    }

    while (*p) {
        
        // start of token
        char* token = p;

        do {
            ++p;
        } while (!isDelim(*p) && *p);

        *p = 0;
       // cout << token;
        result.push_back(token);
        // printf("_%s_\n", token);


        do {
            ++p;
        } while (*p && isDelim(*p));
    }

    free(mutableString);
    return result;
}

bool isConsonant(char c) {
    c = tolower(c);
    /* if c is letter and is not vowel */
    return (c >= 'a' && c <= 'z') && 
            !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}


void *read_file(void *arg) {
    FILE *fp;

    read_arguments *args = (read_arguments*)(arg);
    int id = args->id;
    line *lines = args->lines;
    char genre[20];
    strcpy(genre, args->genre);
    bool isNewLineBefore = true;

    // int size = args->size;
    int size = 0;
    int line_NO = 0;

    // buffer which keeps the data
    char line[501];
    fp = fopen("input1.in", "r");

    if (fp == NULL) {
        printf("Error! Can't open this file.\n");
    } else {
        while (!feof(fp)) {
            fgets(line, 500, (FILE*)fp);
            line_NO++;

            // if this is type of paragraph we want to read
            // !!! problem with compare size
            if (!strncmp(line, genre, 6) && isNewLineBefore == true) {
                // read the entire paragraph, keep a vector of lines(strings)

                // memset(line, 0, sizeof(line));
                // while paragraph is not finished
                while (strcmp(line, newline) && !feof(fp)) {
                    memset(line, 0, sizeof(line));
                    fgets(line, 500, (FILE*)fp);
                    line_NO++;

                    if (!strcmp(line, newline)) {
                        isNewLineBefore = true;
                        break;
                    }
                    lines[size].data = line;
                    lines[size].NO = line_NO;
                    isNewLineBefore = false;

                    size++;

                    // printf("%s", line);
                }

            }
            // printf("%s", line);
            memset(line, 0, sizeof(line));
        }
    }
    args->size = size;
    fclose(fp);
    pthread_exit(NULL);
} 

int main() {

    vector<line> horror_lines(2600);
    vector<line> comedy_lines(2600);
    vector<line> fantasy_lines(2600);
    vector<line> scifi_lines(2600);

    pthread_t threads[P];
    read_arguments *arguments;
    string result;
    int r = 0;
    void *status;

    arguments = (read_arguments*) malloc(P * sizeof(read_arguments));

    pthread_barrier_init(&barrier, NULL, P);
	if (r) {
		printf("Cannot init barrier!\n");
	}

	for (int i = 0; i < P; i++) {
        switch (i) {

            case (HORROR - 1):
                strcpy(arguments[i].genre, HORROR_NAME);
                arguments[i].lines = &horror_lines[0]; 
                break;
            case (COMEDY - 1):
                strcpy(arguments[i].genre, COMEDY_NAME);
                arguments[i].lines = &comedy_lines[0]; 
                break;
            case (FANTASY - 1): 
                strcpy(arguments[i].genre, FANTASY_NAME);
                arguments[i].lines = &fantasy_lines[0]; 
                break;            
            case (SCIFI - 1): 
                strcpy(arguments[i].genre, SCIFI_NAME);
                arguments[i].lines = &scifi_lines[0]; 
                break;
        }

		arguments[i].id = i;
        // arguments[i].lines = &lines[0]; 

		r = pthread_create(&threads[i], NULL, read_file, &arguments[i]);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}

	for (int i = 0; i < P; i++) {
		r = pthread_join(threads[i], &status);
		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

    for (int i = 0; i < 5; i++) {
        cout << horror_lines[i].NO << "  " << horror_lines[i].data;
    }

    // cout << arguments->size << "\n";

	r = pthread_barrier_destroy(&barrier);
	if (r) {
		printf("Cannot destroy barrier.\n");
	}

    free(arguments);

    return 0;

}