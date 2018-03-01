// Copyright (c) 2018 snowvil84@gmail.com
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <map>
#include <queue>
#include <memory>
#include <stdexcept>
#include <chrono>

using namespace std;
using namespace chrono;

class AFX_CLASS_EXPORT CWindowsMessageParameter final
{
public:
	CWindowsMessageParameter();
	~CWindowsMessageParameter();
	CWindowsMessageParameter(const CWindowsMessageParameter& rhs) = delete;
	CWindowsMessageParameter& operator=(const CWindowsMessageParameter& rhs) = delete;


	// Get Expr
	inline constexpr unsigned long		GetUniqueIndex() const noexcept	{ return m_nUniqueIndex; }
	inline constexpr int				GetMsgIdx() const noexcept		{ return m_nMsgIdx; }

	inline constexpr microseconds		GetElapsedMicroSeconds() const noexcept { return duration_cast<microseconds>(m_tpEnd - m_tpStart); }
	// Set Expr
	inline constexpr void				SetUniqueIndex(unsigned long ulUIdx) noexcept	{ m_nUniqueIndex = ulUIdx; }
	inline constexpr void				SetMsgIdx(int nMsgIdx) noexcept				{ m_nMsgIdx = nMsgIdx;	}
	inline void							SetStopWatchStart()	noexcept					{ m_tpStart = system_clock::now(); }
	inline void							SetStopWatchEnd() noexcept						{ m_tpEnd = system_clock::now(); }

	// Template Function
	template <class T> constexpr void	SetData(const T& refData) noexcept;
	template <class T> constexpr void	SetData(const T&& moveData) noexcept;
	template <class T> constexpr T		GetData();




protected:
	template<class T> constexpr std::queue<T>*	FindDataQueue() noexcept;

	template<class T> constexpr void			RemoveDataQueue() noexcept;

	void										ReleaseData();											
private:
	unsigned long				m_nUniqueIndex;						
	int							m_nMsgIdx;							
	system_clock::time_point	m_tpStart;
	system_clock::time_point	m_tpEnd;

	// small data , key = string => map > unordered_map
	std::map<const char*, void* >	m_mapDataQueue;						
};


template <class T>
constexpr void CWindowsMessageParameter::SetData(const T& refData) noexcept
{
	std::queue<T>* pQueue = FindDataQueue<T>();
	pQueue->push(refData);
}

template <class T>
constexpr void CWindowsMessageParameter::SetData(const T&& moveData) noexcept
{
	std::queue<T>* pQueue = FindDataQueue<T>();
	pQueue->push( std::move(moveData) );
}

template<class T>
constexpr T CWindowsMessageParameter::GetData()
{
	std::queue<T>* pQueue = FindDataQueue<T>();

	if (true == pQueue->empty())
	{
		throw std::underflow_error("Container Empty!");
	}

	T tmpData = std::move(pQueue->front());
	pQueue->pop();

	// Å¥ »èÁ¦
	if (true == pQueue->empty())
	{
		RemoveDataQueue<T>();
		delete pQueue;
	}

	return tmpData;

}

template<class T>
constexpr  std::queue<T>* CWindowsMessageParameter::FindDataQueue() noexcept
{
	const char* strTarget = typeid(T).name();
	auto& iter = m_mapDataQueue.find(strTarget);

	if (iter == m_mapDataQueue.end())
	{
		std::queue<T>* pNewQueue = new std::queue<T>();
		m_mapDataQueue.insert(std::map<const char*, void* >::value_type(strTarget, reinterpret_cast<void*>(pNewQueue)));

		return pNewQueue;
	}

	return reinterpret_cast<std::queue<T>*>(iter->second);
}

template<class T>
constexpr  void CWindowsMessageParameter::RemoveDataQueue() noexcept
{
	const char* strTarget = typeid(T).name();
	m_mapDataQueue.erase(strTarget);
}

