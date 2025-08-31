// define standard drawing layers
//
#pragma once
#define G_LANGUAGE 1 // 0=eng, 1=rus
#define MAX_LAYERS 32

enum
{    
	// layout layers
	LAY_SELECTION = 0,
	LAY_BACKGND,
	LAY_VISIBLE_GRID,
	LAY_HILITE,
	LAY_DRC_ERROR,
	LAY_FREE_LINE,
	LAY_PART_OUTLINE,
	LAY_PART_NAME,
	LAY_PIN_LINE,
	LAY_PIN_NAME,
	LAY_CONNECTION,
	LAY_NET_NAME,
	LAY_FOOTPRINT,
	LAY_PART_VALUE,
	LAY_PIN_DESC,
	LAY_ADD_1,
	LAY_ADD_2,
	// invisible layers
	LAY_MASK_TOP = -100,	
	LAY_MASK_BOTTOM = -101,
	LAY_PASTE_TOP = -102,
	LAY_PASTE_BOTTOM = -103
};
#define LAY_INFO_TEXT LAY_HILITE
#define LAY_PCB_BOARD (LAY_ADD_2+3)
#define LAY_PCB_COPPER (LAY_ADD_2+4)
#define LAY_PCB_MASK (LAY_ADD_2+5)
#define LAY_PCB_HOLES (LAY_ADD_2+6)
#define LAY_PCB_SILK (LAY_ADD_2+7)
#define LAY_PCB_NOTES (LAY_ADD_2+8)
enum
{
	// footprint layers
	LAY_FP_SELECTION = 0,
	LAY_FP_BACKGND,
	LAY_FP_VISIBLE_GRID,
	LAY_FP_HILITE,
	LAY_FP_SILK_TOP,
	LAY_FP_SILK_BOTTOM,
	LAY_FP_CENTROID,
	LAY_FP_DOT,
	LAY_FP_PAD_THRU,
	LAY_FP_TOP_MASK,
	LAY_FP_TOP_PASTE,
	LAY_FP_BOTTOM_MASK,
	LAY_FP_BOTTOM_PASTE,
	LAY_FP_TOP_COPPER,
	LAY_FP_INNER_COPPER,
	LAY_FP_BOTTOM_COPPER,
	NUM_FP_LAYERS
};
#if G_LANGUAGE == 0
static char layer_str[MAX_LAYERS][64] = 
{ 
	"dragging",
	"background",
	"visible grid",
	"highlight",
	"DRC error",
	"ownerless line",
	"part outline",
	"part name",
	"pin line",
	"pin name",
	"net polyline",	
	"net name",	
	"footprint",
	"part value",
	"description",
	"front layer 1",
	"front layer 2",
	"back layer 1",
	"back layer 2",
	"back layer 3",
	"back layer 4",
	"back layer 5",
	"back layer 6",
	"back layer 7",
	"back layer 8",
	"back layer 9",
	"back layer 10",
	"back layer 11",
	"back layer 12",
	"back layer 13",
	"back layer 14",
	"undefined"
};

static char fp_layer_str[NUM_FP_LAYERS][64] = 
{ 
	"selection",
	"background",
	"visible grid",
	"highlight",
	"top silk",
	"bottom silk",
	"centroid",
	"adhesive",
	"thru pad",
	"top mask",
	"top paste",
	"bottom mask",
	"bottom paste",
	"top copper",
	"inner",
	"bottom"
};
#else
static char layer_str[MAX_LAYERS][64] =
{
	"перемещение",
	"фон доски",
	"видимая сетка",
	"подсветка",
	"метки (DRC)",
	"безхозный",
	"корпусный",
	"текст обозн.",
	"линни пинов",
	"имена пинов",
	"линни цепей",
	"имена цепей",
	"футпринты",
	"названия",
	"описания",
	"фронтальный",
	"поверхностный",
	"невидимый",
	"фоновый",
	"контурный",
	"медный",
	"защитный",
	"фрезерный",
	"шёлковый",
	"чертёжный",
	"резервный 1",
	"резервный 2",
	"резервный 3",
	"резервный 4",
	"резервный 5",
	"резервный 6",
	"undefined"
};

static char fp_layer_str[NUM_FP_LAYERS][64] =
{
	"selection",
	"background",
	"visible grid",
	"highlight",
	"top silk",
	"bottom silk",
	"centroid",
	"adhesive",
	"thru pad",
	"top mask",
	"top paste",
	"bottom mask",
	"bottom paste",
	"top copper",
	"inner",
	"bottom"
};
#endif

static char layer_char[19] = "12345678QWERTYUIOP";
