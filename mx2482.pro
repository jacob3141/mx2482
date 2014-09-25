TEMPLATE = subdirs
SUBDIRS = mx2482 libqjackaudio

mx2482.subdir = mx2482
mx2482.depends = libqjackaudio

libqjackaudio.subdir = libqjackaudio
libqjackaudio.depends =
