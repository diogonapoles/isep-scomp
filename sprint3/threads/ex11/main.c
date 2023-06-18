#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 5
#define NUM_EXAMS 300

int g_notif = 0;
int g_positive = 0;
int g_negative = 0;
pthread_mutex_t g_mutex_1;
pthread_mutex_t g_mutex_2;

typedef struct {
  int number;
  int g1Result;
  int g2Result;
  int g3Result;
  int finalGrade;
} Exam;

Exam exams[NUM_EXAMS];

void *generate_exam(void *arg) {
  if (pthread_mutex_lock(&g_mutex_1) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  for (int i = 0; i < NUM_EXAMS; i++) {
    exams[i].number = i;
    exams[i].g1Result = (rand() % 100);
    exams[i].g2Result = (rand() % 100);
    exams[i].g3Result = (rand() % 100);
  }

  if (pthread_mutex_unlock(&g_mutex_1) != 0) {
    perror("pthread_mutex_lock");
    exit(EXIT_FAILURE);
  }

  pthread_exit((void *)NULL);
}

void *compute_grade(void *arg) {
  if (pthread_mutex_lock(&g_mutex_2) != 0) {
    perror("Error locking mutexT2T3.\n");
  }

  for (int i = 0; i < NUM_EXAMS; i++) {
    exams[i].finalGrade =
        (exams[i].g1Result + exams[i].g2Result + exams[i].g3Result) / 3;
  }

  if (pthread_mutex_unlock(&g_mutex_2) != 0) {
    perror("Error locking mutexT2T3.\n");
  }

  pthread_exit((void *)NULL);
}


void* positive_grades(void *arg) {
    for (int i = 0; i < NUM_EXAMS; i++) {
        if (exams[i].finalGrade >= 50) 
            g_positive++;
    }

    pthread_exit((void*) NULL);
}

int neg = 0;
void* negative_grades(void* arg) {
    for (int i = 0; i < NUM_EXAMS; i++) {
        if (exams[i].finalGrade < 50)
            g_negative++;
    }

    pthread_exit((void*) NULL);
}

int main() {
  pthread_t threads[NUM_THREADS];

  if (pthread_mutex_init(&g_mutex_1, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_init(&g_mutex_2, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&threads[0], NULL, generate_exam, NULL)) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(threads[0], NULL)) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&threads[1], NULL, compute_grade, NULL)) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(threads[1], NULL)) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < NUM_EXAMS; i++) {
    printf("Student: %d\n", exams[i].number);
    printf("Grade: %d\n", exams[i].finalGrade);
  }

  if (pthread_create(&threads[3], NULL, positive_grades, NULL)) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&threads[4], NULL, negative_grades, NULL)) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(threads[3], NULL)) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(threads[4], NULL)) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  double pos = (g_positive * 100) / NUM_EXAMS;
  double neg = (g_negative * 100) / NUM_EXAMS;

  printf("Positive grades: %.2f%%\n", pos);
  printf("Negative grades: %.2f%%\n", neg);

  if (pthread_mutex_destroy(&g_mutex_1) != 0) {
    perror("pthread_mutex_destroy");
    exit(EXIT_FAILURE);
  }

  if (pthread_mutex_destroy(&g_mutex_2) != 0) {
    perror("pthread_mutex_destroy");
    exit(EXIT_FAILURE);
  }
}