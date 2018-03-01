# Windows Message Helper

This library is useful for sending messages using SendMessage and PostMessage on C ++ / MFC based.
You can easily attach various types of variables.


## Define
```
	// Target Windows Name Definitions
	#define WMH_TARGET_WND			_T("dialog")

	// User Message Definitions
	#define WMH_ID_TEST				WM_USER + 10
```

## Create
```
	CWindowsMessageHelperThread* pWMHelperThread = static_cast<CWindowsMessageHelperThread*>(AfxBeginThread(RUNTIME_CLASS(CWindowsMessageHelperThread),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED));
	pWMHelperThread->AddTargetMsgWnd(WMH_TARGET_WND, (CWnd*)this);
	pWMHelperThread->ResumeThread();
	pWMHelperThread->SetSingleton(pWMHelperThread);
```

## Delete
```
	if(g_MessageHelper != nullptr)
	{
		g_MessageHelper->SetExitThread();
		WaitForSingleObject(g_MessageHelper->m_hThread,INFINITE);
	}
```
## Use
### Sender
```
	CWindowsMessage wmp(WMH_TARGET_WND, CWindowsMessage::eMSG_Type::eType_Post, WMH_ID_TEST);

	// Setting Parameter
	wmp.GetParameter().SetData<int>(1);
	wmp.GetParameter().SetData<int>(2);
	wmp.GetParameter().SetData<float>(0.1f);
	wmp.GetParameter().SetData<float>(0.2f);

	wmp.GetParameter().SetData<CString>(_T("String Test1"));
	wmp.GetParameter().SetData<CString>(_T("String Test2"));
	wmp.GetParameter().SetData< pair<char*,int> >( pair<char*, int>(new char[1024], 1024 ) );

	CTestObj1 obj1;
	obj1.m_nA = 10;
	obj1.m_strB = _T("Class String Test1");
	wmp.GetParameter().SetData<CTestObj1>(obj1);

	CTestObj1* pObj1 = new CTestObj1();
	pObj1->m_nA = 11;
	pObj1->m_strB = _T("Class String Test2");
	wmp.GetParameter().SetData<CTestObj1*>(pObj1);

	// If you enter true for the bUseQueueSync variable, the Sleep code will block the sending thread to prevent performance degradation if the message pool is full.
	// If you enter false for the bUseQueueSync variable, it works asynchronously.
	wmp.Execute(true);
```

### Receive
```
	.H
	afx_msg LRESULT OnWindowsMessageHelperTest(WPARAM wParam, LPARAM lParam);

	.cpp
	BEGIN_MESSAGE_MAP(CMessageHelperTestDlg, CDialog)
	...
		ON_MESSAGE(WMH_ID_TEST, &CMessageHelperTestDlg::OnWindowsMessageHelperTest)
	...
	END_MESSAGE_MAP()


	LRESULT CMessageHelperTestDlg::OnWindowsMessageHelperTest(WPARAM wParam, LPARAM lParam)
	{
		CAutoRecvWMP WMP(wParam);

		int nVar1 = WMP.GetData<int>();
		int nVar2 = WMP.GetData<int>();

		ASSERT(nVar1 == 1);
		ASSERT(nVar2 == 2);

		float fVar1 = WMP.GetData<float>();
		float fVar2 = WMP.GetData<float>();

		ASSERT(fVar1 == 0.1f);
		ASSERT(fVar2 == 0.2f);


		CString strVar1 = WMP.GetData<CString>();
		CString strVar2 = WMP.GetData<CString>();

		ASSERT(strVar1 == _T("String Test1"));
		ASSERT(strVar2 == _T("String Test2"));


		pair<char*, int> buffer = WMP.GetData< pair<char*, int> >();
		delete buffer.first;

		CTestObj1 obj1 = WMP.GetData<CTestObj1>();
		ASSERT(obj1.m_nA == 10);
		ASSERT(obj1.m_strB == _T("Class String Test1"));

		CTestObj1* pObj1 = WMP.GetData<CTestObj1*>();
		ASSERT(pObj1->m_nA == 11);
		ASSERT(pObj1->m_strB == _T("Class String Test2"));
		delete pObj1;

		return 1;
	}
```
