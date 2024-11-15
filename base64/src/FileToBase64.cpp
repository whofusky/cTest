/****************************************
 * 
 *    FileToBase64.cpp
 * 
 *    Create by fushikai
 * 
 *    2023-09-13_10:16:44
 * 
 ****************************************/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <sys/uio.h>
#include <string>
#include "FileToBase64.h"
#include "CustomOutLog.h"




// Size of the buffer for the "raw" (not base64-encoded) data in bytes.
#define BUFFER_RAW_SIZE (1024 * 1024)

// Size of the buffer for the base64-encoded data in bytes. The base64-encoded
// data is 4/3 the size of the input, with some margin to be sure.
#define BUFFER_ENC_SIZE (BUFFER_RAW_SIZE * 4 / 3 + 16)





// config structure.
struct T_Bf_config {


    // Input file name
	const char *rdfile;
    // Output file name
	const char *wtfile;

	// Input file handle.
	FILE *rd_fp;
	// Output file handle.
	FILE *wt_fp;

	// Input file file descriptor: fileno(rd_fp)
    int  rd_fd;
	// Output file descriptor: fileno(wt_fd)
    int  wt_fd;

	// Wrap width in characters, for encoding only.
	size_t wrap;

};

// Input/output buffer structure.
struct T_Bio_Buffer {

	// Runtime-allocated buffer for raw (unencoded) data.
	char *raw;

	// Runtime-allocated buffer for base64-encoded data.
	char *enc;
};






static bool buffer_alloc (struct T_Bio_Buffer *buf)
{
	if ((buf->raw = (char*)malloc(BUFFER_RAW_SIZE)) == NULL ||
	    (buf->enc = (char*)malloc(BUFFER_ENC_SIZE)) == NULL) {
		free(buf->raw);
        
        b_write_log(_ERROR,"malloc:[%d][%s]",errno, strerror(errno));

		return false;
	}

	return true;
}




static void buffer_free (struct T_Bio_Buffer *buf)
{
	free(buf->raw);
	free(buf->enc);
}

static void clear_config (struct T_Bf_config *config)
{
    if ( config == NULL ) return;
    if ( config->rdfile != NULL ) config->rdfile = NULL;
    if ( config->wtfile != NULL ) config->wtfile = NULL;
    if ( config->rd_fp != NULL ) {
        fclose( config->rd_fp );
        config->rd_fp = NULL;
    }
    if ( config->wt_fp != NULL ) {
        fclose( config->wt_fp );
        config->wt_fp = NULL;
    }
    config->rd_fd = -1;
    config->wt_fd = -1;
    config->wrap  = 0;
}




static bool writev_en_retry (const struct T_Bf_config *config, struct iovec *iov, size_t nvec)
{
	// Writing nothing always succeeds.
	if (nvec == 0) {
		return true;
	}

	while (true) {
		ssize_t nwrite;

		// Try to write the vectors to stdout.
		if ((nwrite = writev(config->wt_fd, iov, nvec)) < 0) {

			// Retry on EINTR.
			if (errno == EINTR) {
				continue;
			}

			// Quit on other errors.
            b_write_log(_ERROR,"writev:[%d][%s]", errno, strerror(errno) );
			return false;
		}

		// The return value of `writev' is the number of bytes written.
		// To check for success, we traverse the list and remove all
		// written vectors. The call succeeded if the list is empty.
		while (true) {

			// Retry if this vector is not or partially written.
			if (iov->iov_len > (size_t) nwrite) {
				char *base = (char*)iov->iov_base;

				iov->iov_base = (size_t) nwrite + base;
				iov->iov_len -= (size_t) nwrite;
				break;
			}

			// Move to the next vector.
			nwrite -= iov->iov_len;
			iov++;

			// Return successfully if all vectors were written.
			if (--nvec == 0) {
				return true;
			}
		}
	}
}




static inline bool iov_en_append (const struct T_Bf_config *config, struct iovec *iov,
            size_t *nvec, char *base, const size_t len)
{
	// Add the buffer to the IO vector array.
	iov[*nvec].iov_base = base;
	iov[*nvec].iov_len  = len;

	// Increment the array index. Flush the array if it is full.
	if (++(*nvec) == IOV_MAX) {
		if (writev_en_retry(config, iov, IOV_MAX) == false) {
			return false;
		}
		*nvec = 0;
	}

	return true;
}




