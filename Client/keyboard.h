#include <Windows.h>

namespace keyboard
{
	typedef void(*keyState)(bool altDown);
	//void registerFunc(unsigned int vKey, keyState ks, bool signWhatState); //true for up
	void registerFunc(unsigned int vKey, keyState ks, bool signWhatState);


	bool getKey(unsigned int vKey);
	bool getAlt();

	void pulseServerBinds();

	void receiveWindowMsg(UINT msg, WPARAM key);
}
