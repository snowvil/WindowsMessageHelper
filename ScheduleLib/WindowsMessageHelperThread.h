// Copyright (c) 2018 snowvil84@gmail.com
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <afxmt.h>
#include "Singleton.h"

#include <deque>
#include <map>
using namespace std;

#include <concurrent_queue.h>
#include <ppl.h>
using namespace Concurrency;

#ifdef _DEBUG
#define new DEBUG_NEW
#define _CRTDBG_MAP_ALLOC
#include "crtdbg.h"
#endif

// Foward Declarations
class CWindowsMessageParameter;

#define g_MessageHelper	CWindowsMessageHelperThread::GetSingleton()

/*
	// Target Windows Name Definitions
	#define WMH_TARGET_WND			_T("dialog")

	// User Message Definitions
	#define WMH_ID_TEST				WM_USER + 10

// Create

	CWindowsMessageHelperThread* pWMHelperThread = static_cast<CWindowsMessageHelperThread*>(AfxBeginThread(RUNTIME_CLASS(CWindowsMessageHelperThread),THREAD_PRIORITY_HIGHEST,0,CREATE_SUSPENDED));
	pWMHelperThread->AddTargetMsgWnd(WMH_TARGET_WND, (CWnd*)this);
	pWMHelperThread->ResumeThread();
	pWMHelperThread->SetSingleton(pWMHelperThread);


// Delete

	if(g_MessageHelper != nullptr)
	{
		g_MessageHelper->SetExitThread();
		WaitForSingleObject(g_MessageHelper->m_hThread,INFINITE);
	}

// Use Sender

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

// Use Receive

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

*/


class AFX_CLASS_EXPORT CWindowsMessage
{
public:
	enum eMSG_Type	{	eType_Send = 0,	eType_Post	};
	enum eJOB_Type	{	eType_General = 0, eType_Reserve	};

public:
	CWindowsMessage();
	CWindowsMessage(const CString& strTarget, const eMSG_Type& eMsgType, int nMsg);
	CWindowsMessage(bool bNoAllocParameter) {}	// Use Internal
	~CWindowsMessage(){}

public:
	bool									Execute(bool bUseQueueSync = false) noexcept;

public:
	constexpr CWindowsMessageParameter&		GetParameter() noexcept;
	constexpr CWindowsMessageParameter*		GetParameterPtr() noexcept;


	inline constexpr	eMSG_Type	GetMsgType() const noexcept { return m_eMsgType; }
	inline constexpr	eJOB_Type	GetJobType() const noexcept { return m_eJobType; }
	inline constexpr	bool		GetPriority() const noexcept { return m_bPriority; }
	inline constexpr	int			GetMsg() const noexcept { return m_nMsg; }
	inline constexpr	bool		GetStepSync() const noexcept { return m_bStepSync; }
	inline constexpr	int			GetStepSyncIndex() const noexcept { return m_nStepSyncIndex; }
	inline constexpr	bool		GetClearTask() const noexcept { return m_bClearTask; }
	inline 				CString		GetMsgTargetWnd() const noexcept { return m_strMsgTargetWnd; }


	inline constexpr	void		SetMsgType(eMSG_Type val) noexcept { m_eMsgType = val; }
	inline constexpr	void		SetJobType(eJOB_Type val) noexcept { m_eJobType = val; }
	inline constexpr	void		SetPriority(bool val) noexcept { m_bPriority = val; }
	inline constexpr	void		SetMsg(int val) noexcept { m_nMsg = val; }
	inline constexpr	void		SetStepSync(bool val) noexcept { m_bStepSync = val; }
	inline constexpr	void		SetStepSyncIndex(int val) noexcept { m_nStepSyncIndex = val; }
	inline constexpr	void		SetClearTask(bool val) noexcept { m_bClearTask = val; }
	inline 				void		SetMsgTargetWnd(CString val) noexcept { m_strMsgTargetWnd = val; }

private:
	eMSG_Type					m_eMsgType;					// Message Send Type
	eJOB_Type					m_eJobType;					// Job Type
	bool						m_bPriority;				// Priority Type
	int							m_nMsg;						// WM_USER + ???
	CWindowsMessageParameter*	m_pWindowsMessageParameter;	// User Parameter
	bool						m_bStepSync;				
	int							m_nStepSyncIndex;			
	bool						m_bClearTask;				// Remove All Queue Job
	CString						m_strMsgTargetWnd;			// Target Window Name
};


class AFX_CLASS_EXPORT CWindowsMessageHelperThread : public CWinThread, public CSingleton<CWindowsMessageHelperThread>
{
	DECLARE_DYNCREATE(CWindowsMessageHelperThread)

protected:
	CWindowsMessageHelperThread();           // protected constructor used by dynamic creation
	virtual ~CWindowsMessageHelperThread();

public:
	virtual BOOL						InitInstance();
	virtual int							ExitInstance();
	virtual int							Run();
	void								SetExitThread() noexcept;									//!< Main Thread Exit Signal

protected:
	DECLARE_MESSAGE_MAP()

public:
	void								SetSenderMaxDelay(int nDelayMilliseconds) noexcept;
	CWnd*								FindTargetWnd(const CString& strTargetWnd) noexcept;									// Find Target Window Handle
	bool								AddUCMessage(const CWindowsMessage& ucMsg, bool bUseQueueSync = false) noexcept;		// Attach Message

	void								AddTargetMsgWnd(const CString& strUniqueName, CWnd* pTargetMsgWnd) noexcept;		// Add Target Window Handle
	void								RemoveTargetMsgWnd(const CString& strUniqueName) noexcept;							// Remove Target Window Handle

	void								SetReserveJobLock(bool bIsLock) noexcept;				// Set Reserve Queue Lock
	bool								GetReserveJobLock() noexcept;							// Get Reserve Queue Lock

	bool								ReleaseStepSync(int nStepSyncIndex) noexcept;			// Run Next Sync Message

	int									GetReadyQueueSize();
	int									GetMsgQueueSize();
	int									GetReserveQueueSize();

private:
	void								AllClearQueue() noexcept;								// Clear All Queue

private:
	bool								m_bRun;													// Flag for thread running

	std::map<CString,CWnd*>				m_MapTargetWnd;											// Target Window List

	std::deque<CWindowsMessage>			m_qMsg;													// Normal Scheduler Queue
	std::deque<CWindowsMessage>			m_qReserveMsg;											// Reserve Scheduler Queue

	concurrent_queue<CWindowsMessage>	m_qReadyMsg;											// Ready Queue - Thread Safe

	bool								m_bReserveLock;											// Flag for Reserve Queue Lock
	
	unsigned long						m_lUCPLastIndex;										// Unique Index For Debugging

	CEvent								m_EventRun;												// Event for restart

	CEvent								m_EventStepSync;										// Event for step sync restart
	int									m_nLastStepSyncIndex;									// Step Sync Index

	int									m_nAdjustSleepCount;									// Thread Sleep Schedule Count

	int									m_nSenderMaxDelay;										// Balance Sender Max Delay
};


