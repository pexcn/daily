#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sysexits.h>

#include "globals.h"
#include "version.h"

#include "../../ip_tree.h"
#include "../../ip_tree_io/builder.h"
#include "../../ip_tree_io/print.h"

#include "../../util/dynarray.h"

/**
 * The actual main function.
 *
 * main() wraps this function and provides handling of global vars
 * and interpretation of exit codes.
 *
 * @param g
 * @param argc
 * @param argv
 *
 * @return
 * */
__attribute__((warn_unused_result))
static int main_inner (
   struct ipdedup_globals* const restrict g, int argc, char** argv
);

static int main_push_infile (
   struct ipdedup_globals* const restrict g,
   struct dynarray* const restrict infiles,
   const char* const arg
);

/**
 * Part of the inner main function, parses input files / stdin
 * and builds the IP trees.
 *
 * @param g
 * @param prog_name
 * @param argc
 * @param argv
 *
 * @return 0 on success, else non-zero (see PARSE_IP_RET status codes)
 * */
__attribute__((warn_unused_result))
static int main_inner_parse_input (
   struct ipdedup_globals* const restrict g
);

__attribute__((warn_unused_result))
static int main_interpret_parse_ret ( const int parse_ret );


__attribute__((warn_unused_result))
static int main_parse_dedup_to_tree (
   struct ipdedup_globals* const restrict g,
   struct dynarray* const restrict infiles,
   struct ip_tree** const restrict tree_v4_out,
   struct ip_tree** const restrict tree_v6_out
);



/**
 * tree operations for the inner main function.
 * */
__attribute__((warn_unused_result))
static int main_run (
   struct ipdedup_globals* const restrict g
);


/**
 * Guess tree operations mode based on program name.
 *
 * @param prog_name  progam basename
 *
 * @return IPDEDUP_TREE_MODE
 * */
static int guess_tree_mode ( const char* const restrict prog_name );


/**
 * Determine the program's name from its path.
 *
 * @param prog_path
 *
 * @return prog name
 * */
static const char* get_prog_name ( const char* const restrict prog_path );


/**
 * Prints a message describing the program
 * and its usage to the given output stream.
 *
 * @param stream         output stream
 * @param prog_name      name of the program
 *
 * @return None (implicit)
 * */
static void print_description (
   const struct ipdedup_globals* const restrict g,
   FILE* const restrict stream
);


/**
 * Prints a message describing the program's usage to the given output stream.
 *
 * @param stream         output stream
 * @param prog_name      name of the program
 *
 * @return None (implicit)
 * */
static void print_usage (
   const struct ipdedup_globals* const restrict g,
   FILE* const restrict stream
);


int main ( int argc, char** argv ) {
   int ret;
   struct ipdedup_globals g;

   if ( ipdedup_globals_init ( &g ) != 0 ) {
      return EX_OSERR;
   }

   ret = main_inner ( &g, argc, argv );

   ipdedup_globals_free ( &g );

   if ( ret == 0 ) {
      return EXIT_SUCCESS;

   } else if ( ret < 0 ) {
      return EX_SOFTWARE;

   } else {
      return ret;
   }
}


static int main_push_infile (
   struct ipdedup_globals* const restrict g,
   struct dynarray* const restrict infiles,
   const char* const arg
) {
   char* buf;
   const char* fpath;

   fpath = NULL;

   if ( *arg == '@' ) {
      buf = ipdedup_globals_get_datafile_path ( g, (arg + 1) );
      if ( buf == NULL ) {
         switch ( errno ) {
            case EINVAL:
               return EX_USAGE;

            case ENOENT:
               return EX_IOERR;  /* FIXME: maybe introduce an extra error code */

            default:
               return EX_OSERR;
         }

      } else if ( access ( buf, F_OK ) != 0 ) {
         fprintf ( stderr, "Error: datafile missing: %s\n", buf );
         free ( buf );
         return EX_IOERR;  /* FIXME: maybe introduce an extra error code */

      } else if ( ipdedup_globals_pile_of_shame_push ( g, buf ) != 0 ) {
         free ( buf );
         return EX_OSERR;

      } else {
         fpath = buf;
      }

   } else {
      fpath = arg;
   }

   if ( fpath == NULL ) {
      return EX_SOFTWARE;

   } else if ( dynarray_append ( infiles, (void*) fpath ) != 0 ) {
      return EX_OSERR;
   }

   return 0;
}

