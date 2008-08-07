/*		0		1		2		3		4		5		6		7
==================================================================================
0		DEL		3		5		7		9		+		pound	1
1		ENTER	W		R		Y		I		P		*		leftarrow
2		CRSRrt	A		D		G		J		L		;		CTRL
3		F7		4		6		8		0		-		HOME	2
4		F1		Z		C		B		M		.		RSHIFT	space
5		F3		S		F		H		K		:		=		commodore
6		F5		E		T		U		O		@		uparrow	Q
7		CRSRdn	LSHIFT	X		V		N		,		/		STOP


The following key combinations are not emulated:
SHIFT @
SHIFT ^
SHIFT =
SHIFT *		(can be done from the keypad)
SHIFT +		(can be done from the keypad)

The following keys have bizarre equivalents:
RESTORE		Pause
RUN/STOP	Escape
COMMODORE	Windows

The following keys have different symbols:
pound		\
left arrow	`

Table 1 uses 256 bytes
Table 2 uses   9 bytes
Table 3 uses   9 bytes

*/

// Each scancode results in one of four possible operations: ignore it, remap it to a key, remap it to a shifted key, or do something special
#define	IGNORE			(0<<6)
#define	REMAP			(1<<6)
#define	SHIFT_REMAP		(2<<6)
#define	SPECIAL			(3<<6)

// Operation 0: ignore ==> No definitions are required for ignored keys

// Operations 1 & 2: remap ==> This requires definitions for CBM keys
#define	COL0	(0<<3)
#define	COL1	(1<<3)
#define	COL2	(2<<3)
#define	COL3	(3<<3)
#define	COL4	(4<<3)
#define	COL5	(5<<3)
#define	COL6	(6<<3)
#define	COL7	(7<<3)

#define ROW0	(0<<0)
#define ROW1	(1<<0)
#define ROW2	(2<<0)
#define ROW3	(3<<0)
#define ROW4	(4<<0)
#define ROW5	(5<<0)
#define ROW6	(6<<0)
#define ROW7	(7<<0)

#define	CBM_DEL		(ROW0 + COL0)
#define CBM_3		(ROW0 + COL1)
#define	CBM_5		(ROW0 + COL2)
#define	CBM_7		(ROW0 + COL3)
#define	CBM_9		(ROW0 + COL4)
#define	CBM_PLUS	(ROW0 + COL5)
#define	CBM_POUND	(ROW0 + COL6)
#define	CBM_1		(ROW0 + COL7)

#define	CBM_ENTER	(ROW1 + COL0)
#define	CBM_W		(ROW1 + COL1)
#define	CBM_R		(ROW1 + COL2)
#define	CBM_Y		(ROW1 + COL3)
#define	CBM_I		(ROW1 + COL4)
#define	CBM_P		(ROW1 + COL5)
#define	CBM_STAR	(ROW1 + COL6)
#define	CBM_LEFTARR	(ROW1 + COL7)

#define	CBM_CRSR_RT	(ROW2 + COL0)
#define	CBM_A		(ROW2 + COL1)
#define	CBM_D		(ROW2 + COL2)
#define	CBM_G		(ROW2 + COL3)
#define	CBM_J		(ROW2 + COL4)
#define	CBM_L		(ROW2 + COL5)
#define	CBM_SEMICOL	(ROW2 + COL6)
#define	CBM_CTRL	(ROW2 + COL7)

#define	CBM_F7		(ROW3 + COL0)
#define	CBM_4		(ROW3 + COL1)
#define	CBM_6		(ROW3 + COL2)
#define	CBM_8		(ROW3 + COL3)
#define	CBM_0		(ROW3 + COL4)
#define	CBM_MINUS	(ROW3 + COL5)
#define	CBM_HOME	(ROW3 + COL6)
#define	CBM_2		(ROW3 + COL7)

#define	CBM_F1		(ROW4 + COL0)
#define	CBM_Z		(ROW4 + COL1)
#define	CBM_C		(ROW4 + COL2)
#define	CBM_B		(ROW4 + COL3)
#define	CBM_M		(ROW4 + COL4)
#define	CBM_PERIOD	(ROW4 + COL5)
#define	CBM_RSHIFT	(ROW4 + COL6)
#define	CBM_SPACE	(ROW4 + COL7)

#define	CBM_F3		(ROW5 + COL0)
#define	CBM_S		(ROW5 + COL1)
#define	CBM_F		(ROW5 + COL2)
#define	CBM_H		(ROW5 + COL3)
#define	CBM_K		(ROW5 + COL4)
#define	CBM_COLON	(ROW5 + COL5)
#define	CBM_EQUALS	(ROW5 + COL6)
#define	CBM_LOGO	(ROW5 + COL7)

