#include "mysql_utils.h"
#include "stdio.h"

static char* sql_quries[9] = {
"select distinct works_on.ESSN from works_on where works_on.PNO = '%s'",
"select employee.ENAME from employee where employee.ESSN in \
(select distinct works_on.ESSN from works_on where works_on.PNO = \
(select project.PNO from project where project.PNAME = '%s'))",
"select employee.ENAME, employee.ADDRESS from employee where employee.DNO = \
(select department.DNO from department where department.DNAME = '%s')",
"select employee.ENAME, employee.ADDRESS from employee \
where employee.DNO = (select department.DNO from department where department.DNAME = '%s') \
and employee.SALARY < %s",
"select employee.ENAME from employee \
where employee.ESSN not in \
(select distinct works_on.ESSN from works_on where works_on.PNO = '%s')",
"select employee.ENAME, department.DNAME \
from employee left join department on employee.DNO = department.DNO where employee.SUPERSSN = \
(select employee.ESSN from employee where employee.ENAME = '%s')",
"select works_on_union.ESSN from \
(select distinct works_on.ESSN from works_on where works_on.PNO = '%s' union all \
select distinct works_on.ESSN from works_on where works_on.PNO = '%s') works_on_union \
group by works_on_union.ESSN having count(*) > 1",
"select department.DNAME from department where department.DNO in ( \
select employee.DNO from employee group by employee.DNO having avg(employee.SALARY) < %s)",
"select employee.ENAME from employee where employee.ESSN in \
(select works_on.ESSN from works_on group by works_on.ESSN \
having count(distinct works_on.PNO) > %s and sum(works_on.HOURS) <= %s)"
};

int main(int argc, char* argv[])
{
	int args_count = argc - 4;
	if (args_count < 1 || args_count > 2) return ERROR_BAD_COMMAND;

	if (strcmp(argv[1], "-q") || strcmp(argv[3], "-p")) return ERROR_BAD_COMMAND;

	int selection = atoi(argv[2]) - 1;
	int args_length = 0;
	for (unsigned arg_index = 4; arg_index < argc; arg_index++)
	{
		args_length += strlen(argv[arg_index]);
	}

	unsigned query_size = strlen(sql_quries[selection]) + 1 - (args_count*2) + args_length;
	char* query = (char*)malloc(sizeof(char) * query_size);
	if (args_count == 1) snprintf(query, query_size, sql_quries[selection], argv[4]);
	else if (args_count == 2) snprintf(query, query_size, sql_quries[selection], argv[4], argv[5]);

	MYSQL* mysql = (MYSQL*)malloc(sizeof(MYSQL));
	if (db_alloc(mysql) == NULL) return;

	MYSQL_RES* result = db_query(mysql, query);
	printf_s("[%s]\n\n", query);
	print_result(result);

	mysql_free_result(result);
	result = NULL;

	mysql_close(mysql);
	mysql = NULL;

	return 0;
}