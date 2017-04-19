#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <vector>
using namespace std;

#define REHASH(a, b, h) ((((h) - (a)*d) << 1) + (b))

// Rabin-Karp search (modified from:
// http://www-igm.univ-mlv.fr/~lecroq/string/node5.html#SECTION0050)
int indexOf(const char *needle, size_t needleLen, const char *haystack,
            size_t haystackLen) {
  if (needleLen == 0) {
    return 0;
  } else if (needleLen > haystackLen) {
    return -1;
  }

  int d, hx, hy, i, j;
  /* Preprocessing */
  /* computes d = 2^(m-1) with
    the left-shift operator */
  for (d = i = 1; i < needleLen; ++i)
    d = (d << 1);

  for (hy = hx = i = 0; i < needleLen; ++i) {
    hx = ((hx << 1) + needle[i]);
    hy = ((hy << 1) + haystack[i]);
  }

  /* Searching */
  j = 0;
  while (j <= haystackLen - needleLen) {
    if (hx == hy && memcmp(needle, haystack + j, needleLen) == 0) {
      return j;
    }
    hy = REHASH(haystack[j], haystack[j + needleLen], hy);
    ++j;
  }

  return -1;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    cout << "usage: fastreplacestring <filename> <term> <replacement>\n";
    return 1;
  }

  string filename = argv[argc - 3];
  const char *old = argv[argc - 2];
  const char *newWord = argv[argc - 1];
  FILE *in = fopen(filename.c_str(), "rb");

  // Check if file exists and can is read-write
  // This is actually a shortcut because fopen might fail for a number of
  // reasons
  if (in == NULL) {
    cout << filename + " doesn't exist\n";
    return 1;
  }

  struct stat fileStat;
  if (stat(filename.c_str(), &fileStat) < 0) {
    cout << "Fatal error: Couldn't access " + filename;
    return 1;
  }

  char *s = NULL;
  vector<int> indexCache;
  size_t r, newFilelen;

  // concat filename with ".rewrite" (temp file name)
  string filename_stage = argv[1];
  filename_stage.append(".rewrite");

  // read filelen given by fileState
  // alternatively this could be determined with fseek && ftell
  size_t filelen = fileStat.st_size;

  if ((s = (char *)malloc(filelen)) == NULL) {
    printf("malloc s filelen problem \n");
    exit(1);
  }

  // Read in as much of specified file as possible
  // If there isn't anything to read, finish succesfully :)
  if ((r = fread(s, 1, filelen, in)) == 0) {
    free(s);
    fclose(in);
    return 0;
  }

  fclose(in);

  char *t = NULL;
  char *temp = NULL;

  size_t oldLen = strlen(old);
  size_t newLen = strlen(newWord);

  /* Find all matches and cache their positions. */
  const char *test = NULL;
  test = s;
  int j, start = 0, c = 0;
  int index;

  while ((index = indexOf(old, oldLen, test + start, filelen - start)) != -1) {
    c++;
    j = start;
    j += index;
    indexCache.push_back(j);
    start = j + oldLen;
  }

  if (c == 0) {
    free(s);
    return 0;
  } else {
    const char *pstr = s;
    // calculate new file len & allocate memory
    newFilelen = filelen + c * (newLen - oldLen);
    t = (char *)malloc(newFilelen);

    int i = 0;
    j = 0;
    start = 0;
    temp = t;

    if (temp == NULL) {
      free(s);
      exit(1);
    }
    // replace the bytes
    for (i = 0; i < c; i++) {
      j = indexCache[i];
      memcpy(temp, pstr, j - start);
      temp += j - start;
      pstr = s + j + oldLen;
      memcpy(temp, newWord, newLen);
      temp += newLen;
      start = j + oldLen;
    }
    memcpy(temp, pstr, filelen - j);

    // write the result to a temp file
    FILE *out = fopen(filename_stage.c_str(), "wb");
    fwrite(t, 1, newFilelen, out);

    // rename temp file to the original file & copy permissions
    rename(filename_stage.c_str(), filename.c_str());
    chmod(filename.c_str(), fileStat.st_mode);

    fclose(out);
    free(s);
  }

  return 0;
}