#define	CBM_F5		(ROW6 + COL0)
#define	CBM_E		(ROW6 + COL1)
#define	CBM_T		(ROW6 + COL2)
#define	CBM_U		(ROW6 + COL3)
#define	CBM_O		(ROW6 + COL4)
#define	CBM_ALPHA	(ROW6 + COL5)
#define	CBM_UPARR	(ROW6 + COL6)
#define	CBM_Q		(ROW6 + COL7)

#define	CBM_CRSR_DN	(ROW7 + COL0)
#define	CBM_LSHIFT	(ROW7 + COL1)
#define	CBM_X		(ROW7 + COL2)
#define	CBM_V		(ROW7 + COL3)
#define	CBM_N		(ROW7 + COL4)
#define	CBM_COMMA	(ROW7 + COL5)
#define	CBM_SLASH	(ROW7 + COL6)
#define	CBM_STOP	(ROW7 + COL7)

// Operation 3: special ==> Definitions for special actions
#define	PUNCTUATION1	(1<<3)
#define	PUNCTUATION2	(2<<3)
#define	RESTORE_KEY		(3<<3)
#define	LOCK_KEY		(4<<3)
#define SNAPSHOT		(5<<3)
#define	CBM_RESET		(6<<3)
#define RELEASE_ALL		(7<<3)

// ============== Main translation table ==============

