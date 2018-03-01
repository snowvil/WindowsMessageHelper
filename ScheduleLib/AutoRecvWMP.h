// Copyright (c) 2018 snowvil84@gmail.com
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once

// Foward Declarations
class CWindowsMessageParameter;

using namespace std;

class AFX_CLASS_EXPORT CAutoRecvWMP final
{
public:
	CAutoRecvWMP(WPARAM wParam);
	~CAutoRecvWMP(void);
	CAutoRecvWMP(const CAutoRecvWMP& rhs) = delete;
	CAutoRecvWMP& operator=(const CAutoRecvWMP& rhs) = delete;


	constexpr unsigned long			GetUniqueIndex() const noexcept;
	constexpr int					GetMsgIdx() const noexcept;
	constexpr long long				GetElapsedMicroseconds() const noexcept;

	CWindowsMessageParameter*		GetParameter() noexcept;
	template <class T> constexpr T	GetData();
	
private:
	CWindowsMessageParameter*	m_pWMParam;
};

template <class T>
constexpr T CAutoRecvWMP::GetData()
{
	ASSERT(m_pWMParam);
	return m_pWMParam->GetData<T>();
}

