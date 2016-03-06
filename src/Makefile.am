bin_PROGRAMS = motion

motion_SOURCES = alg.c alg.h conf.c conf.h draw.c event.c event.h ffmpeg.c \
    ffmpeg.h jpegutils.c jpegutils.h logger.c logger.h md5.c md5.h mmx.h   \
    motion.c motion.h netcam.c netcam_ftp.c netcam_ftp.h netcam.h          \
    netcam_jpeg.c netcam_rtsp.c netcam_rtsp.h netcam_wget.c netcam_wget.h  \
    picture.c picture.h pwc-ioctl.h rotate.c rotate.h stream.c 			   \
    stream.h track.c track.h video.h vloopback_motion.c             	   \
    vloopback_motion.h webhttpd.c webhttpd.h
motion_CFLAGS=-Dsysconfdir='"$(sysconfdir)"'

motion_LDADD=@JPEG_LIBS@

if HAVE_SDL
motion_SOURCES += sdl.c sdl.h
endif

if BSD_VIDEO
motion_SOURCES += video_freebsd.c video_freebsd.h
else
motion_SOURCES += video2.c video.c video_common.c video.h
endif
