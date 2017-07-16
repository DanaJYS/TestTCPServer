#pragma once

#include "wx/wx.h"
#include <wx/grid.h>
#include <wx/aui/aui.h>
#include "wx/statline.h"

#include "GlobalValue.h"
#include "TCPServerSocket.h"
#include "TCPCustomSocket.h"


class MyFrame : public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style);
	~MyFrame();

	void				CreateNoteBook();
	void				InitMainPanel();
	void				InitGridPanel();

	static void CALLBACK ClientConnect(void* pOwner, void* pSocket);			//���ڴ���ͻ��������¼�
	static void CALLBACK ClientClose(void* pOwner, void* pSocket);			//���ڴ���ͻ��˹ر��¼�
	static void CALLBACK ClientError(void* pOwner, void* pSocket, int nErrorCode);	//���ڴ���ͻ��˳����¼�
	static void CALLBACK ServerError(void* pOwner, void* pSocket, int nErrorCode);	//���ڴ������˳����¼�
	static void CALLBACK ClientRead(void* pOwner, void* pSocket, const char* buf, DWORD dwBufLen);		//���ڴ����ȡ�ͻ��������¼�

	void				OnListenButton(wxCommandEvent& event);
	void				OnCloseServerButton(wxCommandEvent& event);
	void				OnSendDataButton(wxCommandEvent& event);
	void				OnClearClientButton(wxCommandEvent& event);

public:
	CTCPServerSocket	m_TCPServer;
	wxString			m_ServerIP;
	int					m_ServerPort;

private:
	wxStatusBar*		StatusBar;

	wxAuiNotebook*		m_notemgr;
	wxPanel*			MainPanel;
	wxPanel*			GridPanel;

	wxTextCtrl*			m_textCtrlIP;
	wxTextCtrl*			m_textCtrlPort;
	wxGrid*				m_grid;
	wxTextCtrl*			m_textCtrlSend;
	wxTextCtrl*			m_textCtrlReceive;

	DECLARE_EVENT_TABLE()
};