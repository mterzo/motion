#include "database.h"
#include "motion.h"

int database_init(struct context *cnt){
    int ret = 0;

#if defined(HAVE_MYSQL) || defined(HAVE_PGSQL) || defined(HAVE_SQLITE3)
    if (cnt->conf.database_type) {
        MOTION_LOG(NTC, TYPE_DB, NO_ERRNO, "%s: Database backend %s",  
                   cnt->conf.database_type);
        
#ifdef HAVE_SQLITE3
    if ((!strcmp(cnt->conf.database_type, "sqlite3")) && 
                                                    cnt->conf.sqlite3_db) {
        MOTION_LOG(NTC, TYPE_DB, NO_ERRNO, "%s: DB %s", 
                   cnt->conf.sqlite3_db);

        if (sqlite3_open(cnt->conf.sqlite3_db, &cnt->database_sqlite3) != 
                                                                SQLITE_OK) {
            MOTION_LOG(ERR, TYPE_DB, NO_ERRNO, 
                        "%s: Can't open database %s : %s\n",  
                        cnt->conf.sqlite3_db, 
                        sqlite3_errmsg(cnt->database_sqlite3));
            sqlite3_close(cnt->database_sqlite3);
            ret = -1;
        }
    }
#endif /* HAVE_SQLITE3 */

#ifdef HAVE_MYSQL
        if ((!strcmp(cnt->conf.database_type, "mysql")) && 
                                        (cnt->conf.database_dbname)) { 
            // close database to be sure that we are not leaking
            mysql_close(cnt->database);

            cnt->database = (MYSQL *) mymalloc(sizeof(MYSQL));
            mysql_init(cnt->database);

            if (!mysql_real_connect(cnt->database, cnt->conf.database_host, 
                                        cnt->conf.database_user, 
                                        cnt->conf.database_password, 
                                        cnt->conf.database_dbname, 
                                        0, NULL, 0)) {
                MOTION_LOG(ERR, TYPE_DB, NO_ERRNO, 
                                "%s: Cannot connect to MySQL database %s on "
                                "host %s with user %s",
                                cnt->conf.database_dbname, 
                                cnt->conf.database_host, 
                                cnt->conf.database_user);
                MOTION_LOG(ERR, TYPE_DB, NO_ERRNO, "%s: MySQL error was %s", 
                                    mysql_error(cnt->database));
                ret = -2;
            }
#if (defined(MYSQL_VERSION_ID)) && (MYSQL_VERSION_ID > 50012)
            my_bool my_true = TRUE;
            mysql_options(cnt->database, MYSQL_OPT_RECONNECT, &my_true);
#endif
        }
#endif /* HAVE_MYSQL */

#ifdef HAVE_PGSQL
        if ((!strcmp(cnt->conf.database_type, "postgresql")) && 
                                        (cnt->conf.database_dbname)) {
            char connstring[255];

            /* 
             * Create the connection string.
             * Quote the values so we can have null values (blank)
             */
            snprintf(connstring, 255,
                     "dbname='%s' host='%s' user='%s' password='%s' port='%d'",
                      cnt->conf.database_dbname, /* dbname */
                      (cnt->conf.database_host ? cnt->conf.database_host : ""),
                      (cnt->conf.database_user ? cnt->conf.database_user : ""),
                      (cnt->conf.database_password ? 
                                        cnt->conf.database_password : ""),
                      cnt->conf.database_port
            );

            cnt->database_pg = PQconnectdb(connstring);
            if (PQstatus(cnt->database_pg) == CONNECTION_BAD) {
                MOTION_LOG(ERR, TYPE_DB, NO_ERRNO, 
                    "%s: Connection to PostgreSQL database '%s' failed: %s",
                    cnt->conf.database_dbname, 
                    PQerrorMessage(cnt->database_pg));
                ret = -3;
            }
        }
#endif /* HAVE_PGSQL */

        /* Set the sql mask file according to the SQL config options*/

        cnt->sql_mask = ( cnt->conf.sql_log_image * 
                        (FTYPE_IMAGE + FTYPE_IMAGE_MOTION) ) +
                        (cnt->conf.sql_log_snapshot * FTYPE_IMAGE_SNAPSHOT) +
                        ( cnt->conf.sql_log_movie * 
                        (FTYPE_MPEG + FTYPE_MPEG_MOTION) ) +
                        (cnt->conf.sql_log_timelapse * FTYPE_MPEG_TIMELAPSE);
    }

#endif /* defined(HAVE_MYSQL) || defined(HAVE_PGSQL) || defined(HAVE_SQLITE3) */

    return ret;
}



void database_cleanup(struct context *cnt){
#ifdef HAVE_MYSQL
    if ( (!strcmp(cnt->conf.database_type, "mysql")) && 
                    (cnt->conf.database_dbname)) {    
        mysql_close(cnt->database); 
    }
#endif /* HAVE_MYSQL */

#ifdef HAVE_PGSQL
    if ((!strcmp(cnt->conf.database_type, "postgresql")) && 
                    (cnt->conf.database_dbname)) {
        PQfinish(cnt->database_pg);
    }
#endif /* HAVE_PGSQL */ 

#ifdef HAVE_SQLITE3    
    /* Close the SQLite database */
    if (cnt->conf.sqlite3_db)
        sqlite3_close(cnt->database_sqlite3);
#endif /* HAVE_SQLITE3 */
}


