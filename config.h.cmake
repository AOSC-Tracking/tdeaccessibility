#define VERSION "@VERSION@"

// Defined if you have fvisibility and fvisibility-inlines-hidden support.
#cmakedefine __TDE_HAVE_GCC_VISIBILITY 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#cmakedefine WORDS_BIGENDIAN @WORDS_BIGENDIAN@

/* Defined if you have both <sys/time.h> and <time.h> headers */
#cmakedefine TIME_WITH_SYS_TIME 1

/* Defined if you have either <sys/time.h> or <time.h> headers */
#cmakedefine HAVE_SYS_TIME_H 1
