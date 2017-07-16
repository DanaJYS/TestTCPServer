//Win32 libraries
#pragma once
#pragma comment(lib,"kernel32.lib ")
#pragma comment(lib,"user32.lib ")
#pragma comment(lib,"gdi32.lib ")
#pragma comment(lib,"comdlg32.lib ")
#pragma comment(lib,"winspool.lib ")
#pragma comment(lib,"winmm.lib ")
#pragma comment(lib,"shell32.lib ")
#pragma comment(lib,"comctl32.lib ")
#pragma comment(lib,"ole32.lib ")
#pragma comment(lib,"oleaut32.lib ")
#pragma comment(lib,"uuid.lib  ")
#pragma comment(lib,"rpcrt4.lib  ")
#pragma comment(lib,"advapi32.lib ")
#pragma comment(lib,"wsock32.lib ")
#pragma comment(lib,"wininet.lib")



///FFT//
// #pragma comment(lib, "libfftw3-3.lib")
// #pragma comment(lib, "libfftw3f-3.lib")
// #pragma comment(lib, "libfftw3l-3.lib")
// 
// #pragma comment(lib,"..\\TdmsDll\\nilibddc.lib")

#ifdef _DEBUG
#pragma comment(lib,"wxmsw30ud_aui.lib")
#pragma comment(lib,"wxmsw30ud_html.lib ")
#pragma comment(lib,"wxmsw30ud_adv.lib ")
#pragma comment(lib,"wxmsw30ud_core.lib ")
#pragma comment(lib,"wxbase30ud_xml.lib ")
#pragma comment(lib,"wxbase30ud_net.lib ")
#pragma comment(lib,"wxbase30ud.lib ")
#pragma comment(lib,"wxtiffd.lib ")
#pragma comment(lib,"wxjpegd.lib")
#pragma comment(lib,"wxpngd.lib")
#pragma comment(lib,"wxzlibd.lib")
#pragma comment(lib,"wxregexud.lib")
#pragma comment(lib,"wxexpatd.lib ")
#pragma comment(lib,"comctl32.lib ")//socketËùÐè

#else
#pragma comment(lib,"wxmsw30u_aui.lib")
#pragma comment(lib,"wxmsw30u_html.lib ")
#pragma comment(lib,"wxmsw30u_adv.lib ")
#pragma comment(lib,"wxmsw30u_core.lib ")
#pragma comment(lib,"wxbase30u_xml.lib ")
#pragma comment(lib,"wxbase30u.lib ")
#pragma comment(lib,"wxtiff.lib ")
#pragma comment(lib,"wxjpeg.lib")
#pragma comment(lib,"wxpng.lib")
#pragma comment(lib,"wxzlib.lib")
#pragma comment(lib,"wxregexu.lib")
#pragma comment(lib,"wxexpat.lib ")
#endif