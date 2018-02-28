// WindowsMessageHelperThread.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsMessageHelperThread.h"

#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

#include "WindowsMessageParameter.h"

// CWindowsMessageHelperThread

IMPLEMENT_DYNCREATE(CWindowsMessageHelperThread, CWinThread)

CWindowsMessageHelperThread::CWindowsMessageHelperThread()
{
	timeBeginPeriod(1);

	m_bAutoDelete		= TRUE;
	m_bRun				= false;
	m_lUCPLastIndex		= 0;
	m_bReserveLock		= false;

	m_nAdjustSleepCount	= 0;

	m_nSenderMaxDelay	= 20;
}

CWindowsMessageHelperThread::~CWindowsMessageHelperThread()
{
	m_EventRun.ResetEvent();
	m_EventStepSync.ResetEvent();

	AllClearQueue();

	m_MapTargetWnd.clear();
}


BOOL CWindowsMessageHelperThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	m_bRun = true;
	m_EventRun.ResetEvent();
	m_EventStepSync.ResetEvent();
	return TRUE;
}

int CWindowsMessageHelperThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWindowsMessageHelperThread, CWinThread)
END_MESSAGE_MAP()


// CWindowsMessageHelperThread message handlers
int CWindowsMessageHelperThread::Run()
{
	// TODO: Add your specialized code here and/or call the base class
	while( true == m_bRun )
	{
		if( true == m_qReadyMsg.empty() && true == m_qMsg.empty() && true == m_qReserveMsg.empty() )
		{
			// Ready, Msg, Reserve Queue Empty
			::WaitForSingleObject(m_EventRun,INFINITE);
			m_EventRun.ResetEvent();
		}
		else
		{
			if( true == m_bReserveLock && false == m_qReserveMsg.empty() )
			{
				// ReserveLock True, Reserve Queue Not Empty
				::WaitForSingleObject(m_EventRun,INFINITE);
				m_EventRun.ResetEvent();
			}
			else
			{
				Sleep(0);
			}
		}



		if( true == m_MapTargetWnd.empty() )
		{
			// Target Windows Empty
			::WaitForSingleObject(m_EventRun,INFINITE);
			m_EventRun.ResetEvent();
			continue;
		}
		
		// Ready Queue -> Message, Reserve Queue
		while ( false == m_qReadyMsg.empty() )
		{
			// Empty Object
			CWindowsMessage ucMsg(false);
			if( true == m_qReadyMsg.try_pop(ucMsg) )
			{
				if(true == ucMsg.GetClearTask())
				{
					AllClearQueue();
					delete ucMsg.GetParameterPtr();
					continue;
				}

				// Assign Queue Type
				std::deque<CWindowsMessage>* pdqMsg = nullptr;
				if(CWindowsMessage::eJOB_Type::eType_General == ucMsg.GetJobType())
				{
					pdqMsg = &m_qMsg;
				}
				else	// eType_Reserve
				{
					pdqMsg = &m_qReserveMsg;
				}

				if(true == ucMsg.GetPriority())
				{
					pdqMsg->push_front(ucMsg);
				}
				else
				{
					pdqMsg->push_back(ucMsg);
				}
			}

		}

		// Priority Reserve Queue
		std::deque<CWindowsMessage>* pdqMsg = nullptr;
		if( false == m_qReserveMsg.empty() && false == m_bReserveLock)
			pdqMsg = &m_qReserveMsg;
		else
			pdqMsg = &m_qMsg;

		if( nullptr != pdqMsg  && false == pdqMsg->empty())
		{
			CWindowsMessage& ucMsg = pdqMsg->front();
			CWnd* pTargetWnd = FindTargetWnd(ucMsg.GetMsgTargetWnd());
			if (nullptr != pTargetWnd)
			{

				// Debugging Mark
				if (-1 == ucMsg.GetParameterPtr()->GetUniqueIndex())
				{
					ucMsg.GetParameterPtr()->SetUniqueIndex(++m_lUCPLastIndex);
					ucMsg.GetParameterPtr()->SetMsgIdx(ucMsg.GetMsg());
				}

				if (CWindowsMessage::eMSG_Type::eType_Send == ucMsg.GetMsgType())
				{
					pTargetWnd->SendMessage(ucMsg.GetMsg(), (WPARAM)ucMsg.GetParameterPtr(), 0);
				}
				else if (CWindowsMessage::eMSG_Type::eType_Post == ucMsg.GetMsgType())
				{
					if (FALSE == pTargetWnd->PostMessage(ucMsg.GetMsg(), (WPARAM)ucMsg.GetParameterPtr(), 0))
					{
						if (ERROR_NOT_ENOUGH_QUOTA == GetLastError())
						{
							m_nAdjustSleepCount += 50;
							Sleep(m_nAdjustSleepCount);
							CString strTmp;
							strTmp.Format(_T("Warning - [%s] Message Pool is Full!! Sleep(%d) and Retry PostMessage Msg[%d] !\n"), ucMsg.GetMsgTargetWnd(), m_nAdjustSleepCount, ucMsg.GetMsg());
							OutputDebugString(strTmp);							
						}
						continue;
					}
					else
					{
						if (m_nAdjustSleepCount > 0)
						{
							m_nAdjustSleepCount = (m_nAdjustSleepCount - 5) > 0 ? m_nAdjustSleepCount - 5 : 0;
						}
					}
				}
				Sleep(m_nAdjustSleepCount);
			}
			else
			{
				delete ucMsg.GetParameterPtr();
			}
		
			if(true == ucMsg.GetStepSync())
			{
				m_nLastStepSyncIndex = ucMsg.GetStepSyncIndex();
				::WaitForSingleObject(m_EventStepSync,INFINITE);
				m_EventStepSync.ResetEvent();
			}
			pdqMsg->pop_front();
		}
			
	}
	return ExitInstance();
	return __super::Run();
}

