/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * This file contains various definitions of structures that are used by Vim
 */

/*
 * There is something wrong in the SAS compiler that makes typedefs not
 * valid in include files.  Has been fixed in version 6.58.
 *
 * about sas c see:
 * http://pjhutchison.org/tutorial/sas_c.html
 * https://www.warped.com/amiga/
 *
 * 下面的代码似乎太老了
 */
/* #if defined(SASC) && SASC < 658 */
/* typedef long		linenr_T; */
/* typedef int		colnr_T; */
/* typedef unsigned short	short_u; */
/* #endif */

/*
 * Position in file or buffer.
 */
typedef struct
{
    linenr_T	lnum;	// line number
    colnr_T	col;	// column number
    colnr_T	coladd; // extra virtual column
} pos_T;


/*
 * Same, but without coladd.
 */
typedef struct
{
    linenr_T	lnum;	// line number
    colnr_T	col;	// column number
} lpos_T;

/*
 * Structure used for growing arrays.
 * This is used to store information that only grows, is deleted all at
 * once, and needs to be accessed by index.  See ga_clear() and ga_grow().
 */
typedef struct growarray
{
    int	    ga_len;		    // current number of items used
    int	    ga_maxlen;		    // maximum number of items possible
    int	    ga_itemsize;	    // sizeof(item)
    int	    ga_growsize;	    // number of items to grow each time
    void    *ga_data;		    // pointer to the first item
} garray_T;

#define GA_EMPTY    {0, 0, 0, 0, NULL}

typedef struct window_S		win_T;
typedef struct wininfo_S	wininfo_T;
typedef struct frame_S		frame_T;
typedef int			scid_T;		// script ID
typedef struct file_buffer	buf_T;		// forward declaration
typedef struct terminal_S	term_T;

#ifdef FEAT_MENU
typedef struct VimMenu vimmenu_T;
#endif

// maximum value for sc_version
#define SCRIPT_VERSION_MAX 4
// value for sc_version in a Vim9 script file
#define SCRIPT_VERSION_VIM9 999999

/*
 * SCript ConteXt (SCTX): identifies a script line.
 * When sourcing a script "sc_lnum" is zero, "sourcing_lnum" is the current
 * line number. When executing a user function "sc_lnum" is the line where the
 * function was defined, "sourcing_lnum" is the line number inside the
 * function.  When stored with a function, mapping, option, etc. "sc_lnum" is
 * the line number in the script "sc_sid".
 *
 * sc_version is also here, for convenience.
 */
typedef struct {
    scid_T	sc_sid;		// script ID
    int		sc_seq;		// sourcing sequence number
    linenr_T	sc_lnum;	// line number
    int		sc_version;	// :scriptversion
} sctx_T;

/*
 * Reference to a buffer that stores the value of buf_free_count.
 * bufref_valid() only needs to check "buf" when the count differs.
 */
typedef struct {
    buf_T   *br_buf;
    int	    br_fnum;
    int	    br_buf_free_count;
} bufref_T;

/*
 * This is here because regexp.h needs pos_T and below regprog_T is used.
 */
#include "regexp.h"

/*
 * This is here because gui.h needs the pos_T and win_T, and win_T needs gui.h
 * for scrollbar_T.
 */
#ifdef FEAT_GUI
# include "gui.h"
#else
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
# endif
# define guicolor_T long
# define INVALCOLOR ((guicolor_T)0x1ffffff)
    // only used for cterm.bg_rgb and cterm.fg_rgb: use cterm color
# define CTERMCOLOR ((guicolor_T)0x1fffffe)
#endif
#define COLOR_INVALID(x) ((x) == INVALCOLOR || (x) == CTERMCOLOR)

/*
 * marks: positions in a file
 * (a normal mark is a lnum/col pair, the same as a file position)
 */

// (Note: for EBCDIC there are more than 26, because there are gaps in the
// alphabet coding.  To minimize changes to the code, I decided to just
// increase the number of possible marks.
#define NMARKS		('z' - 'a' + 1)	// max. # of named marks
#define EXTRA_MARKS	10		// marks 0-9
#define JUMPLISTSIZE	100		// max. # of marks in jump list
#define TAGSTACKSIZE	20		// max. # of tags in tag stack

typedef struct filemark
{
    pos_T	mark;		// cursor position
    int		fnum;		// file number
} fmark_T;

// Xtended file mark: also has a file name
typedef struct xfilemark
{
    fmark_T	fmark;
    char_u	*fname;		// file name, used when fnum == 0
#ifdef FEAT_VIMINFO
    time_T	time_set;
#endif
} xfmark_T;

/*
 * The taggy struct is used to store the information about a :tag command.
 */
typedef struct taggy
{
    char_u	*tagname;	// tag name
    fmark_T	fmark;		// cursor position BEFORE ":tag"
    int		cur_match;	// match number
    int		cur_fnum;	// buffer number used for cur_match
    char_u	*user_data;	// used with tagfunc
} taggy_T;

/*
 * Structure that contains all options that are local to a window.
 * Used twice in a window: for the current buffer and for all buffers.
 * Also used in wininfo_T.
 */
typedef struct
{
#ifdef FEAT_ARABIC
    int		wo_arab;
# define w_p_arab w_onebuf_opt.wo_arab	// 'arabic'
#endif
#ifdef FEAT_LINEBREAK
    int		wo_bri;
# define w_p_bri w_onebuf_opt.wo_bri	// 'breakindent'
    char_u	*wo_briopt;
# define w_p_briopt w_onebuf_opt.wo_briopt // 'breakindentopt'
#endif
    char_u	*wo_wcr;
# define w_p_wcr w_onebuf_opt.wo_wcr	// 'wincolor'
#ifdef FEAT_DIFF
    int		wo_diff;
# define w_p_diff w_onebuf_opt.wo_diff	// 'diff'
#endif
#ifdef FEAT_FOLDING
    long	wo_fdc;
# define w_p_fdc w_onebuf_opt.wo_fdc	// 'foldcolumn'
    int		wo_fdc_save;
# define w_p_fdc_save w_onebuf_opt.wo_fdc_save	// 'foldenable' saved for diff mode
    int		wo_fen;
# define w_p_fen w_onebuf_opt.wo_fen	// 'foldenable'
    int		wo_fen_save;
# define w_p_fen_save w_onebuf_opt.wo_fen_save	// 'foldenable' saved for diff mode
    char_u	*wo_fdi;
# define w_p_fdi w_onebuf_opt.wo_fdi	// 'foldignore'
    long	wo_fdl;
# define w_p_fdl w_onebuf_opt.wo_fdl	// 'foldlevel'
    int		wo_fdl_save;
# define w_p_fdl_save w_onebuf_opt.wo_fdl_save	// 'foldlevel' state saved for diff mode
    char_u	*wo_fdm;
# define w_p_fdm w_onebuf_opt.wo_fdm	// 'foldmethod'
    char_u	*wo_fdm_save;
# define w_p_fdm_save w_onebuf_opt.wo_fdm_save	// 'fdm' saved for diff mode
    long	wo_fml;
# define w_p_fml w_onebuf_opt.wo_fml	// 'foldminlines'
    long	wo_fdn;
# define w_p_fdn w_onebuf_opt.wo_fdn	// 'foldnestmax'
# ifdef FEAT_EVAL
    char_u	*wo_fde;
# define w_p_fde w_onebuf_opt.wo_fde	// 'foldexpr'
    char_u	*wo_fdt;
#  define w_p_fdt w_onebuf_opt.wo_fdt	// 'foldtext'
# endif
    char_u	*wo_fmr;
# define w_p_fmr w_onebuf_opt.wo_fmr	// 'foldmarker'
#endif
#ifdef FEAT_LINEBREAK
    int		wo_lbr;
# define w_p_lbr w_onebuf_opt.wo_lbr	// 'linebreak'
#endif
    int		wo_list;
#define w_p_list w_onebuf_opt.wo_list	// 'list'
    int		wo_nu;
#define w_p_nu w_onebuf_opt.wo_nu	// 'number'
    int		wo_rnu;
#define w_p_rnu w_onebuf_opt.wo_rnu	// 'relativenumber'
#ifdef FEAT_LINEBREAK
    long	wo_nuw;
# define w_p_nuw w_onebuf_opt.wo_nuw	// 'numberwidth'
#endif
    int		wo_wfh;
# define w_p_wfh w_onebuf_opt.wo_wfh	// 'winfixheight'
    int		wo_wfw;
# define w_p_wfw w_onebuf_opt.wo_wfw	// 'winfixwidth'
#if defined(FEAT_QUICKFIX)
    int		wo_pvw;
# define w_p_pvw w_onebuf_opt.wo_pvw	// 'previewwindow'
#endif
#ifdef FEAT_RIGHTLEFT
    int		wo_rl;
# define w_p_rl w_onebuf_opt.wo_rl	// 'rightleft'
    char_u	*wo_rlc;
# define w_p_rlc w_onebuf_opt.wo_rlc	// 'rightleftcmd'
#endif
    long	wo_scr;
#define w_p_scr w_onebuf_opt.wo_scr	// 'scroll'
#ifdef FEAT_SPELL
    int		wo_spell;
# define w_p_spell w_onebuf_opt.wo_spell // 'spell'
#endif
#ifdef FEAT_SYN_HL
    int		wo_cuc;
# define w_p_cuc w_onebuf_opt.wo_cuc	// 'cursorcolumn'
    int		wo_cul;
# define w_p_cul w_onebuf_opt.wo_cul	// 'cursorline'
    char_u	*wo_culopt;
# define w_p_culopt w_onebuf_opt.wo_culopt	// 'cursorlineopt'
    char_u	*wo_cc;
# define w_p_cc w_onebuf_opt.wo_cc	// 'colorcolumn'
#endif
#ifdef FEAT_LINEBREAK
    char_u	*wo_sbr;
#define w_p_sbr w_onebuf_opt.wo_sbr	// 'showbreak'
#endif
#ifdef FEAT_STL_OPT
    char_u	*wo_stl;
#define w_p_stl w_onebuf_opt.wo_stl	// 'statusline'
#endif
    int		wo_scb;
#define w_p_scb w_onebuf_opt.wo_scb	// 'scrollbind'
    int		wo_diff_saved; // options were saved for starting diff mode
#define w_p_diff_saved w_onebuf_opt.wo_diff_saved
    int		wo_scb_save;	// 'scrollbind' saved for diff mode
#define w_p_scb_save w_onebuf_opt.wo_scb_save
    int		wo_wrap;
#define w_p_wrap w_onebuf_opt.wo_wrap	// 'wrap'
#ifdef FEAT_DIFF
    int		wo_wrap_save;	// 'wrap' state saved for diff mode
# define w_p_wrap_save w_onebuf_opt.wo_wrap_save
#endif
#ifdef FEAT_CONCEAL
    char_u	*wo_cocu;		// 'concealcursor'
# define w_p_cocu w_onebuf_opt.wo_cocu
    long	wo_cole;		// 'conceallevel'
# define w_p_cole w_onebuf_opt.wo_cole
#endif
    int		wo_crb;
#define w_p_crb w_onebuf_opt.wo_crb	// 'cursorbind'
    int		wo_crb_save;	// 'cursorbind' state saved for diff mode
#define w_p_crb_save w_onebuf_opt.wo_crb_save
#ifdef FEAT_SIGNS
    char_u	*wo_scl;
# define w_p_scl w_onebuf_opt.wo_scl	// 'signcolumn'
#endif
#ifdef FEAT_TERMINAL
    char_u	*wo_twk;
# define w_p_twk w_onebuf_opt.wo_twk	// 'termwinkey'
    char_u	*wo_tws;
# define w_p_tws w_onebuf_opt.wo_tws	// 'termwinsize'
#endif

#ifdef FEAT_EVAL
    sctx_T	wo_script_ctx[WV_COUNT];	// SCTXs for window-local options
# define w_p_script_ctx w_onebuf_opt.wo_script_ctx
#endif
} winopt_T;

/*
 * Window info stored with a buffer.
 *
 * Two types of info are kept for a buffer which are associated with a
 * specific window:
 * 1. Each window can have a different line number associated with a buffer.
 * 2. The window-local options for a buffer work in a similar way.
 * The window-info is kept in a list at b_wininfo.  It is kept in
 * most-recently-used order.
 */
struct wininfo_S
{
    wininfo_T	*wi_next;	// next entry or NULL for last entry
    wininfo_T	*wi_prev;	// previous entry or NULL for first entry
    win_T	*wi_win;	// pointer to window that did set wi_fpos
    pos_T	wi_fpos;	// last cursor position in the file
    int		wi_optset;	// TRUE when wi_opt has useful values
    winopt_T	wi_opt;		// local window options
#ifdef FEAT_FOLDING
    int		wi_fold_manual;	// copy of w_fold_manual
    garray_T	wi_folds;	// clone of w_folds
#endif
};

/*
 * Info used to pass info about a fold from the fold-detection code to the
 * code that displays the foldcolumn.
 */
typedef struct foldinfo
{
    int		fi_level;	// level of the fold; when this is zero the
				// other fields are invalid
    int		fi_lnum;	// line number where fold starts
    int		fi_low_level;	// lowest fold level that starts in the same
				// line
} foldinfo_T;

/*
 * Structure to store info about the Visual area.
 */
typedef struct
{
    pos_T	vi_start;	// start pos of last VIsual
    pos_T	vi_end;		// end position of last VIsual
    int		vi_mode;	// VIsual_mode of last VIsual
    colnr_T	vi_curswant;	// MAXCOL from w_curswant
} visualinfo_T;

/*
 * structures used for undo
 */

// One line saved for undo.  After the NUL terminated text there might be text
// properties, thus ul_len can be larger than STRLEN(ul_line) + 1.
typedef struct {
    char_u	*ul_line;	// text of the line
    long	ul_len;		// length of the line including NUL, plus text
				// properties
} undoline_T;

typedef struct u_entry u_entry_T;
typedef struct u_header u_header_T;
struct u_entry
{
    u_entry_T	*ue_next;	// pointer to next entry in list
    linenr_T	ue_top;		// number of line above undo block
    linenr_T	ue_bot;		// number of line below undo block
    linenr_T	ue_lcount;	// linecount when u_save called
    undoline_T	*ue_array;	// array of lines in undo block
    long	ue_size;	// number of lines in ue_array
#ifdef U_DEBUG
    int		ue_magic;	// magic number to check allocation
#endif
};

struct u_header
{
    // The following have a pointer and a number. The number is used when
    // reading the undo file in u_read_undo()
    union {
	u_header_T *ptr;	// pointer to next undo header in list
	long	   seq;
    } uh_next;
    union {
	u_header_T *ptr;	// pointer to previous header in list
	long	   seq;
    } uh_prev;
    union {
	u_header_T *ptr;	// pointer to next header for alt. redo
	long	   seq;
    } uh_alt_next;
    union {
	u_header_T *ptr;	// pointer to previous header for alt. redo
	long	   seq;
    } uh_alt_prev;
    long	uh_seq;		// sequence number, higher == newer undo
    int		uh_walk;	// used by undo_time()
    u_entry_T	*uh_entry;	// pointer to first entry
    u_entry_T	*uh_getbot_entry; // pointer to where ue_bot must be set
    pos_T	uh_cursor;	// cursor position before saving
    long	uh_cursor_vcol;
    int		uh_flags;	// see below
    pos_T	uh_namedm[NMARKS];	// marks before undo/after redo
    visualinfo_T uh_visual;	// Visual areas before undo/after redo
    time_T	uh_time;	// timestamp when the change was made
    long	uh_save_nr;	// set when the file was saved after the
				// changes in this block
#ifdef U_DEBUG
    int		uh_magic;	// magic number to check allocation
#endif
};

// values for uh_flags
#define UH_CHANGED  0x01	// b_changed flag before undo/after redo
#define UH_EMPTYBUF 0x02	// buffer was empty

/*
 * structures used in undo.c
 */
#define ALIGN_LONG	// longword alignment and use filler byte
#define ALIGN_SIZE (sizeof(long))

#define ALIGN_MASK (ALIGN_SIZE - 1)

typedef struct m_info minfo_T;

/*
 * structure used to link chunks in one of the free chunk lists.
 */
struct m_info
{
#ifdef ALIGN_LONG
    long_u	m_size;		// size of the chunk (including m_info)
#else
    short_u	m_size;		// size of the chunk (including m_info)
#endif
    minfo_T	*m_next;	// pointer to next free chunk in the list
};

/*
 * things used in memfile.c
 */

typedef struct block_hdr    bhdr_T;
typedef struct memfile	    memfile_T;
typedef long		    blocknr_T;

/*
 * mf_hashtab_T is a chained hashtable with blocknr_T key and arbitrary
 * structures as items.  This is an intrusive data structure: we require
 * that items begin with mf_hashitem_T which contains the key and linked
 * list pointers.  List of items in each bucket is doubly-linked.
 */

typedef struct mf_hashitem_S mf_hashitem_T;

struct mf_hashitem_S
{
    mf_hashitem_T   *mhi_next;
    mf_hashitem_T   *mhi_prev;
    blocknr_T	    mhi_key;
};

#define MHT_INIT_SIZE   64

typedef struct mf_hashtab_S
{
    long_u	    mht_mask;	    // mask used for hash value (nr of items
				    // in array is "mht_mask" + 1)
    long_u	    mht_count;	    // nr of items inserted into hashtable
    mf_hashitem_T   **mht_buckets;  // points to mht_small_buckets or
				    //dynamically allocated array
    mf_hashitem_T   *mht_small_buckets[MHT_INIT_SIZE];   // initial buckets
    char	    mht_fixed;	    // non-zero value forbids growth
} mf_hashtab_T;

/*
 * for each (previously) used block in the memfile there is one block header.
 *
 * The block may be linked in the used list OR in the free list.
 * The used blocks are also kept in hash lists.
 *
 * The used list is a doubly linked list, most recently used block first.
 *	The blocks in the used list have a block of memory allocated.
 *	mf_used_count is the number of pages in the used list.
 * The hash lists are used to quickly find a block in the used list.
 * The free list is a single linked list, not sorted.
 *	The blocks in the free list have no block of memory allocated and
 *	the contents of the block in the file (if any) is irrelevant.
 */

struct block_hdr
{
    mf_hashitem_T bh_hashitem;      // header for hash table and key
#define bh_bnum bh_hashitem.mhi_key // block number, part of bh_hashitem

    bhdr_T	*bh_next;	    // next block_hdr in free or used list
    bhdr_T	*bh_prev;	    // previous block_hdr in used list
    char_u	*bh_data;	    // pointer to memory (for used block)
    int		bh_page_count;	    // number of pages in this block

#define BH_DIRTY    1
#define BH_LOCKED   2
    char	bh_flags;	    // BH_DIRTY or BH_LOCKED
};

/*
 * when a block with a negative number is flushed to the file, it gets
 * a positive number. Because the reference to the block is still the negative
 * number, we remember the translation to the new positive number in the
 * double linked trans lists. The structure is the same as the hash lists.
 */
typedef struct nr_trans NR_TRANS;

struct nr_trans
{
    mf_hashitem_T nt_hashitem;		// header for hash table and key
#define nt_old_bnum nt_hashitem.mhi_key	// old, negative, number

    blocknr_T	nt_new_bnum;		// new, positive, number
};


typedef struct buffblock buffblock_T;
typedef struct buffheader buffheader_T;

/*
 * structure used to store one block of the stuff/redo/recording buffers
 */
struct buffblock
{
    buffblock_T	*b_next;	// pointer to next buffblock
    char_u	b_str[1];	// contents (actually longer)
};

