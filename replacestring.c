#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <string.h>
#include <stddef.h>

#if (__STDC_VERSION__ >= 199901L)
#include <stdint.h>
#endif

// source: http://creativeandcritical.net/str-replace-c
char *repl_str(const char *str, const char *from, const char *to)
{
    /* Increment positions cache size initially by this number. */
    size_t cache_sz_inc = 16;
    /* Thereafter, each time capacity needs to be increased,
	 * multiply the increment by this factor. */
    const size_t cache_sz_inc_factor = 3;
    /* But never increment capacity by more than this number. */
    const size_t cache_sz_inc_max = 1048576;

    char *pret, *ret = NULL;
    const char *pstr2, *pstr = str;
    size_t i, count = 0;
	#if (__STDC_VERSION__ >= 199901L)
    	uintptr_t *pos_cache_tmp, *pos_cache = NULL;
	#else
    	ptrdiff_t *pos_cache_tmp, *pos_cache = NULL;
	#endif
    size_t cache_sz = 0;
    size_t cpylen, orglen, retlen, tolen, fromlen = strlen(from);

    /* Find all matches and cache their positions. */
    while ((pstr2 = strstr(pstr, from)) != NULL){
		count++;

		/* Increase the cache size when necessary. */
		if (cache_sz < count) {
			cache_sz += cache_sz_inc;
			pos_cache_tmp = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
			if (pos_cache_tmp == NULL){
				goto end_repl_str;
			}
			else
				pos_cache = pos_cache_tmp;

			cache_sz_inc *= cache_sz_inc_factor;

			if (cache_sz_inc > cache_sz_inc_max){
				cache_sz_inc = cache_sz_inc_max;
			}
		}

		pos_cache[count - 1] = pstr2 - str;
		pstr = pstr2 + fromlen;
    }

    orglen = pstr - str + strlen(pstr);

    /* Allocate memory for the post-replacement string. */
    if (count > 0){
		tolen = strlen(to);
		retlen = orglen + (tolen - fromlen) * count;
    } else
		retlen = orglen;

    ret = malloc(retlen + 1);
	
    if (ret == NULL){
		goto end_repl_str;
    }

    if (count == 0) {
		/* If no matches, then just duplicate the string. */
		strcpy(ret, str);
    } else {
	/* Otherwise, duplicate the string whilst performing
		 * the replacements using the position cache. */
		pret = ret;
		memcpy(pret, str, pos_cache[0]);
		pret += pos_cache[0];
		
		for (i = 0; i < count; i++){
			memcpy(pret, to, tolen);
			pret += tolen;
			pstr = str + pos_cache[i] + fromlen;
			cpylen = (i == count - 1 ? orglen : pos_cache[i + 1]) - pos_cache[i] - fromlen;
			memcpy(pret, pstr, cpylen);
			pret += cpylen;
		}
		ret[retlen] = '\0';
    }

end_repl_str:
    /* Free the cache and return the post-replacement string,
	 * which will be NULL in the event of an error. */
    free(pos_cache);
    return ret;
}

// source: https://www.joelonsoftware.com/2001/12/11/back-to-basics/
char* mystrcat( char* dest, char* src )
{
     while ((*dest)) dest++;
     while ((*dest++ = *src++));
     return --dest;
}

int main(int argc, char **argv){
	// filename, src, dest = sys.argv[1:4]
    /* check if amount of args is correct, otherwise exit */
    if (argc != 4) {
		fprintf(stderr, "`replacestring` except three arguments.\nUsage: replacestring filename src dest\n");
		exit(1);
    } else {
		// filename_stage = filename + '.esy_rewrite'
		// we know exactly how much to allocate: strlen filename + (".esy_rewrite") 12 + ('\0') 1 = 13
		// TODO check if this is a proper reasoning ^^
		char filename_stage[strlen(argv[1]) + 13];
		char *p = filename_stage;
		filename_stage[0] = '\0';
		p = mystrcat(filename_stage, argv[1]);
		p = mystrcat(filename_stage, ".esy_rewrite");

		// filestat = os.stat(filename)
		struct stat fileStat;
		if(stat(argv[1], &fileStat) < 0){
			exit(1);
		}

		// # TODO: we probably should handle symlinks too in a special way,
		// # to modify their location to a rewritten path

		// with open(filename, 'r') as input_file:
		//     data = input_file.read()
		/*
			http://stackoverflow.com/questions/19260209/ftell-returning-incorrect-value
			"You should get in a habit of always doing this, since only binary mode has well-defined behavior in standard C.
			On POSIX systems, binary and text (default) mode behave the same,
			but on windows, munging of newlines takes place in a way that messes up file contents and offsets."
		*/
		FILE *in = fopen(argv[1], "rb");
		if(in == NULL) {
			fprintf(stderr, "Could not open file: %s", argv[1]);
			exit(1);
		}
		char original[fileStat.st_size + 1];
		size_t len = fread(original, 1, fileStat.st_size, in);
		// fread(...) does not zero-terminate a 'string''
		original[len] = '\0';

		// data = data.replace(src, dest)
		char *data = repl_str(original, argv[2], argv[3]);
		if(data == NULL){
			fprintf(stderr, "Could not replace the `src` string with the `dest` string");
			exit(1);
		}

		// with open(filename_stage, 'w') as output_file:
		//     output_file.write(data)
		FILE *out = fopen(filename_stage, "w");
		if(out == NULL) {
			fprintf(stderr, "Could not open file: %s", filename_stage);
			exit(1);
		}
		fprintf(out,"%s", data);

		// os.rename(filename_stage, filename)
		rename(filename_stage, argv[1]);
		// os.chmod(filename, stat.S_IMODE(filestat.st_mode))
		chmod(argv[1], fileStat.st_mode);

		// componentWillUnmount stuff
		fclose(in);
		fclose(out);

		return 0;
    }
}
