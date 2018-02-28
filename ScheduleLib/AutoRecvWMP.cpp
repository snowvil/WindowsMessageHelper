#include "stdafx.h"
#include "AutoRecvWMP.h"

#include "WindowsMessageParameter.h"

#include <iostream> 

CAutoRecvWMP::CAutoRecvWMP(WPARAM wParam)
{
	ASSERT(wParam);
	m_pWMParam = reinterpret_cast<CWindowsMessageParameter*>(wParam);
	m_pWMParam->SetStopWatchEnd();
}


CAutoRecvWMP::~CAutoRecvWMP(void)
{
	delete m_pWMParam;
}

constexpr unsigned long CAutoRecvWMP::GetUniqueIndex() const noexcept
{
	ASSERT(m_pWMParam);
	return m_pWMParam->GetUniqueIndex();
}

constexpr int CAutoRecvWMP::GetMsgIdx() const noexcept
{
	ASSERT(m_pWMParam);
	return m_pWMParam->GetMsgIdx();
}

CWindowsMessageParameter* CAutoRecvWMP::GetParameter() noexcept
{
	ASSERT(m_pWMParam);
	return m_pWMParam;
}

constexpr long long CAutoRecvWMP::GetElapsedMicroseconds() const noexcept
{
	ASSERT(m_pWMParam);
	return m_pWMParam->GetElapsedMicroSeconds().count();
}

