#pragma warning(disable:4996)
#include<stdio.h>
#include<windows.h>
#include<sqlext.h>

typedef struct department
{
	SQLCHAR department_id[12];
	SQLCHAR department_name[20];
	SQLCHAR department_header[20];
	SQLCHAR	teacher_num[5];
}DEPARTMENT;

void execution(SQLHDBC hDbc);						//�ɹ��������Ӻ�ѡ����ɾ�Ĳ����
void SQL(SQLCHAR query[], SQLHDBC hDbc, int SQLFlag);//���������SQL����ִ��

int main()
{
	//��ʼ������
	SQLHENV hEnv;		//�������
	SQLRETURN ret = SQLAllocEnv(&hEnv);		//��ʼ��ODBC���������ػ������
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		//��������
		SQLHDBC hDbc;	//���Ӿ��
		ret = SQLAllocConnect(hEnv, &hDbc);	//Ϊ���Ӿ�������ڴ沢�������Ӿ��
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			SQLCHAR dataSource[] = "StudentData";	//����Դ
			SQLCHAR username[] = "edwin";			//�û���
			SQLCHAR password[] = "wbj321...";		//����
			//����SQL������Դ
			//����windows�����֤�����û���������ΪNULL�������ַ���
			ret = SQLConnect(hDbc, dataSource, SQL_NTS, username, SQL_NTS, password, SQL_NTS);
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				execution(hDbc);		//�ɹ�������ִ��SQL���
				SQLDisconnect(hDbc);	//�ж�����
			}
			SQLFreeConnect(hDbc);		//�ͷ������Ӿ����ص���Դ
		}
		SQLFreeEnv(hEnv);				//�ͷ��뻷�������ص���Դ
	}
	return 0;
}
//�ɹ��������Ӻ�ѡ����ɾ�Ĳ����
void execution(SQLHDBC hDbc)
{
	int quitFlag = 0;
	while (!quitFlag)
	{
		printf("--------------------------------------------------------------------------\n");
		printf("department(department_id��department_name��department_header��teacher_num)\n");
		printf("������Ŷ�department�������Ӧ������\n");
		printf("1.��ѯ 2.���� 3.���� 4.ɾ�� 5.�˳�\n");
		printf("--------------------------------------------------------------------------\n");
		int option = 0;
		while (option < 1 || option>5)
		{
			printf("�����룺");
			scanf("%d", &option);
		}
		switch (option)
		{
		case 1:	//��ѯ
		{
			printf("������Ҫ��ѯ�����ݣ�\n");
			printf("1.����department��¼\n");
			printf("2.�Զ����ѯ����where��eg. where department_id = 'dep_01'��\n");
			int opt1 = 0;
			while (opt1 < 1 || opt1>2)
			{
				printf("�����룺");
				scanf("%d", &opt1);
			}
			if (opt1 == 1)	//��ѯ����department��¼
			{
				SQLCHAR selectAll[] = "select * from department";
				SQL(selectAll, hDbc, 0);
			}
			else if (opt1 == 2)	//�Զ���SQL����ѯ
			{
				SQLCHAR selectDep[2000];	//��ѯ��䳤�Ȳ�����2000���ַ�
				printf("�������Զ���SQL��ѯ��where��䣺��eg. where department_id = 'dep_01'��\n");
				//scanf("%s", selectDep);//scanfʹ�ô��󣬶���where��Ŀո��ֹͣ��
				getchar();//������һ������Ӱ��
				gets(selectDep);
				SQLCHAR select[] = "select * from department %s";
				SQLCHAR selectQuery[2000];
				sprintf((char*)selectQuery, (char*)select, selectDep);
				printf("%s\n", selectQuery);//test
				SQL(selectQuery, hDbc, 0);
			}
			break;
		}
		case 2:	//����
		{
			printf("������Ҫ���µ�ѧԺid��");
			SQLCHAR department_id[12];
			scanf("%s", department_id);
			printf("��������º�ģ�ѧԺid ѧԺ���� Ժ�� ��ʦ�����ո������\n");
			DEPARTMENT dept;
			scanf("%s %s %s %s", dept.department_id, dept.department_name,
				dept.department_header, dept.teacher_num);
			SQLCHAR update[] = "update department set department_id = '%s', department_name = '%s', department_header = '%s', teacher_num = '%s' where department_id = '%s'";
			SQLCHAR updateQuery[2000];
			sprintf((char*)updateQuery, (char*)update, dept.department_id, dept.department_name, dept.department_header, dept.teacher_num, department_id);
			SQL(updateQuery, hDbc, 1);
			break;
		}
		case 3:	//����
		{
			printf("������Ҫ����ģ�ѧԺid ѧԺ���� Ժ�� ��ʦ�����ո��������\n");
			DEPARTMENT dept;
			scanf("%s %s %s %s", dept.department_id, dept.department_name,
				dept.department_header, dept.teacher_num);
			SQLCHAR insert[] = "insert into department values('%s','%s','%s','%s')";
			SQLCHAR insertQuery[2000];
			sprintf((char*)insertQuery, (char*)insert, dept.department_id, dept.department_name, dept.department_header, dept.teacher_num);
			SQL(insertQuery, hDbc, 1);
			break;
		}
		case 4:	//ɾ��
		{
			printf("������Ҫɾ����ѧԺid��");
			SQLCHAR department_id[12];
			scanf("%s", department_id);
			SQLCHAR delete[] = "delete from department where department_id = '%s'";
			SQLCHAR deleteQuery[2000];
			sprintf((char*)deleteQuery, (char*)delete, department_id);
			SQL(deleteQuery, hDbc, 1);
			break;
		}
		case 5: //�˳�
		{
			quitFlag = 1;
			break;
		}
		default:
			break;
		}
	}
}
//�����������SQL����ִ��.SQLFlag=0��ʶ��ѯ������Ϊ1��ʶ��������
void SQL(SQLCHAR query[], SQLHDBC hDbc, int SQLFlag)
{
	//���������
	SQLHSTMT hStm;	//�����
	SQLRETURN ret = SQLAllocStmt(hDbc, &hStm);	//Ϊ����������ڴ�, �����������
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		//��SQL����͵����ݿ������������ִ����SQL��䶨������ݿ���ʣ�
		ret = SQLExecDirect(hStm, query, SQL_NTS);
		if (SQLFlag == 1)//ִ�еĲ��ǲ�ѯ�����������������β�ѯ��������ʾ���
		{
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				printf("�ɹ�ִ�С�\n");
			}
			else printf("ִ��ʧ�ܡ�\n");
			ret = SQLExecDirect(hStm, "select * from department", SQL_NTS);
		}
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			if(SQLFlag == 0)//����Ϊ��ѯ����
				printf("�ɹ�ִ�С�\n");
			printf("------------------------------------------------------------------------\n");
			printf("|department_id  |department_name     |department_header  |teacher_num  |\n");
			printf("------------------------------------------------------------------------\n");
			//�����ݻ����������ݿ��е���Ӧ�ֶΣ�i�ǲ�ѯ������кţ�queryData�ǰ󶨻�������BUFF_LENGTH�ǻ��������ȣ�
			//SQLBindCol(hstmt, i, SQL_C_CHAR, queryData[i-1], BUFF_LENGTH, 0);
			DEPARTMENT dept;
			ret = SQLBindCol(hStm, 1, SQL_C_CHAR, dept.department_id, _countof(dept.department_id), 0);
			ret = SQLBindCol(hStm, 2, SQL_C_CHAR, dept.department_name, _countof(dept.department_name), 0);
			ret = SQLBindCol(hStm, 3, SQL_C_CHAR, dept.department_header, _countof(dept.department_header), 0);
			ret = SQLBindCol(hStm, 4, SQL_C_CHAR, dept.teacher_num, _countof(dept.teacher_num), 0);
			while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				//�������������Ӧ������ 
				ret = SQLFetch(hStm);
				if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
				{
					printf("|%s", dept.department_id);
					for (int i = 0; i < 15 - (int)strlen((char*)dept.department_id); ++i)
						printf(" ");
					printf("|%s", dept.department_name);
					for (int i = 0; i < 20 - (int)strlen((char*)dept.department_name); ++i)
						printf(" ");
					printf("|%s", dept.department_header);
					for (int i = 0; i < 19 - (int)strlen((char*)dept.department_header); ++i)
						printf(" ");
					printf("|%s", dept.teacher_num);
					for (int i = 0; i < 13 - (int)strlen((char*)dept.teacher_num); ++i)
						printf(" ");
					printf("|\n");
					printf("------------------------------------------------------------------------\n");
				}
			}
		}
		else
		{
			if (SQLFlag == 0)//����Ϊ��ѯ����
				printf("ִ��ʧ�ܡ�\n");
		}
		printf("\n");
		SQLFreeStmt(hStm, SQL_CLOSE);	//�ͷ����������ص���Դ��
	}
}