static const U8 translation[256]={
	IGNORE                  ,		// 00
	IGNORE                  ,		// 01	F9
	IGNORE                  ,		// 02
	REMAP      + CBM_F5     ,		// 03	F5
	REMAP      + CBM_F3     ,		// 04	F3
	REMAP      + CBM_F1     ,		// 05	F1
	SHIFT_REMAP+ CBM_F1     ,		// 06	F2
	SPECIAL    + SNAPSHOT   ,		// 07	F12
	IGNORE                  ,		// 08
	IGNORE                  ,		// 09	F10
	SHIFT_REMAP+ CBM_F7     ,		// 0A	F8
	SHIFT_REMAP+ CBM_F5     ,		// 0B	F6
	SHIFT_REMAP+ CBM_F3     ,		// 0C	F4
	IGNORE                  ,		// 0D	Tab	Untab
	REMAP      + CBM_LEFTARR,		// 0E	`	~			{Translates to leftarrow}
	IGNORE                  ,		// 0F
	IGNORE                  ,		// 10
	IGNORE                  ,		// 11	Alt (left)
	REMAP      + CBM_LSHIFT ,		// 12	Shift (left)
	IGNORE                  ,		// 13
	REMAP      + CBM_CTRL   ,		// 14	Control (left)
	REMAP      + CBM_Q      ,		// 15	q	Q
	REMAP      + CBM_1      ,		// 16	1	!
	IGNORE                  ,		// 17
	IGNORE                  ,		// 18
	IGNORE                  ,		// 19
	REMAP      + CBM_Z      ,		// 1A	z	Z
	REMAP      + CBM_S      ,		// 1B	s	S
	REMAP      + CBM_A      ,		// 1C	a	A
	REMAP      + CBM_W      ,		// 1D	w	W
	SPECIAL+PUNCTUATION1 + 2,		// 1E	2	@
	IGNORE                  ,		// 1F
	IGNORE                  ,		// 20
	REMAP      + CBM_C      ,		// 21	c	C
	REMAP      + CBM_X      ,		// 22	x	X
	REMAP      + CBM_D      ,		// 23	d	D
	REMAP      + CBM_E      ,		// 24	e	E
	REMAP      + CBM_4      ,		// 25	4	$
	REMAP      + CBM_3      ,		// 26	3	#
	IGNORE                  ,		// 27
	IGNORE                  ,		// 28
	REMAP      + CBM_SPACE  ,		// 29	space
	REMAP      + CBM_V      ,		// 2A	v	V
	REMAP      + CBM_F      ,		// 2B	f	F
	REMAP      + CBM_T      ,		// 2C	t	T
	REMAP      + CBM_R      ,		// 2D	r	R
	REMAP      + CBM_5      ,		// 2E	5	%
	IGNORE                  ,		// 2F
	IGNORE                  ,		// 30
	REMAP      + CBM_N      ,		// 31	n	N
	REMAP      + CBM_B      ,		// 32	b	B
	REMAP      + CBM_H      ,		// 33	h	H
	REMAP      + CBM_G      ,		// 34	g	G
	REMAP      + CBM_Y      ,		// 35	y	Y
	SPECIAL+PUNCTUATION1 + 3,		// 36	6	^
	IGNORE                  ,		// 37
	IGNORE                  ,		// 38
	IGNORE                  ,		// 39
	REMAP      + CBM_M      ,		// 3A	m	M
	REMAP      + CBM_J      ,		// 3B	j	J
	REMAP      + CBM_U      ,		// 3C	u	U
	SPECIAL+PUNCTUATION1 + 4,		// 3D	7	&
	SPECIAL+PUNCTUATION1 + 5,		// 3E	8	*
	IGNORE                  ,		// 3F
	IGNORE                  ,		// 40
	REMAP      + CBM_COMMA  ,		// 41	,	<
	REMAP      + CBM_K      ,		// 42	k	K
	REMAP      + CBM_I      ,		// 43	i	I
	REMAP      + CBM_O      ,		// 44	o	O
	SPECIAL+PUNCTUATION1 + 7,		// 45	0	)
	SPECIAL+PUNCTUATION1 + 6,		// 46	9	(
	IGNORE                  ,		// 47
	IGNORE                  ,		// 48
	REMAP      + CBM_PERIOD ,		// 49	.	>
	REMAP      + CBM_SLASH  ,		// 4A	/	?
	REMAP      + CBM_L      ,		// 4B	l	L
	SPECIAL+PUNCTUATION1 + 1,		// 4C	;	:
	REMAP      + CBM_P      ,		// 4D	p	P
	REMAP      + CBM_MINUS  ,		// 4E	-	_
	IGNORE                  ,		// 4F
	IGNORE                  ,		// 50
	IGNORE                  ,		// 51
	SPECIAL+PUNCTUATION2 + 0,		// 52	'	"
	IGNORE                  ,		// 53
	SHIFT_REMAP+ CBM_COLON  ,		// 54	[	{
	SPECIAL+PUNCTUATION1 + 0,		// 55	=	+
	IGNORE                  ,		// 56
	IGNORE                  ,		// 57
	SPECIAL    + LOCK_KEY+ 1,		// 58	Caps Lock
	REMAP      + CBM_RSHIFT ,		// 59	Shift (right)
	REMAP      + CBM_ENTER  ,		// 5A	Enter
	SHIFT_REMAP+ CBM_SEMICOL,		// 5B	]	}
	IGNORE                  ,		// 5C
	REMAP      + CBM_POUND  ,		// 5D	\	|		{Translates to the pound symbol}
	IGNORE                  ,		// 5E
	IGNORE                  ,		// 5F
	IGNORE                  ,		// 60
	IGNORE                  ,		// 61
	IGNORE                  ,		// 62
	IGNORE                  ,		// 63
	IGNORE                  ,		// 64
	IGNORE                  ,		// 65
	REMAP      + CBM_DEL    ,		// 66	Backspace
	IGNORE                  ,		// 67
	IGNORE                  ,		// 68
	IGNORE                  ,		// 69	[Keypad] 1 End
	IGNORE                  ,		// 6A
	IGNORE                  ,		// 6B	[Keypad] 4 Left arrow
	IGNORE                  ,		// 6C	[Keypad] 7 Home
	IGNORE                  ,		// 6D
	IGNORE                  ,		// 6E
	IGNORE                  ,		// 6F
	IGNORE                  ,		// 70	[Keypad] 0 Insert
	IGNORE                  ,		// 71	[Keypad] . Delete
	IGNORE                  ,		// 72	[Keypad] 2 Down arrow
	IGNORE                  ,		// 73	[Keypad] 5
	IGNORE                  ,		// 74	[Keypad] 6 Right arrow
	IGNORE                  ,		// 75	[Keypad] 8 Up arrow
	REMAP      + CBM_STOP   ,		// 76	Escape
	SPECIAL    + LOCK_KEY+ 2,		// 77	Num lock
	SPECIAL    + RELEASE_ALL,		// 78	F11
	REMAP      + CBM_PLUS   ,		// 79	[Keypad] +
	IGNORE                  ,		// 7A	[Keypad] 3 Page Down
	REMAP      + CBM_MINUS  ,		// 7B	[Keypad] -
	REMAP      + CBM_STAR   ,		// 7C	[Keypad] *
	IGNORE                  ,		// 7D	[Keypad] 9 Page Up
	SPECIAL    + LOCK_KEY+ 3,		// 7E	Scroll Lock
	IGNORE                  ,		// 7F
									// ====== Scancodes that are prefixed with with E0 =====
	IGNORE                  ,		// 00
	IGNORE                  ,		// 01
	IGNORE                  ,		// 02
	REMAP      + CBM_F7     ,		// 83	F7	!!! Kludge 1 of 2 ==> The actual scan code for F7 is [83] but we are treating it as [E0 03]
	IGNORE                  ,		// 04
	IGNORE                  ,		// 05
	IGNORE                  ,		// 06
	IGNORE                  ,		// 07
	IGNORE                  ,		// 08
	IGNORE                  ,		// 09
	IGNORE                  ,		// 0A
	IGNORE                  ,		// 0B
	IGNORE                  ,		// 0C
	IGNORE                  ,		// 0D
	IGNORE                  ,		// 0E
	IGNORE                  ,		// 0F
	IGNORE                  ,		// 10	[Multimedia] WWW Search
	IGNORE                  ,		// 11	Alt (right)
	IGNORE                  ,		// 12	Print screen (first scancode)
	IGNORE                  ,		// 13
	REMAP      + CBM_CTRL   ,		// 14	Control (right)
	IGNORE                  ,		// 15	[Multimedia] Previous track
	IGNORE                  ,		// 16
	IGNORE                  ,		// 17
	IGNORE                  ,		// 18	[Multimedia] WWW Favorites
	IGNORE                  ,		// 19
	IGNORE                  ,		// 1A
	IGNORE                  ,		// 1B
	IGNORE                  ,		// 1C
	IGNORE                  ,		// 1D
	IGNORE                  ,		// 1E
	REMAP      + CBM_LOGO   ,		// 1F	Windows (left)
	IGNORE                  ,		// 20	[Multimedia] WWW Refresh
	IGNORE                  ,		// 21	[Multimedia] Volume Down
	IGNORE                  ,		// 22
	IGNORE                  ,		// 23	[Multimedia] Mute
	IGNORE                  ,		// 24
	IGNORE                  ,		// 25
	IGNORE                  ,		// 26
	REMAP      + CBM_LOGO   ,		// 27	Windows (right)
	IGNORE                  ,		// 28	[Multimedia] WWW Stop
	IGNORE                  ,		// 29
	IGNORE                  ,		// 2A
	IGNORE                  ,		// 2B	[Multimedia] Calculator
	IGNORE                  ,		// 2C
	IGNORE                  ,		// 2D
	IGNORE                  ,		// 2E
	IGNORE                  ,		// 2F	Context menu
	IGNORE                  ,		// 30	[Multimedia] WWW Forward
	IGNORE                  ,		// 31
	IGNORE                  ,		// 32	[Multimedia] Volume Up
	IGNORE                  ,		// 33
	IGNORE                  ,		// 34	[Multimedia] Play / Pause
	IGNORE                  ,		// 35
	IGNORE                  ,		// 36
	IGNORE                  ,		// 37	[ACPI] Power
	IGNORE                  ,		// 38	[Multimedia] WWW Back
	IGNORE                  ,		// 39
	IGNORE                  ,		// 3A	[Multimedia] WWW Home
	IGNORE                  ,		// 3B	[Multimedia] Stop
	IGNORE                  ,		// 3C
	IGNORE                  ,		// 3D
	IGNORE                  ,		// 3E
	IGNORE                  ,		// 3F	[ACPI] Sleep
	IGNORE                  ,		// 40	[Multimedia] My Computer
	IGNORE                  ,		// 41
	IGNORE                  ,		// 42
	IGNORE                  ,		// 43
	IGNORE                  ,		// 44
	IGNORE                  ,		// 45
	IGNORE                  ,		// 46
	IGNORE                  ,		// 47
	IGNORE                  ,		// 48	[Multimedia] Email
	IGNORE                  ,		// 49
	REMAP      + CBM_SLASH  ,		// 4A	[Keypad] /
	IGNORE                  ,		// 4B
	IGNORE                  ,		// 4C
	IGNORE                  ,		// 4D	[Multimedia] Next track
	IGNORE                  ,		// 4E
	IGNORE                  ,		// 4F
	IGNORE                  ,		// 50	[Multimedia] Media Select
	IGNORE                  ,		// 51
	IGNORE                  ,		// 52
	IGNORE                  ,		// 53
	SPECIAL    + RESTORE_KEY,		// 54	Pause / Break	!!! Kludge 2 of 2 ==> The actual scancode is [E1 14] but we are treating it as [E0 14]
	IGNORE                  ,		// 55
	IGNORE                  ,		// 56
	IGNORE                  ,		// 57
	IGNORE                  ,		// 58
	IGNORE                  ,		// 59
	REMAP      + CBM_ENTER  ,		// 5A	[Keypad] Enter
	IGNORE                  ,		// 5B
	IGNORE                  ,		// 5C
	IGNORE                  ,		// 5D
	IGNORE                  ,		// 5E	[ACPI] Wake
	IGNORE                  ,		// 5F
	IGNORE                  ,		// 60
	IGNORE                  ,		// 61
	IGNORE                  ,		// 62
	IGNORE                  ,		// 63
	IGNORE                  ,		// 64
	IGNORE                  ,		// 65
	IGNORE                  ,		// 66
	IGNORE                  ,		// 67
	IGNORE                  ,		// 68
	IGNORE                  ,		// 69	End
	IGNORE                  ,		// 6A
	SHIFT_REMAP+ CBM_CRSR_RT,		// 6B	Left arrow
	REMAP      + CBM_HOME   ,		// 6C	Home
	IGNORE                  ,		// 6D
	IGNORE                  ,		// 6E
	IGNORE                  ,		// 6F
	SHIFT_REMAP+ CBM_DEL    ,		// 70	Insert
	SPECIAL    + CBM_RESET  ,		// 71	Delete
	REMAP      + CBM_CRSR_DN,		// 72	Down arrow
	IGNORE                  ,		// 73
	REMAP      + CBM_CRSR_RT,		// 74	Right arrow
	SHIFT_REMAP+ CBM_CRSR_DN,		// 75	Up arrow
	IGNORE                  ,		// 76
	IGNORE                  ,		// 77
	IGNORE                  ,		// 78
	IGNORE                  ,		// 79
	IGNORE                  ,		// 7A	Page Down
	IGNORE                  ,		// 7B
	IGNORE                  ,		// 7C	Print screen (second scancode)
	IGNORE                  ,		// 7D	Page Up
	IGNORE                  ,		// 7E
	IGNORE                  		// 7F
};


