/* Converted to FLT using flt.php v1.0 on 2024-11-20T04:29:20+00:00 */
typedef unsigned char _Bool;
typedef signed char int8_t;
typedef int int16_t;
typedef long int32_t;
typedef unsigned char uint8_t;
typedef unsigned uint16_t;
typedef unsigned long uint32_t;
typedef signed char int_least8_t;
typedef int int_least16_t;
typedef long int_least32_t;
typedef unsigned char uint_least8_t;
typedef unsigned uint_least16_t;
typedef unsigned long uint_least32_t;
typedef signed char int_fast8_t;
typedef int int_fast16_t;
typedef long int_fast32_t;
typedef unsigned char uint_fast8_t;
typedef unsigned uint_fast16_t;
typedef unsigned long uint_fast32_t;
typedef int intptr_t;
typedef unsigned uintptr_t;
typedef long intmax_t;
typedef unsigned long uintmax_t;
typedef uint32_t FLT;
typedef enum {E_INFINITE = 1, E_NAN, E_NORMAL, E_SUBNORMAL, E_ZERO} E_CLASS;
typedef enum {E_EQUAL_TO = 1, E_LESS_THAN, E_GREATER_THAN, E_LESS_THAN_OR_EQUAL_TO, E_GREATER_THAN_OR_EQUAL_TO, E_NOT_EQUAL_TO} E_COMPARE;
extern int g_flt_last_scanf_result;
FLT flt_ldexp(const FLT f, const int exponent);
FLT flt_frexp(const FLT f, int *exponent);
FLT flt_modf(const FLT f, FLT *pg);
FLT flt_fmod(const FLT f, const FLT g);
FLT flt_fabs(const FLT f);
FLT flt_round(const FLT f);
FLT flt_trunc(const FLT f);
FLT flt_ceil(const FLT f);
FLT flt_floor(const FLT f);
FLT flt_sqrt(const FLT f);
FLT flt_log(const FLT f);
FLT flt_log10(const FLT f);
FLT flt_exp(const FLT f);
FLT flt_exp10(const FLT f);
FLT flt_pow(const FLT f, FLT g);
FLT flt_sin(const FLT f);
FLT flt_cos(const FLT f);
FLT flt_tan(const FLT f);
FLT flt_asin(const FLT f);
FLT flt_acos(const FLT f);
FLT flt_atan(const FLT f);
FLT flt_atan2(const FLT y, const FLT x);
FLT flt_sinh(const FLT f);
FLT flt_cosh(const FLT f);
FLT flt_tanh(const FLT f);
FLT flt_asinh(const FLT f);
FLT flt_acosh(const FLT f);
FLT flt_atanh(const FLT f);
E_CLASS flt_classify(const FLT f);
_Bool flt_isinf(const FLT f);
_Bool flt_isnan(const FLT f);
_Bool flt_isnormal(const FLT f);
_Bool flt_issubnormal(const FLT f);
_Bool flt_iszero(const FLT f);
_Bool flt_isfinite(const FLT f);
_Bool flt_compare(const FLT f, const FLT g, E_COMPARE compare);
FLT flt_add(const FLT f, const FLT g);
FLT flt_subtract(const FLT f, const FLT g);
FLT flt_multiply(const FLT f, const FLT g);
FLT flt_divide(const FLT f, const FLT g);
FLT flt_negated(const FLT f);
FLT flt_inverted(const FLT f);
FLT flt_add_into(FLT *pf, const FLT g);
FLT flt_subtract_into(FLT *pf, const FLT g);
FLT flt_multiply_into(FLT *pf, const FLT g);
FLT flt_divide_into(FLT *pf, const FLT g);
FLT flt_pre_increment(FLT *pf);
FLT flt_post_increment(FLT *pf);
FLT flt_pre_decrement(FLT *pf);
FLT flt_post_decrement(FLT *pf);
FLT flt_atof(const char *string);
const char *flt_ftoa(const FLT f, const char *format);
char *flt_get_scanf_buffer(int offset);
FLT flt_ltof(const int32_t a);
FLT flt_ultof(const uint32_t a);
int32_t flt_ftol(const FLT f);
uint32_t flt_ftoul(const FLT f);
FLT flt_fmin(const FLT f, const FLT g);
FLT flt_fmax(const FLT f, const FLT g);
int flt_fsgn(const FLT f);
FLT flt_hypot(const FLT f, const FLT g);
FLT flt_log2(const FLT f);
FLT flt_exp2(const FLT f);
typedef unsigned size_t;
typedef struct {
    int rem;
    int quot;
} div_t;
typedef struct {
    long rem;
    long quot;
} ldiv_t;
void* __fastcall__ malloc (size_t size);
void* __fastcall__ calloc (size_t count, size_t size);
void* __fastcall__ realloc (void* block, size_t size);
void __fastcall__ free (void* block);
int __fastcall__ posix_memalign (void** memptr, size_t alignment, size_t size);
void __fastcall__ __heapadd (void* mem, size_t size);
size_t __fastcall__ __heapblocksize (const void* block);
size_t __heapmemavail (void);
size_t __heapmaxavail (void);
int rand (void);
void __fastcall__ srand (unsigned seed);
void __randomize (void);
void abort (void) __attribute__ ((noreturn));
int __fastcall__ abs (int val);
long __fastcall__ labs (long val);
int __fastcall__ atoi (const char* s);
long __fastcall__ atol (const char* s);
int __fastcall__ atexit (void (*exitfunc) (void));
void* __fastcall__ bsearch (const void* key, const void* base, size_t n,
                            size_t size, int __fastcall__ (* cmp) (const void*, const void*));
