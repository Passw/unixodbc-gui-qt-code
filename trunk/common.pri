#
# \brief	These are misc options which are common among all 
#		sources.
#
#		These are global qmake options. You probably do not
#		want to change these. 
#
#		If you are NOT using qmake then you can ignore this file.
#
#
# \sa		*.pri *.pro
#
OBJECTS_DIR	= .tmp
UI_DIR          = .tmp
MOC_DIR	       	= .tmp

win32 {
#   This to resolve a legacy variation of printf used by ODBC. 
#   This is to resolve legacy MessageBoxW used by ODBC.	
    LIBS += -llegacy_stdio_definitions -luser32
}