void CWindowsMessageHelperThread::SetExitThread() noexcept
{
	m_bRun = FALSE;
	m_EventRun.SetEvent();
	m_EventStepSync.SetEvent();
}

void CWindowsMessageHelperThread::SetSenderMaxDelay(int nDelayMilliseconds) noexcept
{
	m_nSenderMaxDelay = nDelayMilliseconds;
}

void CWindowsMessageHelperThread::AddTargetMsgWnd(const CString& strUniqueName, CWnd* pTargetMsgWnd) noexcept
{
	ASSERT(pTargetMsgWnd);

	std::map<CString,CWnd*>::iterator it = m_MapTargetWnd.find(strUniqueName);
	if( it == m_MapTargetWnd.end())
	{
		m_MapTargetWnd.insert( pair<CString,CWnd*>(strUniqueName, pTargetMsgWnd) );
	}
	else
	{
		it->second = pTargetMsgWnd;
	}
	m_EventRun.SetEvent();
}

void CWindowsMessageHelperThread::RemoveTargetMsgWnd(const CString& strUniqueName) noexcept
{
	std::map<CString, CWnd*>::iterator it = m_MapTargetWnd.find(strUniqueName);
	if (it != m_MapTargetWnd.end())
	{
		m_MapTargetWnd.erase(it);
	}

	m_EventRun.SetEvent();
}

bool CWindowsMessageHelperThread::AddUCMessage(const CWindowsMessage& ucMsg, bool bUseQueueSync) noexcept
{
	m_qReadyMsg.push(ucMsg);

	if(true == bUseQueueSync)
	{
		int nSize = GetMsgQueueSize();
		int nSleep = static_cast<int>(nSize / 500);
		if (0 < nSleep)
		{
			Sleep(nSleep > m_nSenderMaxDelay ? m_nSenderMaxDelay : nSleep);
		}
	}

	if(true == ucMsg.GetClearTask())
	{
		m_EventStepSync.SetEvent();
	}

	m_EventRun.SetEvent();
	return true;
}


