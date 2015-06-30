#include "config.h"
#include "paging.h"
#include "vfs.h"

#ifndef __PROCESS_H_
#define __PROCESS_H_

extern const uint8_t pr_state_paused;
extern const uint8_t pr_state_running;

extern const uint8_t pr_syscall_linux;

struct vm_page_t;

struct pr_file_t {
	uint32_t fid;
	struct vfs_t *file;
	uint8_t mode;
};

struct pr_process_t {
	uint32_t pid;
	uint8_t  state;

	uint32_t kernel_stack;
	uint32_t stack_pointer;

	struct pg_table_t *page_table;
	struct vm_page_t *pages;

	uint32_t syscall_mode;

	// Which pages have been allocated?
	// Kernel memory?

	struct pr_process_t *next_process;
};

void setup_process();

struct pr_process_t *pr_create_process();
struct pr_process_t *pr_fork_process(struct pr_process_t *src);

struct pr_process_t *pr_get_current();

void pr_schedule();
void pr_resume(struct pr_process_t *proc);
void pr_suspend(struct pr_process_t *proc);
void pr_terminate(struct pr_process_t *proc);
uint32_t pr_getpid();

#endif
