INCLUDEPATH *= \
    $$PWD/

HEADERS += \
    $$PWD/json/autolink.h \
    $$PWD/json/config.h \
    $$PWD/json/features.h \
    $$PWD/json/forwards.h \
    $$PWD/json/json.h \
    $$PWD/json/reader.h \
    $$PWD/json/value.h \    
    $$PWD/json/writer.h
    
SOURCES += \
    $$PWD/lib_json/json_batchallocator.h \
    $$PWD/lib_json/json_internalarray.inl \
    $$PWD/lib_json/json_internalmap.inl \
    $$PWD/lib_json/json_reader.cpp \
    $$PWD/lib_json/json_value.cpp \
    $$PWD/lib_json/json_valueiterator.inl \
    $$PWD/lib_json/json_writer.cpp