/*
 * header used for the stuff buffer and the redo buffer
 */
struct buffheader
{
    buffblock_T	bh_first;	// first (dummy) block of list
    buffblock_T	*bh_curr;	// buffblock for appending
    int		bh_index;	// index for reading
    int		bh_space;	// space in bh_curr for appending
};

typedef struct
{
    buffheader_T sr_redobuff;
    buffheader_T sr_old_redobuff;
} save_redo_T;

/*
 * used for completion on the command line
 */
typedef struct expand
{
    char_u	*xp_pattern;		// start of item to expand
    int		xp_context;		// type of expansion
    int		xp_pattern_len;		// bytes in xp_pattern before cursor
#if defined(FEAT_EVAL)
    char_u	*xp_arg;		// completion function
    sctx_T	xp_script_ctx;		// SCTX for completion function
#endif
    int		xp_backslash;		// one of the XP_BS_ values
#ifndef BACKSLASH_IN_FILENAME
    int		xp_shell;		// TRUE for a shell command, more
					// characters need to be escaped
#endif
    int		xp_numfiles;		// number of files found by
					// file name completion
    int		xp_col;			// cursor position in line
    char_u	**xp_files;		// list of files
    char_u	*xp_line;		// text being completed
} expand_T;

/*
 * values for xp_backslash
 */
#define XP_BS_NONE	0	// nothing special for backslashes
#define XP_BS_ONE	1	// uses one backslash before a space
#define XP_BS_THREE	2	// uses three backslashes before a space

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
typedef struct
{
    char_u	*cmdbuff;	// pointer to command line buffer
    int		cmdbufflen;	// length of cmdbuff
    int		cmdlen;		// number of chars in command line
    int		cmdpos;		// current cursor position
    int		cmdspos;	// cursor column on screen
    int		cmdfirstc;	// ':', '/', '?', '=', '>' or NUL
    int		cmdindent;	// number of spaces before cmdline
    char_u	*cmdprompt;	// message in front of cmdline
    int		cmdattr;	// attributes for prompt
    int		overstrike;	// Typing mode on the command line.  Shared by
				// getcmdline() and put_on_cmdline().
    expand_T	*xpc;		// struct being used for expansion, xp_pattern
				// may point into cmdbuff
    int		xp_context;	// type of expansion
# ifdef FEAT_EVAL
    char_u	*xp_arg;	// user-defined expansion arg
    int		input_fn;	// when TRUE Invoked for input() function
# endif
} cmdline_info_T;

/*
 * Command modifiers ":vertical", ":browse", ":confirm" and ":hide" set a flag.
 * This needs to be saved for recursive commands, put them in a structure for
 * easy manipulation.
 */
typedef struct
{
    int		hide;			// TRUE when ":hide" was used
# ifdef FEAT_BROWSE_CMD
    int		browse;			// TRUE to invoke file dialog
# endif
    int		split;			// flags for win_split()
    int		tab;			// > 0 when ":tab" was used
# if defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    int		confirm;		// TRUE to invoke yes/no dialog
# endif
    int		keepalt;		// TRUE when ":keepalt" was used
    int		keepmarks;		// TRUE when ":keepmarks" was used
    int		keepjumps;		// TRUE when ":keepjumps" was used
    int		lockmarks;		// TRUE when ":lockmarks" was used
    int		keeppatterns;		// TRUE when ":keeppatterns" was used
    int		noswapfile;		// TRUE when ":noswapfile" was used
    char_u	*save_ei;		// saved value of 'eventignore'
    regmatch_T	filter_regmatch;	// set by :filter /pat/
    int		filter_force;		// set for :filter!
} cmdmod_T;

#define MF_SEED_LEN	8

struct memfile
{
    char_u	*mf_fname;		// name of the file
    char_u	*mf_ffname;		// idem, full path
    int		mf_fd;			// file descriptor
    int		mf_flags;		// flags used when opening this memfile
    int		mf_reopen;		// mf_fd was closed, retry opening
    bhdr_T	*mf_free_first;		// first block_hdr in free list
    bhdr_T	*mf_used_first;		// mru block_hdr in used list
    bhdr_T	*mf_used_last;		// lru block_hdr in used list
    unsigned	mf_used_count;		// number of pages in used list
    unsigned	mf_used_count_max;	// maximum number of pages in memory
    mf_hashtab_T mf_hash;		// hash lists
    mf_hashtab_T mf_trans;		// trans lists
    blocknr_T	mf_blocknr_max;		// highest positive block number + 1
    blocknr_T	mf_blocknr_min;		// lowest negative block number - 1
    blocknr_T	mf_neg_count;		// number of negative blocks numbers
    blocknr_T	mf_infile_count;	// number of pages in the file
    unsigned	mf_page_size;		// number of bytes in a page
    int		mf_dirty;		// TRUE if there are dirty blocks
#ifdef FEAT_CRYPT
    buf_T	*mf_buffer;		// buffer this memfile is for
    char_u	mf_seed[MF_SEED_LEN];	// seed for encryption

    // Values for key, method and seed used for reading data blocks when
    // updating for a newly set key or method. Only when mf_old_key != NULL.
    char_u	*mf_old_key;
    int		mf_old_cm;
    char_u	mf_old_seed[MF_SEED_LEN];
#endif
};

/*
 * things used in memline.c
 */
/*
 * When searching for a specific line, we remember what blocks in the tree
 * are the branches leading to that block. This is stored in ml_stack.  Each
 * entry is a pointer to info in a block (may be data block or pointer block)
 */
typedef struct info_pointer
{
    blocknr_T	ip_bnum;	// block number
    linenr_T	ip_low;		// lowest lnum in this block
    linenr_T	ip_high;	// highest lnum in this block
    int		ip_index;	// index for block with current lnum
} infoptr_T;	// block/index pair

#ifdef FEAT_BYTEOFF
typedef struct ml_chunksize
{
    int		mlcs_numlines;
    long	mlcs_totalsize;
} chunksize_T;

/*
 * Flags when calling ml_updatechunk()
 */
# define ML_CHNK_ADDLINE 1
# define ML_CHNK_DELLINE 2
# define ML_CHNK_UPDLINE 3
#endif

/*
 * the memline structure holds all the information about a memline
 */
typedef struct memline
{
    linenr_T	ml_line_count;	// number of lines in the buffer

    memfile_T	*ml_mfp;	// pointer to associated memfile

    infoptr_T	*ml_stack;	// stack of pointer blocks (array of IPTRs)
    int		ml_stack_top;	// current top of ml_stack
    int		ml_stack_size;	// total number of entries in ml_stack

#define ML_EMPTY	1	// empty buffer
#define ML_LINE_DIRTY	2	// cached line was changed and allocated
#define ML_LOCKED_DIRTY	4	// ml_locked was changed
#define ML_LOCKED_POS	8	// ml_locked needs positive block number
    int		ml_flags;

    colnr_T	ml_line_len;	// length of the cached line, including NUL
    linenr_T	ml_line_lnum;	// line number of cached line, 0 if not valid
    char_u	*ml_line_ptr;	// pointer to cached line

    bhdr_T	*ml_locked;	// block used by last ml_get
    linenr_T	ml_locked_low;	// first line in ml_locked
    linenr_T	ml_locked_high;	// last line in ml_locked
    int		ml_locked_lineadd;  // number of lines inserted in ml_locked
#ifdef FEAT_BYTEOFF
    chunksize_T *ml_chunksize;
    int		ml_numchunks;
    int		ml_usedchunks;
#endif
} memline_T;

// Values for the flags argument of ml_delete_flags().
#define ML_DEL_MESSAGE	    1	// may give a "No lines in buffer" message
#define ML_DEL_UNDO	    2	// called from undo, do not update textprops

// Values for the flags argument of ml_append_int().
#define ML_APPEND_NEW	    1	// starting to edit a new file
#define ML_APPEND_MARK	    2	// mark the new line
#define ML_APPEND_UNDO	    4	// called from undo


/*
 * Structure defining text properties.  These stick with the text.
 * When stored in memline they are after the text, ml_line_len is larger than
 * STRLEN(ml_line_ptr) + 1.
 */
typedef struct textprop_S
{
    colnr_T	tp_col;		// start column (one based, in bytes)
    colnr_T	tp_len;		// length in bytes
    int		tp_id;		// identifier
    int		tp_type;	// property type
    int		tp_flags;	// TP_FLAG_ values
} textprop_T;

#define TP_FLAG_CONT_NEXT	1	// property continues in next line
#define TP_FLAG_CONT_PREV	2	// property was continued from prev line

/*
 * Structure defining a property type.
 */
typedef struct proptype_S
{
    int		pt_id;		// value used for tp_id
    int		pt_type;	// number used for tp_type
    int		pt_hl_id;	// highlighting
    int		pt_priority;	// priority
    int		pt_flags;	// PT_FLAG_ values
    char_u	pt_name[1];	// property type name, actually longer
} proptype_T;

#define PT_FLAG_INS_START_INCL	1	// insert at start included in property
#define PT_FLAG_INS_END_INCL	2	// insert at end included in property
#define PT_FLAG_COMBINE		4	// combine with syntax highlight

// Sign group
typedef struct signgroup_S
{
    int		sg_next_sign_id;	// next sign id for this group
    short_u	sg_refcount;		// number of signs in this group
    char_u	sg_name[1];		// sign group name, actually longer
} signgroup_T;

typedef struct sign_entry sign_entry_T;
struct sign_entry
{
    int		 se_id;		// unique identifier for each placed sign
    int		 se_typenr;	// typenr of sign
    int		 se_priority;	// priority for highlighting
    linenr_T	 se_lnum;	// line number which has this sign
    signgroup_T	 *se_group;	// sign group
    sign_entry_T *se_next;	// next entry in a list of signs
    sign_entry_T *se_prev;	// previous entry -- for easy reordering
};

/*
 * Sign attributes. Used by the screen refresh routines.
 */
typedef struct sign_attrs_S {
    int		sat_typenr;
    void	*sat_icon;
    char_u	*sat_text;
    int		sat_texthl;
    int		sat_linehl;
} sign_attrs_T;

#if defined(FEAT_SIGNS) || defined(PROTO)
// Macros to get the sign group structure from the group name
#define SGN_KEY_OFF	offsetof(signgroup_T, sg_name)
#define HI2SG(hi)	((signgroup_T *)((hi)->hi_key - SGN_KEY_OFF))

// Default sign priority for highlighting
#define SIGN_DEF_PRIO	10

#endif

/*
 * Argument list: Array of file names.
 * Used for the global argument list and the argument lists local to a window.
 */
typedef struct arglist
{
    garray_T	al_ga;		// growarray with the array of file names
    int		al_refcount;	// number of windows using this arglist
    int		id;		// id of this arglist
} alist_T;

/*
 * For each argument remember the file name as it was given, and the buffer
 * number that contains the expanded file name (required for when ":cd" is
 * used).
 */
typedef struct argentry
{
    char_u	*ae_fname;	// file name as specified
    int		ae_fnum;	// buffer number with expanded file name
} aentry_T;

#define ALIST(win)	(win)->w_alist
#define GARGLIST	((aentry_T *)global_alist.al_ga.ga_data)
#define ARGLIST		((aentry_T *)ALIST(curwin)->al_ga.ga_data)
#define WARGLIST(wp)	((aentry_T *)ALIST(wp)->al_ga.ga_data)
#define AARGLIST(al)	((aentry_T *)((al)->al_ga.ga_data))
#define GARGCOUNT	(global_alist.al_ga.ga_len)
#define ARGCOUNT	(ALIST(curwin)->al_ga.ga_len)
#define WARGCOUNT(wp)	(ALIST(wp)->al_ga.ga_len)

/*
 * A list used for saving values of "emsg_silent".  Used by ex_try() to save the
 * value of "emsg_silent" if it was non-zero.  When this is done, the CSF_SILENT
 * flag below is set.
 */

typedef struct eslist_elem eslist_T;
struct eslist_elem
{
    int		saved_emsg_silent;	// saved value of "emsg_silent"
    eslist_T	*next;			// next element on the list
};

/*
 * For conditional commands a stack is kept of nested conditionals.
 * When cs_idx < 0, there is no conditional command.
 */
#define CSTACK_LEN	50

typedef struct {
    short	cs_flags[CSTACK_LEN];	// CSF_ flags
    char	cs_pending[CSTACK_LEN];	// CSTP_: what's pending in ":finally"
    union {
	void	*csp_rv[CSTACK_LEN];	// return typeval for pending return
	void	*csp_ex[CSTACK_LEN];	// exception for pending throw
    }		cs_pend;
    void	*cs_forinfo[CSTACK_LEN]; // info used by ":for"
    int		cs_line[CSTACK_LEN];	// line nr of ":while"/":for" line
    int		cs_idx;			// current entry, or -1 if none
    int		cs_looplevel;		// nr of nested ":while"s and ":for"s
    int		cs_trylevel;		// nr of nested ":try"s
    eslist_T	*cs_emsg_silent_list;	// saved values of "emsg_silent"
    char	cs_lflags;		// loop flags: CSL_ flags
} cstack_T;
# define cs_rettv	cs_pend.csp_rv
# define cs_exception	cs_pend.csp_ex

// There is no CSF_IF, the lack of CSF_WHILE, CSF_FOR and CSF_TRY means ":if"
// was used.
# define CSF_TRUE	0x0001	// condition was TRUE
# define CSF_ACTIVE	0x0002	// current state is active
# define CSF_ELSE	0x0004	// ":else" has been passed
# define CSF_WHILE	0x0008	// is a ":while"
# define CSF_FOR	0x0010	// is a ":for"

# define CSF_TRY	0x0100	// is a ":try"
# define CSF_FINALLY	0x0200	// ":finally" has been passed
# define CSF_THROWN	0x0400	// exception thrown to this try conditional
# define CSF_CAUGHT	0x0800  // exception caught by this try conditional
# define CSF_SILENT	0x1000	// "emsg_silent" reset by ":try"
// Note that CSF_ELSE is only used when CSF_TRY and CSF_WHILE are unset
// (an ":if"), and CSF_SILENT is only used when CSF_TRY is set.

/*
 * What's pending for being reactivated at the ":endtry" of this try
 * conditional:
 */
# define CSTP_NONE	0	// nothing pending in ":finally" clause
# define CSTP_ERROR	1	// an error is pending
# define CSTP_INTERRUPT	2	// an interrupt is pending
# define CSTP_THROW	4	// a throw is pending
# define CSTP_BREAK	8	// ":break" is pending
# define CSTP_CONTINUE	16	// ":continue" is pending
# define CSTP_RETURN	24	// ":return" is pending
# define CSTP_FINISH	32	// ":finish" is pending

/*
 * Flags for the cs_lflags item in cstack_T.
 */
# define CSL_HAD_LOOP	 1	// just found ":while" or ":for"
# define CSL_HAD_ENDLOOP 2	// just found ":endwhile" or ":endfor"
# define CSL_HAD_CONT	 4	// just found ":continue"
# define CSL_HAD_FINA	 8	// just found ":finally"

/*
 * A list of error messages that can be converted to an exception.  "throw_msg"
 * is only set in the first element of the list.  Usually, it points to the
 * original message stored in that element, but sometimes it points to a later
 * message in the list.  See cause_errthrow().
 */
typedef struct msglist msglist_T;
struct msglist
{
    char	*msg;		// original message, allocated
    char	*throw_msg;	// msg to throw: usually original one
    char_u	*sfile;		// value from estack_sfile(), allocated
    long	slnum;		// line number for "sfile"
    msglist_T	*next;		// next of several messages in a row
};

/*
 * The exception types.
 */
typedef enum
{
    ET_USER,		// exception caused by ":throw" command
    ET_ERROR,		// error exception
    ET_INTERRUPT,	// interrupt exception triggered by Ctrl-C
} except_type_T;

/*
 * Structure describing an exception.
 * (don't use "struct exception", it's used by the math library).
 */
typedef struct vim_exception except_T;
struct vim_exception
{
    except_type_T	type;		// exception type
    char		*value;		// exception value
    struct msglist	*messages;	// message(s) causing error exception
    char_u		*throw_name;	// name of the throw point
    linenr_T		throw_lnum;	// line number of the throw point
    except_T		*caught;	// next exception on the caught stack
};

/*
 * Structure to save the error/interrupt/exception state between calls to
 * enter_cleanup() and leave_cleanup().  Must be allocated as an automatic
 * variable by the (common) caller of these functions.
 */
typedef struct cleanup_stuff cleanup_T;
struct cleanup_stuff
{
    int pending;		// error/interrupt/exception state
    except_T *exception;	// exception value
};

#ifdef FEAT_SYN_HL
// struct passed to in_id_list()
struct sp_syn
{
    int		inc_tag;	// ":syn include" unique tag
    short	id;		// highlight group ID of item
    short	*cont_in_list;	// cont.in group IDs, if non-zero
};

/*
 * Each keyword has one keyentry, which is linked in a hash list.
 */
typedef struct keyentry keyentry_T;

struct keyentry
{
    keyentry_T	*ke_next;	// next entry with identical "keyword[]"
    struct sp_syn k_syn;	// struct passed to in_id_list()
    short	*next_list;	// ID list for next match (if non-zero)
    int		flags;
    int		k_char;		// conceal substitute character
    char_u	keyword[1];	// actually longer
};

/*
 * Struct used to store one state of the state stack.
 */
typedef struct buf_state
{
    int		    bs_idx;	 // index of pattern
    int		    bs_flags;	 // flags for pattern
#ifdef FEAT_CONCEAL
    int		    bs_seqnr;	 // stores si_seqnr
    int		    bs_cchar;	 // stores si_cchar
#endif
    reg_extmatch_T *bs_extmatch; // external matches from start pattern
} bufstate_T;

/*
 * syn_state contains the syntax state stack for the start of one line.
 * Used by b_sst_array[].
 */
typedef struct syn_state synstate_T;

struct syn_state
{
    synstate_T	*sst_next;	// next entry in used or free list
    linenr_T	sst_lnum;	// line number for this state
    union
    {
	bufstate_T	sst_stack[SST_FIX_STATES]; // short state stack
	garray_T	sst_ga;	// growarray for long state stack
    } sst_union;
    int		sst_next_flags;	// flags for sst_next_list
    int		sst_stacksize;	// number of states on the stack
    short	*sst_next_list;	// "nextgroup" list in this state
				// (this is a copy, don't free it!
    disptick_T	sst_tick;	// tick when last displayed
    linenr_T	sst_change_lnum;// when non-zero, change in this line
				// may have made the state invalid
};
#endif // FEAT_SYN_HL

#define MAX_HL_ID       20000	// maximum value for a highlight ID.

/*
 * Structure shared between syntax.c, screen.c and gui_x11.c.
 */
typedef struct attr_entry
{
    short	    ae_attr;		// HL_BOLD, etc.
    union
    {
	struct
	{
	    char_u	    *start;	// start escape sequence
	    char_u	    *stop;	// stop escape sequence
	} term;
	struct
	{
	    // These colors need to be > 8 bits to hold 256.
	    short_u	    fg_color;	// foreground color number
	    short_u	    bg_color;	// background color number
	    short_u	    ul_color;	// underline color number
# ifdef FEAT_TERMGUICOLORS
	    guicolor_T	    fg_rgb;	// foreground color RGB
	    guicolor_T	    bg_rgb;	// background color RGB
	    guicolor_T	    ul_rgb;	// underline color RGB
# endif
	} cterm;
# ifdef FEAT_GUI
	struct
	{
	    guicolor_T	    fg_color;	// foreground color handle
	    guicolor_T	    bg_color;	// background color handle
	    guicolor_T	    sp_color;	// special color handle
	    GuiFont	    font;	// font handle
#  ifdef FEAT_XFONTSET
	    GuiFontset	    fontset;	// fontset handle
#  endif
	} gui;
# endif
    } ae_u;
} attrentry_T;

