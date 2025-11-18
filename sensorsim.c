#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/utsname.h>
#include <linux/cdev.h>
#include <linux/timer.h>

void Sensor_TimerCallback(struct timer_list *timer);

static u32 _randomSensorValue = 0;
static ssize_t sensorValue_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf);
static ssize_t sensorValue_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count);

unsigned int _period = 500;
static ssize_t period_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf);
static ssize_t period_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count);


static struct timer_list myTimer;

void Sensor_TimerCallback(struct timer_list *timer)
{
    _randomSensorValue = get_random_u32();
    pr_debug("SensorSim: new random value: %d\n", _randomSensorValue);
    mod_timer(&myTimer, jiffies + msecs_to_jiffies(_period));
}

struct kobject* kobj_ref;
struct kobj_attribute sensorValue = __ATTR(sensorValue, 0660, sensorValue_show, sensorValue_store);
struct kobj_attribute period = __ATTR(period, 0660, period_show, period_store);

static ssize_t sensorValue_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%d", _randomSensorValue);
}

static ssize_t sensorValue_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count)
{
    return count;
}

static ssize_t period_show(struct kobject* kobj, struct kobj_attribute* attr, char* buf)
{
    return sprintf(buf, "%d", _period);
}

static ssize_t period_store(struct kobject* kobj, struct kobj_attribute* attr, const char* buf, size_t count)
{
    unsigned int tempValue = 0;
    if(0 < sscanf(buf, "%d", &tempValue))
    {
        if(tempValue >= 100 && tempValue <= 5000)
        {
            _period = tempValue;
            pr_info("SensorSim: period set to %d ms\n", _period);
        }
    }
    return count;
}

static int setup_sysfs(void)
{
    kobj_ref = kobject_create_and_add("sensorsim", kernel_kobj);
    //value
    if(sysfs_create_file(kobj_ref, &sensorValue.attr)){
        kobject_put(kobj_ref);
        sysfs_remove_file(kernel_kobj, &sensorValue.attr);
        return -1;
    }
    //period
    if(sysfs_create_file(kobj_ref, &period.attr)){
        kobject_put(kobj_ref);
        sysfs_remove_file(kernel_kobj, &period.attr);
        return -1;
    }
    return 0;
}

static int my_init(void)
{
    timer_setup(&myTimer, Sensor_TimerCallback, 0);
    mod_timer(&myTimer, jiffies + msecs_to_jiffies(1000));
    _randomSensorValue = get_random_u32();
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
