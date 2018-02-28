#include "StdAfx.h"
#include "WindowsMessageParameter.h"
#include <stdexcept>
#include <iostream>

CWindowsMessageParameter::CWindowsMessageParameter()
	:	m_nUniqueIndex(-1),
		m_nMsgIdx(-1),
		m_tpStart(system_clock::now()),
		m_tpEnd(system_clock::now())
	
{

}

CWindowsMessageParameter::~CWindowsMessageParameter()
{
	try
	{
		ReleaseData();
	}
	catch (std::exception &e)
	{
		std::cerr << "Caught " << e.what() << std::endl;
		std::cerr << "Type " << typeid(e).name() << std::endl;
	}
	
}

void CWindowsMessageParameter::ReleaseData()
{
	if (false == m_mapDataQueue.empty())
	{
		size_t nSize = m_mapDataQueue.size();
		CStringA strException;
		strException.Format("void CWindowsMessageParameter::ResetData() -> DataQueue is not empty! :: UniqueIdx[%d], MsgIdx[%d], size[%d], ", m_nUniqueIndex, m_nMsgIdx, nSize);
		for (auto& it : m_mapDataQueue)
		{
			CStringA strTmp;
			strTmp.Format("[%s], ", it.first);
			strException.Append(strTmp);
		}
		throw std::runtime_error(strException.GetString());
	}

	m_mapDataQueue.clear();	
}
