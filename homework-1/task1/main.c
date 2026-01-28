#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "task_scheduler.hpp"

#define FAIL(msg) printf("FAIL: %s\n", msg)

#define TEST_FAIL(msg)\
 {\
    printf("FAIL: %s\n", (msg)); \
    task_scheduler_destroy(scheduler);\
    return;\
 }


void test_add_get_count(void){
    Task_scheduler* scheduler = task_scheduler_create();
    if (!scheduler) {
       FAIL("create");
       return;
    }

    int periodic_id = 0;
    int one_time_id = 0;

    if (task_add(scheduler, "PERIODIC", 100, 0, &periodic_id) != 0) {
       TEST_FAIL("add periodic");
    }

    if (task_add(scheduler, "ONE_TIME", 0, 50, &one_time_id) != 0) {
        TEST_FAIL("add one_time");
    }

    if (task_count(scheduler) != 2) {
        TEST_FAIL("count != 2");
    }

    Task t;

    if (task_get(scheduler, periodic_id, &t) != 0) {
        TEST_FAIL("get periodic");
    }

    if (t.period_ms != 100 || t.next_run_ms != 0) {
        TEST_FAIL("fields periodic");
    }

    if (task_get(scheduler, one_time_id, &t) != 0) {
       TEST_FAIL("get one_time");
    }

    if (t.period_ms != 0 || t.next_run_ms != 50) {
       TEST_FAIL("fields one_time");
    }

    printf("OK: test_add_get_count\n");
    task_scheduler_destroy(scheduler);
}

void test_update_pop_one_time_removed(void){
    Task_scheduler* scheduler = task_scheduler_create();
    if (!scheduler) {
       FAIL("create");
        return;
    }

    int periodic_id = 0;
    int one_time_id = 0;

    if (task_add(scheduler, "PER", 100, 0, &periodic_id) != 0) {
        TEST_FAIL("FAIL: add periodic");
    }

    if (task_add(scheduler, "ONE", 0, 50, &one_time_id) != 0) {
        TEST_FAIL("FAIL: add one_time");
    }

    Task ready[8];
    int n = 0;

    task_scheduler_update(scheduler, 0);
    n = task_scheduler_pop_ready_tasks(scheduler, ready, 8);
    if (n != 1 || ready[0].id != periodic_id) {
        TEST_FAIL("FAIL: now=0 pop");
    }

    task_scheduler_update(scheduler, 50);
    n = task_scheduler_pop_ready_tasks(scheduler, ready, 8);
    if (n != 1 || ready[0].id != one_time_id) {
         TEST_FAIL("FAIL: now=50 pop");
    }

    /* after ONE runs once, it should be removed */
    if (task_count(scheduler) != 1) {
       TEST_FAIL("FAIL: one_time not removed\n");
    }

    printf("OK: test_update_pop_one_time_removed\n");
    task_scheduler_destroy(scheduler);
}

void test_remove(void){
    Task_scheduler* scheduler = task_scheduler_create();
    if (!scheduler) {
        FAIL("create");
        return;
    }

    int task_id = 0;

    if (task_add(scheduler, "PER", 100, 0, &task_id) != 0) {
       TEST_FAIL("FAIL: add");
    }

    if (task_count(scheduler) != 1) {
       TEST_FAIL("FAIL: count != 1");
    }

    if (task_remove(scheduler, task_id) != 0) {
        TEST_FAIL("FAIL: remove");
    }

    if (task_count(scheduler) != 0) {
       TEST_FAIL("FAIL: count != 0");
    }

    Task t;
    if (task_get(scheduler, task_id, &t) == 0) {
       TEST_FAIL("FAIL: get after remove should fail");
    }

    printf("OK: test_remove\n");
    task_scheduler_destroy(scheduler);
}

void random(void){
    Task_scheduler* scheduler = task_scheduler_create();
    if (!scheduler) {
        FAIL("create");
        return;
    }

    srand((unsigned)time(NULL));

    printf(" test_random\n");

    for (int i = 0; i < 5; ++i) {
        int task_id = 0;
        int period = (rand() % 991) + 10; 

        if (task_add(scheduler, "PER", period, 0, &task_id) != 0) {
            TEST_FAIL("add");
        }

        printf("id = %d  period = %d ms\n", task_id, period);
    }
           
    Task ready[5];
    int capacity = 5;

    for (int сurrent_ms = 0; сurrent_ms <= 2000; сurrent_ms += 10) {
        task_scheduler_update(scheduler, сurrent_ms);

        int n = task_scheduler_pop_ready_tasks(scheduler, ready, capacity);
        if (n > 0) {
            printf("for t = %d ms indices of ready tasks: ", сurrent_ms);
            for (int k = 0; k < n; ++k) {
                printf("%d ", ready[k].id);
            }
            printf("\n");
        }
    }

    task_scheduler_destroy(scheduler);
}


int main(void){
    test_add_get_count();
    test_update_pop_one_time_removed();
    test_remove();
    random();
    return 0;
}

