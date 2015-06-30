#include "process.h"
#include "process-internal.h"
#include "gdt.h"
#include "memory.h"
#include "kalloc.h"
#include "console.h"

const uint8_t pr_state_paused = 0;
const uint8_t pr_state_running = 1;

const uint8_t pr_syscall_linux = 0;

struct pr_process_t *pr_root;
struct pr_process_t *pr_current;

uint32_t pr_next_pid = 0;
uint16_t pr_tss_desc;

/**
 * Initializes the process manager (mainly TSS)
 */
void setup_process()
{
	pr_root = NULL;
	pr_current = NULL;

	pr_tss_desc = gd_add_entry();

	gd_set_base(pr_tss_desc, (uint32_t) &pr_tss);
	gd_set_limit(pr_tss_desc, sizeof(struct pr_tss_t) - 1);

	gd_set_tss(pr_tss_desc, 
		0, // Granularity
		1, // AVL
		1, // Present
		3, // DPL
		0); // Busy

	gd_reload();

	asm("ltr %w0" : : "r" (0x08 * pr_tss_desc));
}

/**
 * Picks a running process and attempts to resume it
 */
void pr_schedule()
{
	struct pr_process_t *proc = NULL;

	proc = NULL;

	// No processes, return
	if(pr_root == NULL)
		return;

	// Try to load the next process
	if(pr_current != NULL)
		proc = pr_current->next_process;

	// Next process does not exist, load first process
	if(proc == NULL)
		proc = pr_root;

	// Traverse the process tree
	while(proc != pr_current) {
		if(proc->state == pr_state_running)
			pr_resume(proc);

		proc = proc->next_process;

		if(proc == NULL)
			proc = pr_root;
	}

	// Nothing worked, stop the processor
	asm volatile("hlt");
}

/**
 * Saves the current process state and transfers control to another one.
 * This function will *NOT* return, contents of the stack will be lost.
 *
 * @param pr_process_t proc  Process to transfer control to
 */
void pr_resume(struct pr_process_t *proc)
{
	// If a current process is running, store segment registers on its stack
	if(pr_current != NULL) {
		uint32_t *stack = (uint32_t *) (pr_current->kernel_stack + 4096);

		int i;

		for(i = 0; i < 4; i++)
			stack[i - 17] = 0x20;
	}

	// Change process
	pr_current = proc;

	// ESP used when returning (by interrupt), will erase current stack
	pr_tss.esp0 = pr_current->kernel_stack + 4096;
	pr_tss.ss0 = 0x10;

	// Load page directory of target process
	pg_activate_table(pr_current->page_table);

	// Load stack of target process
	asm volatile("movl %0, %%esp" : : "m" (pr_current->stack_pointer));

	asm volatile("pop %es");
	asm volatile("pop %fs");
	asm volatile("pop %gs");
	asm volatile("pop %ds");

	asm volatile("popal");

	asm volatile("sti");

	asm volatile("iret");	
}

/**
 * Returns the PID of the current process. This function handles the 
 * getpid system call for (at least) the POSIX compatilibty layer.
 *
 * @return uint32_t  PID of current process.
 */
uint32_t pr_getpid()
{
	return pr_current->pid;
}

/**
 * This returns a free PID, note that it might have been associated with
 * a previously running process.
 *
 * @return uint32_t  A currently unused PID.
 */
uint32_t pr_free_pid()
{
	return pr_next_pid++;
}

/**
 * Injects a process structure into the process tree
 *
 * @param pr_process_t* proc  Process to inject
 * @return bool  True if process was successfully injected, false otherwise
 */
bool pr_inject_process(struct pr_process_t *proc)
{
	struct pr_process_t *base = pr_root;

	// First process (or has a pid lower than first process; should not happen)
	if(pr_root == NULL || pr_root->pid > proc->pid) {
		proc->next_process = pr_root;
		pr_root = proc;

		return true;
	}

	while(base) {
		if(base->next_process == NULL || base->next_process->pid > proc->pid) {
			proc->next_process = base->next_process;
			base->next_process = proc;

			return true;
		}

		base = base->next_process;
	}

	return false;
}

/**
 * Extracts a processes from the tree
 */
void pr_extract_process(struct pr_process_t *proc)
{
  if(proc == NULL)
	return;

  if(proc == pr_root) {
	pr_root = proc->next_process;
	return;
  }

  struct pr_process_t *base = pr_root;

  while(base) {
	if(base->next_process == proc) {
	  base->next_process = proc->next_process;
	  return;
	}

	base = base->next_process;
  }
}

/**
 * Suspends a process
 */
void pr_suspend(struct pr_process_t *proc)
{
	proc->state = pr_state_paused;

	if(proc == pr_current)
		pr_schedule();
}

struct pr_process_t *pr_get_current()
{
	return pr_current;
}

struct pr_process_t *pr_fork_process(struct pr_process_t *src)
{
	struct pr_process_t *proc = (struct pr_process_t *) kalloc(sizeof(struct pr_process_t));

	if(!pr_inject_process(proc)) {
		kfree(proc);
		return NULL;
	}

	proc->pid = pr_free_pid();
	proc->state = pr_state_paused;		// Start paused! crashes otherwise!

	proc->page_table = pg_copy_user_page_directory(src->page_table);

	// Initialize stack
	uint32_t stack_base = (uint32_t) kalloc(4096);
	uint32_t stack_top = stack_base + 4096;

	uint32_t *stack_dest = (uint32_t *) stack_base;
	uint32_t *stack_src = (uint32_t *) src->kernel_stack;

	for(int i = 0; i < 1024; i++)
		stack_dest[i] = stack_src[i];

	proc->kernel_stack = (uint32_t) stack_base;
	proc->stack_pointer = (uint32_t) stack_base + (src->stack_pointer - src->kernel_stack);

	return proc;
}

/**
 * Creates a new "paused" process
 */
struct pr_process_t *pr_create_process()
{
	struct pr_process_t *proc = (struct pr_process_t *) kalloc(sizeof(struct pr_process_t));

	if(!pr_inject_process(proc)) {
		kfree(proc);
		return NULL;
	}

	proc->pid = pr_free_pid();
	proc->state = pr_state_paused;		// Start paused! crashes otherwise!

	proc->page_table = pg_create_user_page_directory();

	uint32_t stack_base = (uint32_t) kalloc(4096);
	uint32_t stack_top = stack_base + 4096;

	int i;

	uint32_t *stack = (uint32_t *) stack_top;

	*stack--;

	*stack-- = 0x0023;					// SS
	*stack-- = 0x0800;					// ESP (Incorrect value!)
	*stack-- = 0x0202;					// CFLAGS
 	*stack-- = 0x001B;					// CS
	*stack-- = 0; 						// (uint32_t) entrypoint; EIP

	for(i = 0; i < 8; i++)
		*stack-- = 0x0000;

	for(i = 0; i < 4; i++)
		*stack-- = 0x0020;	// Segment selectors

	*stack++;

	proc->kernel_stack = (uint32_t) stack_base;
	
	proc->stack_pointer = (uint32_t) stack;

	return proc;
}

/**
 * Terminates a process
 */
void pr_terminate(struct pr_process_t *proc)
{
  pr_extract_process(proc);

  pg_free_user_page_directory(proc->page_table);
  
  kfree((uint32_t *) proc->kernel_stack);
  kfree(proc);
}
