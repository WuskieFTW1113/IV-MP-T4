#include "apiMath.h"
#include "../SharedDefines/exporting.h"
#include <vector>

#ifndef apiObjects_H
#define apiObjects_H

namespace apiObjects
{
	class object
	{
		private:
			int id;
			apiMath::Vector3 pos;
			apiMath::Quaternion rot;

			unsigned int objHex;
			unsigned int vWorld;

			unsigned int interior;
			int alpha;
			bool hasOffSet;

			float streamDistance;

		public:
			object(int id, apiMath::Vector3 pos, apiMath::Quaternion rot, unsigned int objHex, 
				unsigned int vWorld, bool hasOffset, unsigned int interior, int alpha);
			~object();

			DLL void copyPos(apiMath::Vector3& buf){ buf = pos; }
			DLL void copyRot(apiMath::Quaternion& quat) { quat = rot; }

			DLL apiMath::Vector3 getPos();
			DLL apiMath::Quaternion getRot();
			DLL unsigned int getWorld();
			DLL unsigned int getModel();

			DLL bool hasOffset();
			DLL unsigned int getInterior();
			DLL int getAlpha();

			void setPos(apiMath::Vector3 pos); //internal only
			void setRot(apiMath::Quaternion rot); //internal only

			DLL void setStreamDis(float dis);
			DLL float getStream() { return this->streamDistance; }
			DLL int getId() { return this->id; }
	};

	DLL int addNew(apiMath::Vector3 pos, apiMath::Quaternion rot, unsigned int objHex, 
		unsigned int vWorld, bool hasOffset, unsigned int interior, int alpha, float streamDis = 200.0f);
	DLL void remove(int id);
	DLL bool isValid(int id);
	DLL object* get(int id); //Null or Pointer
	DLL void getAll(std::vector<object*>& vec);

	DLL void moveObject(int id, apiMath::Vector3 pos, apiMath::Quaternion rot, unsigned int time);
}

#endif
