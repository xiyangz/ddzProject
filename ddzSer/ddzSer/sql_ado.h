
#include <string>
#pragma once
using namespace std;
class sql_ado
{
public:
	sql_ado();
	~sql_ado();

	bool beginConnection();


	long beginTrans();
	HRESULT commitTrans();
	
	bool selectSql(CString& selectStr);
	int insertSql(CString& insertStr);
	int deleteSql(CString& deleteStr);
	int updateSql(CString& updataStr);

	_RecordsetPtr m_pRecordset;
private:

	//斜杠“\”在字符串需要转义！！！!!!!!!!!!!!!!!!!!!!!!!!!zgr\SQLEXPRESS  DESKTOP-0AKODUV\SQLEXPRESS
	CString strConnect = L"Provider = SQLOLEDB.1; Integrated Security = SSPI; Persist Security Info = False;"
		" Initial Catalog = ddzDatabase; Data Source = zgr\\SQLEXPRESS";
	
	_ConnectionPtr m_pConnection;
};

extern sql_ado sql;