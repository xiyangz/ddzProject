#include"pch.h"
#include "data_common.h"


CString trim(CString& str)
{
	CString sstr;
	int i = 0;
	int end = str.GetLength();
	while (i < end)
	{
		if (str.GetString()[i] != TEXT(' '))
		{
			sstr += str.GetString()[i];
		}
		i++;
	}

	return sstr;
}

/*参数str一定要有结束符'\0'*/
void trim(TCHAR* str)
{
	int i = 0;
	int k = i;
	while (str[i] != TEXT('\0'))
	{
		if (str[i] != TEXT(' '))
		{
			if (k != i)
				str[k] = str[i];
			k++;
		}
		i++;
	}
	str[k] = TEXT('\0');
}
