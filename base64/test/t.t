//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CustomOutLog.h"
//#include "libbase64.h"
#include "FileToBase64.h"



int main(int argc, char **argv) {

    
    int ret = 0;

    //c_write_log(_DEBUG,"argc[%d],argv0=[%s],ret=[%d]",argc,argv[0],ret);
    //for( int i = 0; i < argc; i++) {
    //    c_write_log(_DEBUG,"argv[%d]=[%s]", i, argv[i]);
    //}

    if ( argc != 3 ) {
        c_write_log(_ERROR,"usage:\n %s <en/de/enf/def> <string/file>\n", argv[0]);
        return 1;
    }
    int enFlag = strcmp( argv[1],"en" );
    int deFlag =  strcmp( argv[1], "de" );
    int enfFlag = strcmp( argv[1],"enf" );
    int defFlag =  strcmp( argv[1], "def" );

    c_write_log(_DEBUG,
            "enFlag[%d],deFlag[%d],enfFlag[%d],defFlag[%d]", 
            enFlag, deFlag,enfFlag,defFlag);

    if ( enFlag != 0 && deFlag != 0 && enfFlag != 0 && defFlag != 0 ) {
        c_write_log(_ERROR,
                "usage:\n %s <en/de/enf/def> <string/file>\n", 
                argv[0]);
        return 2;
    }
    int srcLen= strlen( argv[2] );
    char buf[1024] = {0};
    size_t outlen = 0;

    if ( enFlag == 0 ) { //[string encode]: original tring --> base64 string

        //The buffer in `out` has been allocated by the caller and is at least 4/3 the
        //size of the input.
        //
        //也就是说: out需要自己申请空间大小,且大小需要大于输入src长度的4/3
        //建议申请空间为 ( strlen(src) * (4/3) ) + 16

        //void BASE64_EXPORT base64_encode
        //    ( const char		*src
        //    , size_t		 srclen
        //    , char			*out
        //    , size_t		*outlen
        //    , int			 flags
        //    ) ;

        base64_encode(argv[2],srcLen, buf, &outlen, 0 );

        c_write_log(_INFO,
                "src=[%s],src_len=[%d],encode=[%s],encode_len=[%ld]",
                argv[2], srcLen, buf, outlen );
        return 0;
    }

    if ( deFlag == 0 ) { //[string decode]: base64 string --> original string

        //The buffer in `out` has been allocated by the caller and is at least 3/4 the
        //size of the input
        //
        //也就是说: out需要自己申请空间大小,且大小需要大于输入src长度的3/4

        //int BASE64_EXPORT base64_decode
        //    ( const char		*src
        //    , size_t		 srclen
        //    , char			*out
        //    , size_t		*outlen
        //    , int			 flags
        //    ) ;
        ret = base64_decode(argv[2],srcLen, buf, &outlen, 0 );
        c_write_log(_INFO,
                "src=[%s],src_len=[%d],decode=[%s],decode_len=[%ld],ret=[%d]",
                argv[2], srcLen, buf, outlen, ret );
        return 0;
    }

    C_FB64 fb64(74);
    if ( enfFlag == 0 ) { //[file encode]: original file --> base64 file.encode
        ret= fb64.encode_file( argv[2] );
        c_write_log(_INFO,
                "encode_file[%s],ret=[%d]",
                argv[2],  ret );
        return ret;
    }

    if ( defFlag == 0 ) { //[file decode]: base64 file --> original file.decode
        ret= fb64.decode_file( argv[2] );
        c_write_log(_INFO,
                "decode_file[%s],ret=[%d]",
                argv[2],  ret );
        return ret;
    }

    


    return 0;
}
