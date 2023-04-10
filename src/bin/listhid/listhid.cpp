
#include <Directory.h>
#include <Entry.h>
#include <Path.h>
#include <String.h>

#include <iostream>
#include <iomanip>
#include <map>

#include <HID.h>

using namespace std;

map<int,string> sBusName = {
	{B_HID_BUS_VIRTUAL,"virtual"},
	{B_HID_BUS_USB,"usb"},
	{B_HID_BUS_I2C,"i2c"},
	{B_HID_BUS_SPI,"spi"}
};

status_t
hid_get_info(const char *device,hid_info *info)
{
	int fd = open(device,O_RDWR);
	//clog<<"ioctl to "<<device<<":"<<fd<<":"<<B_HID_IO_GET_INFO<<endl;
	int status = ioctl(fd,B_HID_IO_GET_INFO,info,sizeof(hid_info));
	if (status < 0) {
		cerr<<"ioctl err:"<<status<<endl;
		return B_ERROR;
	}
	close(fd);
	
	return B_OK;
}

status_t
explore_dir(BDirectory dir)
{
	BEntry entry;
	
	if (dir.GetEntry(&entry) != B_OK) {
		return B_ERROR;
	}
	
	while (dir.GetNextEntry(&entry) == B_OK) {
		
		if (entry.IsDirectory()) {
			explore_dir(&entry);
		}
		else {
			BPath path;
			entry.GetPath(&path);
			
			hid_info info;
			
			if (hid_get_info(path.Path(),&info) == B_OK) {
				cout<<path.Path()<<" "<<std::dec<<sBusName[info.bus]<<" "<<std::hex<<std::setfill('0')<<std::setw(4)<<info.vid<<":";
				cout<<std::hex<<std::setfill('0')<<std::setw(4)<<info.pid<<endl;
			}
		}
	}
	
	return B_OK;
}

int
main(int argc, char* argv[])
{

	BDirectory hid_dir("/dev/input/hid/");
	explore_dir(hid_dir);

	return 0;
}
