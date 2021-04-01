#include "easylogging++.h"
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include "version.h"
#pragma comment(lib, "wbemuuid.lib")

int squery[44] = {83,69,76,69,67,84,32,83,101,114,105,97,108,78,117,109,98,101,114,32,70,82,
	79,77,32,87,105,110,51,50,95,80,104,121,115,105,99,97,108,77,101,100,105,97};

int srs[12] = {83,101,114,105,97,108,78,117,109,98,101,114};

std::vector<std::string> serials;
void popVersion(std::string str)
{
	size_t s = serials.size();
	for(size_t i = 0; i < s; i++)
	{
		if(serials[i] == str) return;
	}
	serials.push_back(str);
}

std::string getString(int* list, int size)
{
	std::ostringstream s;
	for(int i = 0; i < size; i++)
	{
		s << (char)list[i];
		//LINFO << squery[i];
	}
	return s.str();
}

void getVersion(std::vector<std::string>& v)
{
	size_t s = serials.size();
	for(size_t i = 0; i < s; i++)
	{
		v.push_back(serials[i]);
		//LINFO << serials[i];
	}
}

void versionInit()
{
    HRESULT hres;

    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------

    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres))
    {
        LINFO << "Failed to initialize COM library. Error code = 0x" 
            << hex << hres;
        return;                  // Program has failed.
    }

    // Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

                      
    if (FAILED(hres))
    {
        LINFO << "Failed to initialize security. Error code = 0x" 
            << hex << hres;
        CoUninitialize();
        return;                    // Program has failed.
    }
    
    // Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
        LINFO << "Failed to create IWbemLocator object."
            << " Err code = 0x"
            << hex << hres;
        CoUninitialize();
        return;                 // Program has failed.
    }

    // Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method

    IWbemServices *pSvc = NULL;
 
    // Connect to the root\cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.
    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (for example, Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
        LINFO << "Could not connect. Error code = 0x" 
             << hex << hres;
        pLoc->Release();     
        CoUninitialize();
        return;                // Program has failed.
    }

    LINFO << "Connected to ROOT";


    // Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
        LINFO << "Could not set proxy blanket. Error code = 0x" 
            << hex << hres;
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
        return;               // Program has failed.
    }

    // Step 6: --------------------------------------------------
    // Use the IWbemServices pointer to make requests of WMI ----

    // For example, get the name of the operating system

    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t(getString(squery,44).c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
        LINFO << "Query for game version name failed."
            << " Error code = 0x" 
            << hex << hres;
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return;               // Program has failed.
    }

    // Step 7: -------------------------------------------------
    // Get the data from the query in step 6 -------------------
 
    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
   
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Name property
		std::string buf = getString(srs, 12);
		std::wstring stemp = std::wstring(buf.begin(), buf.end());

        hr = pclsObj->Get(stemp.c_str(), 0, &vtProp, 0, 0);
		if(S_OK == hr && vtProp.bstrVal != NULL)
		{
			std::wstring ws(vtProp.bstrVal, SysStringLen(vtProp.bstrVal));
			std::string str(ws.begin(), ws.end());
			if(str.size() > 5)
			{
				popVersion(str);
			}
		}
		VariantClear(&vtProp);
        pclsObj->Release();
    }

    // Cleanup
    // ========
    
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();
    CoUninitialize();
}
