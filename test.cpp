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

typedef struct t_arguments {
    int id; 
    /* the result of a thread */
    string result;
    /* splitted sentence and size */
    string *words;
    int size;

} t_arguments;


bool isConsonant(char c) {
    c = tolower(c);
    /* if c is letter and is not vowel */
    return (c >= 'a' && c <= 'z') && 
            !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

string duplicateConsonants(string buffer) {
    
    // define empty string and construct a new buffer
    string res = "";

    int buffer_len = buffer.length();

    for (int i = 0; i < buffer_len; i++) {
        res += buffer[i];

        if (isConsonant(buffer[i])) {
            res += tolower(buffer[i]);
        }
    }

    return res;
}

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

void processComedyString(vector<string> words, string &result) {// , char **result) {
    for (int i = 0; i < words.size(); i++) {
        for (int j = 0; j < words[i].length(); j++) {
            // cout << (words[i])[j] << " ";
            result.push_back((words[i])[j]);
            if ((j + 1) % 2 == 0) {
                result.push_back((words[i])[j]);
            }
        }
        result.push_back(' ');
        // cout << "\n";
    }

}



void processScifi(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        if ((i + 1) % 6 == 0) {
            reverse(words[i].begin(), words[i].end());
        }
        result += words[i];
        result += " ";
    }
}

void *processScifiThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

    for (int i = start; i < end; i++) {
        if ((i + 1) % 7 == 0) {
            reverse(words[i].begin(), words[i].end());
        }
        args->result += words[i];
        args->result += " ";
    }

	pthread_exit(NULL);
}


void processFantasy(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        if (islower((words[i])[0])) {
            (words[i])[0] = toupper((words[i])[0]);
        }
        result += words[i];
        result += " ";
    }
}


void *processFantasyThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

    for (int i = start; i < end; i++) {
        if (islower((words[i])[0])) {
            (words[i])[0] = toupper((words[i])[0]);
        }
        args->result += words[i];
        args->result += " ";
    }

	pthread_exit(NULL);
}


int main() {

    string str = "stiti ca tema asta contine si MPI si Pthreads, nu? ";
    // // cout << duplicateConsonants(str) << "\n";

    vector<string> result;
    tokenize(str, result);

    // string res;
    // //processComedyString(result, res);
    // //cout << res << "\n";


    // // processFantasy(result, res);
    // processScifi(result, res);
    // cout << res << "\n";

    int r = 0;
    void *status;
    pthread_t threads[P];

    t_arguments *arguments;
    arguments = (t_arguments*) malloc(P * sizeof(t_arguments));

    
    pthread_barrier_init(&barrier, NULL, P);
	if (r) {
		printf("Cannot init barrier!\n");
	}

	for (int i = 0; i < P; i++) {
		arguments[i].id = i;
        arguments[i].words = &result[0]; 
        arguments[i].size = result.size();

		// r = pthread_create(&threads[i], NULL, processFantasyThreads, &arguments[i]);
		r = pthread_create(&threads[i], NULL, processScifiThreads, &arguments[i]);

		if (r) {
			printf("Eroare la crearea thread-stdului %d\n", i);
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

    string finalString;
    /* compose the modified string */
    for (int i = 0; i < P; i++) {
        finalString += (arguments[i].result);

        // cout << arguments[i].result << endl;
    }

    // cout << "_" << arguments[0].result << "_\n";
    cout << finalString << "\n";

	r = pthread_barrier_destroy(&barrier);
	if (r) {
		printf("Cannot destroy barrier.\n");
	}

    return 0;

}