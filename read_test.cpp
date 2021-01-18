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


const char COMEDY_NAME[7] = "comedy";
const char HORROR_NAME[7] = "horror";
const char FANTASY_NAME[8] = "fantasy";
const char SCIFI_NAME[16] = "science-fiction";

void *read_file(void *arg) {
    FILE *fp;

    read_arguments *args = (read_arguments*)(arg);
    int id = args->id;
    line *lines = args->lines;
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

            
            // if this is type of paragraph we need to read
            if (!strncmp(line, COMEDY_NAME, 6)) {
                // read the entire paragraph, keep a vector of lines(strings)

                // memset(line, 0, sizeof(line));
                // while paragraph is not finished
                while (strcmp(line, "\n") && !feof(fp)) {
                    memset(line, 0, sizeof(line));
                    fgets(line, 500, (FILE*)fp);
                    line_NO++;

                    if (!strcmp(line, "\n")) {
                        break;
                    }
                    lines[size].data = line;
                    lines[size].NO = line_NO;

                    size++;

                    // printf("%s", line);
                }

            }
            // printf("%s", line);
            memset(line, 0, sizeof(line));
        }
    }
    fclose(fp);
    pthread_exit(NULL);

} 

int main() {

    string paragraph = "da, dar e tema 3 si mereu temele 3 au fost usoare plus ca e scrisa de Hogea, toate temele lui sunt accesibile";
    // string str = "Aceasta tema pare Imposibil de grea si Speram ca nu e imposibil sa o facem";

    P = 1;
    vector<line> lines(2600);
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
		arguments[i].id = i;
        arguments[i].lines = &lines[0]; 
        arguments[i].size = lines.size();

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

    for (int i = 0; i < 10; i++) {
        cout << lines[i].NO << "  " << lines[i].data;
    }

	r = pthread_barrier_destroy(&barrier);
	if (r) {
		printf("Cannot destroy barrier.\n");
	}

    free(arguments);

    return 0;

}