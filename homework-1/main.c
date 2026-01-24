#include <stdio.h>
#include "task_scheduler.hpp"

void test_add_get_count(void)
{
    Task_scheduler* scheduler = task_scheduler_create();
    if (!scheduler) {
        printf("FAIL: create\n");
        return;
    }

    int periodic_id = 0;
    int one_time_id = 0;

    if (task_add(scheduler, "PERIODIC", 100, 0, &periodic_id) != 0) {
        printf("FAIL: add periodic\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (task_add(scheduler, "ONE_TIME", 0, 50, &one_time_id) != 0) {
        printf("FAIL: add one_time\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (task_count(scheduler) != 2) {
        printf("FAIL: count != 2\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    Task t;

    if (task_get(scheduler, periodic_id, &t) != 0) {
        printf("FAIL: get periodic\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (t.period_ms != 100 || t.next_run_ms != 0) {
        printf("FAIL: fields periodic\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (task_get(scheduler, one_time_id, &t) != 0) {
        printf("FAIL: get one_time\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (t.period_ms != 0 || t.next_run_ms != 50) {
        printf("FAIL: fields one_time\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    printf("OK: test_add_get_count\n");
    task_scheduler_destroy(scheduler);
}

void test_update_pop_one_time_removed(void)
{
    Task_scheduler* scheduler = task_scheduler_create();
    if (!scheduler) {
        printf("FAIL: create\n");
        return;
    }

    int periodic_id = 0;
    int one_time_id = 0;

    if (task_add(scheduler, "PER", 100, 0, &periodic_id) != 0) {
        printf("FAIL: add periodic\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (task_add(scheduler, "ONE", 0, 50, &one_time_id) != 0) {
        printf("FAIL: add one_time\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    Task ready[8];
    int n = 0;

    task_scheduler_update(scheduler, 0);
    n = task_scheduler_pop_ready_tasks(scheduler, ready, 8);
    if (n != 1 || ready[0].id != periodic_id) {
        printf("FAIL: now=0 pop\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    task_scheduler_update(scheduler, 50);
    n = task_scheduler_pop_ready_tasks(scheduler, ready, 8);
    if (n != 1 || ready[0].id != one_time_id) {
        printf("FAIL: now=50 pop\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    /* after ONE runs once, it should be removed */
    if (task_count(scheduler) != 1) {
        printf("FAIL: one_time not removed\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    printf("OK: test_update_pop_one_time_removed\n");
    task_scheduler_destroy(scheduler);
}
void test_remove(void)
{
    Task_scheduler* scheduler = task_scheduler_create();
    if (!scheduler) {
        printf("FAIL: create\n");
        return;
    }

    int task_id = 0;

    if (task_add(scheduler, "PER", 100, 0, &task_id) != 0) {
        printf("FAIL: add\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (task_count(scheduler) != 1) {
        printf("FAIL: count != 1\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (task_remove(scheduler, task_id) != 0) {
        printf("FAIL: remove\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    if (task_count(scheduler) != 0) {
        printf("FAIL: count != 0\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    Task t;
    if (task_get(scheduler, task_id, &t) == 0) {
        printf("FAIL: get after remove should fail\n");
        task_scheduler_destroy(scheduler);
        return;
    }

    printf("OK: test_remove\n");
    task_scheduler_destroy(scheduler);
}
int main(void)
{
    test_add_get_count();
    test_update_pop_one_time_removed();
    test_remove();
    return 0;
}