static bool write_outfile (const struct T_Bf_config *config, const char *buf, size_t len)
{
	while (len > 0) {
		ssize_t nwrite;

		// Try to write the buffer to stdout.
		if ((nwrite = write(config->wt_fd, buf, len)) < 0) {

			// Retry on EINTR.
			if (errno == EINTR) {
				continue;
			}

			// Quit on other errors.
            b_write_log(_ERROR,"write:[%d][%s]", errno, strerror(errno) );
			return false;
		}

		// Update the buffer position.
		buf += (size_t) nwrite;
		len -= (size_t) nwrite;
	}

	return true;
}




static bool write_en_wrapped (const struct T_Bf_config *config, char *buf, size_t len)
{
	static size_t col = 0;

	// Special case: if buf is NULL, print final trailing newline.
	if (buf == NULL) {
		if (config->wrap > 0 && col > 0) {
			return write_outfile(config, "\n", 1);
		}
		return true;
	}

	// If no wrap width is given, write the entire buffer.
	if (config->wrap == 0) {
		return write_outfile(config, buf, len);
	}

	// Statically allocated IO vector buffer.
	static struct iovec iov[IOV_MAX];
	size_t nvec = 0;

	while (len > 0) {

		// Number of characters to fill the current line.
		size_t nwrite = config->wrap - col;

		// Do not write more data than is available.
		if (nwrite > len) {
			nwrite = len;
		}

		// Append the data to the IO vector array.
		if (iov_en_append(config, iov, &nvec, buf, nwrite) == false) {
			return false;
		}

		// Advance the buffer.
		len -= nwrite;
		buf += nwrite;
		col += nwrite;

		// If the line is full, append a newline.
		if (col == config->wrap) {
			if (iov_en_append(config, iov, &nvec, (char*)"\n", 1) == false) {
				return false;
			}
			col = 0;
		}
	}

	// Write the remaining vectors.
	if (writev_en_retry(config, iov, nvec) == false) {
		return false;
	}

	return true;
}




static bool encode (const struct T_Bf_config *config, struct T_Bio_Buffer *buf)
{
	size_t nread, nout;
	struct base64_state state;

	// Initialize the encoder's state structure.
	base64_stream_encode_init(&state, 0);

	// Read raw data into the buffer.
	while ((nread = fread(buf->raw, 1, BUFFER_RAW_SIZE, config->rd_fp)) > 0) {

		// Encode the raw input into the encoded buffer.
		base64_stream_encode(&state, buf->raw, nread, buf->enc, &nout);

		// Append the encoded data to the output stream.
		if (write_en_wrapped(config, buf->enc, nout) == false) {
			return false;
		}
	}

	// Check for stream errors.
	if (ferror(config->rd_fp)) {
        b_write_log(_ERROR,"%s: read error",config->rdfile);
		return false;
	}

	// Finalize the encoding by adding proper stream terminators.
	base64_stream_encode_final(&state, buf->enc, &nout);

	// Append this tail to the output stream.
	if (write_en_wrapped(config, buf->enc, nout) == false) {
		return false;
	}

	// Print optional trailing newline.
	if (write_en_wrapped(config, NULL, 0) == false) {
		return false;
	}

	return true;
}




static int decode (const struct T_Bf_config *config, struct T_Bio_Buffer *buf)
{
	size_t nread, nout;
	struct base64_state state;

	// Initialize the decoder's state structure.
	base64_stream_decode_init(&state, 0);

    //size_t lineC = 0; //lineC 表示还并多少字符到换行符


	// Read encoded data into the buffer. Use the smallest buffer size to
	// be on the safe side: the decoded output will fit the raw buffer.
	while ((nread = fread(buf->enc, 1, BUFFER_RAW_SIZE, config->rd_fp)) > 0) {

		// Decode the input into the raw buffer.
        if ( config->wrap == 0 ) {
            if (base64_stream_decode(&state, buf->enc, nread,
                                     buf->raw, &nout) == 0) {
                b_write_log(_ERROR,"%s: decoded error",config->rdfile);
                return false;
            }
        }
        else {

            nout = 0;
            size_t tnout = 0;

            size_t head = 0;
            size_t tail = 0;
            size_t len = 0;

            while ( head < nread ) {
                while ( buf->enc[head] == '\n' && head < nread ) {
                    head++;
                }
                tail = head;
                while (buf->enc[tail] != '\n' && tail < nread ) {
                    tail++;
                }
                len = tail - head;
                if (base64_stream_decode(&state, 
                            buf->enc + head, 
                            len, 
                            buf->raw + nout, 
                            &tnout) == 0) {
                    b_write_log(_ERROR,"%s: decoded error",config->rdfile);
                    return false;
                }
                head =  tail +1;
                nout += tnout;
            }
        }

		// Append the raw data to the output stream.
		if (write_outfile(config, buf->raw, nout) == false) {
			return false;
		}
	}

	// Check for stream errors.
	if (ferror(config->rd_fp)) {
        b_write_log(_ERROR,"%s: read error",config->rdfile);
		return false;
	}

	return true;
}

















