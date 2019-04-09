// Написать модуль, который будет блокировать трафик на порт заданный через sysfs

//echo "3333" > /sys/kernel/etx_sysfs/etx_value
//Tested 4.19.0-4-amd64

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>				//kmalloc()
#include <linux/uaccess.h>			//copy_to/from_user()
#include <linux/sysfs.h>
#include <linux/kobject.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>	//NF_IP_PRI_FIRST,
#include <net/ip.h>					//htons

volatile uint16_t etx_value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
struct kobject *kobj_ref;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/*************** Driver Fuctions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);

/*************** Sysfs Fuctions **********************/
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count);

struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);

static struct file_operations fops =
{
	.owner	= THIS_MODULE,
	.read	= etx_read,
	.write	= etx_write,
	.open	= etx_open,
	.release= etx_release,
};
 
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	printk(KERN_INFO "Sysfs - Read!!!\n");
	return sprintf(buf, "%d", etx_value);
}

static unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr* ip_header;	/* IP header struct */
	struct udphdr* udp_header;	/* UDP header struct */
	struct tcphdr* tcp_header;	/* TCP header struct */
	uint16_t skb_port = 0;
	
	if (!skb)
		return NF_ACCEPT;

	ip_header = (struct iphdr*)skb_network_header(skb);
	
	switch (ip_header->protocol) {
		case IPPROTO_TCP:		//6
			tcp_header = (struct tcphdr*)skb_transport_header(skb);
			skb_port = (uint16_t)ntohs(tcp_header->dest);
			break;
		case IPPROTO_UDP:		//17
			udp_header = (struct udphdr*)(skb_transport_header(skb));
			skb_port = (uint16_t)ntohs(udp_header->dest);
			break;
	}
	if (etx_value && skb_port == etx_value) {
		printk(KERN_INFO "Дропнул пакет с порта №%d\n", etx_value);
		return NF_DROP;
	}
	return NF_ACCEPT;
}

static struct nf_hook_ops pkt_echo_ops __read_mostly = {
    .pf = NFPROTO_IPV4,
    .priority = NF_IP_PRI_FIRST,
    .hooknum = NF_INET_PRE_ROUTING,
    .hook = hook_func,
};

static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t count)
{
	sscanf(buf,"%hd",&etx_value);
	printk(KERN_INFO "Блокирую порт №%d\n", etx_value);
	
	return count;
}

static int etx_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Файл открыт\n");
	return 0;
}

static int etx_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Файл закрыт\n");
	return 0;
}

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Чтение\n");
	return 0;
}
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "Запись\n");
	return 0;
}

static int __init etx_driver_init(void)
{
	/*Allocating Major number*/
	if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0) {
		printk(KERN_INFO "Cannot allocate major number\n");
		return -1;
	}
	printk(KERN_INFO "Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	/*Creating cdev structure*/
	cdev_init(&etx_cdev,&fops);

	/*Adding character device to the system*/
	if((cdev_add(&etx_cdev,dev,1)) < 0) {
		printk(KERN_INFO "Cannot add the device to the system\n");
		goto r_class;
	}

	/*Creating struct class*/
	if((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL) {
		printk(KERN_INFO "Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL) {
		printk(KERN_INFO "Cannot create the Device 1\n");
		goto r_device;
	}

	/*Creating a directory in /sys/kernel/ */
	kobj_ref = kobject_create_and_add("etx_sysfs", kernel_kobj);

	/*Creating sysfs file for etx_value*/
	if(sysfs_create_file(kobj_ref,&etx_attr.attr)) {
		printk(KERN_INFO"Не создана sysfs\n");
		goto r_sysfs;
	}
	nf_register_net_hook(&init_net, &pkt_echo_ops);//TODO Проверка результата?
	
	printk(KERN_INFO "Device Driver Insert...Done!!!\n");
	return 0;

r_sysfs:
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev,1);
	cdev_del(&etx_cdev);
	return -1;
}
 
void __exit etx_driver_exit(void)
{
	nf_unregister_net_hook(&init_net, &pkt_echo_ops);
	
	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Модуль выгружен. Привила отменены\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
