#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#define REHASH(a, b, h) ((((h) - (a)*d) << 1) + (b))

// source: https://www.joelonsoftware.com/2001/12/11/back-to-basics/
char* scat( char* dest, char* src ) {
  while ((*dest)) dest++;
  while ((*dest++ = *src++));
  return --dest;
}

// Rabin-Karp search (modified from: http://www-igm.univ-mlv.fr/~lecroq/string/node5.html#SECTION0050)
int indexOf(const char *needle, size_t needleLen, const char *haystack, size_t haystackLen) {
  if (needleLen == 0) {
    return 0;
  } else if(needleLen > haystackLen) {
    return -1;
  }

  int d, hx, hy, i, j;
  /* Preprocessing */
  /* computes d = 2^(m-1) with
    the left-shift operator */
  for (d = i = 1; i < needleLen; ++i)
    d = (d<<1);

  for (hy = hx = i = 0; i < needleLen; ++i) {
    hx = ((hx<<1) + needle[i]);
    hy = ((hy<<1) + haystack[i]);
  }

   /* Searching */
  j = 0;
  while (j <= haystackLen-needleLen) {
    if (hx == hy && memcmp(needle, haystack + j, needleLen) == 0) {
      return j;
    }
    hy = REHASH(haystack[j], haystack[j + needleLen], hy);
    ++j;
  }

  return -1;
}

int main(int argc, char **argv) {
  FILE *in, *out;
  char *s = NULL;
  size_t cacheSize = 0;
  size_t cacheSizeInc = 16;
  const size_t cacheIncFactor = 3;
  const size_t cacheIncMax = 1048576;
  int *indexCacheTmp, *indexCache = NULL;
  // indexCache = malloc(cacheSize);
  size_t r, filelen, newFilelen;
  struct stat fileStat;
  const char* filename;
  const char* old;
  const char* new;

  filename = argv[argc - 3];
  old = argv[argc - 2];
  new = argv[argc - 1];

  // concat filename with ".rewrite" (temp file name)
  char filename_stage[strlen(argv[1]) + 9];
  char *p = filename_stage;
  filename_stage[0] = '\0';
  p = scat(filename_stage, argv[1]);
  p = scat(filename_stage, ".rewrite");

  if(stat(filename, &fileStat) < 0){
    printf("stat problem \n");
    exit(1);
  }

  // read filelen given by fileState
  // alternatively this could be determined with fseek && ftell
  filelen = fileStat.st_size;

  in = fopen(filename, "rb");

  if ((s = malloc(filelen)) == NULL) {
    printf("malloc s filelen problem \n");
    exit(1);
  }

  // Read in as much of specified file as possible
  // If there isn't anything to read, finish succesfully :)
  if ((r = fread(s, 1, filelen, in)) == 0) {
    free(s);
    free(indexCache);
    fclose(in);
    return 0;
  }

  fclose(in);

  char *t = NULL;
  char *temp = NULL;

  size_t oldLen = strlen(old);
  size_t newLen = strlen(new);

  /* Find all matches and cache their positions. */
  const char *test = NULL;
  test = s;
  int j, start, c = 0;
  int index;

  while((index = indexOf(old, oldLen, test + start, filelen - start)) != -1) {
    c++;
    if (cacheSize  < c) {
      cacheSize += cacheSizeInc;
      indexCacheTmp = realloc(indexCache, sizeof(*indexCache) * cacheSize);
      if (indexCacheTmp == NULL) {
        printf("%s\n", "fastreplacestring.c: couldn't allocate a new cache");
        exit(1);
      } else {
        indexCache = indexCacheTmp;
      }
      cacheSizeInc *= cacheIncFactor;
      if (cacheSizeInc > cacheIncMax) {
				cacheSizeInc = cacheIncMax;
			}
    }
    j = start;
    j += index;
    if(indexCache == NULL) {
      printf("%s\n", "fastreplacestring.c: NULL DEREFERENCE of indexCache");
      exit(1);
    }
    indexCache[c - 1] = j;
    start = j + oldLen;
  }

  if (c == 0) {
    free(s);
    return 0;
  } else {
    const char *pstr = s;
    // calculate new file len & allocate memory
    newFilelen = filelen + c * (newLen - oldLen);
    t = malloc(newFilelen);

    int i, w = 0;
    j = 0;
    start = 0;
    temp = t;

    if(temp == NULL) {
        free(s);
        exit(1);
    }
    // replace the bytes
    for(i = 0; i < c; i++) {
      j = indexCache[i];
      memcpy(temp, pstr, j - start);
      temp += j - start;
      pstr = s + j + oldLen;
      memcpy(temp, new, newLen);
      temp += newLen;
      start = j + oldLen;
    }
    memcpy(temp, pstr, filelen - j);

    // write the result to a temp file
    out = fopen(filename_stage, "wb");
    fwrite(t, 1, newFilelen, out);

    // rename temp file to the original file & copy permissions
    rename(filename_stage, filename);
    chmod(filename, fileStat.st_mode);

    fclose(out);
    free(s);
  }

  return 0;
}
