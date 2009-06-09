
/*
 * Longest common sequence and minimal distance of two strings.
 *
 * By Philippe Queinnec <queinnec@dgac.fr> 19-Jun-93
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * Return the length of the longest common sequence of x and y.
 *
 * The longest common sequence is computed recursively as follow:
 *   - lcs (\empty, y) == lcs (x, \empty) == \empty
 *   - if x = a x',
 *       if a \notin y then
 *          lcs (x, y) = lcs (x', y)   [a is useless]
 *       else
 *          k = maxallsuffix (a, x', y)
 *          lcs (x, y) = longest (lcs (x', y), a . k).
 *       fi
 *     fi
 *
 *  maxallsuffix (a, x', y) returns the longest common sequence of x' and y"
 *  where y = y'.a.y".
 *  As we want to maximize this sequence, it's sufficient to maximize y", and
 *  so we look only for the first occurence of a in y.
 *
 * The non-recursive version is obtained by unrolling the last recursivity,
 * and by using my own stack save{x,y,res} for the first recursivity.
 * This is definitely faster (gain between 30% and 50%).
 *
 */
/*
 * The max size of our stack for the iterative version.
 * Due to the construction, this means that the stack can overflows
 *  only if:
 *      |y| is greater than 2 * MAXSAVE (plus or minus 1)
 *     or
 *      |x| is greater than MAXSAVE (plus or minus 1)
 *  AND
 *      x and y have rather special properties.
 * (for instance if x = "abcdefghij" and y = "0a0b0c0d0e0f0g0h0i0j", a stack
 * of at least 10 (= |x|) is necessary).
 */
#define MAXSAVE 100

#ifdef RECURSIVE
static int lcs (char *x, char *y)
{
    char *first;                /* first occurence of *x in y */
    int res, res2;
    if ((x == NULL) || (y == NULL) || (*x == 0) || (*y == 0))
      return (0);
    first = strchr (y, *x);
    res = lcs (x + 1, y);      /* is the first letter of x useless? */
    if ((first != NULL) && (strlen (first) > res)) {
        /* well, maybe another solution, starting with *x. */
        res2 = lcs (x + 1, first + 1) + 1;
        if (res2 > res)
          res = res2;
    }
    return (res);
}

#else  /* !RECURSIVE */

static int lcs (char *x, char *y)
{
    /* The static's here are just to make things a little faster on some
     * computers. This is not always true, but I don't know of any computer
     * where this is a bad thing. Moreover, this helps people with small
     * stacks. */
    static char *savex[MAXSAVE];
    static char *savey[MAXSAVE];
    static int saveres[MAXSAVE];
    char **savexp = savex;
    char **saveyp = savey;
    int   *saveresp = saveres;
    int bestres = 0;
    int res;
    char *newy;
    *savexp++ = x;
    *saveyp++ = y;
    *saveresp++ = 0;
    while (savexp != savex) {
        x = *--savexp;
        y = *--saveyp;
        res = *--saveresp;
        while ((*x) && y && (*y)) {
            while ((*x) && (*y) && (*x == *y)) { x++; y++; res++; }
            if ((! *x) || (! *y))
              continue;
            newy = strchr (y, *x);
            if (newy != NULL) {
                *savexp++ = x;
                *saveyp++ = newy;
                *saveresp++ = res;
            }
            x++;
        }
        if (res > bestres)
          bestres = res;
    }
    return (bestres);
}
#endif /* !RECURSIVE */

/*
 * The same version with case-insentivity.
 */

static char *strichr (char *s, int c)
{
    int cc = isupper (c) ? tolower (c) : c ;
    int cs;
    while (*s != 0) {
        cs = isupper (*s) ? tolower (*s) : *s ;
        if (cs == cc)
          break;
        s++;
    }
    if (*s == 0)
      return (NULL);
    else
      return (s);
}

#ifdef RECURSIVE

static int lcsi (char *x, char *y)
{
    char *first;                /* first occurence of *x in y */
    int res, res2;
    if ((x == NULL) || (y == NULL) || (*x == 0) || (*y == 0))
      return (0);
    first = strichr (y, *x);
    res = lcsi (x + 1, y);      /* is the first letter of x useless? */
    if ((first != NULL) && (strlen (first) > res)) {
        /* well, may be another solution, starting with *x. */
        res2 = lcsi (x + 1, first + 1) + 1;
        if (res2 > res)
          res = res2;
    }
    return (res);
}

#else  /* !RECURSIVE */

static int lcsi (char *x, char *y)
{
    static char *savex[MAXSAVE];
    static char *savey[MAXSAVE];
    static int saveres[MAXSAVE];
    char **savexp = savex;
    char **saveyp = savey;
    int   *saveresp = saveres;
    int bestres = 0;
    int res;
    char *newy;
    *savexp++ = x;
    *saveyp++ = y;
    *saveresp++ = 0;
    while (savexp != savex) {
        x = *--savexp;
        y = *--saveyp;
        res = *--saveresp;
        while ((*x) && y && (*y)) {
            while ((*x) && (*y)) {
                int c1, c2;
                c1 = isupper (*x) ? tolower (*x) : *x;
                c2 = isupper (*y) ? tolower (*y) : *y;
                if (c1 != c2)
                  break;
                x++; y++; res++;
            }
            if ((! *x) || (! *y))
              continue;
            newy = strichr (y, *x);
            if (newy != NULL) {
                *savexp++ = x;
                *saveyp++ = newy;
                *saveresp++ = res;
            }
            x++;
        }
        if (res > bestres)
          bestres = res;
    }
    return (bestres);
}

#endif /* !RECURSIVE */

/*
 * This minimal distance of x and y is defined as:
 *  |x| + |y| - 2 * lcs(x,y).
 * This is the number of insertions and deletions needed to go from x to y.
 * Note that a replacement is counted as 2: a deletion and a insertion.
 * 
 */
int minimal_distance (char *x, char *y, int case_sens)
{
    if (case_sens)
      return (strlen (x) + strlen (y) - 2 * lcs (x, y));
    else
      return (strlen (x) + strlen (y) - 2 * lcsi (x, y));
}
