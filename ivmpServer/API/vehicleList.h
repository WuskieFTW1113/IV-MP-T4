#include <vector>
#include <exporting.h>

namespace vehicleList
{
	//Internal usage only
	void loadList();
	void getDefaultTune(unsigned int i, size_t& defaultSize, std::vector<int>& tunes);
	unsigned long getModel(int id);

	//API and Internal
	DLL bool isValidModel(int id);
	DLL int getIdByName(const char* carName);
	DLL const char* getModelName(int id);
}