static int main_interpret_parse_ret ( const int parse_ret ) {
   switch ( parse_ret ) {
      case PARSE_IP_RET_SUCCESS:
         return 0;

      case PARSE_IP_RET_BAD_INFILE_DUP:
         fprintf ( stderr, "Error: will not read stdin twice.\n" );
         return EX_USAGE;

      case PARSE_IP_RET_BAD_INFILE:
         return EX_USAGE;

      case PARSE_IP_RET_READ_ERR:
         return EX_IOERR;

      case PARSE_IP_RET_FAIL:
         return EX_SOFTWARE;

      case PARSE_IP_RET_INVALID:
         return EX_DATAERR;

      case PARSE_IP_RET_INVALID_NET:
         fprintf ( stderr, "Error: network address has host bits set.\n" );
         return EX_DATAERR;

      case PARSE_IP_RET_DID_NOT_TRY:
      case PARSE_IP_RET_NOT_IMPLEMENTED:
      case PARSE_IP_RET_EOF:
         return EX_SOFTWARE;

      default:
         return EX_SOFTWARE;
   }
}

static int main_inner_parse_input (
   struct ipdedup_globals* const restrict g
) {
   int parse_ret;

   if ( g->infiles == NULL ) {
      parse_ret = ip_tree_builder_parse_stream_do_insert (
         g->tree_builder, stdin, g->want_keep_going, g->want_strict
      );

   } else {
      parse_ret = ip_tree_builder_parse_files_do_insert (
         g->tree_builder, g->infiles, g->want_keep_going, g->want_strict
      );
      dynarray_free_ptr ( &(g->infiles) );
   }

   return main_interpret_parse_ret ( parse_ret );
}


