%module matrix_features
%{
    #define SWIG_FILE_WITH_INIT
%}

%include <exception.i>
%import(module="rftk.asserts") "asserts.i"
%import(module="rftk.buffers") "buffers.i"