#ifdef USE_ICONV
# ifdef HAVE_ICONV_H
#  include <iconv.h>
# else
#  if defined(MACOS_X)
#   include <sys/errno.h>
#   ifndef EILSEQ
#    define EILSEQ ENOENT // Early MacOS X does not have EILSEQ
#   endif
typedef struct _iconv_t *iconv_t;
#  else
#   include <errno.h>
#  endif
typedef void *iconv_t;
# endif
#endif

/*
 * Used for the typeahead buffer: typebuf.
 */
typedef struct
{
    char_u	*tb_buf;	// buffer for typed characters
    char_u	*tb_noremap;	// mapping flags for characters in tb_buf[]
    int		tb_buflen;	// size of tb_buf[]
    int		tb_off;		// current position in tb_buf[]
    int		tb_len;		// number of valid bytes in tb_buf[]
    int		tb_maplen;	// nr of mapped bytes in tb_buf[]
    int		tb_silent;	// nr of silently mapped bytes in tb_buf[]
    int		tb_no_abbr_cnt; // nr of bytes without abbrev. in tb_buf[]
    int		tb_change_cnt;	// nr of time tb_buf was changed; never zero
} typebuf_T;

// Struct to hold the saved typeahead for save_typeahead().
typedef struct
{
    typebuf_T		save_typebuf;
    int			typebuf_valid;	    // TRUE when save_typebuf valid
    int			old_char;
    int			old_mod_mask;
    buffheader_T	save_readbuf1;
    buffheader_T	save_readbuf2;
#ifdef USE_INPUT_BUF
    char_u		*save_inputbuf;
#endif
} tasave_T;

/*
 * Used for conversion of terminal I/O and script files.
 */
typedef struct
{
    int		vc_type;	// zero or one of the CONV_ values
    int		vc_factor;	// max. expansion factor
# ifdef MSWIN
    int		vc_cpfrom;	// codepage to convert from (CONV_CODEPAGE)
    int		vc_cpto;	// codepage to convert to (CONV_CODEPAGE)
# endif
# ifdef USE_ICONV
    iconv_t	vc_fd;		// for CONV_ICONV
# endif
    int		vc_fail;	// fail for invalid char, don't use '?'
} vimconv_T;

/*
 * Structure used for the command line history.
 */
typedef struct hist_entry
{
    int		hisnum;		// identifying number
    int		viminfo;	// when TRUE hisstr comes from viminfo
    char_u	*hisstr;	// actual entry, separator char after the NUL
    time_t	time_set;	// when it was typed, zero if unknown
} histentry_T;

#define CONV_NONE		0
#define CONV_TO_UTF8		1
#define CONV_9_TO_UTF8		2
#define CONV_TO_LATIN1		3
#define CONV_TO_LATIN9		4
#define CONV_ICONV		5
#ifdef MSWIN
# define CONV_CODEPAGE		10	// codepage -> codepage
#endif
#ifdef MACOS_X
# define CONV_MAC_LATIN1	20
# define CONV_LATIN1_MAC	21
# define CONV_MAC_UTF8		22
# define CONV_UTF8_MAC		23
#endif

/*
 * Structure used for mappings and abbreviations.
 */
typedef struct mapblock mapblock_T;
struct mapblock
{
    mapblock_T	*m_next;	// next mapblock in list
    char_u	*m_keys;	// mapped from, lhs
    char_u	*m_str;		// mapped to, rhs
    char_u	*m_orig_str;	// rhs as entered by the user
    int		m_keylen;	// strlen(m_keys)
    int		m_mode;		// valid mode
    int		m_simplified;	// m_keys was simplified, do not use this map
				// if seenModifyOtherKeys is TRUE
    int		m_noremap;	// if non-zero no re-mapping for m_str
    char	m_silent;	// <silent> used, don't echo commands
    char	m_nowait;	// <nowait> used
#ifdef FEAT_EVAL
    char	m_expr;		// <expr> used, m_str is an expression
    sctx_T	m_script_ctx;	// SCTX where map was defined
#endif
};

/*
 * Used for highlighting in the status line.
 */
struct stl_hlrec
{
    char_u	*start;
    int		userhl;		// 0: no HL, 1-9: User HL, < 0 for syn ID
};


/*
 * Syntax items - usually buffer-specific.
 */

/*
 * Item for a hashtable.  "hi_key" can be one of three values:
 * NULL:	   Never been used
 * HI_KEY_REMOVED: Entry was removed
 * Otherwise:	   Used item, pointer to the actual key; this usually is
 *		   inside the item, subtract an offset to locate the item.
 *		   This reduces the size of hashitem by 1/3.
 */
typedef struct hashitem_S
{
    long_u	hi_hash;	// cached hash number of hi_key
    char_u	*hi_key;
} hashitem_T;

// The address of "hash_removed" is used as a magic number for hi_key to
// indicate a removed item.
#define HI_KEY_REMOVED &hash_removed
#define HASHITEM_EMPTY(hi) ((hi)->hi_key == NULL || (hi)->hi_key == &hash_removed)

// Initial size for a hashtable.  Our items are relatively small and growing
// is expensive, thus use 16 as a start.  Must be a power of 2.
// This allows for storing 10 items (2/3 of 16) before a resize is needed.
#define HT_INIT_SIZE 16

typedef struct hashtable_S
{
    long_u	ht_mask;	// mask used for hash value (nr of items in
				// array is "ht_mask" + 1)
    long_u	ht_used;	// number of items used
    long_u	ht_filled;	// number of items used + removed
    int		ht_changed;	// incremented when adding or removing an item
    int		ht_locked;	// counter for hash_lock()
    int		ht_error;	// when set growing failed, can't add more
				// items before growing works
    hashitem_T	*ht_array;	// points to the array, allocated when it's
				// not "ht_smallarray"
    hashitem_T	ht_smallarray[HT_INIT_SIZE];   // initial array
} hashtab_T;

typedef long_u hash_T;		// Type for hi_hash


// Use 64-bit Number.
#ifdef MSWIN
# ifdef PROTO
   // workaround for cproto that doesn't recognize __int64
   typedef long			varnumber_T;
   typedef unsigned long	uvarnumber_T;
#  define VARNUM_MIN		LONG_MIN
#  define VARNUM_MAX		LONG_MAX
#  define UVARNUM_MAX		ULONG_MAX
# else
   typedef __int64		varnumber_T;
   typedef unsigned __int64	uvarnumber_T;
#  define VARNUM_MIN		_I64_MIN
#  define VARNUM_MAX		_I64_MAX
#  define UVARNUM_MAX		_UI64_MAX
# endif
#elif defined(HAVE_NO_LONG_LONG)
# if defined(HAVE_STDINT_H)
   typedef int64_t		varnumber_T;
   typedef uint64_t		uvarnumber_T;
#  define VARNUM_MIN		INT64_MIN
#  define VARNUM_MAX		INT64_MAX
#  define UVARNUM_MAX		UINT64_MAX
# else
   // this may cause trouble for code that depends on 64 bit ints
   typedef long			varnumber_T;
   typedef unsigned long	uvarnumber_T;
#  define VARNUM_MIN		LONG_MIN
#  define VARNUM_MAX		LONG_MAX
#  define UVARNUM_MAX		ULONG_MAX
# endif
#else
  typedef long long		varnumber_T;
  typedef unsigned long long	uvarnumber_T;
# ifdef LLONG_MIN
#  define VARNUM_MIN		LLONG_MIN
#  define VARNUM_MAX		LLONG_MAX
#  define UVARNUM_MAX		ULLONG_MAX
# else
#  define VARNUM_MIN		LONG_LONG_MIN
#  define VARNUM_MAX		LONG_LONG_MAX
#  define UVARNUM_MAX		ULONG_LONG_MAX
# endif
#endif

// On rare systems "char" is unsigned, sometimes we really want a signed 8-bit
// value.
typedef signed char int8_T;

typedef double	float_T;

typedef struct listvar_S list_T;
typedef struct dictvar_S dict_T;
typedef struct partial_S partial_T;
typedef struct blobvar_S blob_T;

// Struct that holds both a normal function name and a partial_T, as used for a
// callback argument.
// When used temporarily "cb_name" is not allocated.  The refcounts to either
// the function or the partial are incremented and need to be decremented
// later with free_callback().
typedef struct {
    char_u	*cb_name;
    partial_T	*cb_partial;
    int		cb_free_name;	    // cb_name was allocated
} callback_T;

typedef struct isn_S isn_T;	    // instruction
typedef struct dfunc_S dfunc_T;	    // :def function

typedef struct jobvar_S job_T;
typedef struct readq_S readq_T;
typedef struct writeq_S writeq_T;
typedef struct jsonq_S jsonq_T;
typedef struct cbq_S cbq_T;
typedef struct channel_S channel_T;
typedef struct cctx_S cctx_T;

typedef enum
{
    VAR_UNKNOWN = 0,	// not set, any type or "void" allowed
    VAR_ANY,		// used for "any" type
    VAR_VOID,		// no value (function not returning anything)
    VAR_BOOL,		// "v_number" is used: VVAL_TRUE or VVAL_FALSE
    VAR_SPECIAL,	// "v_number" is used: VVAL_NULL or VVAL_NONE
    VAR_NUMBER,		// "v_number" is used
    VAR_FLOAT,		// "v_float" is used
    VAR_STRING,		// "v_string" is used
    VAR_BLOB,		// "v_blob" is used
    VAR_FUNC,		// "v_string" is function name
    VAR_PARTIAL,	// "v_partial" is used
    VAR_LIST,		// "v_list" is used
    VAR_DICT,		// "v_dict" is used
    VAR_JOB,		// "v_job" is used
    VAR_CHANNEL,	// "v_channel" is used
} vartype_T;

// A type specification.
typedef struct type_S type_T;
struct type_S {
    vartype_T	    tt_type;
    int8_T	    tt_argcount;    // for func, incl. vararg, -1 for unknown
    char	    tt_min_argcount; // number of non-optional arguments
    char	    tt_flags;	    // TTFLAG_ values
    type_T	    *tt_member;	    // for list, dict, func return type
    type_T	    **tt_args;	    // func argument types, allocated
};

#define TTFLAG_VARARGS	1	    // func args ends with "..."
#define TTFLAG_OPTARG	2	    // func arg type with "?"

/*
 * Structure to hold an internal variable without a name.
 */
typedef struct
{
    vartype_T	v_type;
    char	v_lock;	    // see below: VAR_LOCKED, VAR_FIXED
    union
    {
	varnumber_T	v_number;	// number value
#ifdef FEAT_FLOAT
	float_T		v_float;	// floating number value
#endif
	char_u		*v_string;	// string value (can be NULL!)
	list_T		*v_list;	// list value (can be NULL!)
	dict_T		*v_dict;	// dict value (can be NULL!)
	partial_T	*v_partial;	// closure: function with args
#ifdef FEAT_JOB_CHANNEL
	job_T		*v_job;		// job value (can be NULL!)
	channel_T	*v_channel;	// channel value (can be NULL!)
#endif
	blob_T		*v_blob;	// blob value (can be NULL!)
    }		vval;
} typval_T;

// Values for "dv_scope".
#define VAR_SCOPE     1	// a:, v:, s:, etc. scope dictionaries
#define VAR_DEF_SCOPE 2	// l:, g: scope dictionaries: here funcrefs are not
			// allowed to mask existing functions

// Values for "v_lock".
#define VAR_LOCKED  1	// locked with lock(), can use unlock()
#define VAR_FIXED   2	// locked forever

/*
 * Structure to hold an item of a list: an internal variable without a name.
 */
typedef struct listitem_S listitem_T;

struct listitem_S
{
    listitem_T	*li_next;	// next item in list
    listitem_T	*li_prev;	// previous item in list
    typval_T	li_tv;		// type and value of the variable
};

// Struct used by those that are using an item in a list.
typedef struct listwatch_S listwatch_T;

struct listwatch_S
{
    listitem_T		*lw_item;	// item being watched
    listwatch_T		*lw_next;	// next watcher
};

/*
 * Structure to hold info about a list.
 * Order of members is optimized to reduce padding.
 * When created by range() it will at first have special value:
 *  lv_first == &range_list_item;
 * and use lv_start, lv_end, lv_stride.
 */
struct listvar_S
{
    listitem_T	*lv_first;	// first item, NULL if none
    listwatch_T	*lv_watch;	// first watcher, NULL if none
    union {
	struct {	// used for non-materialized range list:
			// "lv_first" is &range_list_item
	    varnumber_T lv_start;
	    varnumber_T lv_end;
	    int		lv_stride;
	} nonmat;
	struct {	// used for materialized list
	    listitem_T	*lv_last;	// last item, NULL if none
	    listitem_T	*lv_idx_item;	// when not NULL item at index "lv_idx"
	    int		lv_idx;		// cached index of an item
	} mat;
    } lv_u;
    list_T	*lv_copylist;	// copied list used by deepcopy()
    list_T	*lv_used_next;	// next list in used lists list
    list_T	*lv_used_prev;	// previous list in used lists list
    int		lv_refcount;	// reference count
    int		lv_len;		// number of items
    int		lv_with_items;	// number of items following this struct that
				// should not be freed
    int		lv_copyID;	// ID used by deepcopy()
    char	lv_lock;	// zero, VAR_LOCKED, VAR_FIXED
};

/*
 * Static list with 10 items.  Use init_static_list() to initialize.
 */
typedef struct {
    list_T	sl_list;	// must be first
    listitem_T	sl_items[10];
} staticList10_T;

/*
 * Structure to hold an item of a Dictionary.
 * Also used for a variable.
 * The key is copied into "di_key" to avoid an extra alloc/free for it.
 */
struct dictitem_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_key[1];	// key (actually longer!)
};
typedef struct dictitem_S dictitem_T;

/*
 * A dictitem with a 16 character key (plus NUL).  This is an efficient way to
 * have a fixed-size dictitem.
 */
#define DICTITEM16_KEY_LEN 16
struct dictitem16_S
{
    typval_T	di_tv;		// type and value of the variable
    char_u	di_flags;	// DI_FLAGS_ flags (only used for variable)
    char_u	di_key[DICTITEM16_KEY_LEN + 1];	// key
};
typedef struct dictitem16_S dictitem16_T;

// Flags for "di_flags"
#define DI_FLAGS_RO	   0x01	    // read-only variable
#define DI_FLAGS_RO_SBX	   0x02	    // read-only in the sandbox
#define DI_FLAGS_FIX	   0x04	    // fixed: no :unlet or remove()
#define DI_FLAGS_LOCK	   0x08	    // locked variable
#define DI_FLAGS_ALLOC	   0x10	    // separately allocated
#define DI_FLAGS_RELOAD	   0x20	    // set when script sourced again

/*
 * Structure to hold info about a Dictionary.
 */
struct dictvar_S
{
    char	dv_lock;	// zero, VAR_LOCKED, VAR_FIXED
    char	dv_scope;	// zero, VAR_SCOPE, VAR_DEF_SCOPE
    int		dv_refcount;	// reference count
    int		dv_copyID;	// ID used by deepcopy()
    hashtab_T	dv_hashtab;	// hashtab that refers to the items
    dict_T	*dv_copydict;	// copied dict used by deepcopy()
    dict_T	*dv_used_next;	// next dict in used dicts list
    dict_T	*dv_used_prev;	// previous dict in used dicts list
};

/*
 * Structure to hold info about a blob.
 */
struct blobvar_S
{
    garray_T	bv_ga;		// growarray with the data
    int		bv_refcount;	// reference count
    char	bv_lock;	// zero, VAR_LOCKED, VAR_FIXED
};

typedef int (*cfunc_T)(int argcount, typval_T *argvars, typval_T *rettv, void *state);
typedef void (*cfunc_free_T)(void *state);

#if defined(FEAT_EVAL) || defined(PROTO)
typedef struct funccall_S funccall_T;

// values used for "uf_dfunc_idx"
typedef enum {
    UF_NOT_COMPILED,
    UF_TO_BE_COMPILED,
    UF_COMPILING,
    UF_COMPILED
} def_status_T;

/*
 * Structure to hold info for a user function.
 */
typedef struct
{
    int		uf_varargs;	// variable nr of arguments (old style)
    int		uf_flags;	// FC_ flags
    int		uf_calls;	// nr of active calls
    int		uf_cleared;	// func_clear() was already called
    def_status_T uf_def_status; // UF_NOT_COMPILED, UF_TO_BE_COMPILED, etc.
    int		uf_dfunc_idx;	// only valid if uf_def_status is UF_COMPILED
    garray_T	uf_args;	// arguments, including optional arguments
    garray_T	uf_def_args;	// default argument expressions

    // for :def (for :function uf_ret_type is NULL)
    type_T	**uf_arg_types;	// argument types (count == uf_args.ga_len)
    type_T	*uf_ret_type;	// return type
    garray_T	uf_type_list;	// types used in arg and return types
    int		*uf_def_arg_idx; // instruction indexes for evaluating
				// uf_def_args; length: uf_def_args.ga_len + 1
    char_u	*uf_va_name;	// name from "...name" or NULL
    type_T	*uf_va_type;	// type from "...name: type" or NULL
    type_T	*uf_func_type;	// type of the function, &t_func_any if unknown
# if defined(FEAT_LUA)
    cfunc_T     uf_cb;		// callback function for cfunc
    cfunc_free_T uf_cb_free;    // callback function to free cfunc
    void        *uf_cb_state;   // state of uf_cb
# endif

    garray_T	uf_lines;	// function lines
# ifdef FEAT_PROFILE
    int		uf_profiling;	// TRUE when func is being profiled
    int		uf_prof_initialized;
    // profiling the function as a whole
    int		uf_tm_count;	// nr of calls
    proftime_T	uf_tm_total;	// time spent in function + children
    proftime_T	uf_tm_self;	// time spent in function itself
    proftime_T	uf_tm_children;	// time spent in children this call
    // profiling the function per line
    int		*uf_tml_count;	// nr of times line was executed
    proftime_T	*uf_tml_total;	// time spent in a line + children
    proftime_T	*uf_tml_self;	// time spent in a line itself
    proftime_T	uf_tml_start;	// start time for current line
    proftime_T	uf_tml_children; // time spent in children for this line
    proftime_T	uf_tml_wait;	// start wait time for current line
    int		uf_tml_idx;	// index of line being timed; -1 if none
    int		uf_tml_execed;	// line being timed was executed
# endif
    sctx_T	uf_script_ctx;	// SCTX where function was defined,
				// used for s: variables
    int		uf_refcount;	// reference count, see func_name_refcount()

    funccall_T	*uf_scoped;	// l: local variables for closure

    char_u	*uf_name_exp;	// if "uf_name[]" starts with SNR the name with
				// "<SNR>" as a string, otherwise NULL
    char_u	uf_name[1];	// name of function (actually longer); can
				// start with <SNR>123_ (<SNR> is K_SPECIAL
				// KS_EXTRA KE_SNR)
} ufunc_T;

