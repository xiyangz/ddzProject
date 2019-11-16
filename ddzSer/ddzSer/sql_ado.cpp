#include "pch.h"

#include"sql_ado.h"
#include <iostream>
using namespace std;
sql_ado::sql_ado()
{
	//初始化ado com组件
	HRESULT hr;
	try {
		hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			cout << "初始化ado com组件 成功" << endl;
		}
		else 
			cout << "初始化ado com组件 失败" << endl;
	}
	catch (_com_error e)
	{
		cout <<"exception:"<< e.ErrorMessage() << endl;
	}

	beginConnection();
}

sql_ado::~sql_ado()
{
	//关闭数据库连接
	if ((m_pRecordset->State&adStateOpen))
	{
		m_pRecordset->Close();
	}

	if ((m_pConnection->State&adStateOpen))
	{
		m_pConnection->Close();
		//m_pConnection->Release();
		CoUninitialize();
	}
}

bool sql_ado::beginConnection()
{
	if (!SUCCEEDED(m_pConnection.CreateInstance(__uuidof(Connection))))
	{
		m_pConnection = NULL;
		cout<<"创建连接对象失败" << std::endl;
		return false;
	}
	else
	{
		cout << "创建连接对象成功" << std::endl;
	}
	
	if (SUCCEEDED(m_pConnection->Open(_bstr_t(strConnect), TEXT(""), TEXT(""), adConnectUnspecified)))
	{
		cout << "打开SQL Server 连接成功"<<std::endl;
	}
	else
	{
		cout << "打开SQL Server 连接失败" << std::endl;
		m_pConnection.Release();//自己加的
		return false;
	}

	if (SUCCEEDED(m_pRecordset.CreateInstance(__uuidof(Recordset))))
	{

	}
	else
	{
		m_pRecordset.Release();
		return false;
	}
	return true;
}

long sql_ado::beginTrans()
{
	return m_pConnection->BeginTrans();
}

HRESULT sql_ado::commitTrans()
{
	return m_pConnection->CommitTrans();
}

bool sql_ado::selectSql(CString& cstr)
{
	if (!cstr.GetLength())
	{
		cout<<"未输入SQL查询语句" << std::endl;
		return false;
	}
	try
	{
		m_pRecordset = m_pConnection->Execute(_bstr_t(cstr), NULL, adCmdText);
	}
	catch (_com_error &e)
	{
		CString errorMessage;
		errorMessage = e.ErrorMessage();
		cout << "exception:" << e.Description() << std::endl;
		return false;
	}
	catch (...)
	{
		cout << "执行SQL Select语句发生未知错误" << std::endl;
		return false;
	}

	return true;
}

int sql_ado::insertSql(CString & cstr)
{
	if (!cstr.GetLength())
	{
		cout << "未输入SQL查询语句" << std::endl;
		return 0;
	}
	_variant_t var;//记录增加的行数

	try
	{
		m_pConnection->Execute(_bstr_t(cstr), &var, adCmdText);
	}
	catch (_com_error &e)
	{
		cout << "exception:" << e.Description() << std::endl;
		return 0;
	}
	catch (...)
	{
		cout << "执行SQL Insert语句发生未知错误" << std::endl;
		return 0;
	}
	return int(var);
}

int sql_ado::deleteSql(CString & cstr)
{
	if (!cstr.GetLength())
	{
		cout << "未输入SQL查询语句" << std::endl;
		return false;
	}
	_variant_t var;//记录删除的行数

	try
	{
		m_pConnection->Execute(_bstr_t(cstr), &var, adCmdText);
	}
	catch (_com_error &e)
	{
		
		cout << "exception:" << e.Description()<<std::endl;
		return false;
	}
	catch (...)
	{
		cout << "执行SQL Delete语句发生未知错误" << std::endl;
		return false;
	}

	return int(var);
}

int sql_ado::updateSql(CString & cstr)
{
	if (!cstr.GetLength())
	{
		cout << "未输入SQL查询语句" << std::endl;
		return false;
	}
	_variant_t var;//记录修改的行数

	try
	{
		m_pConnection->Execute(_bstr_t(cstr), &var, adCmdText);
	}
	catch (_com_error &e)
	{
		cout << "exception:" << e.Description() << std::endl;
		return false;
	}
	catch (...)
	{
		cout << "执行SQL Update语句发生未知错误" << std::endl;
		return false;
	}

	return int(var);
}


sql_ado sql;