div_t __fastcall__ div (int numer, int denom);
void __fastcall__ exit (int ret) __attribute__ ((noreturn));
char* __fastcall__ getenv (const char* name);
void __fastcall__ qsort (void* base, size_t count, size_t size,
                         int __fastcall__ (* compare) (const void*, const void*));
long __fastcall__ strtol (const char* nptr, char** endptr, int base);
unsigned long __fastcall__ strtoul (const char* nptr, char** endptr, int base);
int __fastcall__ system (const char* s);
void __fastcall__ __swap (void* p, void* q, size_t size);
char* __fastcall__ itoa (int val, char* buf, int radix);
char* __fastcall__ utoa (unsigned val, char* buf, int radix);
char* __fastcall__ ltoa (long val, char* buf, int radix);
char* __fastcall__ ultoa (unsigned long val, char* buf, int radix);
int __fastcall__ putenv (char* s);
typedef unsigned char* __va_list;
typedef struct _FILE FILE;
typedef unsigned long fpos_t;
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;
void __fastcall__ clearerr (FILE* f);
int __fastcall__ fclose (FILE* f);
int __fastcall__ feof (FILE* f);
int __fastcall__ ferror (FILE* f);
int __fastcall__ fflush (FILE* f);
int __fastcall__ fgetc (FILE* f);
char* __fastcall__ fgets (char* buf, size_t size, FILE* f);
FILE* __fastcall__ fopen (const char* name, const char* mode);
int fprintf (FILE* f, const char* format, ...);
int __fastcall__ fputc (int c, FILE* f);
int __fastcall__ fputs (const char* s, FILE* f);
size_t __fastcall__ fread (void* buf, size_t size, size_t count, FILE* f);
FILE* __fastcall__ freopen (const char* name, const char* mode, FILE* f);
size_t __fastcall__ fwrite (const void* buf, size_t size, size_t count, FILE* f);
int __fastcall__ fgetpos (FILE* f, fpos_t *pos);
int __fastcall__ fsetpos (FILE* f, const fpos_t* pos);
long __fastcall__ ftell (FILE* f);
int __fastcall__ fseek (FILE* f, long offset, int whence);
void __fastcall__ rewind (FILE *f);
int getchar (void);
char* __fastcall__ gets (char* s);
void __fastcall__ perror (const char* s);
int printf (const char* format, ...);
int __fastcall__ putchar (int c);
int __fastcall__ puts (const char* s);
int __fastcall__ remove (const char* name);
int __fastcall__ rename (const char* oldname, const char* newname);
int snprintf (char* buf, size_t size, const char* format, ...);
int sprintf (char* buf, const char* format, ...);
int __fastcall__ ungetc (int c, FILE* f);
int __fastcall__ vfprintf (FILE* f, const char* format, __va_list ap);
int __fastcall__ vprintf (const char* format, __va_list ap);
int __fastcall__ vsnprintf (char* buf, size_t size, const char* format, __va_list ap);
int __fastcall__ vsprintf (char* buf, const char* format, __va_list ap);
int scanf (const char* format, ...);
int fscanf (FILE* f, const char* format, ...);
int sscanf (const char* s, const char* format, ...);
int __fastcall__ vscanf (const char* format, __va_list ap);
int __fastcall__ vsscanf (const char* s, const char* format, __va_list ap);
int __fastcall__ vfscanf (FILE* f, const char* format, __va_list ap);
FILE* __fastcall__ fdopen (int fd, const char* mode);
int __fastcall__ fileno (FILE* f);
void __fastcall__ __poserror (const char* msg);
char* __fastcall__ strcat (char* dest, const char* src);
char* __fastcall__ strchr (const char* s, int c);
int __fastcall__ strcmp (const char* s1, const char* s2);
int __fastcall__ strcoll (const char* s1, const char* s2);
char* __fastcall__ strcpy (char* dest, const char* src);
size_t __fastcall__ strcspn (const char* s1, const char* s2);
char* __fastcall__ strerror (int errcode);
size_t __fastcall__ strlen (const char* s);
char* __fastcall__ strncat (char* s1, const char* s2, size_t count);
int __fastcall__ strncmp (const char* s1, const char* s2, size_t count);
char* __fastcall__ strncpy (char* dest, const char* src, size_t count);
char* __fastcall__ strpbrk (const char* str, const char* set);
char* __fastcall__ strrchr (const char* s, int c);
size_t __fastcall__ strspn (const char* s1, const char* s2);
char* __fastcall__ strstr (const char* str, const char* substr);
char* __fastcall__ strtok (char* s1, const char* s2);
size_t __fastcall__ strxfrm (char* s1, const char* s2, size_t count);
void* __fastcall__ memchr (const void* mem, int c, size_t count);
int __fastcall__ memcmp (const void* p1, const void* p2, size_t count);
void* __fastcall__ memcpy (void* dest, const void* src, size_t count);
void* __fastcall__ memmove (void* dest, const void* src, size_t count);
void* __fastcall__ memset (void* s, int c, size_t count);
void* __fastcall__ __bzero (void* ptr, size_t n);
void __fastcall__ bzero (void* ptr, size_t n);
char* __fastcall__ strdup (const char* s);
int __fastcall__ stricmp (const char* s1, const char* s2);
int __fastcall__ strcasecmp (const char* s1, const char* s2);
char* __fastcall__ strcasestr (const char* str, const char* substr);
int __fastcall__ strnicmp (const char* s1, const char* s2, size_t count);
int __fastcall__ strncasecmp (const char* s1, const char* s2, size_t count);
size_t __fastcall__ strnlen (const char* s, size_t maxlen);
char* __fastcall__ strlwr (char* s);
char* __fastcall__ strlower (char* s);
char* __fastcall__ strupr (char* s);
char* __fastcall__ strupper (char* s);
char* __fastcall__ strqtok (char* s1, const char* s2);
char* __fastcall__ stpcpy (char* dest, const char* src);
const char* __fastcall__ __stroserror (unsigned char errcode);
typedef struct {int16_t x, y, z;} VERTEX;
typedef struct {int16_t x, y;} POINT;
typedef struct {int16_t index_from, index_to;} SEGMENT;
typedef struct {int8_t x, y;} OFFSET;
enum {ERR_FO, ERR_FC, ERR_XM, ERR_GI, ERR_OB, ERR_DC, ERR_NO};
enum {CODE_ID, CODE_EM, CODE_TD, CODE_AD, CODE_VD, CODE_LD, CODE_EF};
enum {CLR16_BLACK, CLR16_WHITE, CLR16_RED, CLR16_CYAN,
 CLR16_MAGENTA, CLR16_GREEN, CLR16_BLUE, CLR16_YELLOW,
 CLR16_ORANGE, CLR16_BROWN, CLR16_LIGHTRED, CLR16_DARKGRAY,
 CLR16_MEDIUMGRAY, CLR16_LIGHTGREEN, CLR16_LIGHTBLUE, CLR16_LIGHTGRAY,
 CLR16_COUNT};