// flags used in uf_flags
#define FC_ABORT    0x01	// abort function on error
#define FC_RANGE    0x02	// function accepts range
#define FC_DICT	    0x04	// Dict function, uses "self"
#define FC_CLOSURE  0x08	// closure, uses outer scope variables
#define FC_DELETED  0x10	// :delfunction used while uf_refcount > 0
#define FC_REMOVED  0x20	// function redefined while uf_refcount > 0
#define FC_SANDBOX  0x40	// function defined in the sandbox
#define FC_DEAD	    0x80	// function kept only for reference to dfunc
#define FC_EXPORT   0x100	// "export def Func()"
#define FC_NOARGS   0x200	// no a: variables in lambda
#define FC_VIM9	    0x400	// defined in vim9 script file
#define FC_CFUNC    0x800	// defined as Lua C func

#define MAX_FUNC_ARGS	20	// maximum number of function arguments
#define VAR_SHORT_LEN	20	// short variable name length
#define FIXVAR_CNT	12	// number of fixed variables

/*
 * Structure to hold info for a function that is currently being executed.
 */
struct funccall_S
{
    ufunc_T	*func;		// function being called
    int		linenr;		// next line to be executed
    int		returned;	// ":return" used
    struct			// fixed variables for arguments
    {
	dictitem_T	var;		// variable (without room for name)
	char_u	room[VAR_SHORT_LEN];	// room for the name
    } fixvar[FIXVAR_CNT];
    dict_T	l_vars;		// l: local function variables
    dictitem_T	l_vars_var;	// variable for l: scope
    dict_T	l_avars;	// a: argument variables
    dictitem_T	l_avars_var;	// variable for a: scope
    list_T	l_varlist;	// list for a:000
    listitem_T	l_listitems[MAX_FUNC_ARGS];	// listitems for a:000
    typval_T	*rettv;		// return value
    linenr_T	breakpoint;	// next line with breakpoint or zero
    int		dbg_tick;	// debug_tick when breakpoint was set
    int		level;		// top nesting level of executed function
#ifdef FEAT_PROFILE
    proftime_T	prof_child;	// time spent in a child
#endif
    funccall_T	*caller;	// calling function or NULL

    // for closure
    int		fc_refcount;	// number of user functions that reference this
				// funccal
    int		fc_copyID;	// for garbage collection
    garray_T	fc_funcs;	// list of ufunc_T* which keep a reference to
				// "func"
};

/*
 * Struct used by trans_function_name()
 */
typedef struct
{
    dict_T	*fd_dict;	// Dictionary used
    char_u	*fd_newkey;	// new key in "dict" in allocated memory
    dictitem_T	*fd_di;		// Dictionary item used
} funcdict_T;

typedef struct funccal_entry funccal_entry_T;
struct funccal_entry {
    void	    *top_funccal;
    funccal_entry_T *next;
};

// From user function to hashitem and back.
#define UF2HIKEY(fp) ((fp)->uf_name)
#define HIKEY2UF(p)  ((ufunc_T *)((p) - offsetof(ufunc_T, uf_name)))
#define HI2UF(hi)     HIKEY2UF((hi)->hi_key)

/*
 * Holds the hashtab with variables local to each sourced script.
 * Each item holds a variable (nameless) that points to the dict_T.
 */
typedef struct
{
    dictitem_T	sv_var;
    dict_T	sv_dict;
} scriptvar_T;

/*
 * Entry for "sn_var_vals".  Used for script-local variables.
 */
typedef struct {
    char_u	*sv_name;	// points into "sn_vars" di_key
    typval_T	*sv_tv;		// points into "sn_vars" di_tv
    type_T	*sv_type;
    int		sv_const;
    int		sv_export;	// "export let var = val"
} svar_T;

typedef struct {
    char_u	*imp_name;	    // name imported as (allocated)
    int		imp_sid;	    // script ID of "from"

    // for "import * as Name", "imp_name" is "Name"
    int		imp_all;

    // for variable
    type_T	*imp_type;
    int		imp_var_vals_idx;   // index in sn_var_vals of "from"

    // for function
    char_u	*imp_funcname;	    // user func name (NOT allocated)
} imported_T;

/*
 * Growarray to store info about already sourced scripts.
 * For Unix also store the dev/ino, so that we don't have to stat() each
 * script when going through the list.
 */
typedef struct
{
    char_u	*sn_name;

    scriptvar_T	*sn_vars;	// stores s: variables for this script
    garray_T	sn_var_vals;	// same variables as a list of svar_T

    garray_T	sn_imports;	// imported items, imported_T

    garray_T	sn_type_list;	// keeps types used by variables

    int		sn_version;	// :scriptversion
    int		sn_had_command;	// TRUE if any command was executed
    char_u	*sn_save_cpo;	// 'cpo' value when :vim9script found

# ifdef FEAT_PROFILE
    int		sn_prof_on;	// TRUE when script is/was profiled
    int		sn_pr_force;	// forceit: profile functions in this script
    proftime_T	sn_pr_child;	// time set when going into first child
    int		sn_pr_nest;	// nesting for sn_pr_child
    // profiling the script as a whole
    int		sn_pr_count;	// nr of times sourced
    proftime_T	sn_pr_total;	// time spent in script + children
    proftime_T	sn_pr_self;	// time spent in script itself
    proftime_T	sn_pr_start;	// time at script start
    proftime_T	sn_pr_children; // time in children after script start
    // profiling the script per line
    garray_T	sn_prl_ga;	// things stored for every line
    proftime_T	sn_prl_start;	// start time for current line
    proftime_T	sn_prl_children; // time spent in children for this line
    proftime_T	sn_prl_wait;	// wait start time for current line
    int		sn_prl_idx;	// index of line being timed; -1 if none
    int		sn_prl_execed;	// line being timed was executed
# endif
} scriptitem_T;

// Struct passed through eval() functions.
// See EVALARG_EVALUATE for a fixed value with eval_flags set to EVAL_EVALUATE.
typedef struct {
    int		eval_flags;	    // EVAL_ flag values below
    int		eval_break_count;   // nr of line breaks consumed

    // copied from exarg_T when "getline" is "getsourceline". Can be NULL.
    char_u	*(*eval_getline)(int, void *, int, int);
    void	*eval_cookie;	    // argument for eval_getline()

    // used when compiling a :def function, NULL otherwise
    cctx_T	*eval_cctx;

    // Used to collect lines while parsing them, so that they can be
    // concatenated later.  Used when "eval_ga.ga_itemsize" is not zero.
    // "eval_ga.ga_data" is a list of pointers to lines.
    garray_T	eval_ga;

    // pointer to the last line obtained with getsourceline()
    char_u	*eval_tofree;

    // pointer to the lines concatenated for a lambda.
    char_u	*eval_tofree_lambda;
} evalarg_T;

// Flags for expression evaluation.
#define EVAL_EVALUATE	    1	    // when missing don't actually evaluate
#define EVAL_CONSTANT	    2	    // when not a constant return FAIL

# ifdef FEAT_PROFILE
/*
 * Struct used in sn_prl_ga for every line of a script.
 */
typedef struct sn_prl_S
{
    int		snp_count;	// nr of times line was executed
    proftime_T	sn_prl_total;	// time spent in a line + children
    proftime_T	sn_prl_self;	// time spent in a line itself
} sn_prl_T;

#  define PRL_ITEM(si, idx)	(((sn_prl_T *)(si)->sn_prl_ga.ga_data)[(idx)])
# endif
#else
// dummy typedefs for use in function prototypes
typedef struct
{
    int	    dummy;
} ufunc_T;
typedef struct
{
    int	    dummy;
} funccall_T;
typedef struct
{
    int	    dummy;
} funcdict_T;
typedef struct
{
    int	    dummy;
} funccal_entry_T;
typedef struct
{
    int	    dummy;
} scriptitem_T;
typedef struct
{
    int	    dummy;
} evalarg_T;
#endif

// Struct passed between functions dealing with function call execution.
//
// "argv_func", when not NULL, can be used to fill in arguments only when the
// invoked function uses them.  It is called like this:
//   new_argcount = argv_func(current_argcount, argv, partial_argcount,
//							called_func_argcount)
//
typedef struct {
    int		(* argv_func)(int, typval_T *, int, int);
    linenr_T	firstline;	// first line of range
    linenr_T	lastline;	// last line of range
    int		*doesrange;	// if not NULL: return: function handled range
    int		evaluate;	// actually evaluate expressions
    partial_T	*partial;	// for extra arguments
    dict_T	*selfdict;	// Dictionary for "self"
    typval_T	*basetv;	// base for base->method()
} funcexe_T;

/*
 * Structure to hold the context of a compiled function, used by closures
 * defined in that function.
 */
typedef struct funcstack_S
{
    garray_T	fs_ga;		// contains the stack, with:
				// - arguments
				// - frame
				// - local variables

    int		fs_refcount;	// nr of closures referencing this funcstack
    int		fs_copyID;	// for garray_T collection
} funcstack_T;

struct partial_S
{
    int		pt_refcount;	// reference count
    char_u	*pt_name;	// function name; when NULL use
				// pt_func->uf_name
    ufunc_T	*pt_func;	// function pointer; when NULL lookup function
				// with pt_name
    int		pt_auto;	// when TRUE the partial was created for using
				// dict.member in handle_subscript()

    // For a compiled closure: the arguments and local variables.
    garray_T	*pt_ectx_stack;	    // where to find local vars
    int		pt_ectx_frame;	    // index of function frame in uf_ectx_stack
    funcstack_T	*pt_funcstack;	    // copy of stack, used after context
				    // function returns

    int		pt_argc;	// number of arguments
    typval_T	*pt_argv;	// arguments in allocated array

    dict_T	*pt_dict;	// dict for "self"
    int		pt_copyID;	// funcstack may contain pointer to partial
};

typedef struct AutoPatCmd_S AutoPatCmd;

/*
 * Entry in the execution stack "exestack".
 */
typedef enum {
    ETYPE_TOP,		    // toplevel
    ETYPE_SCRIPT,           // sourcing script, use es_info.sctx
    ETYPE_UFUNC,            // user function, use es_info.ufunc
    ETYPE_AUCMD,            // autocomand, use es_info.aucmd
    ETYPE_MODELINE,         // modeline, use es_info.sctx
    ETYPE_EXCEPT,           // exception, use es_info.exception
    ETYPE_ARGS,             // command line argument
    ETYPE_ENV,              // environment variable
    ETYPE_INTERNAL,         // internal operation
    ETYPE_SPELL,            // loading spell file
} etype_T;

typedef struct {
    long      es_lnum;      // replaces "sourcing_lnum"
    char_u    *es_name;     // replaces "sourcing_name"
    etype_T   es_type;
    union {
	sctx_T  *sctx;      // script and modeline info
#if defined(FEAT_EVAL)
	ufunc_T *ufunc;     // function info
#endif
	AutoPatCmd *aucmd;  // autocommand info
	except_T   *except; // exception info
    } es_info;
#if defined(FEAT_EVAL)
    scid_T	es_save_sid;	    // saved sc_sid when calling function
#endif
} estack_T;

// Information returned by get_tty_info().
typedef struct {
    int backspace;	// what the Backspace key produces
    int enter;		// what the Enter key produces
    int interrupt;	// interrupt character
    int nl_does_cr;	// TRUE when a NL is expanded to CR-NL on output
} ttyinfo_T;

// Status of a job.  Order matters!
typedef enum
{
    JOB_FAILED,
    JOB_STARTED,
    JOB_ENDED,	    // detected job done
    JOB_FINISHED,   // job done and cleanup done
} jobstatus_T;

/*
 * Structure to hold info about a Job.
 */
struct jobvar_S
{
    job_T	*jv_next;
    job_T	*jv_prev;
#ifdef UNIX
    pid_t	jv_pid;
#endif
#ifdef MSWIN
    PROCESS_INFORMATION	jv_proc_info;
    HANDLE		jv_job_object;
#endif
    char_u	*jv_tty_in;	// controlling tty input, allocated
    char_u	*jv_tty_out;	// controlling tty output, allocated
    jobstatus_T	jv_status;
    char_u	*jv_stoponexit;	// allocated
#ifdef UNIX
    char_u	*jv_termsig;	// allocated
#endif
#ifdef MSWIN
    char_u	*jv_tty_type;	// allocated
#endif
    int		jv_exitval;
    callback_T	jv_exit_cb;

    buf_T	*jv_in_buf;	// buffer from "in-name"

    int		jv_refcount;	// reference count
    int		jv_copyID;

    channel_T	*jv_channel;	// channel for I/O, reference counted
    char	**jv_argv;	// command line used to start the job
};

/*
 * Structures to hold info about a Channel.
 */
struct readq_S
{
    char_u	*rq_buffer;
    long_u	rq_buflen;
    readq_T	*rq_next;
    readq_T	*rq_prev;
};

struct writeq_S
{
    garray_T	wq_ga;
    writeq_T	*wq_next;
    writeq_T	*wq_prev;
};

struct jsonq_S
{
    typval_T	*jq_value;
    jsonq_T	*jq_next;
    jsonq_T	*jq_prev;
    int		jq_no_callback; // TRUE when no callback was found
};

struct cbq_S
{
    callback_T	cq_callback;
    int		cq_seq_nr;
    cbq_T	*cq_next;
    cbq_T	*cq_prev;
};

// mode for a channel
typedef enum
{
    MODE_NL = 0,
    MODE_RAW,
    MODE_JSON,
    MODE_JS,
} ch_mode_T;

typedef enum {
    JIO_PIPE,	    // default
    JIO_NULL,
    JIO_FILE,
    JIO_BUFFER,
    JIO_OUT
} job_io_T;

#define CH_PART_FD(part)	ch_part[part].ch_fd

// Ordering matters, it is used in for loops: IN is last, only SOCK/OUT/ERR
// are polled.
typedef enum {
    PART_SOCK = 0,
#define CH_SOCK_FD	CH_PART_FD(PART_SOCK)
#ifdef FEAT_JOB_CHANNEL
    PART_OUT,
# define CH_OUT_FD	CH_PART_FD(PART_OUT)
    PART_ERR,
# define CH_ERR_FD	CH_PART_FD(PART_ERR)
    PART_IN,
# define CH_IN_FD	CH_PART_FD(PART_IN)
#endif
    PART_COUNT,
} ch_part_T;

#define INVALID_FD	(-1)

// The per-fd info for a channel.
typedef struct {
    sock_T	ch_fd;	    // socket/stdin/stdout/stderr, -1 if not used

# if defined(UNIX) && !defined(HAVE_SELECT)
    int		ch_poll_idx;	// used by channel_poll_setup()
# endif

#ifdef FEAT_GUI_X11
    XtInputId	ch_inputHandler; // Cookie for input
#endif
#ifdef FEAT_GUI_GTK
    gint	ch_inputHandler; // Cookie for input
#endif

    ch_mode_T	ch_mode;
    job_io_T	ch_io;
    int		ch_timeout;	// request timeout in msec

    readq_T	ch_head;	// header for circular raw read queue
    jsonq_T	ch_json_head;	// header for circular json read queue
    garray_T	ch_block_ids;	// list of IDs that channel_read_json_block()
				// is waiting for
    // When ch_wait_len is non-zero use ch_deadline to wait for incomplete
    // message to be complete. The value is the length of the incomplete
    // message when the deadline was set.  If it gets longer (something was
    // received) the deadline is reset.
    size_t	ch_wait_len;
#ifdef MSWIN
    DWORD	ch_deadline;
#else
    struct timeval ch_deadline;
#endif
    int		ch_block_write;	// for testing: 0 when not used, -1 when write
				// does not block, 1 simulate blocking
    int		ch_nonblocking;	// write() is non-blocking
    writeq_T	ch_writeque;	// header for write queue

    cbq_T	ch_cb_head;	// dummy node for per-request callbacks
    callback_T	ch_callback;	// call when a msg is not handled

    bufref_T	ch_bufref;	// buffer to read from or write to
    int		ch_nomodifiable; // TRUE when buffer can be 'nomodifiable'
    int		ch_nomod_error;	// TRUE when e_modifiable was given
    int		ch_buf_append;	// write appended lines instead top-bot
    linenr_T	ch_buf_top;	// next line to send
    linenr_T	ch_buf_bot;	// last line to send
} chanpart_T;

struct channel_S {
    channel_T	*ch_next;
    channel_T	*ch_prev;

    int		ch_id;		// ID of the channel
    int		ch_last_msg_id;	// ID of the last message

    chanpart_T	ch_part[PART_COUNT]; // info for socket, out, err and in
    int		ch_write_text_mode; // write buffer lines with CR, not NL

    char	*ch_hostname;	// only for socket, allocated
    int		ch_port;	// only for socket

    int		ch_to_be_closed; // bitset of readable fds to be closed.
				 // When all readable fds have been closed,
				 // set to (1 << PART_COUNT).
    int		ch_to_be_freed; // When TRUE channel must be freed when it's
				// safe to invoke callbacks.
    int		ch_error;	// When TRUE an error was reported.  Avoids
				// giving pages full of error messages when
				// the other side has exited, only mention the
				// first error until the connection works
				// again.

    void	(*ch_nb_close_cb)(void);
				// callback for Netbeans when channel is
				// closed

#ifdef MSWIN
    int		ch_named_pipe;	// using named pipe instead of pty
#endif
    callback_T	ch_callback;	// call when any msg is not handled
    callback_T	ch_close_cb;	// call when channel is closed
    int		ch_drop_never;
    int		ch_keep_open;	// do not close on read error
    int		ch_nonblock;

    job_T	*ch_job;	// Job that uses this channel; this does not
				// count as a reference to avoid a circular
				// reference, the job refers to the channel.
    int		ch_job_killed;	// TRUE when there was a job and it was killed
				// or we know it died.
    int		ch_anonymous_pipe;  // ConPTY
    int		ch_killing;	    // TerminateJobObject() was called

    int		ch_refcount;	// reference count
    int		ch_copyID;
};

#define JO_MODE		    0x0001	// channel mode
#define JO_IN_MODE	    0x0002	// stdin mode
#define JO_OUT_MODE	    0x0004	// stdout mode
#define JO_ERR_MODE	    0x0008	// stderr mode
#define JO_CALLBACK	    0x0010	// channel callback
#define JO_OUT_CALLBACK	    0x0020	// stdout callback
#define JO_ERR_CALLBACK	    0x0040	// stderr callback
#define JO_CLOSE_CALLBACK   0x0080	// "close_cb"
#define JO_WAITTIME	    0x0100	// only for ch_open()
#define JO_TIMEOUT	    0x0200	// all timeouts
#define JO_OUT_TIMEOUT	    0x0400	// stdout timeouts
#define JO_ERR_TIMEOUT	    0x0800	// stderr timeouts
#define JO_PART		    0x1000	// "part"
#define JO_ID		    0x2000	// "id"
#define JO_STOPONEXIT	    0x4000	// "stoponexit"
#define JO_EXIT_CB	    0x8000	// "exit_cb"
#define JO_OUT_IO	    0x10000	// "out_io"
#define JO_ERR_IO	    0x20000	// "err_io" (JO_OUT_IO << 1)
#define JO_IN_IO	    0x40000	// "in_io" (JO_OUT_IO << 2)
#define JO_OUT_NAME	    0x80000	// "out_name"
#define JO_ERR_NAME	    0x100000	// "err_name" (JO_OUT_NAME << 1)
#define JO_IN_NAME	    0x200000	// "in_name" (JO_OUT_NAME << 2)
#define JO_IN_TOP	    0x400000	// "in_top"
#define JO_IN_BOT	    0x800000	// "in_bot"
#define JO_OUT_BUF	    0x1000000	// "out_buf"
#define JO_ERR_BUF	    0x2000000	// "err_buf" (JO_OUT_BUF << 1)
#define JO_IN_BUF	    0x4000000	// "in_buf" (JO_OUT_BUF << 2)
#define JO_CHANNEL	    0x8000000	// "channel"
#define JO_BLOCK_WRITE	    0x10000000	// "block_write"
#define JO_OUT_MODIFIABLE   0x20000000	// "out_modifiable"
#define JO_ERR_MODIFIABLE   0x40000000	// "err_modifiable" (JO_OUT_ << 1)
#define JO_ALL		    0x7fffffff

