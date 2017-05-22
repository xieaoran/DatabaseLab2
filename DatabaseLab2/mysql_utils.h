#pragma once

#ifndef MYSQL_UTILS_H
#define MYSQL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

	#include "mysql.h"

	#define MYSQL_HOST "localhost"
	#define MYSQL_USER "root"
	#define MYSQL_PASSWD "ilovewindows8*"
	#define MYSQL_DBNAME "company"

	typedef struct
	{
		char* name;
		unsigned name_length;
		unsigned max_length;
	} FIELD_INFO;

	extern MYSQL* db_alloc(MYSQL* mysql);
	extern MYSQL_RES* db_query(MYSQL* mysql, const char *query);
	extern void print_result(MYSQL_RES* result);


#ifdef __cplusplus
}
#endif

#endif /* LIBRSA_H */