static int main_inner (
   struct ipdedup_globals* const restrict g, int argc, char** argv
) {
   static const char* const PROG_OPTIONS = "46aB:C:cD:hikLlo:p:s";

   int opt;
   int ret;

   g->prog_name = get_prog_name ( argv[0] );

   g->purge_infiles = new_dynarray ( 1 );
   if ( g->purge_infiles == NULL ) { return EX_OSERR; }
   dynarray_set_data_readonly ( g->purge_infiles );

   while ( ( opt = getopt ( argc, argv, PROG_OPTIONS ) ) != -1 ) {
      switch ( opt ) {
         case '4':
            g->tree_mode = IPDEDUP_TREE_MODE_IPV4;
            break;

         case '6':
            g->tree_mode = IPDEDUP_TREE_MODE_IPV6;
            break;

         case 'a':
            g->tree_mode = IPDEDUP_TREE_MODE_MIXED;
            break;

         case 'B':
            ret = parse_ip_read_octet ( optarg, 10, &(g->collapse_prefixlen_v4) );
            if ( (ret != 0) || (g->collapse_prefixlen_v4 > 32) ) {
               fprintf ( stderr, "Error: -%c needs a number between 0 and 32\n", opt );
            }
            break;

         case 'C':
            ret = parse_ip_read_octet ( optarg, 10, &(g->collapse_prefixlen_v6) );
            if ( (ret != 0) || (g->collapse_prefixlen_v6 > 128) ) {
               fprintf ( stderr, "Error: -%c needs a number between 0 and 128\n", opt );
            }
            break;

         case 'c':
            g->collapse_prefixlen_v6 = 64;
            break;

         case 'D':
            /* empty arg unsets datadir */
            ipdedup_globals_set_datadir ( g, optarg );
            break;

         case 'h':
            print_description ( g, stdout );
            return 0;

         case 'i':
            g->want_invert = true;
            break;

         case 'k':
            g->want_keep_going = true;
            break;

         case 'L':
#if (defined __unix__) && (defined IPDEDUP_DATADIR)
            /* nftw() or shell out -- using shell variant */
            if ( system ( NULL ) == 0 ) {
               return EX_OSERR;

            /* unsafe system() arg IPDEDUP_DATADIR, chdir and use relpath */
            } else if ( chdir ( IPDEDUP_DATADIR ) != 0 ) {
               return EXIT_FAILURE;

            } else {
               ret = system (
                  ("{ find . -type f | sed -r -e 's=^[.]/=@=' | sort; }")
               );
               return (ret < 0) ? EX_OSERR : ret;
            }
#else
            fprintf ( stderr, "Error: feature not available.\n" );
            return EX_USAGE;
#endif
            return EX_SOFTWARE;  /* unreachable */

         case 'l':
            g->want_long_output = true;
            break;

         case 'o':
            if ( (optarg == NULL) || (*optarg == '\0') ) {
               fprintf ( stderr, "Error: -%c option needs a non-empty argument.\n", opt );
               return EX_USAGE;

            } else if ( (*optarg == '-') && (*(optarg + 1) == '\0') ) {
               /* switch to stdout */
               g->outfile = NULL;

            } else {
               g->outfile = optarg;
            }
            break;

         case 'p':
            if ( (optarg == NULL) || (*optarg == '\0') ) {
               fprintf ( stderr, "Error: -%c option needs a non-empty argument.\n", opt );
               return EX_USAGE;

            } else {
               /* push optarg to array (also if optarg is stdin marker "-") */
               ret = main_push_infile ( g, g->purge_infiles, optarg );
               if ( ret != 0 ) { return ret; }
            }
            break;

         case 's':
            g->want_strict = true;
            break;

         default:
            print_usage ( g, stderr );
            return EX_USAGE;
      }
   }

   if ( optind < argc ) {
      g->infiles = new_dynarray ( (argc - optind) );
      if ( g->infiles == NULL ) { return EX_OSERR; }

      dynarray_set_data_readonly ( g->infiles );

      for ( opt = optind; opt < argc; opt++ ) {
         const char* const arg = argv[opt];

         if ( (arg == NULL) || (*arg == '\0') ) {
            fprintf ( stderr, "Error: expected a non-empty positional argument.\n" );
            return EX_USAGE;

         } else {
            ret = main_push_infile ( g, g->infiles, arg );
            if ( ret != 0 ) { return ret; }
         }
      }
   }

   if ( g->tree_mode == IPDEDUP_TREE_MODE_NONE ) {
      g->tree_mode = guess_tree_mode ( g->prog_name );
   }

   /* initialize tree builder and main dispatcher,
    * depending on tree mode */
   g->tree_builder = ip_tree_builder_new ( g->tree_mode );

   if ( g->tree_builder == NULL ) {
      perror ( "Failed to initialize ip tree data structures" );
      /* possibly EX_SOFTWARE if tree_mode was not valid */
      return EX_OSERR;
   }

   /* initialize tree view */
   ipdedup_globals_init_tree_view ( g );

   /* dispatch */
   return main_run ( g );
}


static int main_run (
   struct ipdedup_globals* const restrict g
) {
   int ret;
   struct ip_tree* purge_tree_v4;
   struct ip_tree* purge_tree_v6;

   purge_tree_v4 = NULL;
   purge_tree_v6 = NULL;

   /* set auto-collapse if requested */
   if ( g->tree_v4 != NULL ) {
      ip_tree_set_auto_collapse ( g->tree_v4, g->collapse_prefixlen_v4 );
   }

   if ( g->tree_v6 != NULL ) {
      ip_tree_set_auto_collapse ( g->tree_v6, g->collapse_prefixlen_v6 );
   }

   /* parse input */
   ret = main_inner_parse_input ( g );  /* uses optind */
   if ( ret != 0 ) {
      fprintf ( stderr, "Error: failed to parse input\n" );
      return ret;
   }

   /* collapse tree */
   if ( g->tree_v4 != NULL ) { ip_tree_collapse ( g->tree_v4 ); }
   if ( g->tree_v6 != NULL ) { ip_tree_collapse ( g->tree_v6 ); }

   /* invert if requested */
   if ( g->want_invert ) {
      if ( g->tree_v4 != NULL ) {
         if ( ip_tree_invert ( g->tree_v4 ) != 0 ) {
            return EX_SOFTWARE;
         }
      }

      if ( g->tree_v6 != NULL ) {
         if ( ip_tree_invert ( g->tree_v6 ) != 0 ) {
            return EX_SOFTWARE;
         }
      }
   }

   /* purge if requested */
   if ( g->purge_infiles->len < 1 ) {
      dynarray_free_ptr ( &(g->purge_infiles) );

   } else {
      ret = main_interpret_parse_ret (
         main_parse_dedup_to_tree (
            g, g->purge_infiles, &purge_tree_v4, &purge_tree_v6
         )
      );

      dynarray_free_ptr ( &(g->purge_infiles) );

      if ( ret != 0 ) {
         /* purge_tree_v4, purge_tree_v6 are NULL - free() not necessary */
         fprintf ( stderr, "Error: failed to parse purge input\n" );
         return ret;
      }

      ret = ip_tree_builder_purge (
         g->tree_builder, purge_tree_v4, purge_tree_v6
      );

      ip_tree_destroy ( &purge_tree_v4 );
      ip_tree_destroy ( &purge_tree_v6 );

      if ( ret != 0 ) {
         fprintf ( stderr, "Error: failed to purge networks\n" );
         return EX_SOFTWARE;
      }
   }

   /* could free g->pile_of_shame here */

   /* open outstream */
   if ( g->outfile == NULL ) {
      g->close_outstream = false;
      g->outstream = stdout;

   } else {
      g->close_outstream = true;
      g->outstream = fopen ( g->outfile, "w" );
      if ( g->outstream == NULL ) { return EX_CANTCREAT; }
   }

   /* print */
   if ( g->tree_v4 != NULL ) {
      fprint_ip4_tree ( g->outstream, g->tree_v4 );
   }

   if ( g->tree_v6 != NULL ) {
      if ( g->want_long_output ) {
         fprint_ip6_tree ( g->outstream, g->tree_v6 );
      } else {
         /* more likely an error in this software than a malloc failure */
         /* COULDFIX: check errno */
         if ( fprint_ip6_tree_compact ( g->outstream, g->tree_v6 ) != 0 ) {
            return EX_SOFTWARE;
         }
      }
   }

   return 0;
}


