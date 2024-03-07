#include <stdlib.h>
#include <string.h>

#include "CustomOutLog.h"
#include "FileToBase64.h"



int main(int argc, char **argv) {

    if ( argc != 3 && argc != 4 ) {
        fprintf(stderr,
                "\n  Input ERROR,please usage:"
                "\n    %s  <en/de/enf/def>  <string/file> [outfile]\n" 
                "        [Options]:\n" 
                "        --en :  original tring --> base64 string\n" 
                "        --de :  base64 string  --> original string\n" 
                "        --enf:  original file  --> base64 file.encode\n" 
                "        --def:  base64 file    --> original file.decode\n" 
                "\n", 
                argv[0]);
        return 1;
    }

    int enFlag  = strcmp( argv[1], "en"  );   //[string encode]: original tring --> base64 string
    int deFlag  = strcmp( argv[1], "de"  );   //[string decode]: base64 string --> original string

    int enfFlag = strcmp( argv[1], "enf" );   //[file encode]: original file --> base64 file.encode
    int defFlag = strcmp( argv[1], "def" );   //[file decode]: base64 file --> original file.decode

    int srcLen  = strlen( argv[2] );

    int ret = 0;


    //c_write_log(_DEBUG,
    //        "enFlag[%d],deFlag[%d],enfFlag[%d],defFlag[%d]", 
    //        enFlag, deFlag,enfFlag,defFlag );

    if ( enFlag != 0 && deFlag != 0 && enfFlag != 0 && defFlag != 0 ) {
        fprintf(stderr,
                "\n  Input ERROR,please usage:"
                "\n    %s  <en/de/enf/def>  <string/file> [outfile]\n" 
                "        [Options]:\n" 
                "        --en :  original tring --> base64 string\n" 
                "        --de :  base64 string  --> original string\n" 
                "        --enf:  original file  --> base64 file.encode\n" 
                "        --def:  base64 file    --> original file.decode\n" 
                "\n", 
                argv[0]);
        return 2;
    }

    char buf[1024] = {0};

    size_t outlen = 0;

    if ( enFlag == 0 ) { //[string encode]: original tring --> base64 string

        //The buffer in `out` has been allocated by the caller and is at least 4/3 the
        //size of the input.
        //
        //也就是说: out需要自己申请空间大小,且大小需要大于输入src长度的4/3
        //建议申请空间为 ( strlen(src) * (4/3) ) + 16

        //void BASE64_EXPORT base64_encode
        //    ( const char   *src
        //    , size_t		 srclen
        //    , char	     *out
        //    , size_t		 *outlen
        //    , int			 flags
        //    ) ;

        base64_encode(argv[2], srcLen, buf, &outlen, 0 );

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
        //    , size_t		    srclen
        //    , char			*out
        //    , size_t		    *outlen
        //    , int			    flags
        //    ) ;
        ret = base64_decode(argv[2],srcLen, buf, &outlen, 0 );
        c_write_log(_INFO,
                "src=[%s],src_len=[%d],decode=[%s],decode_len=[%ld],ret=[%d]",
                argv[2], srcLen, buf, outlen, ret );
        return 0;
    }

    C_FB64 fb64;

    if ( enfFlag == 0 ) { //[file encode]: 

        if ( argc == 4 ) {
            //encode original file --> base64 outfile

            ret= fb64.encode_file( argv[2], argv[3] );
            c_write_log(_INFO, "encode_file[%s]-->[%s],ret=[%d]", 
                    argv[2],argv[3],  ret );
        }
        else {
            //encode original file --> base64 file.encode

            ret= fb64.encode_file( argv[2] );
            c_write_log(_INFO, "encode_file[%s]-->[%s.encode],ret=[%d]", 
                    argv[2],argv[2],  ret );
        }

        return ret;
    }

    if ( defFlag == 0 ) { //[file decode]: 

        if ( argc == 4 ) {
            // decode base64 file --> original outfile
            
            ret= fb64.decode_file( argv[2], argv[3] );
            c_write_log(_INFO, "decode_file[%s]-->[%s],ret=[%d]", 
                    argv[2],argv[3], ret );
        }
        else {
            //decode base64 file --> original file.decode

            ret= fb64.decode_file( argv[2] );
            c_write_log(_INFO, "decode_file[%s]-->[%s.decode],ret=[%d]", 
                    argv[2],argv[2], ret );
        }

        return ret;

    }

    return 0;
}