#define JO2_OUT_MSG	    0x0001	// "out_msg"
#define JO2_ERR_MSG	    0x0002	// "err_msg" (JO_OUT_ << 1)
#define JO2_TERM_NAME	    0x0004	// "term_name"
#define JO2_TERM_FINISH	    0x0008	// "term_finish"
#define JO2_ENV		    0x0010	// "env"
#define JO2_CWD		    0x0020	// "cwd"
#define JO2_TERM_ROWS	    0x0040	// "term_rows"
#define JO2_TERM_COLS	    0x0080	// "term_cols"
#define JO2_VERTICAL	    0x0100	// "vertical"
#define JO2_CURWIN	    0x0200	// "curwin"
#define JO2_HIDDEN	    0x0400	// "hidden"
#define JO2_TERM_OPENCMD    0x0800	// "term_opencmd"
#define JO2_EOF_CHARS	    0x1000	// "eof_chars"
#define JO2_NORESTORE	    0x2000	// "norestore"
#define JO2_TERM_KILL	    0x4000	// "term_kill"
#define JO2_ANSI_COLORS	    0x8000	// "ansi_colors"
#define JO2_TTY_TYPE	    0x10000	// "tty_type"
#define JO2_BUFNR	    0x20000	// "bufnr"
#define JO2_TERM_API	    0x40000	// "term_api"
#define JO2_TERM_HIGHLIGHT  0x80000	// "highlight"

#define JO_MODE_ALL	(JO_MODE + JO_IN_MODE + JO_OUT_MODE + JO_ERR_MODE)
#define JO_CB_ALL \
    (JO_CALLBACK + JO_OUT_CALLBACK + JO_ERR_CALLBACK + JO_CLOSE_CALLBACK)
#define JO_TIMEOUT_ALL	(JO_TIMEOUT + JO_OUT_TIMEOUT + JO_ERR_TIMEOUT)

/*
 * Options for job and channel commands.
 */
typedef struct
{
    int		jo_set;		// JO_ bits for values that were set
    int		jo_set2;	// JO2_ bits for values that were set

    ch_mode_T	jo_mode;
    ch_mode_T	jo_in_mode;
    ch_mode_T	jo_out_mode;
    ch_mode_T	jo_err_mode;
    int		jo_noblock;

    job_io_T	jo_io[4];	// PART_OUT, PART_ERR, PART_IN
    char_u	jo_io_name_buf[4][NUMBUFLEN];
    char_u	*jo_io_name[4];	// not allocated!
    int		jo_io_buf[4];
    int		jo_pty;
    int		jo_modifiable[4];
    int		jo_message[4];
    channel_T	*jo_channel;

    linenr_T	jo_in_top;
    linenr_T	jo_in_bot;

    callback_T	jo_callback;
    callback_T	jo_out_cb;
    callback_T	jo_err_cb;
    callback_T	jo_close_cb;
    callback_T	jo_exit_cb;
    int		jo_drop_never;
    int		jo_waittime;
    int		jo_timeout;
    int		jo_out_timeout;
    int		jo_err_timeout;
    int		jo_block_write;	// for testing only
    int		jo_part;
    int		jo_id;
    char_u	jo_stoponexit_buf[NUMBUFLEN];
    char_u	*jo_stoponexit;
    dict_T	*jo_env;	// environment variables
    char_u	jo_cwd_buf[NUMBUFLEN];
    char_u	*jo_cwd;

#ifdef FEAT_TERMINAL
    // when non-zero run the job in a terminal window of this size
    int		jo_term_rows;
    int		jo_term_cols;
    int		jo_vertical;
    int		jo_curwin;
    buf_T	*jo_bufnr_buf;
    int		jo_hidden;
    int		jo_term_norestore;
    char_u	jo_term_name_buf[NUMBUFLEN];
    char_u	*jo_term_name;
    char_u	jo_term_opencmd_buf[NUMBUFLEN];
    char_u	*jo_term_opencmd;
    int		jo_term_finish;
    char_u	jo_eof_chars_buf[NUMBUFLEN];
    char_u	*jo_eof_chars;
    char_u	jo_term_kill_buf[NUMBUFLEN];
    char_u	*jo_term_kill;
# if defined(FEAT_GUI) || defined(FEAT_TERMGUICOLORS)
    long_u	jo_ansi_colors[16];
# endif
    char_u	jo_term_highlight_buf[NUMBUFLEN];
    char_u	*jo_term_highlight;
    int		jo_tty_type;	    // first character of "tty_type"
    char_u	jo_term_api_buf[NUMBUFLEN];
    char_u	*jo_term_api;
#endif
} jobopt_T;

#ifdef FEAT_EVAL
/*
 * Structure used for listeners added with listener_add().
 */
typedef struct listener_S listener_T;
struct listener_S
{
    listener_T	*lr_next;
    int		lr_id;
    callback_T	lr_callback;
};
#endif

/*
 * structure used for explicit stack while garbage collecting hash tables
 */
typedef struct ht_stack_S
{
    hashtab_T		*ht;
    struct ht_stack_S	*prev;
} ht_stack_T;

/*
 * structure used for explicit stack while garbage collecting lists
 */
typedef struct list_stack_S
{
    list_T		*list;
    struct list_stack_S	*prev;
} list_stack_T;

/*
 * Structure used for iterating over dictionary items.
 * Initialize with dict_iterate_start().
 */
typedef struct
{
    long_u	dit_todo;
    hashitem_T	*dit_hi;
} dict_iterator_T;

// values for b_syn_spell: what to do with toplevel text
#define SYNSPL_DEFAULT	0	// spell check if @Spell not defined
#define SYNSPL_TOP	1	// spell check toplevel text
#define SYNSPL_NOTOP	2	// don't spell check toplevel text

// values for b_syn_foldlevel: how to compute foldlevel on a line
#define SYNFLD_START	0	// use level of item at start of line
#define SYNFLD_MINIMUM	1	// use lowest local minimum level on line

// avoid #ifdefs for when b_spell is not available
#ifdef FEAT_SPELL
# define B_SPELL(buf)  ((buf)->b_spell)
#else
# define B_SPELL(buf)  (0)
#endif

typedef struct qf_info_S qf_info_T;

#ifdef FEAT_PROFILE
/*
 * Used for :syntime: timing of executing a syntax pattern.
 */
typedef struct {
    proftime_T	total;		// total time used
    proftime_T	slowest;	// time of slowest call
    long	count;		// nr of times used
    long	match;		// nr of times matched
} syn_time_T;
#endif

typedef struct timer_S timer_T;
struct timer_S
{
    long	tr_id;
#ifdef FEAT_TIMERS
    timer_T	*tr_next;
    timer_T	*tr_prev;
    proftime_T	tr_due;		    // when the callback is to be invoked
    char	tr_firing;	    // when TRUE callback is being called
    char	tr_paused;	    // when TRUE callback is not invoked
    int		tr_repeat;	    // number of times to repeat, -1 forever
    long	tr_interval;	    // msec
    callback_T	tr_callback;
    int		tr_emsg_count;
#endif
};

#ifdef FEAT_CRYPT
/*
 * Structure to hold the type of encryption and the state of encryption or
 * decryption.
 */
typedef struct {
    int	    method_nr;
    void    *method_state;  // method-specific state information
} cryptstate_T;

// values for method_nr
# define CRYPT_M_ZIP	0
# define CRYPT_M_BF	1
# define CRYPT_M_BF2	2
# define CRYPT_M_COUNT	3 // number of crypt methods

// Currently all crypt methods work inplace.  If one is added that isn't then
// define this.
//  # define CRYPT_NOT_INPLACE 1
#endif

#ifdef FEAT_PROP_POPUP
typedef enum {
    POPPOS_BOTLEFT,
    POPPOS_TOPLEFT,
    POPPOS_BOTRIGHT,
    POPPOS_TOPRIGHT,
    POPPOS_CENTER,
    POPPOS_NONE
} poppos_T;

typedef enum {
    POPCLOSE_NONE,
    POPCLOSE_BUTTON,
    POPCLOSE_CLICK
} popclose_T;

# define POPUPWIN_DEFAULT_ZINDEX	 50
# define POPUPMENU_ZINDEX		100
# define POPUPWIN_DIALOG_ZINDEX		200
# define POPUPWIN_NOTIFICATION_ZINDEX   300
#endif

/*
 * These are items normally related to a buffer.  But when using ":ownsyntax"
 * a window may have its own instance.
 */
typedef struct {
#ifdef FEAT_SYN_HL
    hashtab_T	b_keywtab;		// syntax keywords hash table
    hashtab_T	b_keywtab_ic;		// idem, ignore case
    int		b_syn_error;		// TRUE when error occurred in HL
# ifdef FEAT_RELTIME
    int		b_syn_slow;		// TRUE when 'redrawtime' reached
# endif
    int		b_syn_ic;		// ignore case for :syn cmds
    int		b_syn_foldlevel;	// how to compute foldlevel on a line
    int		b_syn_spell;		// SYNSPL_ values
    garray_T	b_syn_patterns;		// table for syntax patterns
    garray_T	b_syn_clusters;		// table for syntax clusters
    int		b_spell_cluster_id;	// @Spell cluster ID or 0
    int		b_nospell_cluster_id;	// @NoSpell cluster ID or 0
    int		b_syn_containedin;	// TRUE when there is an item with a
					// "containedin" argument
    int		b_syn_sync_flags;	// flags about how to sync
    short	b_syn_sync_id;		// group to sync on
    long	b_syn_sync_minlines;	// minimal sync lines offset
    long	b_syn_sync_maxlines;	// maximal sync lines offset
    long	b_syn_sync_linebreaks;	// offset for multi-line pattern
    char_u	*b_syn_linecont_pat;	// line continuation pattern
    regprog_T	*b_syn_linecont_prog;	// line continuation program
#ifdef FEAT_PROFILE
    syn_time_T  b_syn_linecont_time;
#endif
    int		b_syn_linecont_ic;	// ignore-case flag for above
    int		b_syn_topgrp;		// for ":syntax include"
# ifdef FEAT_CONCEAL
    int		b_syn_conceal;		// auto-conceal for :syn cmds
# endif
# ifdef FEAT_FOLDING
    int		b_syn_folditems;	// number of patterns with the HL_FOLD
					// flag set
# endif
    /*
     * b_sst_array[] contains the state stack for a number of lines, for the
     * start of that line (col == 0).  This avoids having to recompute the
     * syntax state too often.
     * b_sst_array[] is allocated to hold the state for all displayed lines,
     * and states for 1 out of about 20 other lines.
     * b_sst_array	pointer to an array of synstate_T
     * b_sst_len	number of entries in b_sst_array[]
     * b_sst_first	pointer to first used entry in b_sst_array[] or NULL
     * b_sst_firstfree	pointer to first free entry in b_sst_array[] or NULL
     * b_sst_freecount	number of free entries in b_sst_array[]
     * b_sst_check_lnum	entries after this lnum need to be checked for
     *			validity (MAXLNUM means no check needed)
     */
    synstate_T	*b_sst_array;
    int		b_sst_len;
    synstate_T	*b_sst_first;
    synstate_T	*b_sst_firstfree;
    int		b_sst_freecount;
    linenr_T	b_sst_check_lnum;
    short_u	b_sst_lasttick;	// last display tick
#endif // FEAT_SYN_HL

#ifdef FEAT_SPELL
    // for spell checking
    garray_T	b_langp;	    // list of pointers to slang_T, see spell.c
    char_u	b_spell_ismw[256];  // flags: is midword char
    char_u	*b_spell_ismw_mb;   // multi-byte midword chars
    char_u	*b_p_spc;	    // 'spellcapcheck'
    regprog_T	*b_cap_prog;	    // program for 'spellcapcheck'
    char_u	*b_p_spf;	    // 'spellfile'
    char_u	*b_p_spl;	    // 'spelllang'
    char_u	*b_p_spo;	    // 'spelloptions'
    int		b_cjk;		    // all CJK letters as OK
#endif
#if !defined(FEAT_SYN_HL) && !defined(FEAT_SPELL)
    int		dummy;
#endif
    char_u	b_syn_chartab[32];  // syntax iskeyword option
    char_u	*b_syn_isk;	    // iskeyword option
} synblock_T;


/*
 * buffer: structure that holds information about one file
 *
 * Several windows can share a single Buffer
 * A buffer is unallocated if there is no memfile for it.
 * A buffer is new if the associated file has never been loaded yet.
 */

struct file_buffer
{
    memline_T	b_ml;		// associated memline (also contains line
				// count)

    buf_T	*b_next;	// links in list of buffers
    buf_T	*b_prev;

    int		b_nwindows;	// nr of windows open on this buffer

    int		b_flags;	// various BF_ flags
    int		b_locked;	// Buffer is being closed or referenced, don't
				// let autocommands wipe it out.

    /*
     * b_ffname has the full path of the file (NULL for no name).
     * b_sfname is the name as the user typed it (or NULL).
     * b_fname is the same as b_sfname, unless ":cd" has been done,
     *		then it is the same as b_ffname (NULL for no name).
     */
    char_u	*b_ffname;	// full path file name, allocated
    char_u	*b_sfname;	// short file name, allocated, may be equal to
				// b_ffname
    char_u	*b_fname;	// current file name, points to b_ffname or
				// b_sfname

#ifdef UNIX
    int		b_dev_valid;	// TRUE when b_dev has a valid number
    dev_t	b_dev;		// device number
    ino_t	b_ino;		// inode number
#endif
#ifdef FEAT_CW_EDITOR
    FSSpec	b_FSSpec;	// MacOS File Identification
#endif
#ifdef VMS
    char	 b_fab_rfm;	// Record format
    char	 b_fab_rat;	// Record attribute
    unsigned int b_fab_mrs;	// Max record size
#endif
    int		b_fnum;		// buffer number for this file.
    char_u	b_key[VIM_SIZEOF_INT * 2 + 1];
				// key used for buf_hashtab, holds b_fnum as
				// hex string

    int		b_changed;	// 'modified': Set to TRUE if something in the
				// file has been changed and not written out.
    dictitem16_T b_ct_di;	// holds the b:changedtick value in
				// b_ct_di.di_tv.vval.v_number;
				// incremented for each change, also for undo
#define CHANGEDTICK(buf) ((buf)->b_ct_di.di_tv.vval.v_number)

    varnumber_T	b_last_changedtick; // b:changedtick when TextChanged or
				    // TextChangedI was last triggered.
    varnumber_T	b_last_changedtick_pum; // b:changedtick when TextChangedP was
					// last triggered.

    int		b_saving;	// Set to TRUE if we are in the middle of
				// saving the buffer.

    /*
     * Changes to a buffer require updating of the display.  To minimize the
     * work, remember changes made and update everything at once.
     */
    int		b_mod_set;	// TRUE when there are changes since the last
				// time the display was updated
    linenr_T	b_mod_top;	// topmost lnum that was changed
    linenr_T	b_mod_bot;	// lnum below last changed line, AFTER the
				// change
    long	b_mod_xlines;	// number of extra buffer lines inserted;
				// negative when lines were deleted

    wininfo_T	*b_wininfo;	// list of last used info for each window

    long	b_mtime;	// last change time of original file
    long	b_mtime_read;	// last change time when reading
    off_T	b_orig_size;	// size of original file in bytes
    int		b_orig_mode;	// mode of original file
#ifdef FEAT_VIMINFO
    time_T	b_last_used;	// time when the buffer was last used; used
				// for viminfo
#endif

    pos_T	b_namedm[NMARKS]; // current named marks (mark.c)

    // These variables are set when VIsual_active becomes FALSE
    visualinfo_T b_visual;
#ifdef FEAT_EVAL
    int		b_visual_mode_eval;  // b_visual.vi_mode for visualmode()
#endif

    pos_T	b_last_cursor;	// cursor position when last unloading this
				// buffer
    pos_T	b_last_insert;	// where Insert mode was left
    pos_T	b_last_change;	// position of last change: '. mark

#ifdef FEAT_JUMPLIST
    /*
     * the changelist contains old change positions
     */
    pos_T	b_changelist[JUMPLISTSIZE];
    int		b_changelistlen;	// number of active entries
    int		b_new_change;		// set by u_savecommon()
#endif

    /*
     * Character table, only used in charset.c for 'iskeyword'
     * 32 bytes of 8 bits: 1 bit per character 0-255.
     */
    char_u	b_chartab[32];

    // Table used for mappings local to a buffer.
    mapblock_T	*(b_maphash[256]);

    // First abbreviation local to a buffer.
    mapblock_T	*b_first_abbr;

    // User commands local to the buffer.
    garray_T	b_ucmds;
    // start and end of an operator, also used for '[ and ']
    pos_T	b_op_start;
    pos_T	b_op_start_orig;  // used for Insstart_orig
    pos_T	b_op_end;

#ifdef FEAT_VIMINFO
    int		b_marks_read;	// Have we read viminfo marks yet?
#endif

    /*
     * The following only used in undo.c.
     */
    u_header_T	*b_u_oldhead;	// pointer to oldest header
    u_header_T	*b_u_newhead;	// pointer to newest header; may not be valid
				// if b_u_curhead is not NULL
    u_header_T	*b_u_curhead;	// pointer to current header
    int		b_u_numhead;	// current number of headers
    int		b_u_synced;	// entry lists are synced
    long	b_u_seq_last;	// last used undo sequence number
    long	b_u_save_nr_last; // counter for last file write
    long	b_u_seq_cur;	// hu_seq of header below which we are now
    time_T	b_u_time_cur;	// uh_time of header below which we are now
    long	b_u_save_nr_cur; // file write nr after which we are now

    /*
     * variables for "U" command in undo.c
     */
    undoline_T	b_u_line_ptr;	// saved line for "U" command
    linenr_T	b_u_line_lnum;	// line number of line in u_line
    colnr_T	b_u_line_colnr;	// optional column number

    int		b_scanned;	// ^N/^P have scanned this buffer

    // flags for use of ":lmap" and IM control
    long	b_p_iminsert;	// input mode for insert
    long	b_p_imsearch;	// input mode for search
#define B_IMODE_USE_INSERT -1	//	Use b_p_iminsert value for search
#define B_IMODE_NONE 0		//	Input via none
#define B_IMODE_LMAP 1		//	Input via langmap
#define B_IMODE_IM 2		//	Input via input method
#define B_IMODE_LAST 2

#ifdef FEAT_KEYMAP
    short	b_kmap_state;	// using "lmap" mappings
# define KEYMAP_INIT	1	// 'keymap' was set, call keymap_init()
# define KEYMAP_LOADED	2	// 'keymap' mappings have been loaded
    garray_T	b_kmap_ga;	// the keymap table
#endif

    /*
     * Options local to a buffer.
     * They are here because their value depends on the type of file
     * or contents of the file being edited.
     */
    int		b_p_initialized;	// set when options initialized

#ifdef FEAT_EVAL
    sctx_T	b_p_script_ctx[BV_COUNT]; // SCTXs for buffer-local options
#endif

