TEMPLATE	= subdirs
unix {
    equals( QT_MAJOR_VERSION, 4 ) {
        SUBDIRS		= \
            QtCore4 \
            QtGui4
    }

}

