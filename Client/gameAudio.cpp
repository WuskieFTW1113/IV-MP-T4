#include "gameAudio.h"
#include <windowsx.h>
#include "bass.h"
#include <process.h>
#include "easylogging++.h"
#include "../clientLibs/gameWindow.h"
#include "simpleMath.h"

#define BASS_SYNC_HLS_SEGMENT	0x10300
#define BASS_TAG_HLS_EXTINF		0x14000

HWND win = NULL;
CRITICAL_SECTION lock;
DWORD req = 0;	// request number/counter
HSTREAM chan;	// stream handle
bool streamPaused = false;
std::string surl;
float spos[3];
bool stremPickedUp = false;

bool isBassInit = false;
bool frontEnd = false;

/*void CALLBACK StatusProc(const void *buffer, DWORD length, void *user)
{
	bool hasDevice = gameWindow::isWindowActive();
	if(!hasDevice && !streamPaused)
	{
		BASS_ChannelPause(chan);
		streamPaused = true;
	}
}*/

void windowCB(bool focus)
{
	LINFO << "Window callback focus: " << (int)focus;

	if(!focus && !streamPaused)
	{
		BASS_ChannelPause(chan);
		streamPaused = true;
	}
}

void __cdecl OpenURL(void* url)
{
	DWORD c, r;
	//EnterCriticalSection(&lock); // make sure only 1 thread at a time can do the following
	r = ++req; // increment the request counter for this request
	//LeaveCriticalSection(&lock);
	LINFO << "Pre-Timer";
	KillTimer(win, 0); // stop buffer monitoring
	LINFO << "Timer";
	BASS_StreamFree(chan); // close old stream
	LINFO << "Close channel";
	//MESS(31,WM_SETTEXT,0,"connecting...");
	//MESS(30,WM_SETTEXT,0,"");
	//MESS(32,WM_SETTEXT,0,"");
	c = BASS_StreamCreateURL(surl.c_str(), 0, BASS_STREAM_BLOCK | BASS_STREAM_STATUS | BASS_STREAM_AUTOFREE, NULL, (void*)r); // open URL
	LINFO << "Create Stream";
	//free(url); // free temp URL buffer
	LINFO << "Free url";
	chan = c; // this is now the current stream
	if(!chan) { // failed to open
		//MESS(31,WM_SETTEXT,0,"not playing");
		LINFO << "Can't play the stream";
	}
	else {
		// start buffer monitoring

		SetTimer(win, 0, 50, 0);

		// play it!
		BASS_ChannelPlay(chan, FALSE);
		BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, frontEnd ? 0.5f : 0.1f);
		stremPickedUp = true;
	}
}

void gameAudio::create(RakNet::BitStream& bsIn)
{
	frontEnd = true;

	RakNet::RakString s;
	bsIn.Read(s);

	bsIn.Read(spos[0]);
	if(spos[0] < -1.0f || spos[0] > -1.0f)
	{
		bsIn.Read(spos[1]);
		bsIn.Read(spos[2]);
		frontEnd = false;
	}
	surl = s;

	LINFO << "Stream info: " << surl << ", Frontend: " << (int)frontEnd;

	if(HIWORD(BASS_GetVersion()) != BASSVERSION)
	{
		LINFO << "BASS version is wrong";
		return;
	}

	BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1); // enable playlist processing
	BASS_SetConfig(BASS_CONFIG_NET_PREBUF_WAIT, 0); // disable BASS_StreamCreateURL pre-buffering

	/*win = GetForegroundWindow();
	m_wWndProc = SubclassWindow(win, WndProc_Hook);*/

	if(!BASS_Init(-1, 44100, 0, 0, NULL))
	{
		int e = BASS_ErrorGetCode();
		if(e != 14)
		{
			LINFO << "Can't initialize device: " << e;
			return;
		}
	}

	gameWindow::registerWindowCb(windowCB);

	isBassInit = true;
}

bool streamCreated = false;

void gameAudio::reallyStop()
{
	LINFO << "Bass Stop";
	BASS_ChannelStop(chan);
	LINFO << "Bass stream free";
	BASS_StreamFree(chan);
	LINFO << "Bass final free";

	streamCreated = false;
	streamPaused = false;
	stremPickedUp = false;
}

void gameAudio::stop(RakNet::BitStream& bsIn)
{
	isBassInit = false;
	reallyStop();
}

bool gameAudio::exist()
{
	return isBassInit;
}

void gameAudio::remove()
{
	BASS_Free();
}

void gameAudio::pulse(simpleMath::Vector3& v)
{
	if(streamPaused && streamCreated)
	{
		BASS_ChannelPlay(chan, FALSE);
		BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, frontEnd ? 0.5f : 0.1f);
		streamPaused = false;
		return;
	}

	if(frontEnd)
	{
		if(!streamCreated)
		{
			_beginthread(OpenURL, 0, "");
			streamCreated = true;
			streamPaused = false;
		}
		return;
	}

	float d = simpleMath::getDistance(v.x, v.y, v.z, spos[0], spos[1], spos[2]);
	if(!streamCreated)
	{
		if(d < 50.0f)
		{
			_beginthread(OpenURL, 0, "");
			streamCreated = true;
			streamPaused = false;
		}
		return;
	}

	if(d > 50.0f)
	{
		if(!stremPickedUp)
		{
			return;
		}

		reallyStop();
		streamCreated = false;
	}
	else
	{
		d = 50.0f - d;
		if(d > 0.0f)
		{
			BASS_ChannelSetAttribute(chan, BASS_ATTRIB_VOL, d / 50);
		}
	}
}

void gameAudio::registerCb()
{
	gameWindow::registerWindowCb(windowCB);
}