void OutputAsNumber(char prefix, int16_t value);
void ExitProgram(int16_t status);
const char *strings0[] = {
            "\nThe data file has been erased or\nrenamed. Recreate or restore from\nbackup.\n",
            "\nThe data file has been corrupted.\nRecreate or restore from backup.\n",
            "\nA banked memory request larger than 8K\nwas requested.\n",
            "\nThere is a problem involving graphics\ninitialization.\n",
            "\nThere was an attempt to access an array\nout of bounds.\n",
            "\nThe display configuration has been\ncorrupted.\n"
};
const char *arena_vertices0 = "!#$%&*+";
const char *arenas0[] = {
 "+--------------+"
 "|              |"
 "|              |"
 "|              |"
 "|   #------#   |"
 "|   |      |   |"
 "|   #      #   |"
 "|              |"
 "|              |"
 "|   #      #   |"
 "|   |      |   |"
 "|   #------#   |"
 "|              |"
 "|              |"
 "|              |"
 "+--------------+",
 "+--------------+"
 "|              |"
 "|              |"
 "|              |"
 "|  #        #  |"
 "|  |  %  %  |  |"
 "|  |  |  |  |  |"
 "|  |  |  |  |  |"
 "|  |  |  |  |  |"
 "|  |  |  |  |  |"
 "|  |  %  %  |  |"
 "|  #        #  |"
 "|              |"
 "|              |"
 "|              |"
 "+--------------+",
 "+--------------+"
 "|              |"
 "|              |"
 "|  *        *  |"
 "|  |        |  |"
 "|  |        |  |"
 "|  *--------*  |"
 "|              |"
 "|              |"
 "|  *--------*  |"
 "|  |        |  |"
 "|  |        |  |"
 "|  *        *  |"
 "|              |"
 "|              |"
 "+--------------+",
 "+--------------+"
 "|              |"
 "|              |"
 "|   &------&   |"
 "|              |"
 "|              |"
 "|    $----$    |"
 "|              |"
 "|              |"
 "|    $----$    |"
 "|              |"
 "|              |"
 "|   &------&   |"
 "|              |"
 "|              |"
 "+--------------+",
 "+--------------+"
 "|              |"
 "|              |"
 "|     $        |"
 "|     |        |"
 "|     |        |"
 "|     |        |"
 "|  $--$        |"
 "|        $--$  |"
 "|        |     |"
 "|        |     |"
 "|        |     |"
 "|        $     |"
 "|              |"
 "|              |"
 "+--------------+",
 "+--------------+"
 "|              |"
 "|              |"
 "|              |"
 "|      **      |"
 "|      **      |"
 "|              |"
 "|              |"
 "|              |"
 "|              |"
 "|      **      |"
 "|      **      |"
 "|              |"
 "|              |"
 "|              |"
 "+--------------+"
};
const char *vehicle_vertices0 = "@";
const char *vehicles0[] = {
 "            @            "
 "           + +           "
 "          +   +          "
 "          +   +          "
 "         +     +         "
 "        +       +        "
 "       +         +       "
 "      +           +      "
 "      +           +      "
 "     +             +     "
 "    +               +    "
 "   +                 +   "
 "  +                   +  "
 "  +                   +  "
 " +                     + "
 "@                       @"
 " +                     + "
 "  +                   +  "
 "  +                   +  "
 "   +                 +   "
 "    +               +    "
 "     +             +     "
 "     +             +     "
 "      +           +      "
 "       @+++++++++@       ",
 "        @+++++++@        "
 "        +       +        "
 "       +         +       "
 "       +         +       "
 "       +         +       "
 "      +           +      "
 "      +           +      "
 "      +           +      "
 "     +             +     "
 "     +             +     "
 "     +             +     "
 "    +               +    "
 "    +               +    "
 "    +               +    "
 "   +                 +   "
 "   +                 +   "
 "   +                 +   "
 "  +         @         +  "
 "  +        +++        +  "
 "  +      ++   ++      +  "
 " +     ++       ++     + "
 " +   ++           ++   + "
 " + ++               ++ + "
 "+++                   +++"
 "@                       @",
 "           +@+           "
 "          +   +          "
 "          +   +          "
 "         +     +         "
 "        +       +        "
 "        +       +        "
 "       +         +       "
 "      +           +      "
 "      +           +      "
 "     +             +     "
 "     +             +     "
 "    +               +    "
 "   +                 +   "
 "   +                 +   "
 "  +                   +  "
 " +                     + "
 " +                     + "
 "@+++++++++++++++++++++++@"
 " +                     + "
 "  ++                 ++  "
 "    ++             ++    "
 "      ++         ++      "
 "        +       +        "
 "         ++   ++         "
 "           +@+           ",
 "           +@+           "
 "           +++           "
 "          + + +          "
 "          + + +          "
 "         +  +  +         "
 "         +  +  +         "
 "        +   +   +        "
 "        +   +   +        "
 "       +    +    +       "
 "       +    +    +       "
 "      +     +     +      "
 "      +     +     +      "
 "      +     +     +      "
 "     +      +      +     "
 "     +      +      +     "
 "    +       +       +    "
 "    +       +       +    "
 "   +        +        +   "
 "   +        +        +   "
 "  +        +@+        +  "
 "  +      ++   ++      +  "
 " +     ++       ++     + "
 " +   ++           ++   + "
 "+  ++               ++  +"
 "@++                   ++@",
 "      @           @      "
 "      +           +      "
 "     +             +     "
 "     +             +     "
 "    +               +    "
 "    +               +    "
 "    +       @       +    "
 "   +       +++       +   "
 "   +      ++ ++      +   "
 "  +      ++   ++      +  "
 "  +     +       +     +  "
 "  +    +         +    +  "
 " +   ++           ++   + "
 " +  ++             ++  + "
 "+  +                 +  +"
 "+++                   +++"
 "@                       @"
 " ++                   ++ "
 "   +                 +   "
 "    ++             ++    "
 "      +           +      "
 "       ++       ++       "
 "         +     +         "
 "          ++ ++          "
 "            @            ",
 "            @            "
 "            +            "
 "           + +           "
 "           + +           "
 "           + +           "
 "          +   +          "
 "          +   +          "
 "          +   +          "
 "@        +     +        @"
 "+        +     +        +"
 "+        +     +        +"
 " +      +       +      + "
 " +      +       +      + "
 " +     +         +     + "
 " +     +         +     + "
 "  +    +         +    +  "
 "  +   +           +   +  "
 "  +   +           +   +  "
 "  +   +     @     +   +  "
 "   + +     + +     + +   "
 "   + +    +   +    + +   "
 "   + +  ++     ++  + +   "
 "   ++  +         +  ++   "
 "    + +           + +    "
 "    @+             +@    ",
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "            @            "
 "            +            "
 "            +            "
 "            +            "
 "            +            "
 "            +            "
 "            @            "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         ",
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "            @            "
 "           + +           "
 "           + +           "
 "          +   +          "
 "          +   +          "
 "         @     @         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         ",
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "           +@+           "
 "          ++ ++          "
 "         ++   ++         "
 "         @     @         "
 "         ++   ++         "
 "          ++ ++          "
 "           +@+           "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         ",
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "            @            "
 "            +            "
 "            +            "
 "            +            "
 "        @+++++++@        "
 "            +            "
 "            +            "
 "            +            "
 "            @            "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         ",
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "         @     @         "
 "          +   +          "
 "           + +           "
 "            +            "
 "           + +           "
 "          +   +          "
 "         @     @         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
 "                         "
};
const int8_t levels0[] = {
 0, 0, 1, 1, 6, 0, 3,
 1, 1, 1, 2, 6, 0, 3,
 2, 2, 1, 1, 7, 1, 5,
 3, 3, 1, 2, 7, 1, 5,
 4, 4, 1, 1, 8, 2, 8,
 5, 5, 1, 2, 8, 2, 8
};
static void OutputStrings(FILE *ofile);
static void OutputTrigData(FILE *ofile);
static void OutputArenaData(FILE *ofile);
static void OutputVehicleData(FILE *ofile);
static void OutputLevelData(FILE *ofile);
static void OutputArena16x16(const char *arena, FILE *ofile);
static VERTEX *OutputArenaVertices(const char *arena, int16_t limit, FILE *ofile);
static void OutputArenaSegments(const char *arena, VERTEX *vertices, int16_t limit, FILE *ofile);
static int16_t CountArenaVertices(const char *arena);
static int16_t CountArenaSegments(const char *arena);
static char ArenaCharacterAt(const char *arena, int16_t x, int16_t y);
static int16_t IndexOfArenaVertex(VERTEX *vertices, VERTEX *vertex, int16_t count);
static int CompareArenaVertices(const void *p, const void *q);
static OFFSET *GetVehicleVertices(const char *vehicle, int16_t limit);
static int16_t CountVehicleVertices(const char *vehicle);
static void OutputVehicleSegments(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile);
static void OutputVehicleVertices(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile);
static _Bool VehicleVerticesAreConnected(const char *vehicle, OFFSET *v1, OFFSET *v2);
static char VehicleCharacterAt(const char *vehicle, int16_t x, int16_t y);
static int CompareVehicleVertices(const void *p, const void *q);
int main(void)
{
 FILE *ofile;
 if (!(ofile = fopen("explore.dat", "wb"))) {
  fputs("\nCould not create EXPLORE.DAT!\n", stderr);
  return 1;
 }
 OutputStrings(ofile);
 OutputTrigData(ofile);
 OutputArenaData(ofile);
 OutputVehicleData(ofile);
 OutputLevelData(ofile);
 fputc(CODE_EF, ofile);
 fclose(ofile);
 return 0;
}
static void OutputStrings(FILE *ofile)
{
 const char **strings = strings0;
 int16_t t, u, length, max, count = sizeof(strings0) / sizeof(char *);
 fputs("\nError Messages ", stdout);
 fputc(CODE_EM, ofile);
 for (t = 0, max = 0; t < count; ++t) {
  length = strlen(strings[t]);
  if (max < length) max = length;
 }
 ++max;
 fwrite (&max, sizeof(int16_t), 1, ofile);
 for (t = 0; t < count; ++t) {
  length = strlen(strings[t]);
  fputs(strings[t], ofile);
  for (u = length; u < max; ++u)
   fputc('\0', ofile);
 }
 fputs("done\n", stdout);
}
static void OutputTrigData(FILE *ofile)
{
 int16_t t, tmp;
 FLT d, i;
 fputs("Trig Data ", stdout);
 fputc(CODE_TD, ofile);
 d = 0x00000000 /* 0.0 */;
 i = flt_divide(flt_multiply(0x40000000 /* 2.0 */ , 0x40490FDB /* 3.141592653589793 */) , 0x44000000 /* 512 */);
 for (t = 0; t < 512; ++t, flt_add_into(&d , i)) {
  tmp = flt_ftol((flt_multiply(flt_sin(d) , 0x45800000 /* 4096 */)));
  fwrite(&tmp, sizeof(int16_t), 1, ofile);
  if (t % 32 == 31) fputc('*', stdout);
 }
 fputs(" done\n", stdout);
}
static void OutputArenaData(FILE *ofile)
{
 int16_t t, tmp, vertices = 0, segments = 0;
 VERTEX *V;
 fputs("Arena Data ", stdout);
 fputc(CODE_AD, ofile);
 tmp = sizeof(arenas0) / sizeof(char *);
 fwrite(&tmp, sizeof(int16_t), 1, ofile);
 for (t = 0; t < sizeof(arenas0) / sizeof(char *); ++t) {
  tmp = CountArenaVertices(arenas0[t]);
  if (vertices < tmp) vertices = tmp;
  tmp = CountArenaSegments(arenas0[t]);
  if (segments < tmp) segments = tmp;
 }
 tmp = 16 * 16 * sizeof(int16_t) + (vertices + 1) * sizeof(VERTEX) + (segments + 1) * sizeof(SEGMENT);
 fwrite(&tmp, sizeof(int16_t), 1, ofile);
 fwrite(&vertices, sizeof(int16_t), 1, ofile);
 fwrite(&segments, sizeof(int16_t), 1, ofile);
 fputc('*', stdout);
 for (t = 0; t < sizeof(arenas0) / sizeof(char *); ++t) {
  OutputArena16x16(arenas0[t], ofile);
  V = OutputArenaVertices(arenas0[t], vertices + 1, ofile);
  OutputArenaSegments(arenas0[t], V, segments + 1, ofile);
  fputc('*', stdout);
 }
 fputs(" done\n", stdout);
}
static void OutputArena16x16(const char *arena, FILE *ofile)
{
 static int16_t *s_heights = ((void *) 0);
 char focus;
 int16_t x, y, xt, yt, height;
 if (!s_heights)
  s_heights = malloc(sizeof(int16_t) * 16 * 16);
 for (y = 0; y < 16; ++y) {
  for (x = 0; x < 16; ++x) {
   focus = ArenaCharacterAt(arena, x, y);
   if (strchr(arena_vertices0, focus)) {
    height = flt_ftol((flt_multiply(0x46800000 /* 16384.0 */ , (flt_subtract(flt_multiply(0x3D99999A /* 0.075 */ , flt_ltof((focus))) , 0x400E6666 /* 2.225 */)))));
    s_heights[y * 16 + x] = height;
    for (xt = x + 1; ArenaCharacterAt(arena, xt, y) == '-'; ++xt)
     s_heights[y * 16 + xt] = height;
    for (yt = y + 1; ArenaCharacterAt(arena, x, yt) == '|'; ++yt)
     s_heights[yt * 16 + x] = height;
   } else if (focus == ' ')
    s_heights[y * 16 + x] = 0;
  }
 }
 fwrite(s_heights, sizeof(int16_t), 16 * 16, ofile);
}
static VERTEX *OutputArenaVertices(const char *arena, int16_t limit, FILE *ofile)
{
 static VERTEX *s_vertices = ((void *) 0);
 char focus;
 int16_t i, x, y;
 if (!s_vertices)
  s_vertices = malloc(sizeof(VERTEX) * limit);
 for (y = 0, i = 0; y < 16; ++y) {
  for (x = 0; x < 16; ++x) {
   focus = ArenaCharacterAt(arena, x, y);
   if (strchr(arena_vertices0, focus)) {
    s_vertices[i].x = flt_ftol((flt_multiply((flt_add(flt_ltof((x)) , 0x3F000000 /* 0.5 */)) , 0x44800000 /* 1024.0 */)));
    s_vertices[i].y = flt_ftol((flt_multiply((flt_add(flt_ltof((y)) , 0x3F000000 /* 0.5 */)) , 0x44800000 /* 1024.0 */)));
    s_vertices[i].z = flt_ftol((flt_multiply(0x46800000 /* 16384.0 */ , (flt_subtract(flt_multiply(0x3D99999A /* 0.075 */ , flt_ltof((focus))) , 0x400E6666 /* 2.225 */)))));
    ++i;
    s_vertices[i].x = s_vertices[i - 1].x;
    s_vertices[i].y = s_vertices[i - 1].y;
    s_vertices[i].z = 0;
    ++i;
   }
  }
 }
 qsort(s_vertices, i, sizeof(VERTEX), CompareArenaVertices);
 for (; i < limit; ++i)
  s_vertices[i].z = s_vertices[i].y = s_vertices[i].x = -1;
 fwrite(s_vertices, sizeof(VERTEX), limit, ofile);
 return s_vertices;
}
static void OutputArenaSegments(const char *arena, VERTEX *vertices, int16_t limit, FILE *ofile)
{
 static SEGMENT *s_segments = ((void *) 0);
 char focus;
 int16_t i, x, y, xt, yt, count;
 VERTEX v1, v2, v3;
 if (!s_segments)
  s_segments = malloc(sizeof(SEGMENT) * limit);
 count = CountArenaVertices(arena);
 for (y = 0, i = 0; y < 16; ++y) {
  for (x = 0; x < 16; ++x) {
   focus = ArenaCharacterAt(arena, x, y);
   if (strchr(arena_vertices0, focus)) {
    v1.x = v2.x = flt_ftol((flt_multiply((flt_add(flt_ltof((x)) , 0x3F000000 /* 0.5 */)) , 0x44800000 /* 1024.0 */)));
    v1.y = v2.y = flt_ftol((flt_multiply((flt_add(flt_ltof((y)) , 0x3F000000 /* 0.5 */)) , 0x44800000 /* 1024.0 */)));
    v1.z = 0;
    v2.z = flt_ftol((flt_multiply(0x46800000 /* 16384.0 */ , (flt_subtract(flt_multiply(0x3D99999A /* 0.075 */ , flt_ltof((focus))) , 0x400E6666 /* 2.225 */)))));
    s_segments[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
    s_segments[i].index_to = IndexOfArenaVertex(vertices, &v2, count);
    ++i;
    for (xt = x + 1; ArenaCharacterAt(arena, xt, y) == '-'; ++xt) ;
    if (ArenaCharacterAt(arena, xt, y) == focus) {
     v3.x = flt_ftol((flt_multiply((flt_add(flt_ltof((xt)) , 0x3F000000 /* 0.5 */)) , 0x44800000 /* 1024.0 */)));
     v3.y = v2.y;
     v3.z = v2.z;
     s_segments[i].index_from = IndexOfArenaVertex(vertices, &v2, count);
     s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
     ++i;
     v3.z = 0;
     s_segments[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
     s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
     ++i;
    }
    for (yt = y + 1; ArenaCharacterAt(arena, x, yt) == '|'; ++yt) ;
    if (ArenaCharacterAt(arena, x, yt) == focus) {
     v3.x = v2.x;
     v3.y = flt_ftol((flt_multiply((flt_add(flt_ltof((yt)) , 0x3F000000 /* 0.5 */)) , 0x44800000 /* 1024.0 */)));
     v3.z = v2.z;
     s_segments[i].index_from = IndexOfArenaVertex(vertices, &v2, count);
     s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
     ++i;
     v3.z = 0;
     s_segments[i].index_from = IndexOfArenaVertex(vertices, &v1, count);
     s_segments[i].index_to = IndexOfArenaVertex(vertices, &v3, count);
     ++i;
    }
   }
  }
 }
 for (; i < limit; ++i)
  s_segments[i].index_from = s_segments[i].index_to = -1;
 fwrite(s_segments, sizeof(SEGMENT), limit, ofile);
}
static int16_t CountArenaVertices(const char *arena)
{
 int16_t count = 0;
 while (*arena) {
  if (strchr(arena_vertices0, *arena))
   count += 2;
  ++arena;
 }
 return count;
}
static int CountArenaSegments(const char *arena)
{
 char focus, tmp;
 int16_t x, y, count = 0;
 for (y = 0; y < 16; ++y) {
  for (x = 0; x < 16; ++x) {
   focus = ArenaCharacterAt(arena, x, y);
   if (strchr(arena_vertices0, focus)) {
    count += 1;
    tmp = ArenaCharacterAt(arena, x + 1, y);
    if (tmp == '-' || tmp == focus)
     count += 2;
    tmp = ArenaCharacterAt(arena, x, y + 1);
    if (tmp == '|' || tmp == focus)
     count += 2;
   }
  }
 }
 return count;
}
static char ArenaCharacterAt(const char *arena, int16_t x, int16_t y)
{
 char result = ' ';
 if (y >= 0 && y < 16 && x >= 0 && x < 16)
  result = arena[y * 16 + x];
 return result;
}
static int16_t IndexOfArenaVertex(VERTEX *vertices, VERTEX *vertex, int16_t count)
{
 VERTEX *v = bsearch(vertex, vertices, count, sizeof(VERTEX), CompareArenaVertices);
 return v? (int16_t)(v - vertices): -1;
}
static int CompareArenaVertices(const void *p, const void *q)
{
 int16_t result;
 const VERTEX *a = p, *b = q;
 result = a->z - b->z;
 if (!result)
  result = a->x - b->x;
 if (!result)
  result = a->y - b->y;
 return (int)result;
}
static void OutputVehicleData(FILE *ofile)
{
 static const char *s_ordinals[] = {
  "\n  1 ", "\n  2 ", "\n  3 ", "\n  4 ", "\n  5 ", "\n  6 ",
  "\n  7 ", "\n  8 ", "\n  9 ", "\n 10 ", "\n 11 ", "\n 12 "
 };
 int16_t t, tmp, vertices, segments;
 OFFSET *V;
 fputs("Vehicle Data", stdout);
 fputc(CODE_VD, ofile);
 tmp = sizeof(vehicles0) / sizeof(char *);
 fwrite(&tmp, sizeof(int16_t), 1, ofile);
 vertices = 6;
 segments = 6;
 tmp = (segments + 1) * sizeof(SEGMENT) + vertices * sizeof(OFFSET) * 8 * 32;
 fwrite(&tmp, sizeof(int16_t), 1, ofile);
 fwrite(&vertices, sizeof(int16_t), 1, ofile);
 fwrite(&segments, sizeof(int16_t), 1, ofile);
 fputs(" *", stdout);
 for (t = 0; t < sizeof(vehicles0) / sizeof(char *); ++t) {
  fputs(s_ordinals[t], stdout);
  V = GetVehicleVertices(vehicles0[t], vertices + 1);
  OutputVehicleSegments(vehicles0[t], V, segments + 1, ofile);
  OutputVehicleVertices(vehicles0[t], V, vertices, ofile);
 }
 fputs(" done\n", stdout);
}
static int16_t CountVehicleVertices(const char *vehicle)
{
 int16_t count = 0;
 while (*vehicle) {
  if (strchr(vehicle_vertices0, *vehicle))
   ++count;
  ++vehicle;
 }
 return count;
}
static OFFSET *GetVehicleVertices(const char *vehicle, int16_t limit)
{
 static OFFSET *s_vertices = ((void *) 0);
 char focus;
 int16_t i, x, y;
 if (!s_vertices)
  s_vertices = malloc(sizeof(OFFSET) * limit);
 for (y = 0, i = 0; y < 25; ++y) {
  for (x = 0; x < 25; ++x) {
   focus = VehicleCharacterAt(vehicle, x, y);
   if (strchr(vehicle_vertices0, focus)) {
    s_vertices[i].x = x;
    s_vertices[i].y = y;
    ++i;
    if (i == limit) {
     fputs("\nIncrease MAX_VEH_VERTICES!\n", stderr);
     exit(1);
    }
   }
  }
 }
 qsort(s_vertices, i, sizeof(OFFSET), CompareVehicleVertices);
 for (; i < limit; ++i)
  s_vertices[i].y = s_vertices[i].x = -1;
 return s_vertices;
}
static void OutputVehicleSegments(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile)
{
 static SEGMENT *s_segments = ((void *) 0);
 int16_t i, j1, j2, count;
 if (!s_segments)
  s_segments = malloc(sizeof(SEGMENT) * limit);
 count = CountVehicleVertices(vehicle);
 for (i = 0, j1 = 0; j1 < count; ++j1) {
  for (j2 = j1 + 1; j2 < count; ++j2) {
   if (VehicleVerticesAreConnected(vehicle, vertices + j1, vertices + j2)) {
    s_segments[i].index_from = j1;
    s_segments[i].index_to = j2;
    ++i;
    if (i == limit) {
     fputs("\nIncrease MAX_VEH_SEGMENTS!\n", stderr);
     exit(1);
    }
   }
  }
 }
 for (; i < limit; ++i)
  s_segments[i].index_from = s_segments[i].index_to = -1;
 fwrite(s_segments, sizeof(SEGMENT), limit, ofile);
}
static void OutputVehicleVertices(const char *vehicle, OFFSET *vertices, int16_t limit, FILE *ofile)
{
 static OFFSET *s_offsets = ((void *) 0);
 static FLT *s_sn, *s_cs;
 int16_t t, u, height, count;
 FLT d, i, scale, x, y;
 if (!s_offsets) {
  s_offsets = malloc(sizeof(OFFSET) * limit);
  s_sn = malloc(sizeof(FLT) * 32);
  s_cs = malloc(sizeof(FLT) * 32);
  d = 0x00000000 /* 0.0 */;
  i = flt_divide(flt_multiply(0x40000000 /* 2.0 */ , 0x40490FDB /* 3.141592653589793 */) , 0x42000000 /* 32 */);
  for (t = 0; t < 32; ++t, flt_add_into(&d , i)) {
   s_sn[t] = flt_sin(d);
   s_cs[t] = flt_cos(d);
  }
  fputc('*', stdout);
 }
 count = CountVehicleVertices(vehicle);
 i = flt_divide((flt_subtract(0x3F900000 /* 1.1250 */ , 0x3F100000 /* 0.5625 */)) , flt_ltof((8 - 1)));
 for (height = 0, scale = 0x3F900000 /* 1.1250 */; height < 8; ++height, flt_subtract_into(&scale , i)) {
  for (t = 0; t < 32; ++t) {
   for (u = 0; u < count; ++u) {
    x = flt_subtract(flt_ltof(vertices[u].x) , 0x41480000 /* 12.5 */);
    y = flt_subtract(flt_ltof(vertices[u].y) , 0x41480000 /* 12.5 */);
    s_offsets[u].x = flt_ftol(flt_round(flt_divide((flt_subtract(flt_multiply(s_cs[t] , x) , flt_multiply(s_sn[t] , y))) , scale)));
    s_offsets[u].y = flt_ftol(flt_round(flt_divide((flt_add(flt_multiply(s_sn[t] , x) , flt_multiply(s_cs[t] , y))) , scale)));
   }
   for (; u < limit; ++u)
    s_offsets[u].x = s_offsets[u].y = -1;
   fwrite(s_offsets, sizeof(OFFSET), limit, ofile);
  }
  fputc('*', stdout);
 }
}
static _Bool VehicleVerticesAreConnected(const char *vehicle, OFFSET *v1, OFFSET *v2)
{
 FLT x1, y1, x2, y2, x3, y3;
 int16_t x, y, count = 0;
 x2 = flt_divide(flt_ltof((v1->x + v2->x)) , 0x40000000 /* 2.0 */);
 y2 = flt_divide(flt_ltof((v1->y + v2->y)) , 0x40000000 /* 2.0 */);
 for (count = 0, y = -1; y <= 1; ++y) {
  for (x = -1; x <= 1; ++x)
   if (VehicleCharacterAt(vehicle, flt_ftol((flt_add(x2 , flt_ltof(x)))), flt_ftol((flt_add(y2 , flt_ltof(y))))) != ' ')
    ++count;
 }
 if (count < 2)
  return 0;
 x1 = flt_divide((flt_add(flt_ltof(v1->x) , x2)) , 0x40000000 /* 2.0 */);
 y1 = flt_divide((flt_add(flt_ltof(v1->y) , y2)) , 0x40000000 /* 2.0 */);
 for (count = 0, y = -1; y <= 1; ++y) {
  for (x = -1; x <= 1; ++x)
   if (VehicleCharacterAt(vehicle, flt_ftol((flt_add(x1 , flt_ltof(x)))), flt_ftol((flt_add(y1 , flt_ltof(y))))) != ' ')
    ++count;
 }
 if (count < 2)
  return 0;
 x3 = flt_divide((flt_add(x2 , flt_ltof(v2->x))) , 0x40000000 /* 2.0 */);
 y3 = flt_divide((flt_add(y2 , flt_ltof(v2->y))) , 0x40000000 /* 2.0 */);
 for (count = 0, y = -1; y <= 1; ++y) {
  for (x = -1; x <= 1; ++x)
   if (VehicleCharacterAt(vehicle, flt_ftol((flt_add(x3 , flt_ltof(x)))), flt_ftol((flt_add(y3 , flt_ltof(y))))) != ' ')
    ++count;
 }
 if (count < 2)
  return 0;
 return 1;
}
static char VehicleCharacterAt(const char *vehicle, int16_t x, int16_t y)
{
 char result = ' ';
 if (y >= 0 && y < 25 && x >= 0 && x < 25)
  result = vehicle[y * 25 + x];
 return result;
}
static int CompareVehicleVertices(const void *p, const void *q)
{
 int8_t result;
 const OFFSET *a = p, *b = q;
 result = a->x - b->x;
 if (!result)
  result = a->y - b->y;
 return (int)result;
}
static void OutputLevelData(FILE *ofile)
{
 int16_t tmp;
 fputs("Level Data", stdout);
 fputc(CODE_LD, ofile);
 tmp = sizeof(levels0) / sizeof(int8_t) / 7;
 fwrite(&tmp, sizeof(int16_t), 1, ofile);
 fwrite(levels0, sizeof(int8_t), sizeof(levels0) / sizeof(int8_t), ofile);
 fputs(" done\n", stdout);
}