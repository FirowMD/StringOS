__asm("jmp KernelStart");

typedef unsigned char boolean;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef unsigned long size_t;
typedef signed int errno_t;

#define OSMODE ((boolean *)0xbf1c)
#define OSMODE_STD			0
#define OSMODE_BM			1

#define VIDEO_BUF_PTR ((u8 *)0x000b8000)
#define VIDEO_BUF_PTR_END ((u8 *)0x000c7fff)
#define VIDEO_BUF_LINEMAX ((size_t)80)
#define VIDEO_BUF_MAX ((size_t)260)
#define VIDEO_COL_DEF 0x07
#define VIDEO_POS_CHR(X, Y) (*(VIDEO_BUF_PTR + (Y) * VIDEO_BUF_LINEMAX * 2 + (X) * 2))
#define VIDEO_POS_COL(X, Y) (*(VIDEO_BUF_PTR + (Y) * VIDEO_BUF_LINEMAX * 2 + (X) * 2 + 1))
#define VIDEO_SET(CHR, COL, X, Y)  \
	{                              \
		VIDEO_POS_CHR(X, Y) = CHR; \
		VIDEO_POS_COL(X, Y) = COL; \
	}
#define CURSOR_PORT (0x3D4)

#define KSCAN_OFFSET (0x80)
#define KSCAN_SPACE (0x29)
#define KSCAN_RETURN (0x1C)
#define KSCAN_SHIFT (0x2A)
#define KSCAN_CTRL (0x1D)
#define KSCAN_ALT (0x38)
#define KSCAN_BACK (0x0E)

#define TERMINAL_STDIN_SZ ((size_t)0xFF)
#define TERMINAL_STDOUT_SZ ((size_t)0x7FF)
#define TERMINAL_OUT_SZ (TERMINAL_STDIN_SZ + TERMINAL_STDOUT_SZ)
#define TERMINAL_MAX_LINES ((size_t)25)
#define BUFSIZE ((size_t)0xFF)

#define KSHARE_COUNT_MAX ((size_t)0x10)
#define KSHARE_ALLOC_MAX ((size_t)0x1FF)
#define KSHARE_NAMEMAX ((size_t)0x20)

#define PROGRAM_MAX ((size_t)0xFF)		// Maximum loadable number of programs
#define PROGRAM_NAMEMAX	((size_t)32)
#define PROGRAM_ARGMAX ((size_t)4)

#define SUCCESS						0
#define FAIL						1
#define ERR_NULL_POINTER			2
#define ERR_TERMINAL_BUFFER_CRASHED 3
#define ERR_TERMINAL_INPUT_IS_EMPTY	4

#define FALSE 0
#define TRUE 1

#define NULL (0UL)
#define IS_OK(VAR, EXIT)  \
	if ((VAR) != SUCCESS) \
	{                     \
		goto EXIT;        \
	}
#define IS_NULL(VAR, EXIT) \
	if ((VAR) == NULL)     \
	{                      \
		goto EXIT;         \
	}

#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)

#define GDT_CS (0x8)
#define PIC1_PORT (0x20)

//
// Structures
//

struct _IdtEntry
{
	unsigned short BaseLo;
	unsigned short SegmSel;
	unsigned char AlwaysZero;
	unsigned char Flags;
	unsigned short BaseHi;
} __attribute__((packed));
typedef struct _IdtEntry IdtEntry;

struct _IdtPtr
{
	unsigned short Limit;
	unsigned int Base;
} __attribute__((packed));
typedef struct _IdtPtr IdtPtr;

IdtEntry g_idt[256];
IdtPtr g_idt_ptr;

typedef void (*IntrHandler)();

typedef struct _Cursor
{
	size_t X;
	size_t Y;
	boolean IsInput;
} Cursor;

typedef struct _KerShare
{
	struct {
		char Name[KSHARE_NAMEMAX];
		u8 *Ptr;
		u8 Size;
	} Table[KSHARE_COUNT_MAX];
	u8 Memory[KSHARE_ALLOC_MAX];
	u8 *Next;
	u8 *End;
	u16 Count;
} KerShare;

typedef struct _MsgProg
{
	u16 Count;
	char **Args;
} MsgProg;

typedef struct _SingleProg
{
	char Name[PROGRAM_NAMEMAX];
	int (*Main)(MsgProg *);
} SingleProg;

typedef struct _ProgramBox
{
	u16 Count;
	SingleProg Program[PROGRAM_MAX];
} ProgramBox;

static const char alphabet[] = {
	' ', '!', '"', '#', '$', '%', '&', '\'',
	'(', ')', '*', '+', ',', '-', '.', '/',
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', ':', ';', '<', '=', '>', '?',
	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
	'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
	'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w',
	'x', 'y', 'z', '{', '|', '}', '~'
};

//
// Prototypes
//

