#include "motion.h"
#include "motion_redis.h"
#include <time.h>

#define REDIS_RETRY_COUNT 10

int motion_init_redis(struct context *cnt)
{
    int ret = 0;
    if (cnt->conf.redis_host && cnt->conf.redis_port > 0) {
        ret = motion_redis_connect(cnt);
    } else {
        MOTION_LOG(INF, TYPE_REDIS, NO_ERRNO,
                   "[%s] Redis Event notification disabled by configuration");
    }

    return ret;
}

void motion_redis_cleanup(struct context *cnt)
{
    if (cnt->redisContext != NULL) {
        redisFree(cnt->redisContext);
        cnt->redisContext = NULL;
    }
}

int motion_redis_connect(struct context *cnt)
{
    int ret = 0;
    if (cnt->redisContext != NULL) {
        redisFree(cnt->redisContext);
        cnt->redisContext = NULL;
    }

    cnt->redisContext = redisConnect(cnt->conf.redis_host,
                                            cnt->conf.redis_port);
    if(cnt->redisContext == NULL) {
        MOTION_LOG(ERR, TYPE_REDIS, NO_ERRNO,
                   "%s: Can't connect to redis_host (%s:%d)\n",
                   cnt->conf.redis_host, cnt->conf.redis_port);
            ret = 1;
    }else{
        if ( cnt->redisContext->err ) {
            MOTION_LOG(ERR, TYPE_REDIS, NO_ERRNO,
                       "%s: Can't connect to redis_host (%s:%d) %s\n",
                       cnt->conf.redis_host, cnt->conf.redis_port,
                       cnt->redisContext->errstr);
            ret = 1;
        }
    }

    ret = motion_redis_sendCmd(cnt,
                               "set motion_api_version " MOTION_REDIS_API_V );

    ret = motion_redis_sendCmd(cnt,
                               "CONFIG SET timeout " MOTION_REDIS_TIMEOUT);

    return ret;
}

static redisReply* redisSendCmdRetry(struct context *cnt, char *msg)
{
    redisReply *reply = NULL;
    int retryCount = 0;
    struct timeval retryTimeOut;
    retryTimeOut.tv_sec = 0;
    retryTimeOut.tv_usec = 1000;
    while (reply == NULL && retryCount < REDIS_RETRY_COUNT)
    {
        reply = redisCommand( cnt->redisContext, msg);
        if (reply == NULL){
            switch (cnt->redisContext->err){
                case REDIS_ERR_IO:
                    MOTION_LOG(ERR, TYPE_REDIS, SHOW_ERRNO,
                            "[%s] Failed to send to server (%s)", msg);
                    break;
                default:
                    MOTION_LOG(ERR, TYPE_REDIS, NO_ERRNO,
                               "[%s] Failed to send to server (%s)",
                               cnt->redisContext->errstr);
            }
            /* lets sleep some and lets try to reconnect */
            select(0,0,0,0,&retryTimeOut);
            motion_redis_connect(cnt);
            retryCount++;
        }
    }

    return reply;
}

int motion_redis_sendCmd(struct context *cnt, char *msg)
{
    int ret = 0;
    redisReply *reply;
    MOTION_LOG(INF, TYPE_REDIS, NO_ERRNO, "[%s] Executing: %s", msg);
    /*
     * bail out before we do anything with the context if it's not setup
     * this is valid when we didn't configure redis to begin with
     */
    if ( cnt->redisContext == NULL ) {
        MOTION_LOG(DBG, TYPE_REDIS, NO_ERRNO, "[%s] Context is NULL");
        return ret;
    }

    reply = redisSendCmdRetry(cnt, msg);

    if(reply == NULL){
        MOTION_LOG(ERR, TYPE_REDIS, NO_ERRNO,
                   "[%s] Command('%s') Excution Error: %s", msg);
        return 1;
   }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            MOTION_LOG(ERR, TYPE_REDIS, NO_ERRNO,
                       "[%s] Command('%s') Excution Error: %s", msg,
                       reply->str);
            ret = 1;
            break;
        default:
            MOTION_LOG(INF, TYPE_REDIS, NO_ERRNO,
                       "[%s] Command('%s') finished with code: %d", msg,
                       reply->type);
    }

    freeReplyObject(reply);

    return ret;
}


void redis_handle_event(struct context *cnt, motion_event type,
            unsigned char *dummy ATTRIBUTE_UNUSED,
            char *filename, void *arg, struct tm *tm ATTRIBUTE_UNUSED)
{
    char buff[1024];
    /* Todo:
     * json format these events so that data can be serialized and the
     * caller will get something useful per each event type.
     */

    switch ( type ) {
        case EVENT_FILECLOSE:
            snprintf(buff, sizeof(buff), "rpush %s:%s %s",
                     cnt->conf.redis_queue_name, eventToString(type), filename);
            MOTION_LOG(INF, TYPE_REDIS, NO_ERRNO, "[%s] Sending event (%d) %s",
                       (int) type, eventToString(type));
            motion_redis_sendCmd(cnt, buff);
            break;
        case EVENT_FIRSTMOTION:
        case EVENT_ENDMOTION:
            snprintf(buff, sizeof(buff), "rpush %s:%s %s",
                     cnt->conf.redis_queue_name, eventToString(type), filename);
            MOTION_LOG(INF, TYPE_REDIS, NO_ERRNO, "[%s] Sending event (%d) %s",
                       (int) type, eventToString(type));
            break;
        default:
            MOTION_LOG(DBG, TYPE_REDIS, NO_ERRNO, "[%s] Ignoring event (%d) %s",
                   (int) type, eventToString(type));
    }
}
