#ifndef __NOTIFY_H__
#define __NOTIFY_H__

#ifdef __cplusplus
extern "C" {
#endif

int displayInotifyEvent(struct inotify_event *event);

void *notify_pthread(void *pdata); //notify the file

void create_notify_pthread(void);

#ifdef __cplusplus
}
#endif


#endif /* __NOTIFY_H__ */