void CWindowsMessageHelperThread::SetReserveJobLock( bool bIsLock ) noexcept
{
	m_bReserveLock = bIsLock;
	m_EventRun.SetEvent();
}

bool CWindowsMessageHelperThread::GetReserveJobLock() noexcept
{
	return m_bReserveLock;
}


bool CWindowsMessageHelperThread::ReleaseStepSync( int nStepSyncIndex ) noexcept
{
	if(m_nLastStepSyncIndex == nStepSyncIndex)
	{
		m_EventStepSync.SetEvent();
		return true;
	}

	return false;
}

int CWindowsMessageHelperThread::GetReadyQueueSize()
{
	return static_cast<int>( m_qReadyMsg.unsafe_size() );
}

int CWindowsMessageHelperThread::GetMsgQueueSize()
{
	return static_cast<int>(m_qMsg.size());
	
}

int CWindowsMessageHelperThread::GetReserveQueueSize()
{
	return static_cast<int>(m_qReserveMsg.size());
}

void CWindowsMessageHelperThread::AllClearQueue() noexcept
{
	auto lClearReadyMsg = [&]()
	{
		while (m_qReadyMsg.empty() == FALSE)
		{
			CWindowsMessage ucMsg;
			if (m_qReadyMsg.try_pop(ucMsg) == TRUE)
			{
				delete ucMsg.GetParameterPtr();
			}
		}
		m_qReadyMsg.clear();
	};

	auto lClearReserveMsg = [&]()
	{
		for (auto it = m_qReserveMsg.begin(); it != m_qReserveMsg.end(); ++it)
		{
			CWindowsMessage& ucm = *it;
			delete ucm.GetParameterPtr();
		}
		m_qReserveMsg.clear();
	};

	auto lClearMsg = [&]()
	{
		for (auto it = m_qMsg.begin(); it != m_qMsg.end(); ++it)
		{
			CWindowsMessage& ucm = *it;
			delete ucm.GetParameterPtr();
		}
		m_qMsg.clear();
	};

	parallel_invoke(lClearReadyMsg, lClearReserveMsg, lClearMsg);
}

CWnd* CWindowsMessageHelperThread::FindTargetWnd( const CString& strTargetWnd ) noexcept
{
	if( true == m_MapTargetWnd.empty() )
		return nullptr;

	CWnd* pWnd = nullptr;
	std::map<CString,CWnd*>::iterator it = m_MapTargetWnd.find(strTargetWnd);
	if( it == m_MapTargetWnd.end())
	{
		pWnd = nullptr;
	}
	else
	{
		pWnd = it->second;
	}

	return pWnd;
}

CWindowsMessage::CWindowsMessage() :
	m_eMsgType(eMSG_Type::eType_Post),
	m_eJobType(eJOB_Type::eType_General),
	m_nMsg(-1),
	m_pWindowsMessageParameter(new CWindowsMessageParameter()),
	m_bPriority(false),
	m_bStepSync(false),
	m_nStepSyncIndex(0),
	m_bClearTask(false),
	m_strMsgTargetWnd(_T("InValid"))
{

}

CWindowsMessage::CWindowsMessage(const CString& strTarget, const eMSG_Type& eMsgType, int nMsg) : CWindowsMessage()
{
	SetMsgType(eMsgType);
	SetMsg(nMsg);
	SetMsgTargetWnd(strTarget);
}


constexpr CWindowsMessageParameter& CWindowsMessage::GetParameter() noexcept
{
	return *m_pWindowsMessageParameter;
}

constexpr CWindowsMessageParameter* CWindowsMessage::GetParameterPtr() noexcept
{
	return m_pWindowsMessageParameter;
}

bool CWindowsMessage::Execute(bool bUseQueueSync) noexcept
{
	ASSERT(g_MessageHelper);

	if (nullptr == g_MessageHelper)
	{
		return false;
	}

	g_MessageHelper->AddUCMessage(*this, bUseQueueSync);

	return true;
}
