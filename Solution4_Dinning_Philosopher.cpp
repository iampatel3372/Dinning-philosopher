#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <unistd.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <iostream>
#include<fstream>
using namespace std;


#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define RIGHT (phnum + 2) % N
#define LEFT (phnum + 1) % N

int state[N];
int phil[N] = { 0, 1, 2, 3, 4 };

int num_times_eaten[N] = {0,0,0,0,0};
int num_rounds=0;

sem_t Mutex;
sem_t S[N];

ofstream fout;
ofstream foutReport;

void test(int phnum)
{
	if (state[phnum] == HUNGRY
		&& state[LEFT] != EATING
		&& state[RIGHT] != EATING) {
		// state that eating
		state[phnum] = EATING;

		sleep(2);

		cout<<"Philosopher "<<phnum+1<<" takes fork "<<phnum+1<<" and "<<LEFT+1<<endl;
		cout<<"Philosopher "<<phnum+1<<" is eating"<<endl;
		
		fout<<"Philosopher "<<phnum+1<<" takes fork "<<phnum+1<<" and "<<LEFT+1<<endl;
		fout<<"Philosopher "<<phnum+1<<" is eating"<<endl;
		
		
		num_times_eaten [phnum]++;
		cout<<"Philosopher "<<phnum+1<<" has eaten "<<num_times_eaten[phnum]<<" number of times."<<endl;
		fout<<"Philosopher "<<phnum+1<<" has eaten "<<num_times_eaten[phnum]<<" number of times."<<endl;
		num_rounds++;

		// sem_post(&S[phnum]) has no effect
		// during takefork
		// used to wake up hungry philosophers
		// during putfork
		sem_post(&S[phnum]);
	}
}

// take up chopsticks
void take_fork(int phnum)
{

	sem_wait(&Mutex);

	// state that hungry
	state[phnum] = HUNGRY;

	cout<<"Philosopher "<<phnum+1<< " is hungry."<<endl;
	fout<<"Philosopher "<<phnum+1<< " is hungry."<<endl;
	
	// eat if neighbours are not eating
	test(phnum);

	sem_post(&Mutex);

	// if unable to eat wait to be signalled
	sem_wait(&S[phnum]);

	//sleep(1);
	
//PUT chopsticks down WALA PART

	sem_wait(&Mutex);

	// state that thinking
	state[phnum] = THINKING;

	cout<<"Philosopher "<<phnum+1<< " is putting fork "<<phnum+1<< " and " <<LEFT+1<<" down."<<endl;
	cout<<"Philosopher "<<phnum+1<<" is thinking"<<endl;

	fout<<"Philosopher "<<phnum+1<< " is putting fork "<<phnum+1<< " and " <<LEFT+1<<" down."<<endl;
	fout<<"Philosopher "<<phnum+1<<" is thinking"<<endl;

	test(LEFT);
	test(RIGHT);

	sem_post(&Mutex);

}


void report(){
	if(num_rounds%25==0){
		cout<<"--------------------- ROUND "<<num_rounds<<" --------------------"<<endl;
		for(int k=0;k<N;k++){
			cout<<"Philosopher "<<k+1<<" has eaten "<<num_times_eaten[k]<<" times."<<endl;
		}
		cout << "----------------------------------------------------"<<endl;
		
		// Print to text file
		
		foutReport<<"--------------------- ROUND "<<num_rounds<<" --------------------"<<endl;
		for(int k=0;k<N;k++){
			foutReport<<"Philosopher "<<k+1<<" has eaten "<<num_times_eaten[k]<<" times."<<endl;
		}
		foutReport << "----------------------------------------------------"<<endl;
	}
}

void* philospher(void* num)
{

	while (1) {

		int* i = (int *) num;

		sleep(1);

		take_fork(*i);
		
		report();
	}
}
void header()
{
    cout<<"CS558 : COMPUTER SYSTEM LAB ASSIGNMENT - 04\n";
    cout<<"Instructor : Dr. Manas Khatua and Dr. Moumita Patra\n";
    cout<<"TA's : Alakesh Kalita, Arunav Saikia, Meenu Rani Dey, Priya Undriwade, Vinay Kumar Gupta\n";
    cout<<"Sumbittted By : Himanshu Patel(204101029), Ketan Karnakota(204101030)\n";
}

int main()
{
	 header();
    cout<<"-----------------------------------------------------------------------------------------\n";
    

	int i;
	pthread_t thread_id[N];

	// initialize the semaphores
	sem_init(&Mutex, 0, 1);

	for (i = 0; i < N; i++)

		sem_init(&S[i], 0, 0);

	fout.open ("output.txt",ios::app);
	if(fout.is_open())
	      fout<<"\n\nFresh Run\n\n";
	else{
		cout<<"Unable to open file output.txt";
		exit(1);
	}
	foutReport.open("report.txt",ios::app);
	if(foutReport.is_open())
	      foutReport<<"\n\nFresh Run\n\n";
	else{
		cout<<"Unable to open file report.txt";
		exit(1);
	}
	
	for (i = 0; i < N; i++) {

		// create philosopher processes
		pthread_create(&thread_id[i], NULL,
					philospher, &phil[i]);

		cout<<"Philosopher "<<i+1 <<" is thinking."<<endl;
		fout<<"Philosopher "<<i+1 <<" is thinking."<<endl;

	}

	for (i = 0; i < N; i++)

		pthread_join(thread_id[i], NULL);
}

