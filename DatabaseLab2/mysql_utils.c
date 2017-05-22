#include "mysql_utils.h"
#include "stdio.h"

static void print_error(MYSQL* mysql)
{
	if (mysql == NULL) return;
	fprintf_s(stderr, "ERROR %d (%s): %s\n",
		mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
}

static void print_border(const unsigned num_fields, const FIELD_INFO* field_infos)
{
	for (unsigned field_index = 0; field_index < num_fields; field_index++)
	{
		putchar('+');
		for (unsigned char_index = 0;
			char_index < field_infos[field_index].max_length + 2;
			char_index++)
		{
			putchar('-');
		}
	}
	printf_s("+\n");
}

static void print_fields(const unsigned num_fields, const FIELD_INFO* field_infos)
{
	for (unsigned field_index = 0; field_index < num_fields; field_index++)
	{
		printf_s("| ");
		printf_s(field_infos[field_index].name);
		for (unsigned char_index = 0;
			char_index <= field_infos[field_index].max_length - field_infos[field_index].name_length;
			char_index++)
		{
			putchar(' ');
		}
	}
	printf_s("|\n");
}

static void print_row(const MYSQL_ROW row, const unsigned long *row_lengths,
	const unsigned num_fields, const FIELD_INFO* field_infos)
{
	for (unsigned field_index = 0; field_index < num_fields; field_index++)
	{
		unsigned row_length = row_lengths[field_index];

		printf_s("| ");
		if (row_length == 0) {
			row_length = 4;
			printf_s("NULL");
		}
		else printf_s(row[field_index]);

		for (unsigned char_index = 0;
			char_index <= field_infos[field_index].max_length - row_length;
			char_index++)
		{
			putchar(' ');
		}
	}
	printf_s("|\n");
}

MYSQL* db_alloc(MYSQL* mysql)
{
	if (mysql == NULL) return NULL;

	if (mysql_init(mysql) == NULL) {
		print_error(mysql);
		return NULL;
	}

	if (mysql_real_connect(mysql, MYSQL_HOST, MYSQL_USER, MYSQL_PASSWD,
		MYSQL_DBNAME, MYSQL_PORT, NULL, 0) == NULL)
	{
		print_error(mysql);
		return NULL;
	}

	return mysql;
}

MYSQL_RES* db_query(MYSQL* mysql, const char *query)
{
	if (mysql == NULL) return NULL;

	if (mysql_query(mysql, query) < 0) {
		print_error(mysql);
		return NULL;
	}

	MYSQL_RES* result = mysql_store_result(mysql);
	if (result == NULL) {
		if (mysql_field_count(mysql) > 0) print_error(mysql);
		return NULL;
	}
	return result;
}

void print_result(MYSQL_RES* result)
{
	if (result == NULL) return;

	unsigned num_fields = mysql_num_fields(result);
	FIELD_INFO* field_infos = (FIELD_INFO*)malloc(sizeof(FIELD_INFO) * num_fields);

	for (unsigned field_index = 0; field_index < num_fields; field_index++)
	{
		MYSQL_FIELD* field = mysql_fetch_field(result);
		field_infos[field_index].max_length =
			field->max_length > field->name_length ?
			field->max_length : field->name_length;
		field_infos[field_index].name = field->name;
		field_infos[field_index].name_length = field->name_length;
	}

	print_border(num_fields, field_infos);
	print_fields(num_fields, field_infos);
	print_border(num_fields, field_infos);

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result)))
	{
		unsigned long *row_lengths = mysql_fetch_lengths(result);
		print_row(row, row_lengths, num_fields, field_infos);
	}

	print_border(num_fields, field_infos);
}