    int		b_p_ai;		// 'autoindent'
    int		b_p_ai_nopaste;	// b_p_ai saved for paste mode
    char_u	*b_p_bkc;	// 'backupcopy'
    unsigned	b_bkc_flags;    // flags for 'backupcopy'
    int		b_p_ci;		// 'copyindent'
    int		b_p_bin;	// 'binary'
    int		b_p_bomb;	// 'bomb'
    char_u	*b_p_bh;	// 'bufhidden'
    char_u	*b_p_bt;	// 'buftype'
#ifdef FEAT_QUICKFIX
#define BUF_HAS_QF_ENTRY 1
#define BUF_HAS_LL_ENTRY 2
    int		b_has_qf_entry;
#endif
    int		b_p_bl;		// 'buflisted'
#ifdef FEAT_CINDENT
    int		b_p_cin;	// 'cindent'
    char_u	*b_p_cino;	// 'cinoptions'
    char_u	*b_p_cink;	// 'cinkeys'
#endif
#if defined(FEAT_CINDENT) || defined(FEAT_SMARTINDENT)
    char_u	*b_p_cinw;	// 'cinwords'
#endif
    char_u	*b_p_com;	// 'comments'
#ifdef FEAT_FOLDING
    char_u	*b_p_cms;	// 'commentstring'
#endif
    char_u	*b_p_cpt;	// 'complete'
#ifdef BACKSLASH_IN_FILENAME
    char_u	*b_p_csl;	// 'completeslash'
#endif
#ifdef FEAT_COMPL_FUNC
    char_u	*b_p_cfu;	// 'completefunc'
    char_u	*b_p_ofu;	// 'omnifunc'
#endif
#ifdef FEAT_EVAL
    char_u	*b_p_tfu;	// 'tagfunc'
#endif
    int		b_p_eol;	// 'endofline'
    int		b_p_fixeol;	// 'fixendofline'
    int		b_p_et;		// 'expandtab'
    int		b_p_et_nobin;	// b_p_et saved for binary mode
    int		b_p_et_nopaste; // b_p_et saved for paste mode
    char_u	*b_p_fenc;	// 'fileencoding'
    char_u	*b_p_ff;	// 'fileformat'
    char_u	*b_p_ft;	// 'filetype'
    char_u	*b_p_fo;	// 'formatoptions'
    char_u	*b_p_flp;	// 'formatlistpat'
    int		b_p_inf;	// 'infercase'
    char_u	*b_p_isk;	// 'iskeyword'
#ifdef FEAT_FIND_ID
    char_u	*b_p_def;	// 'define' local value
    char_u	*b_p_inc;	// 'include'
# ifdef FEAT_EVAL
    char_u	*b_p_inex;	// 'includeexpr'
    long_u	b_p_inex_flags;	// flags for 'includeexpr'
# endif
#endif
#if defined(FEAT_CINDENT) && defined(FEAT_EVAL)
    char_u	*b_p_inde;	// 'indentexpr'
    long_u	b_p_inde_flags;	// flags for 'indentexpr'
    char_u	*b_p_indk;	// 'indentkeys'
#endif
    char_u	*b_p_fp;	// 'formatprg'
#if defined(FEAT_EVAL)
    char_u	*b_p_fex;	// 'formatexpr'
    long_u	b_p_fex_flags;	// flags for 'formatexpr'
#endif
#ifdef FEAT_CRYPT
    char_u	*b_p_key;	// 'key'
#endif
    char_u	*b_p_kp;	// 'keywordprg'
#ifdef FEAT_LISP
    int		b_p_lisp;	// 'lisp'
#endif
    char_u	*b_p_menc;	// 'makeencoding'
    char_u	*b_p_mps;	// 'matchpairs'
    int		b_p_ml;		// 'modeline'
    int		b_p_ml_nobin;	// b_p_ml saved for binary mode
    int		b_p_ma;		// 'modifiable'
    char_u	*b_p_nf;	// 'nrformats'
    int		b_p_pi;		// 'preserveindent'
#ifdef FEAT_TEXTOBJ
    char_u	*b_p_qe;	// 'quoteescape'
#endif
    int		b_p_ro;		// 'readonly'
    long	b_p_sw;		// 'shiftwidth'
    int		b_p_sn;		// 'shortname'
#ifdef FEAT_SMARTINDENT
    int		b_p_si;		// 'smartindent'
#endif
    long	b_p_sts;	// 'softtabstop'
    long	b_p_sts_nopaste; // b_p_sts saved for paste mode
#ifdef FEAT_SEARCHPATH
    char_u	*b_p_sua;	// 'suffixesadd'
#endif
    int		b_p_swf;	// 'swapfile'
#ifdef FEAT_SYN_HL
    long	b_p_smc;	// 'synmaxcol'
    char_u	*b_p_syn;	// 'syntax'
#endif
    long	b_p_ts;		// 'tabstop'
    int		b_p_tx;		// 'textmode'
    long	b_p_tw;		// 'textwidth'
    long	b_p_tw_nobin;	// b_p_tw saved for binary mode
    long	b_p_tw_nopaste;	// b_p_tw saved for paste mode
    long	b_p_wm;		// 'wrapmargin'
    long	b_p_wm_nobin;	// b_p_wm saved for binary mode
    long	b_p_wm_nopaste;	// b_p_wm saved for paste mode
#ifdef FEAT_VARTABS
    char_u	*b_p_vsts;	// 'varsofttabstop'
    int		*b_p_vsts_array;   // 'varsofttabstop' in internal format
    char_u	*b_p_vsts_nopaste; // b_p_vsts saved for paste mode
    char_u	*b_p_vts;	// 'vartabstop'
    int		*b_p_vts_array;	// 'vartabstop' in internal format
#endif
#ifdef FEAT_KEYMAP
    char_u	*b_p_keymap;	// 'keymap'
#endif

    /*
     * local values for options which are normally global
     */
#ifdef FEAT_QUICKFIX
    char_u	*b_p_gp;	// 'grepprg' local value
    char_u	*b_p_mp;	// 'makeprg' local value
    char_u	*b_p_efm;	// 'errorformat' local value
#endif
    char_u	*b_p_ep;	// 'equalprg' local value
    char_u	*b_p_path;	// 'path' local value
    int		b_p_ar;		// 'autoread' local value
    char_u	*b_p_tags;	// 'tags' local value
    char_u	*b_p_tc;	// 'tagcase' local value
    unsigned	b_tc_flags;     // flags for 'tagcase'
    char_u	*b_p_dict;	// 'dictionary' local value
    char_u	*b_p_tsr;	// 'thesaurus' local value
    long	b_p_ul;		// 'undolevels' local value
#ifdef FEAT_PERSISTENT_UNDO
    int		b_p_udf;	// 'undofile'
#endif
#ifdef FEAT_LISP
    char_u	*b_p_lw;	// 'lispwords' local value
#endif
#ifdef FEAT_TERMINAL
    long	b_p_twsl;	// 'termwinscroll'
#endif

    /*
     * end of buffer options
     */

#ifdef FEAT_CINDENT
    // values set from b_p_cino
    int		b_ind_level;
    int		b_ind_open_imag;
    int		b_ind_no_brace;
    int		b_ind_first_open;
    int		b_ind_open_extra;
    int		b_ind_close_extra;
    int		b_ind_open_left_imag;
    int		b_ind_jump_label;
    int		b_ind_case;
    int		b_ind_case_code;
    int		b_ind_case_break;
    int		b_ind_param;
    int		b_ind_func_type;
    int		b_ind_comment;
    int		b_ind_in_comment;
    int		b_ind_in_comment2;
    int		b_ind_cpp_baseclass;
    int		b_ind_continuation;
    int		b_ind_unclosed;
    int		b_ind_unclosed2;
    int		b_ind_unclosed_noignore;
    int		b_ind_unclosed_wrapped;
    int		b_ind_unclosed_whiteok;
    int		b_ind_matching_paren;
    int		b_ind_paren_prev;
    int		b_ind_maxparen;
    int		b_ind_maxcomment;
    int		b_ind_scopedecl;
    int		b_ind_scopedecl_code;
    int		b_ind_java;
    int		b_ind_js;
    int		b_ind_keep_case_label;
    int		b_ind_hash_comment;
    int		b_ind_cpp_namespace;
    int		b_ind_if_for_while;
    int		b_ind_cpp_extern_c;
    int		b_ind_pragma;
#endif

    linenr_T	b_no_eol_lnum;	// non-zero lnum when last line of next binary
				// write should not have an end-of-line

    int		b_start_eol;	// last line had eol when it was read
    int		b_start_ffc;	// first char of 'ff' when edit started
    char_u	*b_start_fenc;	// 'fileencoding' when edit started or NULL
    int		b_bad_char;	// "++bad=" argument when edit started or 0
    int		b_start_bomb;	// 'bomb' when it was read

#ifdef FEAT_EVAL
    dictitem_T	b_bufvar;	// variable for "b:" Dictionary
    dict_T	*b_vars;	// internal variables, local to buffer

    listener_T	*b_listener;
    list_T	*b_recorded_changes;
#endif
#ifdef FEAT_PROP_POPUP
    int		b_has_textprop;	// TRUE when text props were added
    hashtab_T	*b_proptypes;	// text property types local to buffer
#endif

#if defined(FEAT_BEVAL) && defined(FEAT_EVAL)
    char_u	*b_p_bexpr;	// 'balloonexpr' local value
    long_u	b_p_bexpr_flags;// flags for 'balloonexpr'
#endif
#ifdef FEAT_CRYPT
    char_u	*b_p_cm;	// 'cryptmethod'
#endif

    // When a buffer is created, it starts without a swap file.  b_may_swap is
    // then set to indicate that a swap file may be opened later.  It is reset
    // if a swap file could not be opened.
    int		b_may_swap;
    int		b_did_warn;	// Set to 1 if user has been warned on first
				// change of a read-only file

    // Two special kinds of buffers:
    // help buffer  - used for help files, won't use a swap file.
    // spell buffer - used for spell info, never displayed and doesn't have a
    //		      file name.
    int		b_help;		// TRUE for help file buffer (when set b_p_bt
				// is "help")
#ifdef FEAT_SPELL
    int		b_spell;	// TRUE for a spell file buffer, most fields
				// are not used!  Use the B_SPELL macro to
				// access b_spell without #ifdef.
#endif

    int		b_shortname;	// this file has an 8.3 file name

#ifdef FEAT_JOB_CHANNEL
    char_u	*b_prompt_text;		// set by prompt_setprompt()
    callback_T	b_prompt_callback;	// set by prompt_setcallback()
    callback_T	b_prompt_interrupt;	// set by prompt_setinterrupt()
    int		b_prompt_insert;	// value for restart_edit when entering
					// a prompt buffer window.
#endif
#ifdef FEAT_MZSCHEME
    void	*b_mzscheme_ref; // The MzScheme reference to this buffer
#endif

#ifdef FEAT_PERL
    void	*b_perl_private;
#endif

#ifdef FEAT_PYTHON
    void	*b_python_ref;	// The Python reference to this buffer
#endif

#ifdef FEAT_PYTHON3
    void	*b_python3_ref;	// The Python3 reference to this buffer
#endif

#ifdef FEAT_TCL
    void	*b_tcl_ref;
#endif

#ifdef FEAT_RUBY
    void	*b_ruby_ref;
#endif

#if defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	b_s;		// Info related to syntax highlighting.  w_s
				// normally points to this, but some windows
				// may use a different synblock_T.
#endif

#ifdef FEAT_SIGNS
    sign_entry_T *b_signlist;	   // list of placed signs
# ifdef FEAT_NETBEANS_INTG
    int		b_has_sign_column; // Flag that is set when a first sign is
				   // added and remains set until the end of
				   // the netbeans session.
# endif
#endif

#ifdef FEAT_NETBEANS_INTG
    int		b_netbeans_file;    // TRUE when buffer is owned by NetBeans
    int		b_was_netbeans_file;// TRUE if b_netbeans_file was once set
#endif
#ifdef FEAT_JOB_CHANNEL
    int		b_write_to_channel; // TRUE when appended lines are written to
				    // a channel.
#endif

#ifdef FEAT_CRYPT
    cryptstate_T *b_cryptstate;	// Encryption state while reading or writing
				// the file. NULL when not using encryption.
#endif
    int		b_mapped_ctrl_c; // modes where CTRL-C is mapped

#ifdef FEAT_TERMINAL
    term_T	*b_term;	// When not NULL this buffer is for a terminal
				// window.
#endif
#ifdef FEAT_DIFF
    int		b_diff_failed;	// internal diff failed for this buffer
#endif
}; // file_buffer


#ifdef FEAT_DIFF
/*
 * Stuff for diff mode.
 */
# define DB_COUNT 8	// up to eight buffers can be diff'ed

/*
 * Each diffblock defines where a block of lines starts in each of the buffers
 * and how many lines it occupies in that buffer.  When the lines are missing
 * in the buffer the df_count[] is zero.  This is all counted in
 * buffer lines.
 * There is always at least one unchanged line in between the diffs.
 * Otherwise it would have been included in the diff above or below it.
 * df_lnum[] + df_count[] is the lnum below the change.  When in one buffer
 * lines have been inserted, in the other buffer df_lnum[] is the line below
 * the insertion and df_count[] is zero.  When appending lines at the end of
 * the buffer, df_lnum[] is one beyond the end!
 * This is using a linked list, because the number of differences is expected
 * to be reasonable small.  The list is sorted on lnum.
 */
typedef struct diffblock_S diff_T;
struct diffblock_S
{
    diff_T	*df_next;
    linenr_T	df_lnum[DB_COUNT];	// line number in buffer
    linenr_T	df_count[DB_COUNT];	// nr of inserted/changed lines
};
#endif

#define SNAP_HELP_IDX	0
#define SNAP_AUCMD_IDX 1
#define SNAP_COUNT	2

/*
 * Tab pages point to the top frame of each tab page.
 * Note: Most values are NOT valid for the current tab page!  Use "curwin",
 * "firstwin", etc. for that.  "tp_topframe" is always valid and can be
 * compared against "topframe" to find the current tab page.
 */
typedef struct tabpage_S tabpage_T;
struct tabpage_S
{
    tabpage_T	    *tp_next;	    // next tabpage or NULL
    frame_T	    *tp_topframe;   // topframe for the windows
    win_T	    *tp_curwin;	    // current window in this Tab page
    win_T	    *tp_prevwin;    // previous window in this Tab page
    win_T	    *tp_firstwin;   // first window in this Tab page
    win_T	    *tp_lastwin;    // last window in this Tab page
#ifdef FEAT_PROP_POPUP
    win_T	    *tp_first_popupwin; // first popup window in this Tab page
#endif
    long	    tp_old_Rows;    // Rows when Tab page was left
    long	    tp_old_Columns; // Columns when Tab page was left
    long	    tp_ch_used;	    // value of 'cmdheight' when frame size
				    // was set
#ifdef FEAT_GUI
    int		    tp_prev_which_scrollbars[3];
				    // previous value of which_scrollbars
#endif

    char_u	    *tp_localdir;	// absolute path of local directory or
					// NULL
    char_u	    *tp_prevdir;	// previous directory

#ifdef FEAT_DIFF
    diff_T	    *tp_first_diff;
    buf_T	    *(tp_diffbuf[DB_COUNT]);
    int		    tp_diff_invalid;	// list of diffs is outdated
    int		    tp_diff_update;	// update diffs before redrawing
#endif
    frame_T	    *(tp_snapshot[SNAP_COUNT]);  // window layout snapshots
#ifdef FEAT_EVAL
    dictitem_T	    tp_winvar;	    // variable for "t:" Dictionary
    dict_T	    *tp_vars;	    // internal variables, local to tab page
#endif

#ifdef FEAT_PYTHON
    void	    *tp_python_ref;	// The Python value for this tab page
#endif

#ifdef FEAT_PYTHON3
    void	    *tp_python3_ref;	// The Python value for this tab page
#endif
};

/*
 * Structure to cache info for displayed lines in w_lines[].
 * Each logical line has one entry.
 * The entry tells how the logical line is currently displayed in the window.
 * This is updated when displaying the window.
 * When the display is changed (e.g., when clearing the screen) w_lines_valid
 * is changed to exclude invalid entries.
 * When making changes to the buffer, wl_valid is reset to indicate wl_size
 * may not reflect what is actually in the buffer.  When wl_valid is FALSE,
 * the entries can only be used to count the number of displayed lines used.
 * wl_lnum and wl_lastlnum are invalid too.
 */
typedef struct w_line
{
    linenr_T	wl_lnum;	// buffer line number for logical line
    short_u	wl_size;	// height in screen lines
    char	wl_valid;	// TRUE values are valid for text in buffer
#ifdef FEAT_FOLDING
    char	wl_folded;	// TRUE when this is a range of folded lines
    linenr_T	wl_lastlnum;	// last buffer line number for logical line
#endif
} wline_T;

/*
 * Windows are kept in a tree of frames.  Each frame has a column (FR_COL)
 * or row (FR_ROW) layout or is a leaf, which has a window.
 */
struct frame_S
{
    char	fr_layout;	// FR_LEAF, FR_COL or FR_ROW
    int		fr_width;
    int		fr_newwidth;	// new width used in win_equal_rec()
    int		fr_height;
    int		fr_newheight;	// new height used in win_equal_rec()
    frame_T	*fr_parent;	// containing frame or NULL
    frame_T	*fr_next;	// frame right or below in same parent, NULL
				// for last
    frame_T	*fr_prev;	// frame left or above in same parent, NULL
				// for first
    // fr_child and fr_win are mutually exclusive
    frame_T	*fr_child;	// first contained frame
    win_T	*fr_win;	// window that fills this frame
};

#define FR_LEAF	0	// frame is a leaf
#define FR_ROW	1	// frame with a row of windows
#define FR_COL	2	// frame with a column of windows

/*
 * Struct used for highlighting 'hlsearch' matches, matches defined by
 * ":match" and matches defined by match functions.
 * For 'hlsearch' there is one pattern for all windows.  For ":match" and the
 * match functions there is a different pattern for each window.
 */
typedef struct
{
    regmmatch_T	rm;	    // points to the regexp program; contains last
			    // found match (may continue in next line)
    buf_T	*buf;	    // the buffer to search for a match
    linenr_T	lnum;	    // the line to search for a match
    int		attr;	    // attributes to be used for a match
    int		attr_cur;   // attributes currently active in win_line()
    linenr_T	first_lnum; // first lnum to search for multi-line pat
    colnr_T	startcol;   // in win_line() points to char where HL starts
    colnr_T	endcol;	    // in win_line() points to char where HL ends
    int		is_addpos;  // position specified directly by
			    // matchaddpos(). TRUE/FALSE
#ifdef FEAT_RELTIME
    proftime_T	tm;	    // for a time limit
#endif
} match_T;

// number of positions supported by matchaddpos()
#define MAXPOSMATCH 8

/*
 * Same as lpos_T, but with additional field len.
 */
typedef struct
{
    linenr_T	lnum;	// line number
    colnr_T	col;	// column number
    int		len;	// length: 0 - to the end of line
} llpos_T;

/*
 * posmatch_T provides an array for storing match items for matchaddpos()
 * function.
 */
typedef struct posmatch posmatch_T;
struct posmatch
{
    llpos_T	pos[MAXPOSMATCH];	// array of positions
    int		cur;			// internal position counter
    linenr_T	toplnum;		// top buffer line
    linenr_T	botlnum;		// bottom buffer line
};

/*
 * matchitem_T provides a linked list for storing match items for ":match" and
 * the match functions.
 */
