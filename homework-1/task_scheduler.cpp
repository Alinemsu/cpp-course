#include "task_scheduler.hpp"

#include <unordered_map>
#include <set>
#include <deque>

class TaskSchedulerImpl {
private:
    std::unordered_map<int, Task> task_by_id;

    struct schedule_key {
        int next_run_ms;
        int id;
    };

    struct schedule_by_key_less {
        bool operator()(const schedule_key& a, const schedule_key& b) const {
            if (a.next_run_ms < b.next_run_ms) {
                return true;
            }
            if (a.next_run_ms > b.next_run_ms) {
                return false;
            }
            return a.id < b.id;
        }
    };

    std::set<schedule_key, schedule_by_key_less> schedule_set;
    std::deque<Task> ready_queue;

    int next_task_id = 1;

    void write_task_name(char array[16], const char* name) {
        if (!name) {
            array[0] = '\0';
            return;
        }

        int i = 0;
        for (; i < 15 && name[i] != '\0'; ++i) {
            array[i] = name[i];
        }
        array[i] = '\0';
    }

public:
    TaskSchedulerImpl() {}
    ~TaskSchedulerImpl() {}

    int add_task_impl(const char* name, int period_ms, int next_run_ms, int* out_id) {
        if (out_id == NULL) {
            return -1;
        }
        if (period_ms < 0) {
            return -1;
        }
        if (next_run_ms < 0) {
            return -1;
        }

        int id = next_task_id;
        ++next_task_id;

        Task new_task;
        new_task.id = id;
        write_task_name(new_task.name, name);
        new_task.next_run_ms = next_run_ms;
        new_task.period_ms = period_ms;

        task_by_id[id] = new_task;

        schedule_key key;
        key.next_run_ms = next_run_ms;
        key.id = id;

        schedule_set.insert(key);

        *out_id = id;
        return 0;
    }

    int get_task_impl(int id, Task* got_task) {
        if (got_task == NULL) {
            return -1;
        }

        std::unordered_map<int, Task>::iterator it = task_by_id.find(id);
        if (it == task_by_id.end()) {
            return -1;
        }

        *got_task = it->second;
        return 0;
    }

    int task_count_impl() {
        return (int)task_by_id.size();
    }

    int remove_task_impl(int id) {
        std::unordered_map<int, Task>::iterator it = task_by_id.find(id);
        if (it == task_by_id.end()) {
            return -1;
        }

        schedule_key key;
        key.id = id;
        key.next_run_ms = it->second.next_run_ms;
        schedule_set.erase(key);

        std::deque<Task>::iterator qit = ready_queue.begin();
        while (qit != ready_queue.end()) {
            if (qit->id == id) {
                qit = ready_queue.erase(qit);
            } else {
                ++qit;
            }
        }

        task_by_id.erase(it);
        return 0;
    }

    void task_update_impl(int current_ms) {
        while (!schedule_set.empty()) {
            std::set<schedule_key, schedule_by_key_less>::iterator it_key = schedule_set.begin();
            if (it_key->next_run_ms > current_ms) {
                break;
            }

            int id = it_key->id;
            schedule_set.erase(it_key);

            std::unordered_map<int, Task>::iterator it_task = task_by_id.find(id);
            if (it_task == task_by_id.end()) {
                continue;
            }

            ready_queue.push_back(it_task->second);

            if (it_task->second.period_ms > 0) {
                it_task->second.next_run_ms =
                    it_task->second.next_run_ms + it_task->second.period_ms;

                schedule_key new_key;
                new_key.next_run_ms = it_task->second.next_run_ms;
                new_key.id = id;

                schedule_set.insert(new_key);
            } else {
                task_by_id.erase(it_task);
            }
        }
    }

    int task_ready_count_impl() {
        return (int)ready_queue.size();
    }

    int pop_ready_tasks_impl(Task* out_tasks, int max_count_tasks) {
        if (out_tasks == NULL) {
            return -1;
        }
        if (max_count_tasks <= 0) {
            return 0;
        }

        int count = 0;
        while (count < max_count_tasks && !ready_queue.empty()) {
            out_tasks[count] = ready_queue.front();
            ready_queue.pop_front();
            ++count;
        }

        return count;
    }
};

typedef struct Task_scheduler {
    TaskSchedulerImpl* impl;
} Task_scheduler;

extern "C" {

Task_scheduler* task_scheduler_create(void) {
    Task_scheduler* s = new Task_scheduler;
    s->impl = new TaskSchedulerImpl();
    return s;
}

void task_scheduler_destroy(Task_scheduler* task_scheduler) {
    if (!task_scheduler) {
        return;
    }
    delete task_scheduler->impl;
    delete task_scheduler;
}

int task_add(Task_scheduler* task_scheduler,
             const char* name,
             int period_ms,
             int next_run_ms,
             int* out_id) {
    if (!task_scheduler || !task_scheduler->impl) {
        return -1;
    }
    return task_scheduler->impl->add_task_impl(name, period_ms, next_run_ms, out_id);
}

int task_remove(Task_scheduler* task_scheduler, int id) {
    if (!task_scheduler || !task_scheduler->impl) {
        return -1;
    }
    return task_scheduler->impl->remove_task_impl(id);
}

int task_get(Task_scheduler* task_scheduler, int id, Task* out_task) {
    if (!task_scheduler || !task_scheduler->impl) {
        return -1;
    }
    return task_scheduler->impl->get_task_impl(id, out_task);
}

int task_count(Task_scheduler* task_scheduler) {
    if (!task_scheduler || !task_scheduler->impl) {
        return -1;
    }
    return task_scheduler->impl->task_count_impl();
}

void task_scheduler_update(Task_scheduler* task_scheduler, int current_ms) {
    if (!task_scheduler || !task_scheduler->impl) {
        return;
    }
    task_scheduler->impl->task_update_impl(current_ms);
}

int task_scheduler_pop_ready_tasks(Task_scheduler* task_scheduler,
                                   Task* out_tasks,
                                   int max_count_tasks) {
    if (!task_scheduler || !task_scheduler->impl) {
        return -1;
    }
    return task_scheduler->impl->pop_ready_tasks_impl(out_tasks, max_count_tasks);
}

int task_scheduler_ready_count_tasks_in_queue(Task_scheduler* task_scheduler) {
    if (!task_scheduler || !task_scheduler->impl) {
        return -1;
    }
    return task_scheduler->impl->task_ready_count_impl();
}

} 