static int main_parse_dedup_to_tree (
   struct ipdedup_globals* const restrict g,
   struct dynarray* const restrict infiles,
   struct ip_tree** const restrict tree_v4_out,
   struct ip_tree** const restrict tree_v6_out
) {
   int ret;
   struct ip_tree_build_data* tree_builder;

   *tree_v4_out = NULL;
   *tree_v6_out = NULL;

   tree_builder = ip_tree_builder_new ( g->tree_mode );
   if ( tree_builder == NULL ) { return -1; }

   ret = ip_tree_builder_parse_files_do_insert (
      tree_builder, infiles, g->want_keep_going, g->want_strict
   );

   if ( ret != PARSE_IP_RET_SUCCESS ) {
      ip_tree_builder_destroy ( &tree_builder );
      return ret;
   }

   *tree_v4_out = ip_tree_builder_steal_v4 ( tree_builder );
   *tree_v6_out = ip_tree_builder_steal_v6 ( tree_builder );

   ip_tree_builder_destroy ( &tree_builder );

   return PARSE_IP_RET_SUCCESS;
}


static int guess_tree_mode ( const char* const restrict prog_name ) {
   const char* s;

   if ( prog_name != NULL ) {
      s = prog_name;

      /* ip4, ipv4 | ip6, ipv6 */
      if ( (*s == 'i') && (*(s + 1) == 'p') ) {
         s += 2;
         if ( *s == 'v' ) { s++; }

         switch ( *s ) {
            case '4':
               return IPDEDUP_TREE_MODE_IPV4;

            case '6':
               return IPDEDUP_TREE_MODE_IPV6;
         }
      }
   }

   return IPDEDUP_TREE_MODE_MIXED;
}


/* POSIX basename() may modify input argument, use own impl */
static const char* get_prog_name ( const char* const restrict prog_path ) {
   const char* prog_name;
   const char* s;

   if ( prog_path == NULL ) { return prog_path; }

   prog_name = prog_path;

   for ( s = prog_path; ; s++ ) {
      switch ( *s ) {
         case '\0':
            return ( ( *prog_name == '\0' ) ? prog_path : prog_name );

         case '/':
            prog_name = s + 1;
            break;
      }
   }
}


