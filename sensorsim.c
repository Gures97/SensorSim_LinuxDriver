#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/utsname.h>
#include <linux/cdev.h>
#include <linux/timer.h>

#define RANDVAL_STRING_BUFSIZE 10


static ssize_t sysfs_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf);
static ssize_t sysfs_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count);

static u32 randValue = 0;
static char randValStringBuf[RANDVAL_STRING_BUFSIZE]; 

static struct timer_list myTimer;

struct kobject* kobj_ref;
struct kobj_attribute sensorValue = __ATTR(sensorValue, 0660, sysfs_show, sysfs_store);

void Sensor_TimerCallback(struct timer_list *timer)
{
    randValue = get_random_u32();
    printk("SensorSim: new random value\n");
    mod_timer(&myTimer, jiffies + msecs_to_jiffies(500));
}

static ssize_t sysfs_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%d", randValue);
}

static ssize_t sysfs_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count)
{
    return count;
}

static int setup_sysfs(void)
{
    kobj_ref = kobject_create_and_add("sensorsim", kernel_kobj);
    if(sysfs_create_file(kobj_ref, &sensorValue.attr)){
        kobject_put(kobj_ref);
        sysfs_remove_file(kernel_kobj, &sensorValue.attr);
        return -1;
    }
    return 0;
}

static int my_init(void)
{
    timer_setup(&myTimer, Sensor_TimerCallback, 0);
    mod_timer(&myTimer, jiffies + msecs_to_jiffies(1000));
    randValue = get_random_u32();
    if(setup_sysfs() < 0){
        pr_warn("SensorSim: Creating sysfs file failed!\n");
        return -1;
    }
    pr_info("SensorSim: Module loaded\n");

    return 0;
}

static void my_cleanup(void)
{
    del_timer(&myTimer);
    kobject_put(kobj_ref);
    sysfs_remove_file(kernel_kobj, &sensorValue.attr);
}

module_init(my_init);
module_exit(my_cleanup);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gures97");
MODULE_DESCRIPTION("Sensor Simulator Module");
MODULE_VERSION("1.0");
