#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/version.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

#define TIMEOUT 10

int major;
static void led_on(void);
static void led_off(void);
static int open_dev(struct inode *a,struct file *filep);
static int closedev(struct inode *a,struct file *filep);
static ssize_t write_led(struct file *,const char *,size_t ,loff_t *off);

static struct file_operations fops = {
    .open = open_dev,
    .release = closedev,
    .write = write_led, 
};

int atoi(const char * str) {
    int res = 0, i = 0; 
    for(i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
    return res;
}

static int open_dev(struct inode *a,struct file *filep) {
    printk("Keyboard LED device opened ...\n");   
    return 0;    
}

static int closedev(struct inode *a,struct file *filep) {
    printk("Keyboard LED device closed ...\n");
    led_off();
    return 0;
}

ssize_t write_led(struct file *filp,const char *buff,size_t len, loff_t *off) { 
    char *buf = kmalloc(30*sizeof(char),GFP_KERNEL);
    int k = copy_from_user(buf,buff,len);
    int value = atoi(buf);

    if(value)
        led_on();
    else 
        led_off();

    return k;
}

static void led_on(void) {
	outb(0xed,0x60); 
	while (inb(0x60) != 0xfa)
		udelay(TIMEOUT);
	outb(0x04, 0x60);
}

static void led_off(void) {
    outb(0xed,0x60); 
	while (inb(0x60) != 0xfa)
		udelay(TIMEOUT);
	outb(0x00, 0x60);
}

static int __init init_led_module(void) {
    if((major = (register_chrdev(0,"keyled",&fops))) < 0)
        return -1;     
    printk("Keyboard LED device loaded...%d\n",major);
    led_on();
    return 0;
}

static void __exit exit_led_module(void) {
    unregister_chrdev(major,"keyled");
    printk("Keyboard LED device unloaded ...\n");
}

module_init(init_led_module);
module_exit(exit_led_module);