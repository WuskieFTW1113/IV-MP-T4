#include "apiMath.h"
#include "../SharedDefines/exporting.h"
#include <vector>

namespace apiCheckPoint
{
	class checkPoint
	{
		private:
			int id;
			apiMath::Vector3 pos;
			float radius;
			unsigned int hexColor;
			unsigned int vWorld;
			int type;
			int blipType;

			float streamDis;
			
		public:
			checkPoint(int id, apiMath::Vector3 pos, float radius, unsigned int hexColor, int type, int blipType, unsigned int vWorld);
			~checkPoint();

			DLL void copyPos(apiMath::Vector3& buf){ buf = pos; }

			DLL apiMath::Vector3 getPos();
			DLL float getRadius();
			DLL int getType();
			DLL unsigned int getWorld();
			DLL unsigned int getColor();
			DLL int getBlip();

			DLL void setStream(float d);
			DLL float getStream(){ return this->streamDis; }
			DLL int getId() { return this->id; };
	};

	DLL int addNew(apiMath::Vector3 pos, float radius, unsigned int hexColor, int type, int blipType, unsigned int vWorld); //Hex is RGBA
	DLL void remove(int id);
	DLL bool isValid(int id);

	DLL checkPoint* get(int id);
	DLL void getAll(std::vector<checkPoint*>& vec);

	DLL void setBlipTypeForPlayer(int cpId, int player, int type);
	DLL void setShowingForPlayer(int cpId, int player, bool showing);
}
