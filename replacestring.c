#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <string.h>
#include <stddef.h>

#if (__STDC_VERSION__ >= 199901L)
#include <stdint.h>
#endif

char *repl_str(const char *str, const char *from, const char *to)
{
    /* Adjust each of the below values to suit your needs. */

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

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
}

// What if the file is greater than 1,000 bytes?
// There are various system calls that will give you the size of a file; a common one is stat.
char *readFilee(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    char *code;
    size_t n = 0;
    int c;

    if (file == NULL)
        return NULL; //could not open file

    code = malloc(1000);

    while ((c = fgetc(file)) != EOF)
    {
        code[n++] = (char) c;
    }

    // don't forget to terminate with the null character
    code[n] = '\0';        

    return code;
}

int main(int argc, char **argv){
		// filename, src, dest = sys.argv[1:4]
    /* check if amount of args is correct, otherwise exit */
    if (argc != 4) {
			fprintf(stderr, "`replaceString` except three arguments.\nUsage: replaceString filename src dest\n");
			exit(1);
    } else {
			// filename_stage = filename + '.esy_rewrite'
			char* filename_stage = concat(argv[1], ".esy_rewrite");

			// filestat = os.stat(filename)
			struct stat fileStat;
			if(stat(argv[1], &fileStat) < 0)    
        exit(1);
			
			// # TODO: we probably should handle symlinks too in a special way,
			// # to modify their location to a rewritten path
			
			// with open(filename, 'r') as input_file:
			//     data = input_file.read()
			char *original = readFilee(argv[1]);

			// data = data.replace(src, dest)
			

			// with open(filename_stage, 'w') as output_file:
			//     output_file.write(data)
			FILE *out = fopen(filename_stage, "w");
			fprintf(out,"%s", repl_str(original, argv[2], argv[3]));

			// os.rename(filename_stage, filename)
			rename(filename_stage, argv[1]);
			// os.chmod(filename, stat.S_IMODE(filestat.st_mode))
			chmod(argv[1], fileStat.st_mode);

			// componentWillUnmount stuff
			fclose(out);
			free(filename_stage);
			free(original);

			return 0;
    }
}