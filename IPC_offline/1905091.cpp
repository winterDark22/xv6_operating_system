#include <iostream>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <cmath>
#include <semaphore.h>
#include "defs.h"
using namespace std;

const int N = 100;

int n, m, delay_print, delay_bind, delay_rw;
int student_delay[N], staff_delay[3], state[N], cnt_done[N];
int submission_cnt, cr;
time_t st_time, en_time, dif;
sem_t s[N], all_finished[N], binding_empty, db; 
pthread_mutex_t tickslock, mutex, cnt_lock[N], reader_lock;

void* reader(void* arg) {
    while(1) {
        sleep(staff_delay[*(int*)arg]);

        int id = *(int*) arg;
        pthread_mutex_lock(&reader_lock);
        cr = cr + 1;
        if(cr == 1) sem_wait(&db);
        pthread_mutex_unlock(&reader_lock);

        pthread_mutex_lock(&tickslock);
        en_time = time(NULL);
        dif = difftime(en_time, st_time);
        cout << "Staff " << id << " has started reading the entry book at time " << dif << ". No. of submission = " << submission_cnt << endl;
        pthread_mutex_unlock(&tickslock);

        sleep(delay_rw); //reading

        if(submission_cnt == n/m) {
            break;
        }
        pthread_mutex_lock(&reader_lock);
        cr = cr - 1;
        if(cr == 0) sem_post(&db);
        pthread_mutex_unlock(&reader_lock);
    }
    free(arg);
	return NULL;
}

void writer(int id) {
    sem_wait(&db);
    sleep(delay_rw); //writing
    submission_cnt++;

    pthread_mutex_lock(&tickslock);
    en_time = time(NULL);
    dif = difftime(en_time, st_time);
	cout << "Group "<< id << " has submitted the report at time " << dif << endl;
    pthread_mutex_unlock(&tickslock);

    sem_post(&db);
}
void bindingStation(int id) {
    sem_wait(&binding_empty);

    pthread_mutex_lock(&tickslock);
    en_time = time(NULL);
    dif = difftime(en_time, st_time);
	cout<< "Group " << id << " has started binding at time " << dif << endl;
    pthread_mutex_unlock(&tickslock);

    sleep(delay_bind); //binding
    sem_post(&binding_empty);

    pthread_mutex_lock(&tickslock);
    en_time = time(NULL);
    dif = difftime(en_time, st_time);
	cout<< "Group " << id << " has finished binding at time " << dif << endl;
    pthread_mutex_unlock(&tickslock);

    writer(id);
}
void test(int i) {
    int flag = 1;
    if(state[i] != WANT_TO_PRINT) {
        flag = 0;
    }
    for(int j=1; j<=n; j++) {
        if(j == i) continue;
        if(abs(i-j) % 4 != 0) continue;
        if(state[j] == PRINTING) {
            flag = 0;
            break;
        }
    }
    if(flag) {
        state[i] = PRINTING;
        sem_post(&s[i]);
    }
}
void takePrinter(int i) {
    pthread_mutex_lock(&mutex);
    state[i] = WANT_TO_PRINT;
    test(i);
    pthread_mutex_unlock(&mutex);
    sem_wait(&s[i]);
}

void awake(int i, int type) {
    for(int j=1; j<=n; j++) {
        if(i == j) continue;;
        if(type == SAME_GROUP && (i-1)/m == (j-1)/m) test(j);
        if(type == DIFFERENT_GROUP && (i-1)/m != (j-1)/m) test(j);
    }
}
void putPrinter(int i) {
    pthread_mutex_lock(&mutex);
	state[i] = DONE_PRINTING;
	
    awake(i, SAME_GROUP);
    awake(i, DIFFERENT_GROUP);

	pthread_mutex_unlock(&mutex);
}
void* printingStation(void* arg) {
    sleep(student_delay[*(int*)arg]); //wait for arrival

    int id = *(int*)arg;
    pthread_mutex_lock(&tickslock);
    en_time = time(NULL);
    dif = difftime(en_time, st_time);
	cout << "Student " << id << " has arrived at the print station at time " << dif << endl;
    pthread_mutex_unlock(&tickslock);

    takePrinter(id);
    sleep(delay_print); //printing
    putPrinter(id);

    pthread_mutex_lock(&tickslock);
    en_time = time(NULL);
    dif = difftime(en_time, st_time);
	cout << "Student " << id << " has finished printing at time " << dif << endl;
    pthread_mutex_unlock(&tickslock);

    int groupID = (id-1)/m + 1;
    int done = 0;
    pthread_mutex_lock(&cnt_lock[groupID]);
    cnt_done[groupID]++;
    if(cnt_done[groupID] == n/m) {
        done = 1;
    }
    pthread_mutex_unlock(&cnt_lock[groupID]);

    if(id%m == 0 && !done) {
        sem_wait(&all_finished[groupID]);
    }
    if(done) {
        sem_post(&all_finished[groupID]);
    }
    if(id%m == 0) {
        pthread_mutex_lock(&tickslock);
        en_time = time(NULL);
        dif = difftime(en_time, st_time);
		cout << "Group " << groupID << " has finished printing at time " << dif << endl;
        pthread_mutex_unlock(&tickslock);

        bindingStation(groupID);
    }
    free(arg);
    return NULL;
}


int main() {
    cin >> n >> m;
    cin >> delay_print >> delay_bind >> delay_rw;

    student_delay[0] = 3; //min arrival time of any student
    for(int i=1; i<=n; i++) {
        student_delay[i] = student_delay[i-1] + getRand(2);
    }
    for(int i=1; i<=2; i++) {
        staff_delay[i] = getRand(20);
    }
    //initializing mutex and semaphores
    pthread_mutex_init(&tickslock, NULL);
    pthread_mutex_init(&mutex, NULL);
    for(int i=1; i<=n; i++) {
        sem_init(&s[i], 0, 0);
    }
    for(int i=1; i<=(n-1)/m + 1; i++) {
        pthread_mutex_init(&cnt_lock[i], NULL);
        sem_init(&all_finished[i], 0, 0);
    }
    pthread_mutex_init(&reader_lock, NULL);
    sem_init(&binding_empty, 0, 2);
    sem_init(&db, 0, 1);

    srand(time(NULL));
    st_time = time(NULL);
    //creating threads
    pthread_t studentThreads[n+1];
    pthread_t staffThreads[3];
    for(int i=1; i<=n; i++) {
        int* idx = new int(i);
        pthread_create(&studentThreads[i], NULL, printingStation, idx);
    }
    for(int i=1; i<=2; i++) {
        int* idx = new int(i);
        pthread_create(&staffThreads[i], NULL, reader, idx);
    }
    //joining threads
    for(int i=1; i<=n; i++) {
        pthread_join(studentThreads[i], NULL);
    }
    for(int i=1; i<=2; i++) {
        pthread_join(staffThreads[i], NULL);
    }
    //destroy all
    pthread_mutex_destroy(&tickslock);
    pthread_mutex_destroy(&mutex);
    for(int i=1; i<=n; i++) sem_destroy(&s[i]);
    for(int i=1; i<= (n-1)/m + 1; i++) {
        pthread_mutex_destroy(&cnt_lock[i]);
        sem_destroy(&all_finished[i]);
    }
    pthread_mutex_destroy(&reader_lock);
    sem_destroy(&binding_empty);
    sem_destroy(&db);
}   
