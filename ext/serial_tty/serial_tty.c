#include <ruby.h>

#include "extconf.h"
#if defined(__APPLE__)
  #include <CoreFoundation/CoreFoundation.h>
  #include <mach/error.h>
  #include <mach/mach_error.h>
  #include <IOKit/IOKitLib.h>
  #include <IOKit/IOCFBundle.h>
  #include <IOKit/IOCFPlugIn.h>
  #include <IOKit/usb/IOUSBLib.h>
  #include <IOKit/serial/IOSerialKeys.h>
  #include <IOKit/serial/ioss.h>
#elif defined(__linux__)
	#include <X11/Xlib.h>
	#include "xdisplay.h"
#endif


static VALUE serial_tty_name(VALUE self, VALUE rb_device_id, VALUE rb_intf_number);

void Init_serial_tty()
{
  VALUE mod = rb_define_module("SerialTTY");
  rb_define_singleton_method(mod, "tty_name", serial_tty_name, 2);
}


static VALUE serial_tty_name(VALUE self, VALUE rb_vid, VALUE rb_pid)
{
  uint32_t req_vid = NUM2UINT(rb_vid);
  uint32_t req_pid = NUM2UINT(rb_pid);
  VALUE ret = Qnil;

#if defined(__APPLE__)
  io_iterator_t iterator = MACH_PORT_NULL;
  io_object_t port;
  
  
  CFMutableDictionaryRef keywordDict = IOServiceMatching(kIOSerialBSDServiceValue);

  // Note: kIOMasterPortDefault is deprecated starting macOS 12 to reduce confusion surrounding
  // the possibility that the main port could have subordinates (which it very much does).
  // Unfortunately, earlier macOS versions don't export the newer, trendier constant, so we're 
  // stuck silencing a deprecation warning. Ce la vie.
  //
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wdeprecated-declarations"
  kern_return_t result = IOServiceGetMatchingServices(kIOMasterPortDefault, keywordDict, &iterator);
  #pragma clang diagnostic pop

  if (KERN_SUCCESS != result)
    return Qnil;

  bool found = false;
  
  while ((port = IOIteratorNext(iterator)) && !found)
  {
    io_object_t parent = 0;                     
    io_object_t device = port;
    while (KERN_SUCCESS == IORegistryEntryGetParentEntry(device, kIOServicePlane, &parent))
    {
        CFTypeRef vendor_id = IORegistryEntryCreateCFProperty(parent, CFSTR(kUSBVendorID), kCFAllocatorDefault, 0);
        CFTypeRef pr_id = IORegistryEntryCreateCFProperty(parent, CFSTR(kUSBProductID), kCFAllocatorDefault, 0);
      
        if (NULL != vendor_id || NULL != pr_id)
        {
          uint32_t vid, pid;
          CFNumberGetValue(vendor_id, kCFNumberSInt32Type, &vid);
          CFNumberGetValue(pr_id, kCFNumberSInt32Type, &pid);
          if((vid==req_vid) && (pid==req_pid))
          {
              // MY SERIAL DEVICE DETECTED !! 
            CFTypeRef deviceName = IORegistryEntryCreateCFProperty(device, CFSTR(kIOTTYDeviceKey), NULL, 0);

            char buf[256];
            CFStringGetCString(deviceName, buf, 256, kCFStringEncodingUTF8);

            // CFTypeRef callOutDevice = IORegistryEntryCreateCFProperty(device, CFSTR(kIOCalloutDeviceKey), NULL, 0);
            // CFTypeRef dialInDevice = IORegistryEntryCreateCFProperty(device, CFSTR(kIODialinDeviceKey), NULL, 0);

            ret = rb_str_new2(buf);
            found = true;
            break;
          }
          
          CFRelease(vendor_id);
          CFRelease(pr_id);
        }

      device = parent;
    }
  }
#elif defined(__linux__)
#error Linux is not yet supported
#endif
  
  return ret;
}

