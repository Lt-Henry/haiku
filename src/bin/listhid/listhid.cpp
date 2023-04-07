
#include <Directory.h>
#include <Entry.h>
#include <Path.h>
#include <String.h>

#include <iostream>

using namespace std;

int
main(int argc, char* argv[])
{

	BDirectory hid_dir("/dev/input/hid/");
	BEntry entry;
	char filename[B_FILE_NAME_LENGTH];
	L0:

	if (hid_dir.GetEntry(&entry) == B_OK) {
		entry.GetName(filename);
		cout<<filename<<endl;
		goto L0;
	}

	return 0;
}
