#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <afx.h>
#include <Windows.h>
#include <winnt.h>
#include <Shlwapi.h>
#include <icrsint.h>  //����icrsint.h�ļ�������VC++��չ��һЩԤ����ָ���ȵĶ��壬����COM���ʱʹ�á�
#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF", "adoEOF")
#include <iostream>
#pragma comment(lib,"Ws2_32.lib")

#include "data_login.h"
#include "data_common.h"
#include "data_run.h"
#include "struct_common.h"
#include "struct_login.h"
#include "struct_team.h"
#include "struct_trans.h"
#include "sql_ado.h"