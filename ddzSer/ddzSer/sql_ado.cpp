#include "pch.h"

#include"sql_ado.h"
#include <iostream>
using namespace std;
sql_ado::sql_ado()
{
	//��ʼ��ado com���
	HRESULT hr;
	try {
		hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			cout << "��ʼ��ado com��� �ɹ�" << endl;
		}
		else 
			cout << "��ʼ��ado com��� ʧ��" << endl;
	}
	catch (_com_error e)
	{
		cout <<"exception:"<< e.ErrorMessage() << endl;
	}

	beginConnection();
}

sql_ado::~sql_ado()
{
	//�ر����ݿ�����
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
		cout<<"�������Ӷ���ʧ��" << std::endl;
		return false;
	}
	else
	{
		cout << "�������Ӷ���ɹ�" << std::endl;
	}
	
	if (SUCCEEDED(m_pConnection->Open(_bstr_t(strConnect), TEXT(""), TEXT(""), adConnectUnspecified)))
	{
		cout << "��SQL Server ���ӳɹ�"<<std::endl;
	}
	else
	{
		cout << "��SQL Server ����ʧ��" << std::endl;
		m_pConnection.Release();//�Լ��ӵ�
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
		cout<<"δ����SQL��ѯ���" << std::endl;
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
		cout << "ִ��SQL Select��䷢��δ֪����" << std::endl;
		return false;
	}

	return true;
}

int sql_ado::insertSql(CString & cstr)
{
	if (!cstr.GetLength())
	{
		cout << "δ����SQL��ѯ���" << std::endl;
		return 0;
	}
	_variant_t var;//��¼���ӵ�����

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
		cout << "ִ��SQL Insert��䷢��δ֪����" << std::endl;
		return 0;
	}
	return int(var);
}

int sql_ado::deleteSql(CString & cstr)
{
	if (!cstr.GetLength())
	{
		cout << "δ����SQL��ѯ���" << std::endl;
		return false;
	}
	_variant_t var;//��¼ɾ��������

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
		cout << "ִ��SQL Delete��䷢��δ֪����" << std::endl;
		return false;
	}

	return int(var);
}

int sql_ado::updateSql(CString & cstr)
{
	if (!cstr.GetLength())
	{
		cout << "δ����SQL��ѯ���" << std::endl;
		return false;
	}
	_variant_t var;//��¼�޸ĵ�����

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
		cout << "ִ��SQL Update��䷢��δ֪����" << std::endl;
		return false;
	}

	return int(var);
}


sql_ado sql;
