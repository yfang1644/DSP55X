/*
 * DEC6713 USB Host driver
 *
 * This driver is based on the 2.6.3 version of drivers/usb/usb-skeleton.c
 * but has been rewritten to be easier to read and use.
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
//#include <linux/slab.h>
#include <linux/module.h>
//#include <linux/kref.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include "dec6713_usb.h"

/* Define these values to match your devices */
#define USB_DEC_VENDOR_ID	0x4b4
#define USB_DEC_PRODUCT_ID	0x82

/* table of devices that work with this driver */
static struct usb_device_id dec_table [] = {
	{ USB_DEVICE(USB_DEC_VENDOR_ID, USB_DEC_PRODUCT_ID) },
	{ }					/* Terminating entry */
};
MODULE_DEVICE_TABLE(usb, dec_table);


/* Get a minor range for your devices from the usb maintainer */
#define USB_DEC_MINOR_BASE	192

/* our private defines. if this grows any larger, use your own .h file */
#define MAX_TRANSFER		(PAGE_SIZE - 512)
/* MAX_TRANSFER is chosen so that the VM is not stressed by
   allocations > PAGE_SIZE and the number of packets in a page
   is an integer 512 is the largest possible packet on EHCI */
#define WRITES_IN_FLIGHT	8
/* arbitrarily chosen */

/* Structure to hold all of our device specific stuff */
struct usb_dec {
	struct usb_device	*udev;			/* the usb device for this device */
	struct usb_interface	*interface;		/* the interface for this device */
	struct semaphore	limit_sem;		/* limiting the number of writes in progress */
	struct usb_anchor	submitted;		/* in case we need to retract our submissions */
	unsigned char           *bulk_in_buffer;	/* the buffer to receive data */
	size_t			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	int			errors;			/* the last request tanked */
	int			open_count;		/* count the number of openers */
	spinlock_t		err_lock;		/* lock for errors */
	struct kref		kref;
	struct mutex		io_mutex;		/* synchronize I/O with disconnect */
};

static struct usb_driver dec_driver;
static void dec_draw_down(struct usb_dec *dev);

static void dec_delete(struct kref *kref)
{
	struct usb_dec *dev = container_of(kref, struct usb_dec, kref);

	usb_put_dev(dev->udev);
	kfree(dev->bulk_in_buffer);
	kfree(dev);
}

