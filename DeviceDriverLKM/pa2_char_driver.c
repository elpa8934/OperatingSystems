#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 1024

/* Define device_buffer and other global data structures you will need here */
static int opens = 0;
static int closes = 0;
//static int locInBuffer = 0;
char *device_buffer;

ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	printk(KERN_ALERT "In the read function\n");
	if (*offset <= BUFFER_SIZE){
		if (*offset + length > BUFFER_SIZE){
			length = BUFFER_SIZE - *offset;
		}
		printk(KERN_ALERT "Calling copy_to_user\n");
		int errorFlag = copy_to_user(buffer, device_buffer + *offset, length);
		if (errorFlag != 0){
			printk(KERN_ALERT "Error reading\n");
			length = 0;
		}
	} else {
		printk(KERN_ALERT "Offset error\n");
		length = 0;
	}	
	*offset += length;
	printk(KERN_ALERT "Btyes read: %d\n", length);
	printk(KERN_ALERT "Offset at end of read: %d\n", *offset);
	return 0;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	printk(KERN_ALERT "In the Write Function\n");
	if (*offset <= BUFFER_SIZE){
		if (*offset + length > BUFFER_SIZE){
			length = BUFFER_SIZE - *offset;
		}
		printk(KERN_ALERT "Calling copy_from_user\n");
		int errorFlag = copy_from_user(device_buffer + *offset, buffer, length);
		device_buffer[*offset + length] = '\0';
		if (errorFlag != 0){
			printk(KERN_ALERT "Error writing\n");
			length = 0;
		}
	} else {
                printk(KERN_ALERT "Offset error\n");     
                length = 0;
        }
 
	*offset += length;
	printk(KERN_ALERT "Btyes written: %d\n", length);
	printk(KERN_ALERT "Offset at end of write: %d\n", *offset);
	return length;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	opens += 1;
	printk(KERN_ALERT "Opening Device. Num Opens: %d\n", opens);
	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	closes += 1;
	printk(KERN_ALERT "Closing Device. Num Closes: %d\n", closes);
	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	printk(KERN_ALERT "In the seek function\n");
	int newOffset = 0;
	if (offset >= 0 && offset < BUFFER_SIZE){
		if (whence == 0){
			newOffset = offset;
		} else if (whence == 1){
			newOffset = pfile->f_pos + offset;
		} else if (whence == 2){
			newOffset = BUFFER_SIZE - offset;
		} else {
			printk(KERN_ALERT "Whence Error\n");
		}
	} else {
		printk(KERN_ALERT "Seek Error\n");
	}
	printk(KERN_ALERT "Offset After Seeking %d\n", newOffset);
	pfile->f_pos = newOffset;
	return newOffset;
}

struct file_operations pa2_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
	.read = pa2_char_driver_read,
	.write = pa2_char_driver_write,
	.open = pa2_char_driver_open,
	.release = pa2_char_driver_close,
	.llseek = pa2_char_driver_seek,
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	printk(KERN_INFO "Init Function Called\n");
	/* register the device */
	register_chrdev(321, "pa2_char_driver", &pa2_char_driver_file_operations);
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	return 0;
}

static void pa2_char_driver_exit(void)
{
	kfree(device_buffer);
	/* print to the log file that the exit function is called.*/
	printk(KERN_INFO "Exit Function Called\n");
	/* unregister  the device using the register_chrdev() function. */
	unregister_chrdev(321, "pa2_char_driver");
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa2_char_driver_init)
module_exit(pa2_char_driver_exit)