i32 idiv(i32 a, i32 b);
u32 udiv(u32 a, u32 b);
u32 mod(u32 a, u32 b);
char *itoa(size_t num, char *p_str, size_t base);

void TerminalClear(void);
void TerminalApplyBackspace(void);
errno_t TerminalGetChar(const char c);
errno_t TerminalPutChar(const char c);
void TerminalDelLastChar();
errno_t TerminalPrint(const char *p_str);
void TerminalKeyHandle(Cursor *p_cur, const char c);
Cursor *TerminalCursor(Cursor *p_cur);
void TerminalCursorSetPos(size_t x, size_t y);
void TerminalInput(char *p_data, size_t *p_dataSz);
void TerminalEnter();
void TerminalOpen();
boolean TerminalReturn();
void TerminalClose();
boolean TerminalIsOpen();
void TerminalFlush();

errno_t StrCatA(char *p_dst, size_t dstSz, const char *p_src);
errno_t StrCpyA(char *p_dst, size_t dstSz, const char *p_src);
i8 StrCmpA(char *p_str, char *p_cmp);
i8 StrnCmpA(char *p_str, char *p_cmp);
size_t StrLenA(const char *p_str);
const char *StrChrA(const char *p_str, const char c);
char *StrChrA(char *p_str, const char c);
const char *StrStrA(const char *p_str, const char *p_sub);
char *StrTokA(char *p_str, const char delim);
char *StrTokA(char *p_str, const char *p_sub);
char *IntToStrA(size_t val);
extern inline boolean IsUpper(const char c);
extern inline boolean IsLower(const char c);
char ToUpper(const char c);
char ToLower(const char c);

void ZeroMemory(void *p_buf, size_t bufSz);
void CopyMemory(void *p_buf, void *p_data, size_t dataSz);
char GetKeyChar(u8 code);

char *Stdin(char p_newBuf[TERMINAL_STDIN_SZ]);
char *Stdout(char p_newBuf[TERMINAL_STDOUT_SZ]);

void DefaultIntrHandler();
void IntrRegHandler(i32 num, u16 segmSel, u16 flags, IntrHandler hndlr);
void IntrStart();
void IntrEnable();
void IntrDisable();
extern inline char inb(u16 port);
extern inline void outb(u16 port, char data);
extern inline void outw(unsigned short port, unsigned int data);
void KeyboardHandler();
void KeyboardKey();
void KeyHandler(u8 code);

extern inline boolean GetOsMode();
void InitTerminal();
void InitIntr();
void InitKeyboard();
void InitShare();
void InitProgBox();

KerShare *KernelShare(KerShare *p_ks);
u8 *KernelNewShare(const char *p_name, size_t blockSz);
u8 *KernelGetShare(const char *p_name, size_t *p_size);

errno_t ProgStart(const char *p_progName, MsgProg *p_arg, int *p_result);
boolean ProgExists(const char *p_progName, u16 *p_progId);
errno_t ProgAdd(const char *p_name, int (*main)(MsgProg *));
ProgramBox *ProgBox(ProgramBox *p_progBox);

void BoyerMooreBuildShift(const char *p_sub, size_t p_shift[sizeof(alphabet)]);
size_t BoyerMoore(const char *p_str, const char *p_sub);

//
// Program staff
//

void PrintFmt(const char *p_format, ...);

//
// Programs
//

static int StringOs_Help(MsgProg *p_msg);
static int StringOs_Info(MsgProg *p_msg);
static int StringOs_Upcase(MsgProg *p_msg);
static int StringOs_Downcase(MsgProg *p_msg);
static int StringOs_Titlize(MsgProg *p_msg);
static int StringOs_Template(MsgProg *p_msg);
static int StringOs_Search(MsgProg *p_msg);
static int StringOs_Shutdown(MsgProg *p_msg);

//
// Definitions
//

extern "C" int KernelStart()
{
	InitIntr();
	InitKeyboard();
	InitTerminal();
	IntrStart();
	IntrEnable();
	InitShare();
	InitProgBox();

	TerminalPrint("Welcome to StringOS!\n");
	TerminalFlush();

	while (TRUE)
	{
		TerminalEnter();
	}

	return SUCCESS;
}

i32 idiv(i32 a, i32 b)
{
	i32 res;
	a = (a > 0) ? a : -a;
	b = (b > 0) ? b : -b;
	for (res = 0; a > 0; res++)
	{
		a -= b;
	}
	return res;
}

u32 udiv(u32 a, u32 b)
{
	u32 res;
	for (res = 0; a > 0; res++)
	{
		a -= b;
	}
	return res;
}

u32 mod(u32 a, u32 b)
{
	for (;;)
	{
		if (a <= b)
		{
			return a;
		}
		a -= b;
	}
}

