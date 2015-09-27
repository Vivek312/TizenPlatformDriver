
/*
 * Operating Systems - IIT Delhi
 * Courtesy : LIACS, Leiden University
 * Template file for Loadable Kernels Assignment
 */
/*
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/errno.h>
//#include <asm/system.h>
#include <asm/uaccess.h>

#include <linux/proc_fs.h>    /* Necessary because we use the proc fs */

#define procfs_name "fifobuf"
/* Device name of our FIFO device */
#define FIFO_DEVICE_NAME "fifobuffer"
#define BUFFER_SIZE 1024
static int fifo_device_major = 0; /* Dynamically assigned device major number */
static int fifo_device_state = 0; /* Status of device */
int ret_flag; // changed by veeru
int fifo_minor,m,r;
int i,k=0;
char *fifo_buffer;
char fifo_buf[8][1024];
char fifo_in[8];
struct proc_dir_entry *Our_Proc_File;
//static char *buff_rptr;


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Vivek Parmar");
MODULE_DESCRIPTION("A simple character device driver.");
MODULE_SUPPORTED_DEVICE(DEVICE_NAME);

/* FIFO Device operation prototypes */
static int fifo_device_open(struct inode *, struct file *);
static int fifo_device_release(struct inode *, struct file *);
static ssize_t fifo_device_read(struct file *, char *, size_t, loff_t *);
static ssize_t fifo_device_write(struct file *, const char *, size_t, loff_t *);

/* File operations to be associated with our FIFO device */
static struct file_operations fifo_fops = {
  .read     = fifo_device_read,
  .write    = fifo_device_write,
  .open     = fifo_device_open,
  .release  = fifo_device_release
};


int procfile_read(char *buffer,
          char **buffer_location,
          off_t offset, int buffer_length, int *eof, void *data)
{
    int ret;
   
    printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);
   
    if (offset > 0) {
        /* we have finished to read, return 0 */
        ret  = 0;
    } else {
        /* fill the buffer, return the buffer size */
        ret = sprintf(buffer, "usage:\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",fifo_in[0],fifo_in[1],fifo_in[2],fifo_in[3],fifo_in[4],fifo_in[5],fifo_in[6],fifo_in[7]);
    }

    return ret;
}


/* Module initialization */
int init_module(void)
{
  fifo_device_major = register_chrdev(0, FIFO_DEVICE_NAME, &fifo_fops);
  fifo_buffer=kmalloc(132,GFP_KERNEL);
  strcpy(fifo_buffer,"Kernel data string not yet written to\n");
  if (fifo_device_major < 0) {
    printk(KERN_ALERT "Registering char device failed with %d\n", fifo_device_major);
    return fifo_device_major;
  }
  else {
    printk(KERN_INFO "Registered char device with major %d\n", fifo_device_major);
    printk(KERN_INFO "Now run   mknod /dev/%s c %d 0   to create the device file.\n", FIFO_DEVICE_NAME, fifo_device_major);
  }
  memset(fifo_in,0,8);

//  Our_Proc_File = proc_create(procfs_name,  0, NULL, &proc_fops);
    Our_Proc_File = create_proc_entry(procfs_name, 644, NULL);
       
    if (Our_Proc_File == NULL) {
        //remove_proc_entry(procfs_name, &proc_root);
        printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
               procfs_name);
        return -ENOMEM;
    }

    Our_Proc_File->read_proc = procfile_read;
    //Our_Proc_File->owner      = THIS_MODULE;
    Our_Proc_File->mode      = S_IFREG | S_IRUGO;
    Our_Proc_File->uid      = 0;
    Our_Proc_File->gid      = 0;
    Our_Proc_File->size      = 128;

    printk(KERN_INFO "/proc/%s created\n", procfs_name);   
    return 0;    /* everything is ok */     
}


/* Module unloading */
void cleanup_module(void)
{
  unregister_chrdev(fifo_device_major, FIFO_DEVICE_NAME);
  remove_proc_entry(procfs_name, NULL);   
  kfree(fifo_buffer);
  printk(KERN_INFO "Unregistered char device with major %d\n", fifo_device_major);
}


/* Called when a process tries to open the device file.
 * Here we "lock" the device file and increase module use count. */
static int fifo_device_open(struct inode *inode, struct file *file)
{
  if (fifo_device_state)
    return -EBUSY;
  ret_flag=0;   // changed by veeru
  fifo_device_state++;
  try_module_get(THIS_MODULE);

  return 0;
}


/* Called when a process closes the device file.
 * Here we "unlock" the device file and decrease module use count. */
static int fifo_device_release(struct inode *inode, struct file *file)
{
  fifo_device_state--;

  module_put(THIS_MODULE);

  return 0;
}


static ssize_t
fifo_device_read(struct file *filp, char *buffer, size_t count, loff_t * offset)
{
   
  /* TODO
    This function is called when a process reads from an associated FIFO dev
    file, e.g.: cat /dev/myfifo
    The user space program expects the read data to be written to "buffer"
    (which is a user space pointer!)

    For now, just write a character into the buffer and return #bytes read
  */
  //put_user('A', buffer);
    fifo_minor=iminor(filp->f_path.dentry->d_inode);
    m=fifo_minor%8;   
   	
    printk("Example driver read:size:%d\n",count);
    if(ret_flag == 1){ret_flag = 0; return 0;} 
    i=0;
	k=fifo_in[m];
    //printk("%d %d",m,fifo_in[m]);
	while(i<k)
    {   
       if(m<4)      
        	put_user(fifo_buf[m][i],buffer+i);
		else
			put_user(fifo_buf[m][k-i-1],buffer+i);
        //printk("%c",fifo_buf[m][i]);
        i++;
    }

    
    ret_flag = 1; 
    return(fifo_in[m]);
}


static ssize_t
fifo_device_write(struct file *filp, const char *buffer, size_t count, loff_t * offset)
{
  /* TODO
    This function is called when a process writes to an associated FIFO dev
    file, e.g.: echo "abc" > /dev/myfifo
    The data to be written is at "buffer" (which is a user space pointer!)

    Return #bytes written
   */
    fifo_minor=iminor(filp->f_path.dentry->d_inode);
    m=fifo_minor%8;   
   
    printk("Device id : %d",fifo_minor);
    r=fifo_in[m];
    if(copy_from_user(*fifo_buf+m*1024+r,buffer,count)!=0)
        printk("User to kernel copy failed\n");
    fifo_in[m]+=count-1;
    return count;    
}
