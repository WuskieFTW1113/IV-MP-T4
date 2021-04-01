#include <exporting.h>

namespace apiDebugTask
{
	DLL int getTask();
	DLL void setTask(int i);

	DLL int getSubTask();
	DLL void setSubTask(int i);

	DLL void setDump();
}