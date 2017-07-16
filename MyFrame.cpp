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
	m_notemgr->AddPage(MainPanel, wxT("服务端管理"));

	GridPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	InitGridPanel();
	m_notemgr->AddPage(GridPanel, wxT("客户端列表"));
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
	TopSizer = new wxBoxSizer( wxVERTICAL );					//创建顶层布局

	wxFlexGridSizer* SetIPSizer;								//创建IP设置区可变网格布局
	SetIPSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	SetIPSizer->SetFlexibleDirection( wxBOTH );
	SetIPSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_TextIP = new wxStaticText( MainPanel, wxID_ANY, wxT("服务器IP:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextIP->Wrap( -1 );
	SetIPSizer->Add( m_TextIP, 0, wxALL, 5 );

	m_textCtrlIP = new wxTextCtrl( MainPanel, wxID_ANY, m_ServerIP, wxDefaultPosition, wxDefaultSize, 0 );
	SetIPSizer->Add( m_textCtrlIP, 0, wxALL, 5 );

	m_TextPort = new wxStaticText( MainPanel, wxID_ANY, wxT("服务器端口号:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextPort->Wrap( -1 );
	SetIPSizer->Add( m_TextPort, 0, wxALL, 5 );

	wxString str;
	str = wxString::Format(wxT("%d"), m_ServerPort);
	m_textCtrlPort = new wxTextCtrl( MainPanel, wxID_ANY, str, wxDefaultPosition, wxDefaultSize, 0 );
	SetIPSizer->Add( m_textCtrlPort, 0, wxALL, 5 );


	TopSizer->Add( SetIPSizer, 1, wxEXPAND, 5 );					//将IP设置区可变网格布局添加到顶层布局

	m_staticline1 = new wxStaticLine( MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	TopSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* ButtonSizer;
	ButtonSizer = new wxBoxSizer( wxHORIZONTAL );					//创建按键区水平布局

	m_ListenButton = new wxButton( MainPanel, ID_ListenButton, wxT("监听"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( m_ListenButton, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );

	m_CloseButton = new wxButton( MainPanel, ID_DisConnectButton, wxT("关闭"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( m_CloseButton, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );

	m_SendButton = new wxButton( MainPanel, ID_SendDataButton, wxT("发送数据"), wxDefaultPosition, wxDefaultSize, 0 );
	ButtonSizer->Add( m_SendButton, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );


	TopSizer->Add( ButtonSizer, 1, wxEXPAND, 5 );					//将按键区水平布局添加到顶层布局

	m_staticline3 = new wxStaticLine( MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	TopSizer->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* SendSizer;										//创建数据发送区垂直布局
	SendSizer = new wxBoxSizer( wxVERTICAL );

	m_TextSend = new wxStaticText( MainPanel, wxID_ANY, wxT("发送数据:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextSend->Wrap( -1 );
	SendSizer->Add( m_TextSend, 0, wxALL, 5 );

	m_textCtrlSend = new wxTextCtrl( MainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_AUTO_URL|wxTE_MULTILINE );
	SendSizer->Add( m_textCtrlSend, 1, wxALL|wxEXPAND, 5 );


	TopSizer->Add( SendSizer, 3, wxEXPAND, 5 );

	m_staticline4 = new wxStaticLine( MainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	TopSizer->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* RecSizer;
	RecSizer = new wxBoxSizer( wxVERTICAL );

	m_staticTextReceive = new wxStaticText( MainPanel, wxID_ANY, wxT("接收数据:"), wxDefaultPosition, wxDefaultSize, 0 );
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
	TopSizer = new wxStaticBoxSizer( new wxStaticBox( GridPanel, wxID_ANY, wxT("已连接客户端:") ), wxVERTICAL );

	//创建表格，用于显示已经连接的客户端IP与端口号
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

	m_grid->SetColLabelValue(0, wxT("客户端IP"));
	m_grid->SetColLabelValue(1, wxT("客户端端口号"));
	m_grid->SetColLabelValue(2, wxT("选中"));

	for (int i = 0; i<10; i++)
	{
		m_grid->SetCellRenderer(i, 2, new wxGridCellBoolRenderer);
		m_grid->SetCellEditor(i, 2, new wxGridCellBoolEditor);
	}

	TopSizer->Add( m_grid, 0, wxALL|wxEXPAND, 5 );

	//添加清除按键
	ClearButton = new wxButton(GridPanel, ID_ClearButton, wxT("清除客户端"), wxDefaultPosition, wxDefaultSize, 0 );
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
		m_ServerPort = wxAtoi(m_textCtrlPort->GetValue());				//wxAtoi()用于将wxString类型转换为int类型
	}

	if(m_TCPServer.Open(this, m_ServerIP.mb_str(), m_ServerPort))
	{
		SetStatusText(wxT("开始监听"));
	}
	else
	{
		SetStatusText(wxT("监听失败"));
	}

	m_TCPServer.OnClientConnect = ClientConnect;
	m_TCPServer.OnClientClose = ClientClose;
	m_TCPServer.OnClientError = ClientError;
	m_TCPServer.OnClientRead = ClientRead;
	m_TCPServer.OnServerError = ServerError;
}

void MyFrame::OnCloseServerButton(wxCommandEvent& event)
{
	//清除界面表格中数据
	int ClientNumberCSB = ActiveClientList.size();
	for (int i = 0; i<ClientNumberCSB; i++)
	{
		m_grid->SetCellValue(i, 0, wxT(""));
		m_grid->SetCellValue(i, 1, wxT(""));
		//m_grid->SetCellValue(i, 2, wxT("0"));
	}

	if (m_TCPServer.Close())
	{
		SetStatusText(wxT("断开成功"));
	}
	else
	{
		SetStatusText(wxT("断开失败"));
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

	wxString strClientIP(pClientConnect->m_RemoteHost, wxConvUTF8);							//将客户端的IP地址由char型转换为wxString型
	wxString strClientPort = wxString::Format(wxT("%d"), pClientConnect->m_RemotePort);		//将客户端的端口号由int型转换为wxString型
	pWnd->SetStatusText(wxT("客户端IP:")+strClientIP+wxT(", ")+strClientPort+wxT(" 连接成功"));

	//更新界面上表格
	
	int ClientNumber = ActiveClientList.size();
	int GridRowsNumber = pWnd->m_grid->GetNumberRows();

	//如果已连接客户端数大于表格行数，则新增足够的行数
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

	//将客户端存储列表中的数据显示到表格中
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

	wxString strClientIP(pClientClose->m_RemoteHost, wxConvUTF8);							//将客户端的IP地址由char型转换为wxString型
	wxString strClientPort = wxString::Format(wxT("%d"), pClientClose->m_RemotePort);		//将客户端的端口号由int型转换为wxString型
	pWnd->SetStatusText(wxT("客户端IP:")+strClientIP+wxT(", ")+strClientPort+wxT(" 断开连接"));

	int ClientNumber = ActiveClientList.size();
	int GridRowsNum = pWnd->m_grid->GetNumberRows();
	for (int i = 0; i<ClientNumber; i++)
	{
		if (pClientClose == ActiveClientList[i])
		{
			pWnd->m_grid->DeleteRows(i, 1, false);
			pWnd->m_grid->AppendRows(1);					//删除某一特定行后，追加一空白行，保持总行数不改变
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

	wxString strClientIP(pClientError->m_RemoteHost, wxConvUTF8);							//将客户端的IP地址由char型转换为wxString型
	wxString strClientPort = wxString::Format(wxT("%d"), pClientError->m_RemotePort);		//将客户端的端口号由int型转换为wxString型
	pWnd->SetStatusText(wxT("客户端IP:")+strClientIP+wxT(", ")+strClientPort+wxT(" 出错"));
}

void CALLBACK MyFrame::ServerError(void* pOwner, void* pSocket, int nErrorCode)
{
	MyFrame* pWnd = (MyFrame*)pOwner;
	pWnd->SetStatusText(wxT("服务端出错，监听结束"));
}

void CALLBACK MyFrame::ClientRead(void* pOwner, void* pSocket, const char* buf, DWORD dwBufLen)
{
	MyFrame* pWnd = (MyFrame*)pOwner;
	CTCPCustomSocket* pClientSocket1 = (CTCPCustomSocket*)pSocket;

	wxString strClientIP(pClientSocket1->m_RemoteHost, wxConvUTF8);		//将发送数据客户端的IP地址转换为wxString型字符
	wxString strClientPort = wxString::Format(wxT("%d"), pClientSocket1->m_RemotePort);		//将发送数据客户端的端口号转换为wxString型字符
	pWnd->m_textCtrlReceive->AppendText(wxT("来自客户端：IP:")+strClientIP+wxT("; 端口号:")+strClientPort+"\n");

	wxString strData(buf, wxConvUTF8);							//将char类型转换为wxString类型
	pWnd->m_textCtrlReceive->AppendText(strData+"\n");
}

void MyFrame::OnClearClientButton(wxCommandEvent& event)
{
	int GridRowsNum = m_grid->GetNumberRows();						//获取界面上表格的行数
	int ClientCCB = ActiveClientList.size();						//获取已连接客户端的个数

	for (int i = ClientCCB; i>=0; i--)								//此处一定要倒序循环，正序的话删除一行会使行号变化出错，倒序则删除后面的不会影响前面的
	{
		int IsChoose = wxAtoi(m_grid->GetCellValue(i, 2));			//判断表格的某一行是否选中，或者说某个已连接客户端是否选中
		if (IsChoose == 1)											//如果表格的某行被选中，则删除该行，并断开与该客户端的链接
		{
			CTCPCustomSocket* pClientChoosed = (CTCPCustomSocket*)ActiveClientList[i];
			//结束客户端线程，关闭socket
			pClientChoosed->Close();
			//释放内存，将该客户端从存储客户端列表中移除
			m_TCPServer.RemoteClient(pClientChoosed);
			//在表格中清除客户端显示
			m_grid->DeleteRows(i, 1, false);
			m_grid->AppendRows(1);									//删除某一特定行后，追加一空白行，保持总行数不改变
			m_grid->SetCellRenderer(GridRowsNum-1, 2, new wxGridCellBoolRenderer);
			m_grid->SetCellEditor(GridRowsNum-1, 2, new wxGridCellBoolEditor);
		}
	}
}
