#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <asm/uaccess.h>

#ifndef SET_MODULE_OWNER
#  define SET_MODULE_OWNER(structure) /* nothing */
#endif

#ifdef NEED_VERSION
	static char kernel_version[] = UTS_RELEASE;
#endif

#define BUF_SIZE (size_t) 511
#define N_DEVS (int) 16

DECLARE_WAIT_QUEUE_HEAD (gc100_read_queue);

struct gc100_dev
{
	struct semaphore sem;
	int minor_number;
};

static int gc100_major;
static int * gc100_s_to_d_flag;
static int * gc100_d_to_s_flag;

static struct gc100_dev * gc100_devices;

// Try using larger buffers
static char gc100_s_to_d_buf[N_DEVS][BUF_SIZE+1];
static char gc100_d_to_s_buf[N_DEVS][BUF_SIZE+1];
static int gc100_d_to_s_pos[N_DEVS];
static int gc100_s_to_d_pos[N_DEVS];
static int gc100_d_to_s_len[N_DEVS];
static int gc100_s_to_d_len[N_DEVS];

ssize_t gc100_read(struct file * filp, char * buffer, size_t length, loff_t * offset)
{
	struct gc100_dev * dev = filp->private_data;
	int minor_number, which_driver_set;
	int bufreadsize;
	ssize_t ret = 0;

    if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	
	printk("<1> Read PID: %d\n", current->pid);
	
	minor_number = dev->minor_number & 0x0f;
	which_driver_set = (int) (minor_number / 2);
	bufreadsize = min(length, BUF_SIZE);
	
    if (minor_number % 2 == 0)
	{ // s reading
		if (gc100_d_to_s_flag[which_driver_set] == 1)
		{
			bufreadsize = min(bufreadsize,(gc100_d_to_s_len[which_driver_set] - gc100_d_to_s_pos[which_driver_set]));  // max size is what's in the buffer
			bufreadsize = 1; // DEBUG

			/*if (copy_to_user(buf, &gc100_d_to_s_data[which_driver_set], 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_to_user(buffer, &gc100_d_to_s_buf[which_driver_set][gc100_d_to_s_pos[which_driver_set]], bufreadsize))
			{
				ret = -EFAULT;
				goto out;
			}
			gc100_d_to_s_pos[which_driver_set] += bufreadsize;

			// If the buffer's been emptied, it's ready for another read
			if (gc100_d_to_s_len[which_driver_set] == gc100_d_to_s_pos[which_driver_set])
			{
				gc100_d_to_s_flag[which_driver_set] = 0;
			}
			ret = bufreadsize;
		}
		else
		{
			// buffer is not ready
			ret = 0;
		}
	}
	else
	{ // d reading
		if (gc100_s_to_d_flag[which_driver_set] == 1)
		{
			bufreadsize = min(bufreadsize, (gc100_s_to_d_len[which_driver_set] - gc100_s_to_d_pos[which_driver_set]));  // max size is what's in the buffer
			bufreadsize = 1; // DEBUG

			/*if (copy_to_user(buf, &gc100_s_to_d_data[which_driver_set], 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_to_user(buffer, &gc100_s_to_d_buf[which_driver_set][gc100_s_to_d_pos[which_driver_set]], bufreadsize))
			{
				ret = -EFAULT;
				goto out;
			}
			gc100_s_to_d_pos[which_driver_set] += bufreadsize;
			if (gc100_s_to_d_len[which_driver_set] == gc100_s_to_d_pos[which_driver_set])
			{
				gc100_s_to_d_flag[which_driver_set] = 0;
			}
			ret = bufreadsize;
		}
		else
		{
			// buffer is not ready
			ret = 0;
		}
	}
	
out:
	up(&dev->sem);
	return ret;
}

ssize_t gc100_write(struct file * filp, const char * buffer, size_t length, loff_t * f_pos)
{
    struct gc100_dev * dev = filp->private_data;
	ssize_t ret = -ENOMEM; /* value used in "goto out" statements */

	int which_driver_set, minor_number;
	int bufwritesize;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	printk("<1> Write PID: %d\n", current->pid);
	
	minor_number = dev->minor_number & 0x0f;
	which_driver_set = (int) (minor_number / 2);
	bufwritesize = min(length, BUF_SIZE);

	if (minor_number % 2 == 0)
	{
		if (gc100_s_to_d_flag[which_driver_set] == 0)
		{
			/*if (copy_from_user(&gc100_s_to_d_data[which_driver_set], buf, 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_from_user(&gc100_s_to_d_buf[which_driver_set][0], buffer, bufwritesize))
			{
				ret = -EFAULT;
				goto out;
			}
			gc100_s_to_d_len[which_driver_set] = bufwritesize;
			gc100_s_to_d_pos[which_driver_set] = 0;
			gc100_s_to_d_flag[which_driver_set] = 1;

			ret = bufwritesize;
		}
		else
		{
			// buffer is full
			ret = 0;
		}
	}
	else
	{
		if (gc100_d_to_s_flag[which_driver_set] == 0)
		{
			/*if (copy_from_user(&gc100_d_to_s_data[which_driver_set], buf, 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_from_user(&gc100_d_to_s_buf[which_driver_set][0], buffer, bufwritesize))
			{
				ret = -EFAULT;
				goto out;
			}
			gc100_d_to_s_flag[which_driver_set] = 1;
			gc100_d_to_s_pos[which_driver_set] = 0;
			gc100_d_to_s_len[which_driver_set] = bufwritesize;
			ret = bufwritesize;
		}
		else
		{
			// buffer is full
			ret = 0;
		}
	}

out:
	up(&dev->sem);
	return ret;
}

static int gc100_device_open(struct inode * inode, struct file * filp)
{
	struct gc100_dev * dev;
	int num = MINOR(inode->i_rdev);

	printk("<1> Open PID: %d\n", current->pid);
    dev = (struct gc100_dev *) filp->private_data;
	if (!dev)
	{
		if (num >= N_DEVS)
			return -ENODEV;
		dev = &gc100_devices[num];
		filp->private_data = dev; /* for other methods */
	}
	dev->minor_number = MINOR(inode->i_rdev);
	
	try_module_get(THIS_MODULE);
//	MOD_INC_USE_COUNT;
	
	return 0;
}

static int gc100_device_release(struct inode * inode, struct file * filp)
{
	printk("<1> Close PID: %d\n", current->pid);
	module_put(THIS_MODULE);
//	MOD_DEC_USE_COUNT;
	return 0;
}

unsigned int gc100_poll(struct file * filp, struct poll_table_struct * poll_table)
{
    struct gc100_dev * dev = filp->private_data;

	int which_driver_set, minor_number;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	printk("<1> Release PID: %d\n", current->pid);
	minor_number = dev->minor_number & 0x0f;
	which_driver_set = (int) (minor_number / 2);

    if (minor_number % 2 == 0)
	{ // s polling
		if (gc100_d_to_s_flag[which_driver_set] == 1)
		{
			return POLLIN | POLLRDNORM;
		}
	}
	else
	{ // d polling
		if (gc100_s_to_d_flag[which_driver_set] == 1)
		{
			return POLLIN | POLLRDNORM;
		}
	}
	return 0;
}

static void /*__exit*/ gc100_exit(void)
{
	kfree(gc100_devices);
	kfree(gc100_s_to_d_flag);
	kfree(gc100_d_to_s_flag);
	unregister_chrdev(gc100_major, "gc100");
}

static struct file_operations gc100_fops =
{
//	.poll = gc100_poll,
	.read = gc100_read,
	.write = gc100_write,
	.open = gc100_device_open,
	.release = gc100_device_release
};

static int /*__init*/ gc100_init(void)
{
	int i;

//	SET_MODULE_OWNER(&gc100_fops);

	gc100_major = register_chrdev(0, "gc100", &gc100_fops);

	gc100_devices = kmalloc(N_DEVS * sizeof(struct gc100_dev), GFP_KERNEL);
	gc100_s_to_d_flag = kmalloc(N_DEVS * sizeof(int), GFP_KERNEL);
	gc100_d_to_s_flag = kmalloc(N_DEVS * sizeof(int), GFP_KERNEL);

	memset(gc100_devices, 0, N_DEVS * sizeof(struct gc100_dev));
	memset(gc100_s_to_d_flag, 0, N_DEVS * sizeof(int));
	memset(gc100_d_to_s_flag, 0, N_DEVS * sizeof(int));

	for (i = 0; i < N_DEVS; i++)
	{
		sema_init(&gc100_devices[i].sem, 1);
		gc100_d_to_s_flag[i] = 0;
		gc100_s_to_d_flag[i] = 0;
	}
	
	return 0;

//fail:
//	gc100_exit();
//	return result;
}

module_init(gc100_init);
module_exit(gc100_exit);
MODULE_LICENSE("Dual BSD/GPL");

