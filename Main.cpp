#include "Main.h"
#include "MyFrame.h"

DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
	MyFrame* frame = new MyFrame(wxT("�����ʾ��"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL);

	frame->Show(true);
	return true;
}