char *itoa(size_t num, char *p_str, size_t base)
{
	size_t i = 0;
	bool isNegative = false;

	if (num == 0)
	{
		p_str[i++] = '0';
		p_str[i] = '\0';
		return p_str;
	}

	if (num < 0 && base == 10)
	{
		isNegative = true;
		num = -num;
	}

	while (num != 0)
	{
		size_t rem = num % base;
		p_str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	if (isNegative)
		p_str[i++] = '-';

	p_str[i] = '\0';

	size_t start = 0;
	size_t end = i - 1;
	char buf;

	while (start < end)
	{
		buf = *(p_str + start);
		*(p_str + start) = *(p_str + end);
		*(p_str + end) = buf;
		start++;
		end--;
	}

	return p_str;
}

void TerminalClear(void)
{
	u8 *p_video_buf = VIDEO_BUF_PTR;
	while (p_video_buf < VIDEO_BUF_PTR_END)
	{
		*p_video_buf = 0;
		p_video_buf++;
	}
}

void TerminalApplyBackspace(void)
{
	char *p_stdin = Stdin(NULL);
	char *p_stdout = Stdout(NULL);
	size_t stdin_sz = StrLenA(p_stdin);
	size_t stdout_sz = StrLenA(p_stdout);

	for (size_t i = 0; i < stdin_sz; i++)
	{
		if (p_stdin[i] == '\b')
		{
			if (i > 0)
			{
				p_stdin[i] = 0;
				p_stdin[i - 1] = 0;
			}
			else
			{
				p_stdin[i] = 0;
			}
		}
	}

	for (size_t i = 0; i < stdout_sz; i++)
	{
		if (p_stdout[i] == '\b')
		{
			if (i > 0)
			{
				p_stdout[i] = 0;
				p_stdout[i - 1] = 0;
			}
			else
			{
				p_stdout[i] = 0;
			}
		}
	}
}

errno_t TerminalGetChar(const char c)
{
	char *p_stdin = Stdin(NULL);
	size_t stdout_sz = StrLenA(p_stdin);
	if (stdout_sz >= 40 && c != '\b' && c != '\n')
	{
		return FAIL;
	}
	char p_buf[2] = { c, '\0' };
	return StrCatA(p_stdin, TERMINAL_STDIN_SZ, p_buf);
}

errno_t TerminalPutChar(const char c)
{
	char *p_stdout = Stdout(NULL);
	char p_buf[2] = { c, '\0' };
	return StrCatA(p_stdout, TERMINAL_STDOUT_SZ, p_buf);
}

void TerminalDelLastChar()
{
	char *p_stdin = Stdin(NULL);
	size_t stdin_sz = StrLenA(p_stdin);
	
	if (stdin_sz > 0)
	{
		p_stdin[stdin_sz] = 0;
	}
}

errno_t TerminalPrint(const char *p_str)
{
	char *p_stdout = Stdout(NULL);
	return StrCatA(p_stdout, TERMINAL_STDOUT_SZ, p_str);
}

void TerminalKeyHandle(Cursor *p_cur, const char c)
{
	static boolean is_input = FALSE;

	if (p_cur != NULL)
	{
		is_input = p_cur->IsInput;
	}

	if (is_input && c != '\0')
	{
		if (c == '`')
		{
			TerminalClear();
			ZeroMemory(Stdout(NULL), StrLenA(Stdout(NULL)));
			TerminalPrint("(> ");
			TerminalFlush();
		}
		else
		{
			TerminalGetChar(c);
		}
	}
}

Cursor *TerminalCursor(Cursor *p_cur)
{
	static Cursor *p = NULL;
	if (p_cur != NULL)
	{
		p = p_cur;
	}
	return p;
}

void TerminalCursorSetPos(size_t x, size_t y)
{
	size_t new_pos = (y * VIDEO_BUF_LINEMAX) + x;
	outb(CURSOR_PORT, 0x0F);
	outb(CURSOR_PORT + 1, (char)(new_pos & 0xFF));
	outb(CURSOR_PORT, 0x0E);
	outb(CURSOR_PORT + 1, (char)((new_pos >> 8) & 0xFF));
}

void TerminalInput(char *p_data, size_t *p_dataSz)
{
	char *p_stdin = Stdin(NULL);
	size_t stdin_sz;
	TerminalOpen();
	while (TerminalReturn() == FALSE)
	{
		TerminalFlush();
	}
	TerminalClose();

	stdin_sz = StrLenA(p_stdin);
	for (size_t i = 0; i < stdin_sz; i++)
	{
		if (p_stdin[i] == '\n')
		{
			p_data[i] = '\0';
			*p_dataSz = i + 1;
			break;
		}
		p_data[i] = p_stdin[i];
	}

	ZeroMemory(p_stdin, stdin_sz);
}

void TerminalEnter()
{
	char p_data[BUFSIZE];
	static char pp_args[PROGRAM_ARGMAX][PROGRAM_NAMEMAX] = {0};
	char *p_token;
	size_t data_sz;
	size_t i;
	MsgProg msg = {0};
	int result;
	errno_t err;

	TerminalPrint("(> ");
	TerminalFlush();
	TerminalInput(p_data, &data_sz);

	if (p_data[0] == 0)
	{
		return;
	}

	p_token = StrTokA(p_data, ' ');
	StrCpyA(pp_args[0], PROGRAM_NAMEMAX, p_token);
	for (i = 1; i < PROGRAM_MAX; i++)
	{
		p_token = StrTokA(NULL, ' ');
		if (p_token == NULL)
		{
			break;
		}
		StrCpyA(pp_args[i], PROGRAM_NAMEMAX, p_token);
	}

	msg.Count = i;
	for (i = 0; i < msg.Count; i++)
	{
		msg.Args[i] = pp_args[i];
	}
	
	ProgStart(pp_args[0], &msg, &result);
}

void TerminalOpen()
{
	Cursor *p_cur = TerminalCursor(NULL);
	p_cur->IsInput = TRUE;
	TerminalKeyHandle(p_cur, '\0');
}

boolean TerminalReturn()
{
	char *p_buf = Stdin(NULL);
	size_t len = StrLenA(p_buf);
	
	if (p_buf[len - 1] == '\n')
	{
		return TRUE;
	}

	return FALSE;
}

void TerminalClose()
{
	Cursor *p_cur = TerminalCursor(NULL);
	p_cur->IsInput = FALSE;
	StrCatA(Stdout(NULL), TERMINAL_STDOUT_SZ, Stdin(NULL));
	TerminalKeyHandle(p_cur, '\0');
}

boolean TerminalIsOpen()
{
	return TerminalCursor(NULL)->IsInput;
}

void TerminalFlush()
{
	Cursor *p_cur = TerminalCursor(NULL);
	char out_buf[TERMINAL_OUT_SZ];
	size_t out_len;
	size_t x;
	size_t y;
	
	TerminalApplyBackspace();
	ZeroMemory(out_buf, TERMINAL_OUT_SZ);
	StrCatA(out_buf, TERMINAL_OUT_SZ, Stdout(NULL));

	if (p_cur->IsInput)
	{
		StrCatA(out_buf, TERMINAL_OUT_SZ, Stdin(NULL));
	}

	out_len = StrLenA(out_buf);
	x = 0;
	y = 0;

	for (size_t i = 0; i < TERMINAL_OUT_SZ; i++)
	{
		if (out_buf[i] == '\n')
		{
			x = 0;
			y += 1;
			if (y == TERMINAL_MAX_LINES)
			{
				ZeroMemory(Stdout(NULL), StrLenA(Stdout(NULL)));
				TerminalPrint("(> ");
				TerminalClear();
				y = 0;
			}
		}
		else
		{
			if (i == out_len)
			{
				if (p_cur->IsInput)
				{
					p_cur->X = x;
					p_cur->Y = y;
					TerminalCursorSetPos(p_cur->X, p_cur->Y);
				}
			}

			VIDEO_SET(out_buf[i], VIDEO_COL_DEF, x, y);

			if (x == VIDEO_BUF_LINEMAX)
			{
				x = 0;
				y += 1;
			}
			else
			{
				x += 1;
			}
		}
	}
}

errno_t StrCatA(char *p_dst, size_t dstSz, const char *p_src)
{
	errno_t res = SUCCESS;
	size_t src_len = StrLenA(p_src);
	char *p_from = (char *)p_src;
	for (; dstSz > src_len || ((res = FAIL) && 0); dstSz--)
	{
		if (*p_dst == 0)
		{
			break;
		}
		p_dst++;
	}
	if (res == SUCCESS)
	{
		while (*p_from)
		{
			*p_dst = *p_from;
			p_dst++;
			p_from++;
		}
	}
	return res;
}

errno_t StrCpyA(char *p_dst, size_t dstSz, const char *p_src)
{
	for (size_t i = 0; i < dstSz; i++)
	{
		if (i == dstSz)
		{
			return FAIL;
		}
		p_dst[i] = p_src[i];
	}
	return SUCCESS;
}

i8 StrCmpA(char *p_str, char *p_cmp)
{
	i8 res = 0;
	while ((*p_str != '\0' && *p_cmp != '\0') && *p_str == *p_cmp)
	{
		p_str++;
		p_cmp++;
	}
	res = (*p_str == *p_cmp) ? 0 : (*p_str > *p_cmp) ? 1
										 : -1;
	return res;
}

i8 StrnCmpA(char *p_str, char *p_cmp, size_t n)
{
	i8 res = 0;
	for (size_t i = 0; i < n; i++)
	{
		if (p_str[i] != p_cmp[i])
		{
			res = (p_str[i] == p_cmp[i]) ? 0 : (p_str[i] > p_cmp[i]) ? 1
																	 : -1;
			break;
		}
		if (p_str[i] == 0 || p_cmp[i] == 0)
		{
			res = (p_str[i] == p_cmp[i]) ? 0 : (p_str[i] > p_cmp[i]) ? 1
																	 : -1;
			break;
		}
	}
	return res;
}

size_t StrLenA(const char *p_str)
{
	size_t len = 0;
	for (; p_str[len]; len++)
	{
		continue;
	}
	return len;
}

const char *StrChrA(const char *p_str, const char c)
{
	size_t len = StrLenA(p_str);
	for (size_t i = 0; i < len; i++)
	{
		if (p_str[i] == c)
		{
			return p_str + i;
		}
	}
	return NULL;
}

char *StrChrA(char *p_str, const char c)
{
	size_t len = StrLenA(p_str);
	for (size_t i = 0; i < len; i++)
	{
		if (p_str[i] == c)
		{
			return p_str + i;
		}
	}
	return NULL;
}

const char *StrStrA(const char *p_str, const char *p_sub)
{
	size_t off;
	if (GetOsMode() == OSMODE_BM)
	{
		off = BoyerMoore(p_str, p_sub);
		if (off != -1)
		{
			return p_str + off;
		}
	}
	else
	{
		if (StrLenA(p_str) < StrLenA(p_sub))
		{
			return NULL;
		}
		size_t len = StrLenA(p_str) - StrLenA(p_sub);
		for (off = 0; off <= len; off++)
		{
			if (StrnCmpA((char *)(p_str + off), (char *)p_sub, StrLenA(p_sub)) == 0)
			{
				return p_str + off;
			}
		}
	}
	return NULL;
}

char *StrTokA(char *p_str, const char delim)
{
	static char p_save[BUFSIZE];
	static char *p_ret;
	static size_t len;
	static size_t i;
	char c;

	if (p_str)
	{
		len = StrLenA(p_str);
		if (len >= BUFSIZE || len == 0)
		{
			return NULL;
		}
		StrCpyA(p_save, BUFSIZE, p_str);
		i = 0;
	}

	if (i)
	{
		i++;
	}
	
	if (i >= len)
	{
		return NULL;
	}

	p_ret = p_save + i;
	for (; p_save[i] && p_save[i] != delim; i++)
	{
		continue;
	}

	p_save[i] = '\0';
	return p_ret;
}

char *StrTokA(char *p_str, const char *p_delim)
{
	static char *buf;
	char *ret;
	char *b;
	const char *d;

	if (p_str != NULL) buf = p_str;
	if (buf[0] == '\0') return NULL;

	ret = buf;

	for (b = buf; *b != '\0'; b++)
	{
		for (d = p_delim; *d != '\0'; d++)
		{
			if (*b == *d)
			{
				*b = '\0';
				buf = b + 1;

				if (b == ret)
				{
					ret++;
					continue;
				}
				return ret;
			}
		}
	}

	return ret;
}

char *IntToStrA(size_t val)
{
	static char buf[BUFSIZE];
	return itoa(val, buf, 10);
}

inline boolean IsUpper(const char c)
{
	return (c >= 'A') && (c <= 'Z');
}

inline boolean IsLower(const char c)
{
	return (c >= 'a') && (c <= 'z');
}

char ToUpper(const char c)
{
	if (IsLower(c))
	{
		return c - ('a' - 'A');
	}
	else
	{
		return c;
	}
}

char ToLower(const char c)
{
	if (IsUpper(c))
	{
		return c + ('a' - 'A');
	}
	else
	{
		return c;
	}
}

void ZeroMemory(void *p_buf, size_t bufSz)
{
	u8 *p = (u8 *)p_buf;
	for (; bufSz; bufSz--)
	{
		*p = 0;
		p++;
	}
}

void CopyMemory(void *p_buf, void *p_data, size_t dataSz)
{
	i8 *p_dst = (i8 *)p_buf;
	i8 *p_src = (i8 *)p_data;
	for (; dataSz; dataSz--)
	{
		*p_dst = *p_src;
		p_dst++;
		p_src++;
	}
}

char GetKeyChar(u8 code)
{
	static char key_map[128] = {
		0, '\x13', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',		// 0x0B
		'-', '+', '\b', '?', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',		// 0x18
		'p', 0, 0, '\n', 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k',	'l',	// 0x26
		':', '\'', '`', 0, 0, 'z', 'x', 'c', 'v', 'b', 'n', 'm', 0, 0, '/',	// 0x35
		0, '*', 0, ' '
	};

	return key_map[code];
}

char *Stdin(char p_newBuf[TERMINAL_STDIN_SZ])
{
	static char *p_buf = NULL;
	if (p_newBuf != NULL)
	{
		p_buf = p_newBuf;
	}
	return p_buf;
}

char *Stdout(char p_newBuf[TERMINAL_STDOUT_SZ])
{
	static char *p_buf = NULL;
	if (p_newBuf != NULL)
	{
		p_buf = p_newBuf;
	}
	return p_buf;
}

void DefaultIntrHandler()
{
	asm("pusha");
	asm("popa; leave; iret");
}

void IntrRegHandler(i32 num, u16 segmSel, u16 flags, IntrHandler hndlr)
{
	u32 hndlr_addr = (u32)hndlr;
	g_idt[num].BaseLo = (u16)(hndlr_addr & 0xFFFF);
	g_idt[num].SegmSel = segmSel;
	g_idt[num].AlwaysZero = 0;
	g_idt[num].Flags = flags;
	g_idt[num].BaseHi = (u16)(hndlr_addr >> 16);
}

void IntrStart()
{
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
	g_idt_ptr.Base = (unsigned int)(&g_idt[0]);
	g_idt_ptr.Limit = (sizeof(struct _IdtEntry) * idt_count) - 1;
	asm("lidt %0" : : "m"(g_idt_ptr));
}

void IntrEnable()
{
	asm("sti");
}

void IntrDisable()
{
	asm("cli");
}

inline char inb(u16 port)
{
	unsigned char data;
	asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port));
	return data;
}

