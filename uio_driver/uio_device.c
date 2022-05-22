/*
 * This module creates a user input/output device: https://www.kernel.org/doc/html/v4.13/driver-api/uio-howto.html
 * This type of device gives the possibility to handle a fake or virtual interrupt on the device created.
 * In fact, real interrupts are only handled on real devices.
 * To compile the module and the user app type: make 
 * After what you can insert it by doing: sudo insmod uio_device.ko 
 * Before inserting an uio device, you have to load the uio module: sudo modprobe uio
 * If everything is ok, this will create the device at /dev/uio0
 * You can then launch the user app with: sudo ./uio_user "/dev/uio0"
 */

/* Files to include for module primitives. */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/uio_driver.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/pid.h>
#include <linux/preempt.h>

#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/timer.h>

#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TP Module UIO");
MODULE_DESCRIPTION("UIO driver");

#define MAXPID 10

static struct uio_info* info; //structure to create and register a new uio device
static struct platform_device *pdev; //the device to be registered
static struct preempt_notifier notifier; //key for installing preemption notifiers

void *shared_mem_buffer = NULL; //the space shared with the user space
unsigned long *fill_buffer = NULL;//this array will be mapped to the shared_mm_buff to fill it

static int    pidarray[MAXPID] = { 0,0,0,0,0,0,0,0,0,0 }; //this array will contain the list of pids passed as arguments
module_param_array(pidarray, int, NULL, 0);
MODULE_PARM_DESC(pidarray, "Array of pids to be tracked");

/* When releasing the module, method to unregister the uio device. */
void unregister_uio(void)
{
    /* Unregister the device and free the uio_info variable */
    if (info) {
        uio_unregister_device(info);
        kfree(info);
    }
}

static int uio_device_open(struct uio_info *info, struct inode *inode)
{
    pr_info("%s called\n", __FUNCTION__);
    return 0;
}

static int uio_device_release(struct uio_info *info, struct inode *inode)
{
    pr_info("%s called\n", __FUNCTION__);
    return 0;
}

int uio_hotplug_mmap(struct uio_info *info, struct vm_area_struct *vma){
    unsigned long len, pfn;
    int ret ;

    len = vma->vm_end - vma->vm_start;
    pfn = virt_to_phys((void *)shared_mem_buffer)>>PAGE_SHIFT;

    ret = remap_pfn_range(vma, vma->vm_start, pfn, len, vma->vm_page_prot);
    if (ret < 0) {
        pr_err("could not map the address area\n");
        kfree(shared_mem_buffer);
        return -EIO;
    }

    pr_info("memory map called success \n");

    return ret;
}

/*****************************************************************************/
/* The function to be called when a tracked proccess is scheduled in by the OS */
static void sched_in_notify(struct preempt_notifier *notifier, int cpu_curr)
{
    fill_buffer[0] = 1;
    fill_buffer[1] = current->pid;
    uio_event_notify(info);
}

/* The function to be called when a tracked proccess is scheduled out by the OS */
static void sched_out_notify(struct preempt_notifier *notifier, struct task_struct *next)
{
    fill_buffer[0] = 0;
    fill_buffer[1] = current->pid;
    uio_event_notify(info);
}

static struct preempt_ops operations = {
    .sched_in = sched_in_notify,
    .sched_out = sched_out_notify,
};

/**
 * preempt_notifier_register_process - tell me when a process (task) is being preempted & rescheduled
 * @notifier: notifier struct to register
 */
void preempt_notifier_register_process(struct preempt_notifier *notifier, struct task_struct *task)
{
    hlist_add_head(&notifier->link, &task->preempt_notifiers);
}

/**
 * preempt_notifier_register_process - tell me when a process (task) is being preempted & scheduled_out
 * @notifier: notifier struct to register
 */
void preempt_notifier_unregister_process(struct task_struct *task)
{
     hlist_del(task->preempt_notifiers);
}

/**
 * find_process_by_pid - find a process with a matching PID value.
 * @pid: the pid in question.
 *
 * The task of @pid, if found. %NULL otherwise.
 */
static struct task_struct *find_process_by_pid(pid_t pid)
{
	return pid ? find_task_by_vpid(pid) : current;//we needed to export this function from the core-kernel (EXPORT_SYMBOL_GPL(find_task_by_vpid))
}
/*****************************************************************************/

static int __init uio_device_init(void)
{
    int i = 0;
    struct task_struct *task;

    /* Create and register a device. */
    pdev = platform_device_register_simple("pdev", -1, NULL, 0);

    if (IS_ERR(pdev)) {
        platform_device_unregister(pdev);
        return errno;
    }

    /* Allocate memory for the device */
    info = kzalloc(sizeof(struct uio_info), GFP_KERNEL); //use kzalloc
    
    if (!info)
        return -ENOMEM;
    
    /* Allocate memory for the shared buffer */
    shared_mem_buffer = kzalloc(, GFP_KERNEL);
    if (!shared_mem_buffer) {
        return -ENOMEM;
    }

    fill_buffer = (unsigned long *) shared_mem_buffer; 

    /* Initialize the fields. */
    info->name = "uio_driver";
    info->version = "0.0.1";
    info->mem[0].memtype = UIO_MEM_LOGICAL;
    info->mem[0].addr = (phys_addr_t) shared_mem_buffer;
    info->mem[0].size = sizeof(struct uio_info);
    info->irq = UIO_IRQ_CUSTOM;
    info->handler = 0;
    info->open = uio_device_open;
    info->release = uio_device_release;
	
    /* Register the new userspace IO device -- @info: UIO device capabilities */
    uio_register_device(pdev, info);

    /* Preemption initialization */
    preempt_notifier_inc();
    preempt_notifier_init(&notifier, &operations);

    /* Parse the array argument to get the pids */
    while( pidarray[i] ){
        rcu_read_lock(); //always take a lock when working on struct task_struct
        task = find_process_by_pid(pidarray[i]);
        if(task)
            preempt_notifier_register_process(&notifier, task)
        rcu_read_unlock();
        i++;
    }

    pr_info("UIO device loaded\n");
    return 0;

devmem:
    kfree(info);    
    return -ENODEV;
}

static void __exit uio_device_cleanup(void)
{
   if (pdev)
        platform_device_unregister(pdev)

   if (shared_mem_buffer)
        kfree(shared_mem_buffer)

   while( pidarray[k] ){rcu_read_lock();
       proc = find_process_by_pid(pidarray[k]);
       if(proc)
           preempt_notifier_unregister_process(proc);
       rcu_read_unlock();
       k++;
    }

   /* decrement the preempted notifications */
   preempt_notifier_dec();
   
   pr_info("Cleaning up module.\n");
}

module_init(uio_device_init);
module_exit(uio_device_cleanup);
