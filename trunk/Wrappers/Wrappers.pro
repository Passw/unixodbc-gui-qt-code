TEMPLATE	= subdirs
unix {
    equals( QT_MAJOR_VERSION, 4 ) {
        SUBDIRS		= \
            QtCore4 \
            QtGui4
    }

    equals( QT_MAJOR_VERSION, 5 ) {
        SUBDIRS		= \
            QtCore5 \
            QtGui5
    }
}

