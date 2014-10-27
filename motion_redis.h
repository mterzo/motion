#ifndef __MOTION_REDIS_H
#define __MOTION_REDIS_H

#include "motion.h"

#include <hiredis/hiredis.h>
#include "event.h"

#define MOTION_REDIS_API_V "1.0"
#define MOTION_REDIS_QUEUE "motion"
#define MOTION_REDIS_PORT  6379
#define MOTION_REDIS_TIMEOUT "0"

extern int motion_init_redis(struct context *cnt);
extern void motion_redis_cleanup(struct context *cnt);
extern int motion_redis_connect(struct context *cnt);
extern int motion_redis_sendCmd(struct context *cnt, char *msg);
extern void redis_handle_event(struct context *cnt, motion_event type,
                               unsigned char *dummy ATTRIBUTE_UNUSED,
                               char *filename, void *arg,
                               struct tm *tm ATTRIBUTE_UNUSED);

#endif