static const U8 punctuation_norm[9] ={
			(6<<3) + (5<<0),		// SPECIAL PUNCTUATION1 0 KEY		=	==>		=
			(6<<3) + (2<<0),		// SPECIAL PUNCTUATION1 1 KEY		;	==>		;
			(7<<3) + (3<<0),		// SPECIAL PUNCTUATION1 2 KEY		2	==>		2
			(2<<3) + (3<<0),		// SPECIAL PUNCTUATION1 3 KEY		6	==>		6
			(3<<3) + (0<<0),		// SPECIAL PUNCTUATION1 4 KEY		7	==>		7
			(3<<3) + (3<<0),		// SPECIAL PUNCTUATION1 5 KEY		8	==>		8
			(4<<3) + (0<<0),		// SPECIAL PUNCTUATION1 6 KEY		9	==>		9
			(4<<3) + (3<<0),		// SPECIAL PUNCTUATION1 7 KEY		0	==>		0
	(1<<6)+	(3<<3) + (0<<0)			// SPECIAL PUNCTUATION2 0 KEY		'	==>		SHIFT 7
};


static const U8 punctuation_swap[9] ={
			(5<<3) + (0<<0),		// SPECIAL PUNCTUATION1 0 SHIFTED	=	==>		+
			(5<<3) + (5<<0),		// SPECIAL PUNCTUATION1 1 SHIFTED	;	==>		:
			(5<<3) + (6<<0),		// SPECIAL PUNCTUATION1 2 SHIFTED	2	==>		@
			(6<<3) + (6<<0),		// SPECIAL PUNCTUATION1 3 SHIFTED	6	==>		^
	(1<<6) +(2<<3) + (3<<0),		// SPECIAL PUNCTUATION1 4 SHIFTED	7	==>		SHIFT 6
			(6<<3) + (1<<0),		// SPECIAL PUNCTUATION1 5 SHIFTED	8	==>		*
	(1<<6) +(3<<3) + (3<<0),		// SPECIAL PUNCTUATION1 6 SHIFTED	9	==>		SHIFT 8
	(1<<6) +(4<<3) + (0<<0),		// SPECIAL PUNCTUATION1 7 SHIFTED	0	==>		SHIFT 9
	(1<<6) +(7<<3) + (3<<0)			// SPECIAL PUNCTUATION2 0 SHIFTED	'	==>		SHIFT 2
};