inline void outb(u16 port, char data)
{
	asm volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

inline void outw (unsigned short port, unsigned int data)
{
  asm volatile ("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

void KeyboardHandler()
{
	asm("pusha");
	KeyboardKey();
	outb(PIC1_PORT, 0x20);
	asm("popa; leave; iret");
}

void KeyboardKey()
{
	if (inb(0x64) & 0x01)
	{
		u8 scan_code;
		scan_code = inb(0x60);
		KeyHandler(scan_code);
	}
}

void KeyHandler(u8 code)
{
	static boolean is_shift_pressed = FALSE;
	char c;

	if (code < 128) // Pressed
	{
		if (code == KSCAN_SHIFT)
		{
			is_shift_pressed = TRUE;
			return;
		}

		c = GetKeyChar(code);
		if (c != '\0')
		{
			if (is_shift_pressed)
			{
				if (c == '8')
				{
					c = '*';
				}
				else
				{
					c -= 'a' - 'A';
				}
			}
			TerminalKeyHandle(NULL, c);
		}
	}
	else // Released
	{
		if (code == KSCAN_SHIFT + KSCAN_OFFSET)
		{
			is_shift_pressed = FALSE;
			return;
		}
	}
}

inline boolean GetOsMode()
{
	return *(OSMODE);
}

void InitTerminal()
{
	static Cursor cur = {0};
	static char p_stdin[TERMINAL_STDIN_SZ];
	static char p_stdout[TERMINAL_STDOUT_SZ];
	TerminalClear();
	TerminalCursor(&cur);
	ZeroMemory(p_stdin, TERMINAL_STDIN_SZ);
	ZeroMemory(p_stdout, TERMINAL_STDOUT_SZ);
	Stdin(p_stdin);
	Stdout(p_stdout);
}

void InitIntr()
{
	int i;
	int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
	for (i = 0; i < idt_count; i++)
		IntrRegHandler(i, GDT_CS, 0x80 | IDT_TYPE_INTR, DefaultIntrHandler);
}

void InitKeyboard()
{
	IntrRegHandler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, KeyboardHandler);
	outb(PIC1_PORT + 1, 0xFF ^ 0x02);
}

void InitShare()
{
	static KerShare ks = {0};
	KernelShare(&ks);
}

void InitProgBox()
{
	static ProgramBox prog_box;
	ZeroMemory(&prog_box, sizeof(ProgramBox));
	ProgBox(&prog_box);

	// Programs here
	ProgAdd("help", StringOs_Help);
	ProgAdd("info", StringOs_Info);
	ProgAdd("upcase", StringOs_Upcase);
	ProgAdd("downcase", StringOs_Downcase);
	ProgAdd("titlize", StringOs_Titlize);
	ProgAdd("template", StringOs_Template);
	ProgAdd("search", StringOs_Search);
	ProgAdd("shutdown", StringOs_Shutdown);
}

KerShare *KernelShare(KerShare *p_ks)
{
	static KerShare *p = NULL;
	if (p_ks != NULL)
	{
		p = p_ks;
	}
	return p;
}

u8 *KernelNewShare(const char *p_name, size_t blockSz)
{
	KerShare *p_ks = KernelShare(NULL);
	u8 *p_ret = NULL;
	if (p_ks->Next == NULL)
	{
		p_ks->Next = p_ks->Memory;
		p_ks->End = p_ks->Memory + KSHARE_ALLOC_MAX;
	}
	if (p_ks->Count < KSHARE_COUNT_MAX)
	{
		if (p_ks->Next + blockSz < p_ks->End)
		{
			p_ret = p_ks->Next;
			p_ks->Next += blockSz;
			StrCpyA(p_ks->Table[p_ks->Count].Name, KSHARE_NAMEMAX, p_name);
			p_ks->Table[p_ks->Count].Ptr = p_ret;
			p_ks->Table[p_ks->Count].Size = blockSz;
			p_ks->Count += 1;
		}
	}
	return p_ret;
}

u8 *KernelGetShare(const char *p_name, size_t *p_size)
{
	KerShare *p_ks = KernelShare(NULL);
	u16 count = p_ks->Count;
	for (u16 i = 0; i < count; i++)
	{
		if (StrCmpA(p_ks->Table[i].Name, (char*)p_name) == 0)
		{
			*p_size = p_ks->Table[i].Size;
			return p_ks->Table[i].Ptr;
		}
	}
	return NULL;
}

errno_t ProgStart(const char *p_progName, MsgProg *p_arg, int *p_result)
{
	errno_t err = FAIL;
	char *p_stdout = Stdout(NULL);
	size_t stdout_sz = StrLenA(p_stdout);
	ProgramBox *p_prog_box = ProgBox(NULL);
	u16 prog_id;
	
	// TerminalClear();
	// ZeroMemory(p_stdout, stdout_sz);

	if (ProgExists(p_progName, &prog_id))
	{
		*p_result = p_prog_box->Program[prog_id].Main(p_arg);
		err = SUCCESS;
	}
	else
	{
		TerminalPrint("Program ");
		TerminalPrint(p_progName);
		TerminalPrint(" doesn't exists\n");
		TerminalFlush();
	}

	return err;
}

boolean ProgExists(const char *p_progName, u16 *p_progId)
{
	ProgramBox *p_prog_box = ProgBox(NULL);
	SingleProg *p_program = p_prog_box->Program;
	u16 n = p_prog_box->Count;

	for (u16 i = 0; i < n; i++)
	{
		if (StrCmpA(p_program[i].Name, (char*)p_progName) == 0)
		{
			*p_progId = i;
			return TRUE;
		}
	}
	return FALSE;
}

errno_t ProgAdd(const char *p_name, int (*main)(MsgProg *))
{
	errno_t err = FAIL;
	ProgramBox *p_prog_box = ProgBox(NULL);
	u16 n = p_prog_box->Count;

	if (p_name && main)
	{
		if (n < PROGRAM_MAX)
		{
			StrCpyA(p_prog_box->Program[n].Name, PROGRAM_NAMEMAX, p_name);
			p_prog_box->Program[n].Main = main;
			p_prog_box->Count += 1;
			err = SUCCESS;
		}
	}

	return err;
}

ProgramBox *ProgBox(ProgramBox *p_progBox)
{
	static ProgramBox *p = NULL;
	if (p_progBox != NULL)
	{
		p = p_progBox;
	}
	return p;
}

void BoyerMooreBuildShift(const char *p_sub, size_t p_shift[sizeof(alphabet)])
{
	size_t len = StrLenA(p_sub);
	for (size_t i = 0; i < sizeof(alphabet); i++)
	{
		const char *p = StrChrA(p_sub, alphabet[i]);
		if (p != NULL && p != p_sub + len - 1)
		{
			p_shift[i] = (size_t)(p_sub + len - 1 - p);
		}
		else
		{
			p_shift[i] = len;
		}
	}
}

size_t BoyerMoore(const char *p_str, const char *p_sub)
{
	size_t p_shift[sizeof(alphabet)];
	size_t str_len = StrLenA(p_str);
	size_t sub_len = StrLenA(p_sub);
	size_t i = sub_len - 1;
	size_t v = i;
	size_t k = i;

	if (sub_len > str_len)
	{
		return -1;
	}

	BoyerMooreBuildShift(p_sub, p_shift);

	while (v >= 0 && i <= str_len - 1)
	{
		v = sub_len - 1;
		k = i;
		while (p_str[k] == p_sub[v])
		{
			if (v == 0)
			{
				return k;
			}
			k--;
			v--;
		}
		i += p_shift[p_str[i] - 32]; // 32 - 127 ASCII range
	}

	if (k >= str_len - sub_len)
	{
		return -1;
	}
	else
	{
		return k;
	}
}

//
// Program staff
//

void PrintFmt(const char *p_format, ...)
{
	size_t i;
	char c;
	register char **v asm("esi");

	asm("lea 12(%%ebp), %0" : "=r"(v));

	for (size_t i = 0;; i++)
	{
		c = p_format[i];
		if (c == '\0')
		{
			break;
		}
		if (c == '%')
		{
			TerminalPrint(*v);
			v++;
		}
		else if (c == '$')
		{
			TerminalPrint(IntToStrA(*((size_t *)v)));
			v++;
		}
		else if (c == '^')
		{
			TerminalPutChar(*((char *)v));
			v++;
		}
		else
		{
			TerminalPutChar(c);
		}
		TerminalFlush();
	}
}

//
// Programs
//

static int StringOs_Help(MsgProg *p_msg)
{
	ProgramBox *p_pb = ProgBox(NULL);
	SingleProg *p_sp = p_pb->Program;
	u16 count = p_pb->Count;

	PrintFmt("Available commands:\n");
	for (u16 i = 0; i < count; i++)
	{
		PrintFmt("$: %\n", i, p_sp[i].Name);
	}

	return 0;
}

static int StringOs_Info(MsgProg *p_msg)
{
	PrintFmt(
		"Author: Kuchiev.\n"
		"OS: Linux\n"
		"Translator: GNU assembler, AT&T\n"
		"Compiler: GCC\n"
		"Task: StringOS\n"
		"Mode: %\n",
		(GetOsMode() == OSMODE_STD) ? "Standard" : "Boyer-Moore"
	);
	return 0;
}

static int StringOs_Upcase(MsgProg *p_msg)
{
	if (p_msg->Count < 2)
	{
		PrintFmt("Usage % <word> [word] [word..\n", p_msg->Args[0]);
		return 1;
	}
	for (u16 i = 1; i < p_msg->Count; i++)
	{
		for (size_t v = 0; v < StrLenA(p_msg->Args[i]); v++)
		{
			PrintFmt("^", ToUpper(p_msg->Args[i][v]));
		}
		PrintFmt(" ");
	}
	PrintFmt("\n");
	return 0;
}

static int StringOs_Downcase(MsgProg *p_msg)
{
	if (p_msg->Count < 2)
	{
		PrintFmt("Usage % <word> [word] [word..\n", p_msg->Args[0]);
		return 1;
	}
	for (u16 i = 1; i < p_msg->Count; i++)
	{
		for (size_t v = 0; v < StrLenA(p_msg->Args[i]); v++)
		{
			PrintFmt("^", ToLower(p_msg->Args[i][v]));
		}
		PrintFmt(" ");
	}
	PrintFmt("\n");
	return 0;
}

static int StringOs_Titlize(MsgProg *p_msg)
{
	if (p_msg->Count < 2)
	{
		PrintFmt("Usage % <word> [word] [word..\n", p_msg->Args[0]);
		return 1;
	}
	for (u16 i = 1; i < p_msg->Count; i++)
	{
		PrintFmt("^", ToUpper(p_msg->Args[i][0]));
		for (size_t v = 1; v < StrLenA(p_msg->Args[i]); v++)
		{
			PrintFmt("^", p_msg->Args[i][v]);
		}
		PrintFmt(" ");
	}
	PrintFmt("\n");
	return 0;
}

static int StringOs_Template(MsgProg *p_msg)
{	
	if (p_msg->Count < 2)
	{
		PrintFmt("Usage % <substring>\n", p_msg->Args[0]);
		return 1;
	}

	char *p_temp;
	size_t unused;
	size_t p_shift[sizeof(alphabet)];

	p_temp = (char *)KernelGetShare("temp", &unused);
	if (p_temp == NULL)
	{
		p_temp = (char *)KernelNewShare("temp", BUFSIZE);
		if (p_temp == NULL)
		{
			PrintFmt("Out of memory\n");
			return 2;
		}
	}

	StrCpyA(p_temp, BUFSIZE, p_msg->Args[1]);
	PrintFmt("Template '%' loaded. ", p_temp);
	if (GetOsMode() == OSMODE_BM)
	{
		PrintFmt("BM info:\n");
		BoyerMooreBuildShift(p_temp, p_shift);
		for (size_t i = 0; p_temp[i]; i++)
		{
			PrintFmt("^:$ ", p_temp[i], p_shift[p_temp[i] - 32]);
		}
	}
	PrintFmt("\n");
	return 0;
}

static int StringOs_Search(MsgProg *p_msg)
{
	if (p_msg->Count < 2)
	{
		PrintFmt("Usage % <substring>\n", p_msg->Args[0]);
		return 1;
	}

	char *p_temp;
	size_t temp_sz;
	const char *p_res;

	p_temp = (char *)KernelGetShare("temp", &temp_sz);
	if (p_temp == NULL)
	{
		PrintFmt("No template loaded. Use <template> command to add template.\n");
		return 2;
	}

	p_res = StrStrA(p_msg->Args[1], p_temp);
	if (p_res)
	{
		PrintFmt("Found '%' at pos: $\n", p_temp, (size_t)(p_res - p_msg->Args[1]));
	}
	else
	{
		PrintFmt("Not found '%'\n", p_temp);
	}

	return 0;
}

static int StringOs_Shutdown(MsgProg *p_msg)
{
  	outw (0x604, 0x2000);
	return 0;
}