#include "apiMath.h"
#include <vector>
#include <exporting.h>

#ifndef apiDialog_H
#define apiDialog_H

namespace apiDialog
{
	class dialogList
	{
		private:
			unsigned int id;
			char* windowName;
			unsigned int columns;
			char* headers;
			char* btnNames[2];

		public:
			dialogList(unsigned int id, char* windowName, unsigned int columns);
			~dialogList();

			DLL void setColumnsHeaders(const char* s);
			DLL void setBtnNames(const char* b1, const char* b2);
			DLL void addRow(const char* s, int customId = -1);
			DLL void clearRows();
			DLL void save();
	};

	DLL bool addNew(unsigned int id, char* windowName, unsigned int columns);
	DLL void remove(int id);
	DLL bool isValid(int id);
	DLL dialogList* get(int id);
}

#endif