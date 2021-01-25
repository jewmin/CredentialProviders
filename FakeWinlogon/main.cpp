#include <Windows.h>

int main(int argc, const char * * argv) {
    HINSTANCE hDll = ::LoadLibrary(L"Gina.dll");
    FARPROC DebugGINA = ::GetProcAddress(hDll, "DebugGINA");
    DebugGINA();
    return 0;
}