static void print_description (
   const struct ipdedup_globals* const restrict g,
   FILE* const restrict stream
) {
   fprintf (
      stream,
      (
         "%s %u.%u%s\n"
         "\n"
         "  Merges and aggregates IPv4/IPv6 addresses/networks and writes\n"
         "  the resulting networks in <addr>/<prefixlen> notation to stdout.\n"
         "  Networks can be excluded from the final result by using the purge option.\n"
         "\n"
         "  The output will be sorted. When operating in mixed mode,\n"
         "  IPv4 networks will be written before any IPv6 networks.\n"
         "\n"
         "  The input format is one network address per line,\n"
         "  optionally followed by a slash char '/' and a prefixlen.\n"
         "  If omitted, the prefixlen is guessed based on the number\n"
         "  of non-zero blocks in the network address.\n"
         "\n"
      ),
      IPDEDUP_NAME,
      IPDEDUP_VER_MAJOR,
      IPDEDUP_VER_MINOR,
      ((IPDEDUP_VER_SUFFIX != NULL) ? IPDEDUP_VER_SUFFIX : "")
   );

   print_usage ( g, stream );
}


static void print_usage (
   const struct ipdedup_globals* const restrict g,
   FILE* const restrict stream
) {
   fprintf (
      stream,
      (
         "Usage:\n"
         "  %s {-4|-6|-a|-B <N>|-C <N>|-c|-D <DIR>|-h|-i|-k|-L|-l|-o <FILE>|-p <FILE>|-s} [<FILE>...]\n"
         "\n"
         "Options:\n"
         "  -4           IPv4 mode\n"
         "  -6           IPv6 mode\n"
         "  -a           IPv4/IPv6 mixed mode\n"
         "  -B <N>       collapse IPv4 input networks with a prefixlen\n"
         "               greater than <N> to a /<N> network\n"
         "               (if <N> greater than zero)\n"
         "  -C <N>       collapse IPv6 input networks with a prefixlen\n"
         "               greater than <N> to a /<N> network\n"
         "               (if <N> greater than zero)\n"
         "  -c           collapse IPv6 at prefixlen 64 (same as -C 64)\n"
         "  -D <DIR>     look up all following @name input files in <DIR>\n"
         "  -h           print this help message and exit\n"
         "  -i           invert networks\n"
         "  -k           skip invalid input instead of exiting with non-zero code\n"
#if (defined __unix__ ) && (defined IPDEDUP_DATADIR)
         "  -L           list default datadir include files\n"
#else
         "  -L           (feature not available)\n"
#endif
         "  -l           long output form (currently only affects IPv6 addresses)\n"
         "  -o <FILE>    write output to <FILE> instead of stdout\n"
         "  -p <FILE>    read network excludes from <FILE>\n"
         "               can be specified more than once\n"
         "  -s           verify that no host bits are set in parsed addresses\n"
         "               By default, lax rules are applied and network addresses\n"
         "               get silently truncated to their prefix length.\n"
         "\n"
         "Positional Arguments:\n"
         "  FILE...      read networks from files instead of stdin\n"
         "\n"
         "Exit Codes:\n"
         "  %3d          success\n"
         "  %3d          unspecified error\n"
         "  %3d          usage error\n"
         "  %3d          failed to parse input network specification\n"
         "  %3d          software logic broken\n"
         "  %3d          failed to get system resources\n"
         "  %3d          failed to open output file\n"
         "  %3d          failed to open input file\n"
         "\n"
         "Notes:\n"
         "  Input and exclude file names starting with a '@' char are looked up\n"
         "  in the data directory after removing the prefix char.\n"
         "  (currently %s)\n"
         "\n"
         "  For each line in the input (files),\n"
         "  leading and trailing whitespace is removed.\n"
         "  Empty lines, possibly caused by this conversion, are silently ignored.\n"
         "  Likewise, lines starting with a '#' char are ignored, too.\n"
         "\n"
         "  The keep-going mode (-k) allows for invalid input.\n"
         "  However, if no input network specification could be parsed\n"
         "  but at least one was seen, then %s exits non-zero (%d).\n"
         "  This rule applies to each individual input file.\n"
      ),

      /* Usage: */
      g->prog_name,

      /* Options: */

      /* Arguments: */

      /* Exit Codes: */
      EXIT_SUCCESS,
      EXIT_FAILURE,
      EX_USAGE,
      EX_DATAERR,
      EX_SOFTWARE,
      EX_OSERR,
      EX_CANTCREAT,
      EX_IOERR,

      /* Notes: */
      ( (g->datadir != NULL) ? g->datadir : "<unset>" ),
      g->prog_name,
      EX_DATAERR
   );
}
