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

void execution(SQLHDBC hDbc);						//成功建立连接后选择增删改查操作
void SQL(SQLCHAR query[], SQLHDBC hDbc, int SQLFlag);//分配语句句柄SQL语句的执行

int main()
{
	//初始化环境
	SQLHENV hEnv;		//环境句柄
	SQLRETURN ret = SQLAllocEnv(&hEnv);		//初始化ODBC环境，返回环境句柄
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		//建立连接
		SQLHDBC hDbc;	//连接句柄
		ret = SQLAllocConnect(hEnv, &hDbc);	//为连接句柄分配内存并返回连接句柄
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			SQLCHAR dataSource[] = "StudentData";	//数据源
			SQLCHAR username[] = "edwin";			//用户名
			SQLCHAR password[] = "wbj321...";		//密码
			//连接SQL数据资源
			//若是windows身份验证，则用户名密码设为NULL或任意字符串
			ret = SQLConnect(hDbc, dataSource, SQL_NTS, username, SQL_NTS, password, SQL_NTS);
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				execution(hDbc);		//成功建立后执行SQL语句
				SQLDisconnect(hDbc);	//切断连接
			}
			SQLFreeConnect(hDbc);		//释放与连接句柄相关的资源
		}
		SQLFreeEnv(hEnv);				//释放与环境句柄相关的资源
	}
	return 0;
}
//成功建立连接后选择增删改查操作
void execution(SQLHDBC hDbc)
{
	int quitFlag = 0;
	while (!quitFlag)
	{
		printf("--------------------------------------------------------------------------\n");
		printf("department(department_id，department_name，department_header，teacher_num)\n");
		printf("键入序号对department表进行相应操作：\n");
		printf("1.查询 2.更新 3.插入 4.删除 5.退出\n");
		printf("--------------------------------------------------------------------------\n");
		int option = 0;
		while (option < 1 || option>5)
		{
			printf("请输入：");
			scanf("%d", &option);
		}
		switch (option)
		{
		case 1:	//查询
		{
			printf("请输入要查询的内容：\n");
			printf("1.所有department记录\n");
			printf("2.自定义查询条件where（eg. where department_id = 'dep_01'）\n");
			int opt1 = 0;
			while (opt1 < 1 || opt1>2)
			{
				printf("请输入：");
				scanf("%d", &opt1);
			}
			if (opt1 == 1)	//查询所有department记录
			{
				SQLCHAR selectAll[] = "select * from department";
				SQL(selectAll, hDbc, 0);
			}
			else if (opt1 == 2)	//自定义SQL语句查询
			{
				SQLCHAR selectDep[2000];	//查询语句长度不超过2000个字符
				printf("请输入自定义SQL查询的where语句：（eg. where department_id = 'dep_01'）\n");
				//scanf("%s", selectDep);//scanf使用错误，读到where后的空格就停止了
				getchar();//消除上一个换行影响
				gets(selectDep);
				SQLCHAR select[] = "select * from department %s";
				SQLCHAR selectQuery[2000];
				sprintf((char*)selectQuery, (char*)select, selectDep);
				printf("%s\n", selectQuery);//test
				SQL(selectQuery, hDbc, 0);
			}
			break;
		}
		case 2:	//更新
		{
			printf("请输入要更新的学院id：");
			SQLCHAR department_id[12];
			scanf("%s", department_id);
			printf("请输入更新后的：学院id 学院名称 院长 教师数（空格隔开）\n");
			DEPARTMENT dept;
			scanf("%s %s %s %s", dept.department_id, dept.department_name,
				dept.department_header, dept.teacher_num);
			SQLCHAR update[] = "update department set department_id = '%s', department_name = '%s', department_header = '%s', teacher_num = '%s' where department_id = '%s'";
			SQLCHAR updateQuery[2000];
			sprintf((char*)updateQuery, (char*)update, dept.department_id, dept.department_name, dept.department_header, dept.teacher_num, department_id);
			SQL(updateQuery, hDbc, 1);
			break;
		}
		case 3:	//插入
		{
			printf("请输入要插入的：学院id 学院名称 院长 教师数（空格隔开）：\n");
			DEPARTMENT dept;
			scanf("%s %s %s %s", dept.department_id, dept.department_name,
				dept.department_header, dept.teacher_num);
			SQLCHAR insert[] = "insert into department values('%s','%s','%s','%s')";
			SQLCHAR insertQuery[2000];
			sprintf((char*)insertQuery, (char*)insert, dept.department_id, dept.department_name, dept.department_header, dept.teacher_num);
			SQL(insertQuery, hDbc, 1);
			break;
		}
		case 4:	//删除
		{
			printf("请输入要删除的学院id：");
			SQLCHAR department_id[12];
			scanf("%s", department_id);
			SQLCHAR delete[] = "delete from department where department_id = '%s'";
			SQLCHAR deleteQuery[2000];
			sprintf((char*)deleteQuery, (char*)delete, department_id);
			SQL(deleteQuery, hDbc, 1);
			break;
		}
		case 5: //退出
		{
			quitFlag = 1;
			break;
		}
		default:
			break;
		}
	}
}
//分配语句句柄及SQL语句的执行.SQLFlag=0标识查询操作，为1标识其他操作
void SQL(SQLCHAR query[], SQLHDBC hDbc, int SQLFlag)
{
	//分配语句句柄
	SQLHSTMT hStm;	//语句句柄
	SQLRETURN ret = SQLAllocStmt(hDbc, &hStm);	//为语句句柄分配内存, 并返回语句句柄
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		//把SQL语句送到数据库服务器，请求执行由SQL语句定义的数据库访问；
		ret = SQLExecDirect(hStm, query, SQL_NTS);
		if (SQLFlag == 1)//执行的不是查询操作，则再增加依次查询操作来显示结果
		{
			if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				printf("成功执行√\n");
			}
			else printf("执行失败×\n");
			ret = SQLExecDirect(hStm, "select * from department", SQL_NTS);
		}
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			if(SQLFlag == 0)//本身即为查询操作
				printf("成功执行√\n");
			printf("------------------------------------------------------------------------\n");
			printf("|department_id  |department_name     |department_header  |teacher_num  |\n");
			printf("------------------------------------------------------------------------\n");
			//将数据缓冲区绑定数据库中的相应字段（i是查询结果集列号，queryData是绑定缓冲区，BUFF_LENGTH是缓冲区长度）
			//SQLBindCol(hstmt, i, SQL_C_CHAR, queryData[i-1], BUFF_LENGTH, 0);
			DEPARTMENT dept;
			ret = SQLBindCol(hStm, 1, SQL_C_CHAR, dept.department_id, _countof(dept.department_id), 0);
			ret = SQLBindCol(hStm, 2, SQL_C_CHAR, dept.department_name, _countof(dept.department_name), 0);
			ret = SQLBindCol(hStm, 3, SQL_C_CHAR, dept.department_header, _countof(dept.department_header), 0);
			ret = SQLBindCol(hStm, 4, SQL_C_CHAR, dept.teacher_num, _countof(dept.teacher_num), 0);
			while (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
			{
				//遍历结果集到相应缓冲区 
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
			if (SQLFlag == 0)//本身即为查询操作
				printf("执行失败×\n");
		}
		printf("\n");
		SQLFreeStmt(hStm, SQL_CLOSE);	//释放与语句句柄相关的资源；
	}
}