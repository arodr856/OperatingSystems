#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>


void *father_thread(void *varg);
void *mother_thread(void *varg);

char *mother_tasks[] = {"woken up", "fed breakfast", "sent to school", "given dinner", "given a bath"};
char *father_tasks[] = {"read a book", "tucked in bed"};

sem_t *mother_mutex;
sem_t *father_sem;


int main(int argc, char **argv){

    int number_of_cycles = atoi(argv[1]);
    sem_unlink("m_sem");
    sem_unlink("f_sem");

    mother_mutex = sem_open("m_sem", O_CREAT, 0644, 1);
    father_sem = sem_open("f_sem", O_CREAT, 0644, 0);

    pthread_t f_thread;
    pthread_t m_thread;

    pthread_create(&m_thread, NULL, mother_thread, &number_of_cycles);
    pthread_create(&f_thread, NULL, father_thread, &number_of_cycles);
    
    pthread_join(m_thread, NULL);
    pthread_join(f_thread, NULL);
    
    sem_close(mother_mutex);
    sem_close(father_sem);

    sem_unlink("m_sem");
    sem_unlink("f_sem");
    
    return 0;
}

void *mother_thread(void *varg){ 
    
    int cycles = *((int *) varg);
    
    for(int day = 0; day < cycles; day++){
        sem_wait(mother_mutex);
        printf("This is day #%d of a day in the life of Mother Hubbard\n\n", (day + 1));
        printf("Father is going to sleep and waking up Mother to take care of the children. . .\n\n");
        for(int task = 0; task < 5; task++){
            for(int child = 0; child < 12; child++){
                printf("Mother: Child #%d was %s\n\n", (child + 1), mother_tasks[task]);
                usleep(100);
                if(task == 4){
                    if(child == 11)
                        printf("Mother is taking a nap break. . .\n\n");
                    sem_post(father_sem);
                }
            } // end of children for loop
        } // end of tasks for loop
        // printf("Mother is taking a nap break. . .\n\n");
    } // end of days for loop
    return NULL;  
} 

void *father_thread(void *varg){ 

    int cycles = *((int *) varg);

    // sem_wait(father_sem);
    for(int day = 0; day < cycles; day++){
        for(int child = 0; child < 12; child++){
            sem_wait(father_sem);
            if(child == 0){
                printf("Father is waking up to take care of the children. . .\n\n");
            }
            for(int task = 0; task < 2; task++){
                printf("Father: Child #%d was %s\n\n", (child + 1), father_tasks[task]);
            }
        }
        sem_post(mother_mutex);
    }
   return NULL;
} 