C_FB64::C_FB64()
{
    m_wrap = 0;
}

C_FB64::C_FB64(size_t wrap)
{
    m_wrap = wrap;
}


C_FB64::~C_FB64()
{
    //m_wrap = 0;
}


void C_FB64::set_wrap(size_t wrap)
{
    m_wrap = wrap;
}


//将文件 file 的内容转换成base64 存储为新文件file.encode 
// return: 0 sucess
int  C_FB64::encode_file(const char* file, const char *outfile)
{
    FILE *rd_fp = fopen( file, "rb" );
    if ( rd_fp == NULL ) {
        b_write_log(_ERROR,
                "fopen [%s],return[%d][%s]",
                file, errno, strerror(errno) );
        return errno;
    }

    std::string enFile;
    if ( outfile != NULL ) {
        enFile = outfile;
    }
    else {
        enFile = file;
        enFile += ".encode";
    }
    FILE *wt_fp = fopen( enFile.c_str(), "w+");
    if ( wt_fp == NULL ) {
        b_write_log(_ERROR,
                "fopen [%s],return[%d][%s]",
                enFile.c_str(), errno, strerror(errno) );
        return errno;
    }
    int rd_fd = fileno( rd_fp );
    if ( rd_fd < 0 ) {
        b_write_log(_ERROR,
                "fileno [%s],return[%d][%s]",
                file, errno, strerror(errno) );
        return errno;
    }
    int wt_fd = fileno( wt_fp );
    if ( wt_fd < 0 ) {
        b_write_log(_ERROR,
                "fileno [%s],return[%d][%s]",
                enFile.c_str(), errno, strerror(errno) );
        return errno;
    }

	// Default program config.
	struct T_Bf_config config = {
		.rdfile       = file,
		.wtfile       = enFile.c_str(),
		.rd_fp        = rd_fp,
		.wt_fp        = wt_fp,
		.rd_fd        = rd_fd,
		.wt_fd        = wt_fd,
        .wrap         = m_wrap,
	};

	struct T_Bio_Buffer buf;

	// Allocate buffers.
	if (buffer_alloc(&buf) == false) {
        clear_config( &config );
		return 1;
	}
    bool bret = encode( &config, &buf );

	buffer_free(&buf);
    clear_config( &config );

	return bret ? 0 : 1;
}



//将文件内容为base64的文件file 的内容decode成原内容存储为新文件file.decode 
// return: 0 sucess
int  C_FB64::decode_file(const char* file, const char *outfile)
{
    FILE *rd_fp = fopen( file, "rb" );
    if ( rd_fp == NULL ) {
        b_write_log(_ERROR,
                "fopen [%s],return[%d][%s]",
                file, errno, strerror(errno) );
        return errno;
    }

    std::string deFile;
    if ( outfile != NULL ) {
        deFile = outfile;
    }
    else {
        deFile = file;
        deFile += ".decode";
    }
    FILE *wt_fp = fopen( deFile.c_str(), "w+");
    if ( wt_fp == NULL ) {
        b_write_log(_ERROR,
                "fopen [%s],return[%d][%s]",
                deFile.c_str(), errno, strerror(errno) );
        return errno;
    }
    int rd_fd = fileno( rd_fp );
    if ( rd_fd < 0 ) {
        b_write_log(_ERROR,
                "fileno [%s],return[%d][%s]",
                file, errno, strerror(errno) );
        return errno;
    }
    int wt_fd = fileno( wt_fp );
    if ( wt_fd < 0 ) {
        b_write_log(_ERROR,
                "fileno [%s],return[%d][%s]",
                deFile.c_str(), errno, strerror(errno) );
        return errno;
    }

	// Default program config.
	struct T_Bf_config config = {
		.rdfile       = file,
		.wtfile       = deFile.c_str(),
		.rd_fp        = rd_fp,
		.wt_fp        = wt_fp,
		.rd_fd        = rd_fd,
		.wt_fd        = wt_fd,
        .wrap         = m_wrap,
	};

	struct T_Bio_Buffer buf;

	// Allocate buffers.
	if (buffer_alloc(&buf) == false) {
        clear_config( &config );
		return 1;
	}
    bool bret = decode( &config, &buf );

	buffer_free(&buf);
    clear_config( &config );

	return bret ? 0 : 1;
}


