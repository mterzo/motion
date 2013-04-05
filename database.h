/* Includes */
#include "motion.h"

#ifdef HAVE_MYSQL
#include <mysql.h>
#endif

#ifdef HAVE_SQLITE3
#include <sqlite3.h>
#endif

#ifdef HAVE_PGSQL
#include <libpq-fe.h>
#endif


int database_init(struct context *);
void database_cleanup(struct context *);

