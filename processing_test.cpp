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

int P = 2;
pthread_barrier_t barrier;

typedef struct t_arguments {
    int id; 
    /* the result of a thread */
    string result;
    /* splitted sentence and size */
    string *words;
    int size;

} t_arguments;

// http://www.joshbarczak.com/blog/?p=970
// manual solution works faster
static bool isDelim(char c) {
    const char* DELIMS = " ";

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

void *processHorrorThreads(void *arg) {

    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

    for (int i = start; i < end; i++) {
        args->result += duplicateConsonants(words[i]);
        args->result += " ";
    }

	pthread_exit(NULL);
}


void processComedy(vector<string> words, string &result) {
    for (int i = 0; i < words.size(); i++) {
        for (int j = 0; j < words[i].length(); j++) {
            result.push_back((words[i])[j]);
            if ((j + 1) % 2 == 0) {
                result.push_back((words[i])[j]);
            }
        }
        result.push_back(' ');
        // cout << "\n";
    }
}



void *processComedyThreads(void *arg) {
    
    t_arguments *args = (t_arguments*)(arg);
    string *words = args->words;
    int id = args->id;
    int size = args->size;

    int start = id * (double)size / P;
    int end = MIN((id + 1) * (double)size / P, size);

    for (int i = start; i < end; i++) {
        for (int j = 0; j < words[i].length(); j++) {
            // cout << (words[i])[j] << " ";
            if ((j + 1) % 2 == 0) {
                args->result.push_back(toupper((words[i])[j]));
            } else {
                args->result.push_back((words[i])[j]);
            }
        }
        args->result.push_back(' ');
        // cout << "\n";
    }

	pthread_exit(NULL);
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
        //cout << "\n" << words[i] << "\n";
        if (islower((words[i])[0]) == false) {
            (words[i])[0] = toupper((words[i])[0]);
        }
        args->result += words[i];
        args->result += " ";
    }

	pthread_exit(NULL);
}


int main() {

    // string paragraph = "Aceasta tema pare\nImposibil de grea si\nSperam ca nu e imposibil sa o facem\n";
    string paragraph = "cel\nmai\nprobabil\nse\nface in 2-3 ore maxim";
    vector<string> words;
    pthread_t threads[P];
    t_arguments *thread_args;
    string result;
    int r = 0;
    void *thread_status;

    tokenize(paragraph, words);

    for (int i = 0; i < words.size(); i++) {
        //cout << words[i] << "\n\n";
    }
    // cout << words.size() << endl;
    thread_args = (t_arguments*) malloc(P * sizeof(t_arguments));

    pthread_barrier_init(&barrier, NULL, P);
	if (r) {
		printf("Cannot init barrier!\n");
	}

	for (int i = 0; i < P; i++) {
		thread_args[i].id = i;
        thread_args[i].words = &words[0]; 
        thread_args[i].size = words.size();

		// r = pthread_create(&threads[i], NULL, processHorrorThreads, &thread_args[i]);
		// r = pthread_create(&threads[i], NULL, processComedyThreads, &thread_args[i]);
		r = pthread_create(&threads[i], NULL, processFantasyThreads, &thread_args[i]);
		// r = pthread_create(&threads[i], NULL, processScifiThreads, &thread_args[i]);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}

	for (int i = 0; i < P; i++) {
		r = pthread_join(threads[i], &thread_status);
		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

    /* compose the modified string */
    for (int i = 0; i < P; i++) {
        result += (thread_args[i].result);
    }

    cout << result << "\n";

	r = pthread_barrier_destroy(&barrier);
	if (r) {
		printf("Cannot destroy barrier.\n");
	}

    free(thread_args);

    string test = "Ana\n";

    return 0;

}