static int dec_open(struct inode *inode, struct file *file)
{
	struct usb_dec *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	interface = usb_find_interface(&dec_driver, subminor);
	if (!interface) {
		err ("%s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}

	/* increment our usage count for the device */
	kref_get(&dev->kref);

	/* save our object in the file's private structure */
	file->private_data = dev;

exit:
	return retval;
}

static int dec_release(struct inode *inode, struct file *file)
{
	struct usb_dec *dev;

	dev = (struct usb_dec *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	/* decrement the count on our device */
	kref_put(&dev->kref, dec_delete);
	return 0;
}

static ssize_t dec_read(struct file *file, char *buffer, size_t count, loff_t *ppos)
{
	struct usb_dec *dev;
	int retval;

	dev = (struct usb_dec *)file->private_data;

	/* do a blocking bulk read to get data from the device */
	retval = usb_bulk_msg(dev->udev,
					usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
					dev->bulk_in_buffer,
					min(dev->bulk_in_size, count),
					&count, 20*HZ);
	printk("read:%d\n", retval);
	/* if the read was successful, copy the data to userspace */
	if (!retval) {
		if (copy_to_user(buffer, dev->bulk_in_buffer, count))
			retval = -EFAULT;
		else
			retval = count;
	}

	return retval;
}

static ssize_t dec_write(struct file *file, const char *user_buffer, size_t count, loff_t *ppos)
{
	struct usb_dec *dev;
	int retval = 0;
	char buf[MAX_TRANSFER];
	size_t writesize = min(count, (size_t)MAX_TRANSFER);

	dev = (struct usb_dec *)file->private_data;

	copy_from_user(buf, user_buffer, writesize);

	usb_bulk_msg(dev->udev,
				usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
				buf,
				writesize,
				&retval, 20*HZ);

	return retval;
}

static int dec_ioctl(struct inode *inode, struct file *file,
				unsigned int cmd, unsigned long arg)
{
	struct usb_dec *dev;
	unsigned char buf[8];
	unsigned char request;
	unsigned int value;
	int retval;
	
	dev = (struct usb_dec *)file->private_data;
	if (dev == NULL)
		return -ENODEV;

	switch(cmd)
	{
		case DEC_GETIOCTL:
			request = (unsigned char)(arg >> 16);
			usb_control_msg(dev->udev,
					usb_rcvctrlpipe(dev->udev, 0),
					request,						// __u8 request
					USB_DIR_IN | USB_TYPE_VENDOR,	// __u8 request type
					0x0,							// __u16 value
					0,								// __u16 index
					buf,							// void *data pointer
					4,								// __u16 size
					10*HZ);
//			retval = (int)buf[0];
			retval = *(int *)&buf[0];
			return retval;
			break;
		case DEC_SETIOCTL:
			request = (unsigned char)(arg >> 16);
			value = (unsigned short)arg;
			usb_control_msg(dev->udev,
					usb_sndctrlpipe(dev->udev, 0),
					request,
					USB_DIR_OUT | USB_TYPE_VENDOR,
					value,
					0,
					buf,
					2,
					10*HZ);
			return 0;
			break;
		default:
			break;
	}
	return -1;
}

static const struct file_operations dec_fops = {
	.owner	=	THIS_MODULE,
	.read	=	dec_read,
	.write	=	dec_write,
	.ioctl	=	dec_ioctl,
	.open	=	dec_open,
	.release =	dec_release,
};

/*
 * usb class driver info in order to get a minor number from the usb core,
 * and to have the device registered with the driver core
 */
static struct usb_class_driver dec_class = {
	.name =		"dec6713",
	.fops =		&dec_fops,
	.minor_base =	USB_DEC_MINOR_BASE,
};

static int dec_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_dec *dev;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;

	/* allocate memory for our device state and initialize it */
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		err("Out of memory");
		goto error;
	}
	kref_init(&dev->kref);
	sema_init(&dev->limit_sem, WRITES_IN_FLIGHT);
	mutex_init(&dev->io_mutex);
	spin_lock_init(&dev->err_lock);
	init_usb_anchor(&dev->submitted);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	/* set up the endpoint information */
	/* use only the first bulk-in and bulk-out endpoints */
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		if (!dev->bulk_in_endpointAddr &&
			usb_endpoint_is_bulk_in(endpoint)) {
			/* we found a bulk in endpoint */
			buffer_size = le16_to_cpu(endpoint->wMaxPacketSize);
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
				err("Could not allocate bulk_in_buffer");
				goto error;
			}
		}

		if (!dev->bulk_out_endpointAddr &&
		    usb_endpoint_is_bulk_out(endpoint)) {
			/* we found a bulk out endpoint */
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}
	}
	if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {
		err("Could not find both bulk-in and bulk-out endpoints");
		goto error;
	}

	printk("In:%x, Out: %x\n", dev->bulk_in_endpointAddr,
				dev->bulk_out_endpointAddr);
	
	/* save our data pointer in this interface device */
	usb_set_intfdata(interface, dev);

	/* we can register the device now, as it is ready */
	retval = usb_register_dev(interface, &dec_class);
	if (retval) {
		/* something prevented us from registering this driver */
		err("Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	/* let the user know what node this device is now attached to */
	dev_info(&interface->dev,
		"USB Skeleton device now attached to USBSkel-%d", interface->minor);
	return 0;

error:
	if (dev)
		/* this frees allocated memory */
		kref_put(&dev->kref, dec_delete);
	return retval;
}

static void dec_disconnect(struct usb_interface *interface)
{
	struct usb_dec *dev;
	int minor = interface->minor;

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	/* give back our minor */
	usb_deregister_dev(interface, &dec_class);

	/* prevent more I/O from starting */
	mutex_lock(&dev->io_mutex);
	dev->interface = NULL;
	mutex_unlock(&dev->io_mutex);

	usb_kill_anchored_urbs(&dev->submitted);

	/* decrement our usage count */
	kref_put(&dev->kref, dec_delete);

	dev_info(&interface->dev, "USB Skeleton #%d now disconnected", minor);
}

static void dec_draw_down(struct usb_dec *dev)
{
	int time;

	time = usb_wait_anchor_empty_timeout(&dev->submitted, 1000);
	if (!time)
		usb_kill_anchored_urbs(&dev->submitted);
}

static int dec_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct usb_dec *dev = usb_get_intfdata(intf);

	if (!dev)
		return 0;
	dec_draw_down(dev);
	return 0;
}

static int dec_resume(struct usb_interface *intf)
{
	return 0;
}

static int dec_pre_reset(struct usb_interface *intf)
{
	struct usb_dec *dev = usb_get_intfdata(intf);

	mutex_lock(&dev->io_mutex);
	dec_draw_down(dev);

	return 0;
}

static int dec_post_reset(struct usb_interface *intf)
{
	struct usb_dec *dev = usb_get_intfdata(intf);

	/* we are sure no URBs are active - no locking needed */
	dev->errors = -EPIPE;
	mutex_unlock(&dev->io_mutex);

	return 0;
}

static struct usb_driver dec_driver = {
	.name		=	"DEC_6713",
	.probe		=	dec_probe,
	.disconnect =	dec_disconnect,
	.suspend 	=	dec_suspend,
	.resume		=	dec_resume,
	.pre_reset	=	dec_pre_reset,
	.post_reset	=	dec_post_reset,
	.id_table	=	dec_table,
	.supports_autosuspend = 1,
};

static int __init usb_dec_init(void)
{
	int result;

	/* register this driver with the USB subsystem */
	result = usb_register(&dec_driver);
	if (result)
		err("usb_register failed. Error number %d", result);

	return result;
}

static void __exit usb_dec_exit(void)
{
	/* deregister this driver with the USB subsystem */
	usb_deregister(&dec_driver);
}

module_init(usb_dec_init);
module_exit(usb_dec_exit);

MODULE_LICENSE("GPL");
