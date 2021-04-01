#include <exporting.h>

namespace playerList
{
	void loadList();

	DLL bool isValidModel(int i);
	DLL unsigned long getModel(int i);
	DLL int getIdByName(const char* carName);
	DLL int getIdByHash(unsigned int hash);
	DLL const char* getModelName(int i);
}