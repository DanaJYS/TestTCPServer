#include "MyFrame.h"

enum
{
	ID_ListenButton,
	ID_DisConnectButton,
	ID_SendDataButton,
	ID_ClearButton
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_BUTTON(ID_ListenButton, MyFrame::OnListenButton)
	EVT_BUTTON(ID_DisConnectButton, MyFrame::OnCloseServerButton)
	EVT_BUTTON(ID_SendDataButton, MyFrame::OnSendDataButton)
	EVT_BUTTON(ID_ClearButton, MyFrame::OnClearClientButton)
END_EVENT_TABLE()


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
				:wxFrame((wxFrame*)NULL, wxID_ANY, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	m_ServerIP = wxT("192.168.1.109");
	m_ServerPort = 8080;

	CreateNoteBook();

	StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	m_notemgr->Update();
}

MyFrame::~MyFrame()
{
	
}

void MyFrame::CreateNoteBook()
{
	wxSize clientSize = GetClientSize();
	m_notemgr = new wxAuiNotebook(this, wxID_ANY,wxPoint(clientSize.x, clientSize.y),wxDefaultSize,wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER);

	MainPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	InitMainPanel();
	m_notemgr->AddPage(MainPanel, wxT("����˹���"));

	GridPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	InitGridPanel();
	m_notemgr->AddPage(GridPanel, wxT("�ͻ����б�"));
}

void MyFrame::InitMainPanel()
{
	wxStaticText*			m_TextIP;
	wxStaticText*			m_TextPort;
	wxStaticLine*			m_staticline1;
	wxButton*				m_ListenButton;
	wxButton*				m_CloseButton;
	wxButton*				m_SendButton;
	wxStaticLine*			m_staticline3;
	wxStaticText*			m_TextSend;
	wxStaticLine*			m_staticline4;
	wxStaticText*			m_staticTextReceive;

	wxBoxSizer* TopSizer;
	TopSizer = new wxBoxSizer( wxVERTICAL );					//�������㲼��

	wxFlexGridSizer* SetIPSizer;								//����IP�������ɱ����񲼾�
	SetIPSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	SetIPSizer->SetFlexibleDirection( wxBOTH );
	SetIPSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_TextIP = new wxStaticText( MainPanel, wxID_ANY, wxT("������IP:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextIP->Wrap( -1 );
	SetIPSizer->Add( m_TextIP, 0, wxALL, 5 );

	m_textCtrlIP = new wxTextCtrl( MainPanel, wxID_ANY, m_ServerIP, wxDefaultPosition, wxDefaultSize, 0 );
	SetIPSizer->Add( m_textCtrlIP, 0, wxALL, 5 );

	m_TextPort = new wxStaticText( MainPanel, wxID_ANY, wxT("�������˿ں�:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextPort->Wrap( -1 );
	SetIPSizer->Add( m_TextPort, 0, wxALL, 5 );

	wxString str;
	str = wxString::Format(wxT("%d"), m_ServerPort);
	m_textCtrlPort = new wxTextCtrl( MainPanel, wxID_ANY, str, wxDefaultPosition, wxDefaultSize, 0 );
	SetIPSizer->Add( m_textCtrlPort, 0, wxALL, 5 );


	TopSizer->Add( SetIPSizer, 1, wxEXPAND, 5 );					//��IP�������ɱ����񲼾���ӵ����㲼��

	m_staticline1 = new wxStaticLine( MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	TopSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* ButtonSizer;
	ButtonSizer = new wxBoxSizer( wxHORIZONTAL );					//����������ˮƽ����

	m_ListenButton = new wxButton( MainPanel, ID_ListenButton, wxT("����"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( m_ListenButton, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );

	m_CloseButton = new wxButton( MainPanel, ID_DisConnectButton, wxT("�ر�"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( m_CloseButton, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );

	m_SendButton = new wxButton( MainPanel, ID_SendDataButton, wxT("��������"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( m_SendButton, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );


	TopSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );					//��������ˮƽ������ӵ����㲼��

	m_staticline3 = new wxStaticLine( MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	TopSizer->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* SendSizer;										//�������ݷ�������ֱ����
	SendSizer = new wxBoxSizer( wxVERTICAL );

	m_TextSend = new wxStaticText( MainPanel, wxID_ANY, wxT("��������:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextSend->Wrap( -1 );
	SendSizer->Add( m_TextSend, 0, wxALL, 5 );

	m_textCtrlSend = new wxTextCtrl( MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL|wxTE_MULTILINE );
	SendSizer->Add( m_textCtrlSend, 1, wxALL|wxEXPAND, 5 );


	TopSizer->Add( SendSizer, 3, wxEXPAND, 5 );

	m_staticline4 = new wxStaticLine( MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	TopSizer->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* RecSizer;
	RecSizer = new wxBoxSizer( wxVERTICAL );

	m_staticTextReceive = new wxStaticText( MainPanel, wxID_ANY, wxT("��������:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextReceive->Wrap( -1 );
	RecSizer->Add( m_staticTextReceive, 0, wxALL, 5 );

	m_textCtrlReceive = new wxTextCtrl( MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL|wxTE_MULTILINE );
	RecSizer->Add( m_textCtrlReceive, 1, wxALL|wxEXPAND, 5 );


	TopSizer->Add( RecSizer, 3, wxEXPAND, 5 );


	MainPanel->SetSizer( TopSizer );
	MainPanel->Layout();
}

void MyFrame::InitGridPanel()
{
	wxButton* ClearButton;

	GridPanel->SetSizeHints(wxDefaultSize, wxDefaultSize);
	wxStaticBoxSizer* TopSizer;
	TopSizer = new wxStaticBoxSizer( new wxStaticBox( GridPanel, wxID_ANY, wxT("�����ӿͻ���:") ), wxVERTICAL );

	//�������������ʾ�Ѿ����ӵĿͻ���IP��˿ں�
	m_grid = new wxGrid( GridPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	//Grid
	m_grid->CreateGrid( 10, 3 );
	m_grid->EnableEditing( true );
	m_grid->EnableGridLines( true );
	m_grid->EnableDragGridSize( false );
	m_grid->SetMargins( 0, 0 );

	// Columns
	m_grid->EnableDragColMove( false );
	m_grid->EnableDragColSize( true );
	m_grid->SetColLabelSize( 30 );
	m_grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Rows
	m_grid->EnableDragRowSize( true );
	m_grid->SetRowLabelSize( 80 );
	m_grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
	
	// Label Appearance
	
	// Cell Defaults
	m_grid->SetDefaultCellAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	m_grid->SetColLabelValue(0, wxT("�ͻ���IP"));
	m_grid->SetColLabelValue(1, wxT("�ͻ��˶˿ں�"));
	m_grid->SetColLabelValue(2, wxT("ѡ��"));

	for (int i = 0; i<10; i++)
	{
		m_grid->SetCellRenderer(i, 2, new wxGridCellBoolRenderer);
		m_grid->SetCellEditor(i, 2, new wxGridCellBoolEditor);
	}

	TopSizer->Add( m_grid, 0, wxALL|wxEXPAND, 5 );

	//����������
	ClearButton = new wxButton(GridPanel, ID_ClearButton, wxT("����ͻ���"), wxDefaultPosition, wxDefaultSize, 0 );
	TopSizer->Add(ClearButton, 0, wxALL, 5);

	GridPanel->SetSizer(TopSizer);
	GridPanel->Layout();

	GridPanel->Center(wxBOTH);
}

void MyFrame::OnListenButton(wxCommandEvent& event)
{
	if (m_ServerIP != m_textCtrlIP->GetValue())
	{
		m_ServerIP = m_textCtrlIP->GetValue();
	}

	if (m_ServerPort != wxAtoi(m_textCtrlPort->GetValue()))
	{
		m_ServerPort = wxAtoi(m_textCtrlPort->GetValue());				//wxAtoi()���ڽ�wxString����ת��Ϊint����
	}

	if(m_TCPServer.Open(this, m_ServerIP.mb_str(), m_ServerPort))
	{
		SetStatusText(wxT("��ʼ����"));
	}
	else
	{
		SetStatusText(wxT("����ʧ��"));
	}

	m_TCPServer.OnClientConnect = ClientConnect;
	m_TCPServer.OnClientClose = ClientClose;
	m_TCPServer.OnClientError = ClientError;
	m_TCPServer.OnClientRead = ClientRead;
	m_TCPServer.OnServerError = ServerError;
}

void MyFrame::OnCloseServerButton(wxCommandEvent& event)
{
	//���������������
	int ClientNumberCSB = ActiveClientList.size();
	for (int i = 0; i<ClientNumberCSB; i++)
	{
		m_grid->SetCellValue(i, 0, wxT(""));
		m_grid->SetCellValue(i, 1, wxT(""));
		//m_grid->SetCellValue(i, 2, wxT("0"));
	}

	if (m_TCPServer.Close())
	{
		SetStatusText(wxT("�Ͽ��ɹ�"));
	}
	else
	{
		SetStatusText(wxT("�Ͽ�ʧ��"));
	}
}

void MyFrame::OnSendDataButton(wxCommandEvent& event)
{
	wxString strSend = m_textCtrlSend->GetValue();
	const char* charSend = strSend.mb_str();

	int dataLen = strlen(charSend);

	int ClientNumberSD = ActiveClientList.size();
	for (int i = 0; i<ClientNumberSD; i++)
	{
		int IsSend = wxAtoi(m_grid->GetCellValue(i, 2));
		if (IsSend == 1)
		{
			CTCPCustomSocket* pCustomSocketSend = (CTCPCustomSocket*)ActiveClientList[i];
			pCustomSocketSend->SendData(charSend, dataLen);
		}
	}
}

void CALLBACK MyFrame::ClientConnect(void* pOwner, void* pSocket)
{
	MyFrame* pWnd = (MyFrame*)pOwner;
	CTCPCustomSocket* pClientConnect = (CTCPCustomSocket*)pSocket;

	wxString strClientIP(pClientConnect->m_RemoteHost, wxConvUTF8);							//���ͻ��˵�IP��ַ��char��ת��ΪwxString��
	wxString strClientPort = wxString::Format(wxT("%d"), pClientConnect->m_RemotePort);		//���ͻ��˵Ķ˿ں���int��ת��ΪwxString��
	pWnd->SetStatusText(wxT("�ͻ���IP:")+strClientIP+wxT(", ")+strClientPort+wxT(" ���ӳɹ�"));

	//���½����ϱ��
	
	int ClientNumber = ActiveClientList.size();
	int GridRowsNumber = pWnd->m_grid->GetNumberRows();

	//��������ӿͻ��������ڱ���������������㹻������
	if (ClientNumber > GridRowsNumber)
	{
		int NewRows = ClientNumber-GridRowsNumber;
		pWnd->m_grid->AppendRows(NewRows);
		for (int j = GridRowsNumber; j<ClientNumber; j++)
		{
			pWnd->m_grid->SetCellRenderer(j, 2, new wxGridCellBoolRenderer);
			pWnd->m_grid->SetCellEditor(j, 2, new wxGridCellBoolEditor);
		}
	}

	//���ͻ��˴洢�б��е�������ʾ�������
	for (int i = 0; i<ClientNumber; i++)
	{
		CTCPCustomSocket* pClientConnectRF = (CTCPCustomSocket*)ActiveClientList[i];
		wxString strClientRFIP(pClientConnectRF->m_RemoteHost, wxConvUTF8);
		wxString strClientRFPort = wxString::Format(wxT("%d"), pClientConnectRF->m_RemotePort);
		pWnd->m_grid->SetCellValue(i, 0, strClientRFIP);
		pWnd->m_grid->SetCellValue(i, 1, strClientRFPort);
	}
}

void CALLBACK MyFrame::ClientClose(void* pOwner, void* pSocket)
{
	MyFrame* pWnd = (MyFrame*)pOwner;
	CTCPCustomSocket* pClientClose = (CTCPCustomSocket*)pSocket;

	wxString strClientIP(pClientClose->m_RemoteHost, wxConvUTF8);							//���ͻ��˵�IP��ַ��char��ת��ΪwxString��
	wxString strClientPort = wxString::Format(wxT("%d"), pClientClose->m_RemotePort);		//���ͻ��˵Ķ˿ں���int��ת��ΪwxString��
	pWnd->SetStatusText(wxT("�ͻ���IP:")+strClientIP+wxT(", ")+strClientPort+wxT(" �Ͽ�����"));

	int ClientNumber = ActiveClientList.size();
	int GridRowsNum = pWnd->m_grid->GetNumberRows();
	for (int i = 0; i<ClientNumber; i++)
	{
		if (pClientClose == ActiveClientList[i])
		{
			pWnd->m_grid->DeleteRows(i, 1, false);
			pWnd->m_grid->AppendRows(1);					//ɾ��ĳһ�ض��к�׷��һ�հ��У��������������ı�
			pWnd->m_grid->SetCellRenderer(GridRowsNum-1, 2, new wxGridCellBoolRenderer);
			pWnd->m_grid->SetCellEditor(GridRowsNum-1, 2, new wxGridCellBoolEditor);
			break;
		}
	}
}

void CALLBACK MyFrame::ClientError(void* pOwner, void* pSocket, int nErrorCode)
{
	MyFrame* pWnd = (MyFrame*)pOwner;
	CTCPCustomSocket* pClientError= (CTCPCustomSocket*)pSocket;

	wxString strClientIP(pClientError->m_RemoteHost, wxConvUTF8);							//���ͻ��˵�IP��ַ��char��ת��ΪwxString��
	wxString strClientPort = wxString::Format(wxT("%d"), pClientError->m_RemotePort);		//���ͻ��˵Ķ˿ں���int��ת��ΪwxString��
	pWnd->SetStatusText(wxT("�ͻ���IP:")+strClientIP+wxT(", ")+strClientPort+wxT(" ����"));
}

void CALLBACK MyFrame::ServerError(void* pOwner, void* pSocket, int nErrorCode)
{
	MyFrame* pWnd = (MyFrame*)pOwner;
	pWnd->SetStatusText(wxT("����˳�����������"));
}

void CALLBACK MyFrame::ClientRead(void* pOwner, void* pSocket, const char* buf, DWORD dwBufLen)
{
	MyFrame* pWnd = (MyFrame*)pOwner;
	CTCPCustomSocket* pClientSocket1 = (CTCPCustomSocket*)pSocket;

	wxString strClientIP(pClientSocket1->m_RemoteHost, wxConvUTF8);		//���������ݿͻ��˵�IP��ַת��ΪwxString���ַ�
	wxString strClientPort = wxString::Format(wxT("%d"), pClientSocket1->m_RemotePort);		//���������ݿͻ��˵Ķ˿ں�ת��ΪwxString���ַ�
	pWnd->m_textCtrlReceive->AppendText(wxT("���Կͻ��ˣ�IP:")+strClientIP+wxT("; �˿ں�:")+strClientPort+"\n");

	wxString strData(buf, wxConvUTF8);							//��char����ת��ΪwxString����
	pWnd->m_textCtrlReceive->AppendText(strData+"\n");
}

void MyFrame::OnClearClientButton(wxCommandEvent& event)
{
	int GridRowsNum = m_grid->GetNumberRows();						//��ȡ�����ϱ�������
	int ClientCCB = ActiveClientList.size();						//��ȡ�����ӿͻ��˵ĸ���

	for (int i = ClientCCB; i>=0; i--)								//�˴�һ��Ҫ����ѭ��������Ļ�ɾ��һ�л�ʹ�кű仯����������ɾ������Ĳ���Ӱ��ǰ���
	{
		int IsChoose = wxAtoi(m_grid->GetCellValue(i, 2));			//�жϱ���ĳһ���Ƿ�ѡ�У�����˵ĳ�������ӿͻ����Ƿ�ѡ��
		if (IsChoose == 1)											//�������ĳ�б�ѡ�У���ɾ�����У����Ͽ���ÿͻ��˵�����
		{
			CTCPCustomSocket* pClientChoosed = (CTCPCustomSocket*)ActiveClientList[i];
			//�����ͻ����̣߳��ر�socket
			pClientChoosed->Close();
			//�ͷ��ڴ棬���ÿͻ��˴Ӵ洢�ͻ����б����Ƴ�
			m_TCPServer.RemoteClient(pClientChoosed);
			//�ڱ��������ͻ�����ʾ
			m_grid->DeleteRows(i, 1, false);
			m_grid->AppendRows(1);									//ɾ��ĳһ�ض��к�׷��һ�հ��У��������������ı�
			m_grid->SetCellRenderer(GridRowsNum-1, 2, new wxGridCellBoolRenderer);
			m_grid->SetCellEditor(GridRowsNum-1, 2, new wxGridCellBoolEditor);
		}
	}
}
