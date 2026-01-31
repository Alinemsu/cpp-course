#ifndef TASK_SCHEDULER_HPP
#define TASK_SCHEDULER_HPP

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct Task_scheduler Task_scheduler;

    typedef struct Task
    {
        int id;
        char name[16];
        int period_ms;
        int next_run_ms;
    } Task;

    Task_scheduler *task_scheduler_create(void);
    void task_scheduler_destroy(Task_scheduler *task_scheduler);
    int task_add(Task_scheduler *task_scheduler,
                 const char *name,
                 int period_ms,
                 int next_run_ms,
                 int *out_id);
    int task_remove(Task_scheduler *task_scheduler,
                    int id);
    int task_get(Task_scheduler *task_scheduler,
                 int id,
                 Task *out_task);
    int task_count(Task_scheduler *task_scheduler);
    void task_scheduler_update(Task_scheduler *task_scheduler,
                               int current_ms);
    int task_scheduler_pop_ready_tasks(Task_scheduler *task_scheduler,
                                       Task *out_tasks,
                                       int max_count_tasks);
    int task_scheduler_ready_count_tasks_in_queue(Task_scheduler *task_scheduler);

#ifdef __cplusplus
}
#endif
#endif