typedef struct matchitem matchitem_T;
struct matchitem
{
    matchitem_T	*next;
    int		id;	    // match ID
    int		priority;   // match priority
    char_u	*pattern;   // pattern to highlight
    regmmatch_T	match;	    // regexp program for pattern
    posmatch_T	pos;	    // position matches
    match_T	hl;	    // struct for doing the actual highlighting
    int		hlg_id;	    // highlight group ID
#ifdef FEAT_CONCEAL
    int		conceal_char; // cchar for Conceal highlighting
#endif
};

// Structure to store last cursor position and topline.  Used by check_lnums()
// and reset_lnums().
typedef struct
{
    int		w_topline_save;	// original topline value
    int		w_topline_corr;	// corrected topline value
    pos_T	w_cursor_save;	// original cursor position
    pos_T	w_cursor_corr;	// corrected cursor position
} pos_save_T;

#ifdef FEAT_MENU
typedef struct {
    int		wb_startcol;
    int		wb_endcol;
    vimmenu_T	*wb_menu;
} winbar_item_T;
#endif

/*
 * Structure which contains all information that belongs to a window
 *
 * All row numbers are relative to the start of the window, except w_winrow.
 */
struct window_S
{
    int		w_id;		    // unique window ID

    buf_T	*w_buffer;	    // buffer we are a window into

    win_T	*w_prev;	    // link to previous window
    win_T	*w_next;	    // link to next window

#if defined(FEAT_SYN_HL) || defined(FEAT_SPELL)
    synblock_T	*w_s;		    // for :ownsyntax
#endif

    int		w_closing;	    // window is being closed, don't let
				    // autocommands close it too.

    frame_T	*w_frame;	    // frame containing this window

    pos_T	w_cursor;	    // cursor position in buffer

    colnr_T	w_curswant;	    // The column we'd like to be at.  This is
				    // used to try to stay in the same column
				    // for up/down cursor motions.

    int		w_set_curswant;	    // If set, then update w_curswant the next
				    // time through cursupdate() to the
				    // current virtual column

#ifdef FEAT_SYN_HL
    linenr_T	w_last_cursorline;  // where last time 'cursorline' was drawn
#endif

    /*
     * the next seven are used to update the Visual highlighting
     */
    char	w_old_visual_mode;  // last known VIsual_mode
    linenr_T	w_old_cursor_lnum;  // last known end of visual part
    colnr_T	w_old_cursor_fcol;  // first column for block visual part
    colnr_T	w_old_cursor_lcol;  // last column for block visual part
    linenr_T	w_old_visual_lnum;  // last known start of visual part
    colnr_T	w_old_visual_col;   // last known start of visual part
    colnr_T	w_old_curswant;	    // last known value of Curswant

    /*
     * "w_topline", "w_leftcol" and "w_skipcol" specify the offsets for
     * displaying the buffer.
     */
    linenr_T	w_topline;	    // buffer line number of the line at the
				    // top of the window
    char	w_topline_was_set;  // flag set to TRUE when topline is set,
				    // e.g. by winrestview()
#ifdef FEAT_DIFF
    int		w_topfill;	    // number of filler lines above w_topline
    int		w_old_topfill;	    // w_topfill at last redraw
    int		w_botfill;	    // TRUE when filler lines are actually
				    // below w_topline (at end of file)
    int		w_old_botfill;	    // w_botfill at last redraw
#endif
    colnr_T	w_leftcol;	    // window column number of the left most
				    // character in the window; used when
				    // 'wrap' is off
    colnr_T	w_skipcol;	    // starting column when a single line
				    // doesn't fit in the window

    /*
     * Layout of the window in the screen.
     * May need to add "msg_scrolled" to "w_winrow" in rare situations.
     */
    int		w_winrow;	    // first row of window in screen
    int		w_height;	    // number of rows in window, excluding
				    // status/command/winbar line(s)
    int		w_status_height;    // number of status lines (0 or 1)
    int		w_wincol;	    // Leftmost column of window in screen.
    int		w_width;	    // Width of window, excluding separation.
    int		w_vsep_width;	    // Number of separator columns (0 or 1).
    pos_save_T	w_save_cursor;	    // backup of cursor pos and topline
#ifdef FEAT_PROP_POPUP
    int		w_popup_flags;	    // POPF_ values
    int		w_popup_handled;    // POPUP_HANDLE[0-9] flags
    char_u	*w_popup_title;
    poppos_T	w_popup_pos;
    int		w_popup_fixed;	    // do not shift popup to fit on screen
    int		w_popup_prop_type;  // when not zero: textprop type ID
    win_T	*w_popup_prop_win;  // window to search for textprop
    int		w_popup_prop_id;    // when not zero: textprop ID
    int		w_zindex;
    int		w_minheight;	    // "minheight" for popup window
    int		w_minwidth;	    // "minwidth" for popup window
    int		w_maxheight;	    // "maxheight" for popup window
    int		w_maxwidth;	    // "maxwidth" for popup window
    int		w_maxwidth_opt;	    // maxwidth from option
    int		w_wantline;	    // "line" for popup window
    int		w_wantcol;	    // "col" for popup window
    int		w_firstline;	    // "firstline" for popup window
    int		w_want_scrollbar;   // when zero don't use a scrollbar
    int		w_has_scrollbar;    // 1 if scrollbar displayed, 0 otherwise
    char_u	*w_scrollbar_highlight; // "scrollbarhighlight"
    char_u	*w_thumb_highlight; // "thumbhighlight"
    int		w_popup_padding[4]; // popup padding top/right/bot/left
    int		w_popup_border[4];  // popup border top/right/bot/left
    char_u	*w_border_highlight[4];  // popup border highlight
    int		w_border_char[8];   // popup border characters

    int		w_popup_leftoff;    // columns left of the screen
    int		w_popup_rightoff;   // columns right of the screen
    varnumber_T	w_popup_last_changedtick; // b:changedtick of popup buffer
					  // when position was computed
    varnumber_T	w_popup_prop_changedtick; // b:changedtick of buffer with
					  // w_popup_prop_type when position
					  // was computed
    int		w_popup_prop_topline; // w_topline of window with
				      // w_popup_prop_type when position was
				      // computed
    linenr_T	w_popup_last_curline; // last known w_cursor.lnum of window
				      // with "cursorline" set
    callback_T	w_close_cb;	    // popup close callback
    callback_T	w_filter_cb;	    // popup filter callback
    int		w_filter_mode;	    // mode when filter callback is used

    win_T	*w_popup_curwin;    // close popup if curwin differs
    linenr_T	w_popup_lnum;	    // close popup if cursor not on this line
    colnr_T	w_popup_mincol;	    // close popup if cursor before this col
    colnr_T	w_popup_maxcol;	    // close popup if cursor after this col
    int		w_popup_mouse_row;  // close popup if mouse moves away
    int		w_popup_mouse_mincol;  // close popup if mouse moves away
    int		w_popup_mouse_maxcol;  // close popup if mouse moves away
    popclose_T	w_popup_close;	    // allow closing the popup with the mouse

    list_T	*w_popup_mask;	     // list of lists for "mask"
    char_u	*w_popup_mask_cells; // cached mask cells
    int		w_popup_mask_height; // height of w_popup_mask_cells
    int		w_popup_mask_width;  // width of w_popup_mask_cells
# if defined(FEAT_TIMERS)
    timer_T	*w_popup_timer;	    // timer for closing popup window
# endif
#endif


    /*
     * === start of cached values ====
     */
    /*
     * Recomputing is minimized by storing the result of computations.
     * Use functions in screen.c to check if they are valid and to update.
     * w_valid is a bitfield of flags, which indicate if specific values are
     * valid or need to be recomputed.	See screen.c for values.
     */
    int		w_valid;
    pos_T	w_valid_cursor;	    // last known position of w_cursor, used
				    // to adjust w_valid
    colnr_T	w_valid_leftcol;    // last known w_leftcol

    /*
     * w_cline_height is the number of physical lines taken by the buffer line
     * that the cursor is on.  We use this to avoid extra calls to plines().
     */
    int		w_cline_height;	    // current size of cursor line
#ifdef FEAT_FOLDING
    int		w_cline_folded;	    // cursor line is folded
#endif

    int		w_cline_row;	    // starting row of the cursor line

    colnr_T	w_virtcol;	    // column number of the cursor in the
				    // buffer line, as opposed to the column
				    // number we're at on the screen.  This
				    // makes a difference on lines which span
				    // more than one screen line or when
				    // w_leftcol is non-zero

    /*
     * w_wrow and w_wcol specify the cursor position in the window.
     * This is related to positions in the window, not in the display or
     * buffer, thus w_wrow is relative to w_winrow.
     */
    int		w_wrow, w_wcol;	    // cursor position in window

    linenr_T	w_botline;	    // number of the line below the bottom of
				    // the window
    int		w_empty_rows;	    // number of ~ rows in window
#ifdef FEAT_DIFF
    int		w_filler_rows;	    // number of filler rows at the end of the
				    // window
#endif

    /*
     * Info about the lines currently in the window is remembered to avoid
     * recomputing it every time.  The allocated size of w_lines[] is Rows.
     * Only the w_lines_valid entries are actually valid.
     * When the display is up-to-date w_lines[0].wl_lnum is equal to w_topline
     * and w_lines[w_lines_valid - 1].wl_lnum is equal to w_botline.
     * Between changing text and updating the display w_lines[] represents
     * what is currently displayed.  wl_valid is reset to indicated this.
     * This is used for efficient redrawing.
     */
    int		w_lines_valid;	    // number of valid entries
    wline_T	*w_lines;

#ifdef FEAT_FOLDING
    garray_T	w_folds;	    // array of nested folds
    char	w_fold_manual;	    // when TRUE: some folds are opened/closed
				    // manually
    char	w_foldinvalid;	    // when TRUE: folding needs to be
				    // recomputed
#endif
#ifdef FEAT_LINEBREAK
    int		w_nrwidth;	    // width of 'number' and 'relativenumber'
				    // column being used
#endif

    /*
     * === end of cached values ===
     */

    int		w_redr_type;	    // type of redraw to be performed on win
    int		w_upd_rows;	    // number of window lines to update when
				    // w_redr_type is REDRAW_TOP
    linenr_T	w_redraw_top;	    // when != 0: first line needing redraw
    linenr_T	w_redraw_bot;	    // when != 0: last line needing redraw
    int		w_redr_status;	    // if TRUE status line must be redrawn

#ifdef FEAT_CMDL_INFO
    // remember what is shown in the ruler for this window (if 'ruler' set)
    pos_T	w_ru_cursor;	    // cursor position shown in ruler
    colnr_T	w_ru_virtcol;	    // virtcol shown in ruler
    linenr_T	w_ru_topline;	    // topline shown in ruler
    linenr_T	w_ru_line_count;    // line count used for ruler
# ifdef FEAT_DIFF
    int		w_ru_topfill;	    // topfill shown in ruler
# endif
    char	w_ru_empty;	    // TRUE if ruler shows 0-1 (empty line)
#endif

    int		w_alt_fnum;	    // alternate file (for # and CTRL-^)

    alist_T	*w_alist;	    // pointer to arglist for this window
    int		w_arg_idx;	    // current index in argument list (can be
				    // out of range!)
    int		w_arg_idx_invalid;  // editing another file than w_arg_idx

    char_u	*w_localdir;	    // absolute path of local directory or
				    // NULL
    char_u	*w_prevdir;	    // previous directory
#ifdef FEAT_MENU
    vimmenu_T	*w_winbar;	    // The root of the WinBar menu hierarchy.
    winbar_item_T *w_winbar_items;  // list of items in the WinBar
    int		w_winbar_height;    // 1 if there is a window toolbar
#endif

    /*
     * Options local to a window.
     * They are local because they influence the layout of the window or
     * depend on the window layout.
     * There are two values: w_onebuf_opt is local to the buffer currently in
     * this window, w_allbuf_opt is for all buffers in this window.
     */
    winopt_T	w_onebuf_opt;
    winopt_T	w_allbuf_opt;

    // A few options have local flags for P_INSECURE.
#ifdef FEAT_STL_OPT
    long_u	w_p_stl_flags;	    // flags for 'statusline'
#endif
#ifdef FEAT_EVAL
    long_u	w_p_fde_flags;	    // flags for 'foldexpr'
    long_u	w_p_fdt_flags;	    // flags for 'foldtext'
#endif
#ifdef FEAT_SYN_HL
    int		*w_p_cc_cols;	    // array of columns to highlight or NULL
    char_u	w_p_culopt_flags;   // flags for cursorline highlighting
#endif
    long	w_p_siso;	    // 'sidescrolloff' local value
    long	w_p_so;		    // 'scrolloff' local value

#ifdef FEAT_LINEBREAK
    int		w_briopt_min;	    // minimum width for breakindent
    int		w_briopt_shift;	    // additional shift for breakindent
    int		w_briopt_sbr;	    // sbr in 'briopt'
#endif

    // transform a pointer to a "onebuf" option into a "allbuf" option
#define GLOBAL_WO(p)	((char *)p + sizeof(winopt_T))

    long	w_scbind_pos;

#ifdef FEAT_EVAL
    dictitem_T	w_winvar;	// variable for "w:" Dictionary
    dict_T	*w_vars;	// internal variables, local to window
#endif

    /*
     * The w_prev_pcmark field is used to check whether we really did jump to
     * a new line after setting the w_pcmark.  If not, then we revert to
     * using the previous w_pcmark.
     */
    pos_T	w_pcmark;	// previous context mark
    pos_T	w_prev_pcmark;	// previous w_pcmark

#ifdef FEAT_JUMPLIST
    /*
     * the jumplist contains old cursor positions
     */
    xfmark_T	w_jumplist[JUMPLISTSIZE];
    int		w_jumplistlen;		// number of active entries
    int		w_jumplistidx;		// current position

    int		w_changelistidx;	// current position in b_changelist
#endif

#ifdef FEAT_SEARCH_EXTRA
    matchitem_T	*w_match_head;		// head of match list
    int		w_next_match_id;	// next match ID
#endif

    /*
     * the tagstack grows from 0 upwards:
     * entry 0: older
     * entry 1: newer
     * entry 2: newest
     */
    taggy_T	w_tagstack[TAGSTACKSIZE];   // the tag stack
    int		w_tagstackidx;		    // idx just below active entry
    int		w_tagstacklen;		    // number of tags on stack

    /*
     * w_fraction is the fractional row of the cursor within the window, from
     * 0 at the top row to FRACTION_MULT at the last row.
     * w_prev_fraction_row was the actual cursor row when w_fraction was last
     * calculated.
     */
    int		w_fraction;
    int		w_prev_fraction_row;

#ifdef FEAT_GUI
    scrollbar_T	w_scrollbars[2];	// vert. Scrollbars for this window
#endif
#ifdef FEAT_LINEBREAK
    linenr_T	w_nrwidth_line_count;	// line count when ml_nrwidth_width
					// was computed.
    long	w_nuw_cached;		// 'numberwidth' option cached
    int		w_nrwidth_width;	// nr of chars to print line count.
#endif

#ifdef FEAT_QUICKFIX
    qf_info_T	*w_llist;		// Location list for this window
    /*
     * Location list reference used in the location list window.
     * In a non-location list window, w_llist_ref is NULL.
     */
    qf_info_T	*w_llist_ref;
#endif

#ifdef FEAT_MZSCHEME
    void	*w_mzscheme_ref;	// The MzScheme value for this window
#endif

#ifdef FEAT_PERL
    void	*w_perl_private;
#endif

#ifdef FEAT_PYTHON
    void	*w_python_ref;		// The Python value for this window
#endif

#ifdef FEAT_PYTHON3
    void	*w_python3_ref;		// The Python value for this window
#endif

#ifdef FEAT_TCL
    void	*w_tcl_ref;
#endif

#ifdef FEAT_RUBY
    void	*w_ruby_ref;
#endif
};

/*
 * Arguments for operators.
 */
typedef struct oparg_S
{
    int		op_type;	// current pending operator type
    int		regname;	// register to use for the operator
    int		motion_type;	// type of the current cursor motion
    int		motion_force;	// force motion type: 'v', 'V' or CTRL-V
    int		use_reg_one;	// TRUE if delete uses reg 1 even when not
				// linewise
    int		inclusive;	// TRUE if char motion is inclusive (only
				// valid when motion_type is MCHAR
    int		end_adjusted;	// backuped b_op_end one char (only used by
				// do_format())
    pos_T	start;		// start of the operator
    pos_T	end;		// end of the operator
    pos_T	cursor_start;	// cursor position before motion for "gw"

    long	line_count;	// number of lines from op_start to op_end
				// (inclusive)
    int		empty;		// op_start and op_end the same (only used by
				// do_change())
    int		is_VIsual;	// operator on Visual area
    int		block_mode;	// current operator is Visual block mode
    colnr_T	start_vcol;	// start col for block mode operator
    colnr_T	end_vcol;	// end col for block mode operator
    long	prev_opcount;	// ca.opcount saved for K_CURSORHOLD
    long	prev_count0;	// ca.count0 saved for K_CURSORHOLD
} oparg_T;

/*
 * Arguments for Normal mode commands.
 */
typedef struct cmdarg_S
{
    oparg_T	*oap;		// Operator arguments
    int		prechar;	// prefix character (optional, always 'g')
    int		cmdchar;	// command character
    int		nchar;		// next command character (optional)
    int		ncharC1;	// first composing character (optional)
    int		ncharC2;	// second composing character (optional)
    int		extra_char;	// yet another character (optional)
    long	opcount;	// count before an operator
    long	count0;		// count before command, default 0
    long	count1;		// count before command, default 1
    int		arg;		// extra argument from nv_cmds[]
    int		retval;		// return: CA_* values
    char_u	*searchbuf;	// return: pointer to search pattern or NULL
} cmdarg_T;

// values for retval:
#define CA_COMMAND_BUSY	    1	// skip restarting edit() once
#define CA_NO_ADJ_OP_END    2	// don't adjust operator end

#ifdef CURSOR_SHAPE
/*
 * struct to store values from 'guicursor' and 'mouseshape'
 */
// Indexes in shape_table[]
#define SHAPE_IDX_N	0	// Normal mode
#define SHAPE_IDX_V	1	// Visual mode
#define SHAPE_IDX_I	2	// Insert mode
#define SHAPE_IDX_R	3	// Replace mode
#define SHAPE_IDX_C	4	// Command line Normal mode
#define SHAPE_IDX_CI	5	// Command line Insert mode
#define SHAPE_IDX_CR	6	// Command line Replace mode
#define SHAPE_IDX_O	7	// Operator-pending mode
#define SHAPE_IDX_VE	8	// Visual mode with 'selection' exclusive
#define SHAPE_IDX_CLINE	9	// On command line
#define SHAPE_IDX_STATUS 10	// A status line
#define SHAPE_IDX_SDRAG 11	// dragging a status line
#define SHAPE_IDX_VSEP	12	// A vertical separator line
#define SHAPE_IDX_VDRAG 13	// dragging a vertical separator line
#define SHAPE_IDX_MORE	14	// Hit-return or More
#define SHAPE_IDX_MOREL	15	// Hit-return or More in last line
#define SHAPE_IDX_SM	16	// showing matching paren
#define SHAPE_IDX_COUNT	17

#define SHAPE_BLOCK	0	// block cursor
#define SHAPE_HOR	1	// horizontal bar cursor
#define SHAPE_VER	2	// vertical bar cursor

#define MSHAPE_NUMBERED	1000	// offset for shapes identified by number
#define MSHAPE_HIDE	1	// hide mouse pointer

#define SHAPE_MOUSE	1	// used for mouse pointer shape
#define SHAPE_CURSOR	2	// used for text cursor shape

typedef struct cursor_entry
{
    int		shape;		// one of the SHAPE_ defines
    int		mshape;		// one of the MSHAPE defines
    int		percentage;	// percentage of cell for bar
    long	blinkwait;	// blinking, wait time before blinking starts
    long	blinkon;	// blinking, on time
    long	blinkoff;	// blinking, off time
    int		id;		// highlight group ID
    int		id_lm;		// highlight group ID for :lmap mode
    char	*name;		// mode name (fixed)
    char	used_for;	// SHAPE_MOUSE and/or SHAPE_CURSOR
} cursorentry_T;
#endif // CURSOR_SHAPE

#ifdef FEAT_MENU

// Indices into vimmenu_T->strings[] and vimmenu_T->noremap[] for each mode
#define MENU_INDEX_INVALID	-1
#define MENU_INDEX_NORMAL	0
#define MENU_INDEX_VISUAL	1
#define MENU_INDEX_SELECT	2
#define MENU_INDEX_OP_PENDING	3
#define MENU_INDEX_INSERT	4
#define MENU_INDEX_CMDLINE	5
#define MENU_INDEX_TERMINAL	6
#define MENU_INDEX_TIP		7
#define MENU_MODES		8

// Menu modes
#define MENU_NORMAL_MODE	(1 << MENU_INDEX_NORMAL)
#define MENU_VISUAL_MODE	(1 << MENU_INDEX_VISUAL)
#define MENU_SELECT_MODE	(1 << MENU_INDEX_SELECT)
#define MENU_OP_PENDING_MODE	(1 << MENU_INDEX_OP_PENDING)
#define MENU_INSERT_MODE	(1 << MENU_INDEX_INSERT)
#define MENU_CMDLINE_MODE	(1 << MENU_INDEX_CMDLINE)
#define MENU_TERMINAL_MODE	(1 << MENU_INDEX_TERMINAL)
#define MENU_TIP_MODE		(1 << MENU_INDEX_TIP)
#define MENU_ALL_MODES		((1 << MENU_INDEX_TIP) - 1)
// note MENU_INDEX_TIP is not a 'real' mode

// Start a menu name with this to not include it on the main menu bar
#define MNU_HIDDEN_CHAR		']'

struct VimMenu
{
    int		modes;		    // Which modes is this menu visible for?
    int		enabled;	    // for which modes the menu is enabled
    char_u	*name;		    // Name of menu, possibly translated
    char_u	*dname;		    // Displayed Name ("name" without '&')
#ifdef FEAT_MULTI_LANG
    char_u	*en_name;	    // "name" untranslated, NULL when "name"
				    // was not translated
    char_u	*en_dname;	    // "dname" untranslated, NULL when "dname"
				    // was not translated
#endif
    int		mnemonic;	    // mnemonic key (after '&')
    char_u	*actext;	    // accelerator text (after TAB)
    int		priority;	    // Menu order priority
#ifdef FEAT_GUI
    void	(*cb)(vimmenu_T *); // Call-back function
#endif
#ifdef FEAT_TOOLBAR
    char_u	*iconfile;	    // name of file for icon or NULL
    int		iconidx;	    // icon index (-1 if not set)
    int		icon_builtin;	    // icon names is BuiltIn{nr}
#endif
    char_u	*strings[MENU_MODES]; // Mapped string for each mode
    int		noremap[MENU_MODES]; // A REMAP_ flag for each mode
    char	silent[MENU_MODES]; // A silent flag for each mode
    vimmenu_T	*children;	    // Children of sub-menu
    vimmenu_T	*parent;	    // Parent of menu
    vimmenu_T	*next;		    // Next item in menu
#ifdef FEAT_GUI_X11
    Widget	id;		    // Manage this to enable item
    Widget	submenu_id;	    // If this is submenu, add children here
#endif
#ifdef FEAT_GUI_GTK
    GtkWidget	*id;		    // Manage this to enable item
    GtkWidget	*submenu_id;	    // If this is submenu, add children here
# if defined(GTK_CHECK_VERSION) && !GTK_CHECK_VERSION(3,4,0)
    GtkWidget	*tearoff_handle;
# endif
    GtkWidget   *label;		    // Used by "set wak=" code.
#endif
#ifdef FEAT_GUI_MOTIF
    int		sensitive;	    // turn button on/off
    char	**xpm;		    // pixmap data
    char	*xpm_fname;	    // file with pixmap data
#endif
#ifdef FEAT_GUI_ATHENA
    Pixmap	image;		    // Toolbar image
#endif
#ifdef FEAT_BEVAL_TIP
    BalloonEval *tip;		    // tooltip for this menu item
#endif
#ifdef FEAT_GUI_MSWIN
    UINT	id;		    // Id of menu item
    HMENU	submenu_id;	    // If this is submenu, add children here
    HWND	tearoff_handle;	    // hWnd of tearoff if created
#endif
#if FEAT_GUI_HAIKU
    BMenuItem  *id;		    // Id of menu item
    BMenu  *submenu_id;		    // If this is submenu, add children here
# ifdef FEAT_TOOLBAR
    BPictureButton *button;
# endif
#endif
#ifdef FEAT_GUI_MAC
//  MenuHandle	id;
//  short	index;		    // the item index within the father menu
    short	menu_id;	    // the menu id to which this item belongs
    short	submenu_id;	    // the menu id of the children (could be
				    // get through some tricks)
    MenuHandle	menu_handle;
    MenuHandle	submenu_handle;
#endif
#ifdef FEAT_GUI_PHOTON
    PtWidget_t	*id;
    PtWidget_t	*submenu_id;
#endif
};
#else
// For generating prototypes when FEAT_MENU isn't defined.
typedef int vimmenu_T;

#endif // FEAT_MENU

/*
 * Struct to save values in before executing autocommands for a buffer that is
 * not the current buffer.
 */
typedef struct
{
    buf_T	*save_curbuf;	// saved curbuf
    int		use_aucmd_win;	// using aucmd_win
    win_T	*save_curwin;	// saved curwin
    win_T	*new_curwin;	// new curwin
    win_T	*save_prevwin;	// saved prevwin
    bufref_T	new_curbuf;	// new curbuf
    char_u	*globaldir;	// saved value of globaldir
} aco_save_T;

/*
 * Generic option table item, only used for printer at the moment.
 */
typedef struct
{
    const char	*name;
    int		hasnum;
    long	number;
    char_u	*string;	// points into option string
    int		strlen;
    int		present;
} option_table_T;

/*
 * Structure to hold printing color and font attributes.
 */
typedef struct
{
    long_u	fg_color;
    long_u	bg_color;
    int		bold;
    int		italic;
    int		underline;
    int		undercurl;
} prt_text_attr_T;

/*
 * Structure passed back to the generic printer code.
 */
typedef struct
{
    int		n_collated_copies;
    int		n_uncollated_copies;
    int		duplex;
    int		chars_per_line;
    int		lines_per_page;
    int		has_color;
    prt_text_attr_T number;
#ifdef FEAT_SYN_HL
    int		modec;
    int		do_syntax;
#endif
    int		user_abort;
    char_u	*jobname;
#ifdef FEAT_POSTSCRIPT
    char_u	*outfile;
    char_u	*arguments;
#endif
} prt_settings_T;

#define PRINT_NUMBER_WIDTH 8

/*
 * Used for popup menu items.
 */
typedef struct
{
    char_u	*pum_text;	// main menu text
    char_u	*pum_kind;	// extra kind text (may be truncated)
    char_u	*pum_extra;	// extra menu text (may be truncated)
    char_u	*pum_info;	// extra info
} pumitem_T;

/*
 * Structure used for get_tagfname().
 */
typedef struct
{
    char_u	*tn_tags;	// value of 'tags' when starting
    char_u	*tn_np;		// current position in tn_tags
    int		tn_did_filefind_init;
    int		tn_hf_idx;
    void	*tn_search_ctx;
} tagname_T;

typedef struct {
  UINT32_T total[2];
  UINT32_T state[8];
  char_u   buffer[64];
} context_sha256_T;

/*
 * types for expressions.
 */
typedef enum
{
    EXPR_UNKNOWN = 0,
    EXPR_EQUAL,		// ==
    EXPR_NEQUAL,	// !=
    EXPR_GREATER,	// >
    EXPR_GEQUAL,	// >=
    EXPR_SMALLER,	// <
    EXPR_SEQUAL,	// <=
    EXPR_MATCH,		// =~
    EXPR_NOMATCH,	// !~
    EXPR_IS,		// is
    EXPR_ISNOT,		// isnot
    // used with ISN_OPNR
    EXPR_ADD,		// +
    EXPR_SUB,		// -
    EXPR_MULT,		// *
    EXPR_DIV,		// /
    EXPR_REM,		// %
} exptype_T;

/*
 * Structure used for reading in json_decode().
 */
struct js_reader
{
    char_u	*js_buf;	// text to be decoded
    char_u	*js_end;	// NUL in js_buf
    int		js_used;	// bytes used from js_buf
    int		(*js_fill)(struct js_reader *);
				// function to fill the buffer or NULL;
				// return TRUE when the buffer was filled
    void	*js_cookie;	// can be used by js_fill
    int		js_cookie_arg;	// can be used by js_fill
};
typedef struct js_reader js_read_T;

// Maximum number of commands from + or -c arguments.
#define MAX_ARG_CMDS 10

// values for "window_layout"
#define WIN_HOR	    1	    // "-o" horizontally split windows
#define	WIN_VER	    2	    // "-O" vertically split windows
#define	WIN_TABS    3	    // "-p" windows on tab pages

// Struct for various parameters passed between main() and other functions.
typedef struct
{
    int		argc;
    char	**argv;

    char_u	*fname;			// first file to edit

    int		evim_mode;		// started as "evim"
    char_u	*use_vimrc;		// vimrc from -u argument
    int		clean;			// --clean argument

    int		n_commands;		     // no. of commands from + or -c
    char_u	*commands[MAX_ARG_CMDS];     // commands from + or -c arg.
    char_u	cmds_tofree[MAX_ARG_CMDS];   // commands that need free()
    int		n_pre_commands;		     // no. of commands from --cmd
    char_u	*pre_commands[MAX_ARG_CMDS]; // commands from --cmd argument

    int		edit_type;		// type of editing to do
    char_u	*tagname;		// tag from -t argument
#ifdef FEAT_QUICKFIX
    char_u	*use_ef;		// 'errorfile' from -q argument
#endif

    int		want_full_screen;
    int		not_a_term;		// no warning for missing term?
    int		tty_fail;		// exit if not a tty
    char_u	*term;			// specified terminal name
#ifdef FEAT_CRYPT
    int		ask_for_key;		// -x argument
#endif
    int		no_swap_file;		// "-n" argument used
#ifdef FEAT_EVAL
    int		use_debug_break_level;
#endif
    int		window_count;		// number of windows to use
    int		window_layout;		// 0, WIN_HOR, WIN_VER or WIN_TABS

#ifdef FEAT_CLIENTSERVER
    int		serverArg;		// TRUE when argument for a server
    char_u	*serverName_arg;	// cmdline arg for server name
    char_u	*serverStr;		// remote server command
    char_u	*serverStrEnc;		// encoding of serverStr
    char_u	*servername;		// allocated name for our server
#endif
#if !defined(UNIX)
# define EXPAND_FILENAMES
    int		literal;		// don't expand file names
#endif
#ifdef MSWIN
    int		full_path;		// file name argument was full path
#endif
#ifdef FEAT_DIFF
    int		diff_mode;		// start with 'diff' set
#endif
} mparm_T;

/*
 * Structure returned by get_lval() and used by set_var_lval().
 * For a plain name:
 *	"name"	    points to the variable name.
 *	"exp_name"  is NULL.
 *	"tv"	    is NULL
 * For a magic braces name:
 *	"name"	    points to the expanded variable name.
 *	"exp_name"  is non-NULL, to be freed later.
 *	"tv"	    is NULL
 * For an index in a list:
 *	"name"	    points to the (expanded) variable name.
 *	"exp_name"  NULL or non-NULL, to be freed later.
 *	"tv"	    points to the (first) list item value
 *	"li"	    points to the (first) list item
 *	"range", "n1", "n2" and "empty2" indicate what items are used.
 * For an existing Dict item:
 *	"name"	    points to the (expanded) variable name.
 *	"exp_name"  NULL or non-NULL, to be freed later.
 *	"tv"	    points to the dict item value
 *	"newkey"    is NULL
 * For a non-existing Dict item:
 *	"name"	    points to the (expanded) variable name.
 *	"exp_name"  NULL or non-NULL, to be freed later.
 *	"tv"	    points to the Dictionary typval_T
 *	"newkey"    is the key for the new item.
 */
typedef struct lval_S
{
    char_u	*ll_name;	// start of variable name (can be NULL)
    char_u	*ll_name_end;	// end of variable name (can be NULL)
    type_T	*ll_type;	// type of variable (can be NULL)
    char_u	*ll_exp_name;	// NULL or expanded name in allocated memory.
    typval_T	*ll_tv;		// Typeval of item being used.  If "newkey"
				// isn't NULL it's the Dict to which to add
				// the item.
    listitem_T	*ll_li;		// The list item or NULL.
    list_T	*ll_list;	// The list or NULL.
    int		ll_range;	// TRUE when a [i:j] range was used
    int		ll_empty2;	// Second index is empty: [i:]
    long	ll_n1;		// First index for list
    long	ll_n2;		// Second index for list range
    dict_T	*ll_dict;	// The Dictionary or NULL
    dictitem_T	*ll_di;		// The dictitem or NULL
    char_u	*ll_newkey;	// New key for Dict in alloc. mem or NULL.
    blob_T	*ll_blob;	// The Blob or NULL
} lval_T;

// Structure used to save the current state.  Used when executing Normal mode
// commands while in any other mode.
typedef struct {
    int		save_msg_scroll;
    int		save_restart_edit;
    int		save_msg_didout;
    int		save_State;
    int		save_insertmode;
    int		save_finish_op;
    int		save_opcount;
    int		save_reg_executing;
    tasave_T	tabuf;
} save_state_T;

typedef struct {
    varnumber_T vv_prevcount;
    varnumber_T vv_count;
    varnumber_T vv_count1;
} vimvars_save_T;

// Scope for changing directory
typedef enum {
    CDSCOPE_GLOBAL,	// :cd
    CDSCOPE_TABPAGE,	// :tcd
    CDSCOPE_WINDOW	// :lcd
} cdscope_T;

// Variable flavor
typedef enum
{
    VAR_FLAVOUR_DEFAULT,	// doesn't start with uppercase
    VAR_FLAVOUR_SESSION,	// starts with uppercase, some lower
    VAR_FLAVOUR_VIMINFO		// all uppercase
} var_flavour_T;

// argument for mouse_find_win()
typedef enum {
    IGNORE_POPUP,	// only check non-popup windows
    FIND_POPUP,		// also find popup windows
    FAIL_POPUP		// return NULL if mouse on popup window
} mouse_find_T;

// Symbolic names for some registers.
#define DELETION_REGISTER	36
#ifdef FEAT_CLIPBOARD
# define STAR_REGISTER		37
#  ifdef FEAT_X11
#   define PLUS_REGISTER	38
#  else
#   define PLUS_REGISTER	STAR_REGISTER	    // there is only one
#  endif
#endif
#ifdef FEAT_DND
# define TILDE_REGISTER		(PLUS_REGISTER + 1)
#endif

#ifdef FEAT_CLIPBOARD
# ifdef FEAT_DND
#  define NUM_REGISTERS		(TILDE_REGISTER + 1)
# else
#  define NUM_REGISTERS		(PLUS_REGISTER + 1)
# endif
#else
# define NUM_REGISTERS		37
#endif

// structure used by block_prep, op_delete and op_yank for blockwise operators
// also op_change, op_shift, op_insert, op_replace - AKelly
struct block_def
{
    int		startspaces;	// 'extra' cols before first char
    int		endspaces;	// 'extra' cols after last char
    int		textlen;	// chars in block
    char_u	*textstart;	// pointer to 1st char (partially) in block
    colnr_T	textcol;	// index of chars (partially) in block
    colnr_T	start_vcol;	// start col of 1st char wholly inside block
    colnr_T	end_vcol;	// start col of 1st char wholly after block
    int		is_short;	// TRUE if line is too short to fit in block
    int		is_MAX;		// TRUE if curswant==MAXCOL when starting
    int		is_oneChar;	// TRUE if block within one character
    int		pre_whitesp;	// screen cols of ws before block
    int		pre_whitesp_c;	// chars of ws before block
    colnr_T	end_char_vcols;	// number of vcols of post-block char
    colnr_T	start_char_vcols; // number of vcols of pre-block char
};

// Each yank register has an array of pointers to lines.
typedef struct
{
    char_u	**y_array;	// pointer to array of line pointers
    linenr_T	y_size;		// number of lines in y_array
    char_u	y_type;		// MLINE, MCHAR or MBLOCK
    colnr_T	y_width;	// only set if y_type == MBLOCK
#ifdef FEAT_VIMINFO
    time_t	y_time_set;
#endif
} yankreg_T;

// The offset for a search command is store in a soff struct
// Note: only spats[0].off is really used
typedef struct soffset
{
    int		dir;		// search direction, '/' or '?'
    int		line;		// search has line offset
    int		end;		// search set cursor at end
    long	off;		// line or char offset
} soffset_T;

// A search pattern and its attributes are stored in a spat struct
typedef struct spat
{
    char_u	    *pat;	// the pattern (in allocated memory) or NULL
    int		    magic;	// magicness of the pattern
    int		    no_scs;	// no smartcase for this pattern
    soffset_T	    off;
} spat_T;

/*
 * Optional extra arguments for searchit().
 */
typedef struct
{
    linenr_T	sa_stop_lnum;	// stop after this line number when != 0
#ifdef FEAT_RELTIME
    proftime_T	*sa_tm;		// timeout limit or NULL
    int		sa_timed_out;	// set when timed out
#endif
    int		sa_wrapped;	// search wrapped around
} searchit_arg_T;


#define WRITEBUFSIZE	8192	// size of normal write buffer

#define FIO_LATIN1	0x01	// convert Latin1
#define FIO_UTF8	0x02	// convert UTF-8
#define FIO_UCS2	0x04	// convert UCS-2
#define FIO_UCS4	0x08	// convert UCS-4
#define FIO_UTF16	0x10	// convert UTF-16
#ifdef MSWIN
# define FIO_CODEPAGE	0x20	// convert MS-Windows codepage
# define FIO_PUT_CP(x) (((x) & 0xffff) << 16)	// put codepage in top word
# define FIO_GET_CP(x)	(((x)>>16) & 0xffff)	// get codepage from top word
#endif
#ifdef MACOS_CONVERT
# define FIO_MACROMAN	0x20	// convert MacRoman
#endif
#define FIO_ENDIAN_L	0x80	// little endian
#define FIO_ENCRYPTED	0x1000	// encrypt written bytes
#define FIO_NOCONVERT	0x2000	// skip encoding conversion
#define FIO_UCSBOM	0x4000	// check for BOM at start of file
#define FIO_ALL	-1	// allow all formats

// When converting, a read() or write() may leave some bytes to be converted
// for the next call.  The value is guessed...
#define CONV_RESTLEN 30

// We have to guess how much a sequence of bytes may expand when converting
// with iconv() to be able to allocate a buffer.
#define ICONV_MULT 8
