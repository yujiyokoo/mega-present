#include <genesis.h>
char * answerlist[] = {
"ABOUT",
"ABOVE",
"ABUSE",
"ACTOR",
"ADAPT",
"ADDED",
"ADMIT",
"ADOPT",
"ADULT",
"AFTER",
"AGAIN",
"AGENT",
"AGREE",
"AHEAD",
"AISLE",
"ALARM",
"ALBUM",
"ALIEN",
"ALIKE",
"ALIVE",
"ALLEY",
"ALLOW",
"ALONE",
"ALONG",
"ALTER",
"AMONG",
"ANGEL",
"ANGER",
"ANGLE",
"ANGRY",
"ANKLE",
"APART",
"APPLE",
"APPLY",
"ARENA",
"ARGUE",
"ARISE",
"ARMED",
"ARRAY",
"ARROW",
"ASIDE",
"ASSET",
"AVOID",
"AWAIT",
"AWAKE",
"AWARD",
"AWARE",
"AWFUL",
"BADLY",
"BASIC",
"BASIS",
"BEACH",
"BEARD",
"BEAST",
"BEGIN",
"BEING",
"BELLY",
"BELOW",
"BENCH",
"BIBLE",
"BIRTH",
"BLACK",
"BLADE",
"BLAME",
"BLANK",
"BLAST",
"BLEND",
"BLESS",
"BLIND",
"BLINK",
"BLOCK",
"BLOND",
"BLOOD",
"BOARD",
"BOAST",
"BONUS",
"BOOST",
"BOOTH",
"BRAIN",
"BRAKE",
"BRAND",
"BRAVE",
"BREAD",
"BREAK",
"BRICK",
"BRIDE",
"BRIEF",
"BRING",
"BROAD",
"BROWN",
"BRUSH",
"BUDDY",
"BUILD",
"BUNCH",
"BUYER",
"CABIN",
"CABLE",
"CANDY",
"CARGO",
"CARRY",
"CARVE",
"CATCH",
"CAUSE",
"CEASE",
"CHAIN",
"CHAIR",
"CHAOS",
"CHARM",
"CHART",
"CHASE",
"CHEAP",
"CHEAT",
"CHECK",
"CHEEK",
"CHEER",
"CHEST",
"CHIEF",
"CHILD",
"CHILL",
"CHUNK",
"CIVIC",
"CIVIL",
"CLAIM",
"CLASS",
"CLEAN",
"CLEAR",
"CLERK",
"CLICK",
"CLIFF",
"CLIMB",
"CLING",
"CLOCK",
"CLOSE",
"CLOTH",
"CLOUD",
"COACH",
"COAST",
"COLOR",
"COUCH",
"COULD",
"COUNT",
"COURT",
"COVER",
"CRACK",
"CRAFT",
"CRASH",
"CRAWL",
"CRAZY",
"CREAM",
"CRIME",
"CROSS",
"CROWD",
"CRUEL",
"CRUSH",
"CURVE",
"CYCLE",
"DAILY",
"DANCE",
"DEATH",
"DEBUT",
"DELAY",
"DENSE",
"DEPTH",
"DEVIL",
"DIARY",
"DIRTY",
"DONOR",
"DOUBT",
"DOUGH",
"DOZEN",
"DRAFT",
"DRAIN",
"DRAMA",
"DREAM",
"DRESS",
"DRIED",
"DRIFT",
"DRILL",
"DRINK",
"DRIVE",
"DROWN",
"DRUNK",
"DYING",
"EAGER",
"EARLY",
"EARTH",
"EIGHT",
"ELBOW",
"ELDER",
"ELECT",
"ELITE",
"EMPTY",
"ENACT",
"ENEMY",
"ENJOY",
"ENTER",
"ENTRY",
"EQUAL",
"EQUIP",
"ERROR",
"ESSAY",
"EVENT",
"EVERY",
"EXACT",
"EXIST",
"EXTRA",
"FAINT",
"FAITH",
"FALSE",
"FATAL",
"FAULT",
"FAVOR",
"FENCE",
"FEVER",
"FEWER",
"FIBER",
"FIELD",
"FIFTH",
"FIFTY",
"FIGHT",
"FINAL",
"FIRST",
"FIXED",
"FLAME",
"FLASH",
"FLEET",
"FLESH",
"FLOAT",
"FLOOD",
"FLOOR",
"FLOUR",
"FLUID",
"FOCUS",
"FORCE",
"FORTH",
"FORTY",
"FORUM",
"FOUND",
"FRAME",
"FRAUD",
"FRESH",
"FRONT",
"FROWN",
"FRUIT",
"FULLY",
"FUNNY",
"GENRE",
"GHOST",
"GIANT",
"GIVEN",
"GLASS",
"GLOBE",
"GLORY",
"GLOVE",
"GRACE",
"GRADE",
"GRAIN",
"GRAND",
"GRANT",
"GRAPE",
"GRASP",
"GRASS",
"GRAVE",
"GREAT",
"GREEN",
"GREET",
"GRIEF",
"GROSS",
"GROUP",
"GUARD",
"GUESS",
"GUEST",
"GUIDE",
"GUILT",
"HABIT",
"HAPPY",
"HARSH",
"HEART",
"HEAVY",
"HELLO",
"HENCE",
"HONEY",
"HONOR",
"HORSE",
"HOTEL",
"HOUSE",
"HUMAN",
"HUMOR",
"HURRY",
"IDEAL",
"IMAGE",
"IMPLY",
"INDEX",
"INNER",
"INPUT",
"IRONY",
"ISSUE",
"JEANS",
"JOINT",
"JUDGE",
"JUICE",
"JUROR",
"KNEEL",
"KNIFE",
"KNOCK",
"KNOWN",
"LABEL",
"LABOR",
"LARGE",
"LASER",
"LATER",
"LAUGH",
"LAYER",
"LEARN",
"LEAST",
"LEAVE",
"LEGAL",
"LEMON",
"LEVEL",
"LIGHT",
"LIMIT",
"LIVER",
"LOBBY",
"LOCAL",
"LOGIC",
"LOOSE",
"LOVER",
"LOWER",
"LOYAL",
"LUCKY",
"LUNCH",
"MAGIC",
"MAJOR",
"MAKER",
"MARCH",
"MARRY",
"MATCH",
"MAYBE",
"MAYOR",
"MEDAL",
"MEDIA",
"MERIT",
"METAL",
"METER",
"MIDST",
"MIGHT",
"MINOR",
"MIXED",
"MODEL",
"MONEY",
"MONTH",
"MORAL",
"MOTOR",
"MOUNT",
"MOUSE",
"MOUTH",
"MOVIE",
"MUSIC",
"NAKED",
"NASTY",
"NERVE",
"NEVER",
"NEWLY",
"NIGHT",
"NOISE",
"NORTH",
"NOVEL",
"NURSE",
"OCCUR",
"OCEAN",
"OFFER",
"OFTEN",
"ONION",
"OPERA",
"ORBIT",
"ORDER",
"ORGAN",
"OTHER",
"OUGHT",
"OUTER",
"OWNER",
"PAINT",
"PANEL",
"PANIC",
"PAPER",
"PARTY",
"PASTA",
"PATCH",
"PAUSE",
"PEACE",
"PHASE",
"PHONE",
"PHOTO",
"PIANO",
"PIECE",
"PILOT",
"PITCH",
"PIZZA",
"PLACE",
"PLAIN",
"PLANE",
"PLANT",
"PLATE",
"PLEAD",
"POINT",
"PORCH",
"POUND",
"POWER",
"PRESS",
"PRICE",
"PRIDE",
"PRIME",
"PRINT",
"PRIOR",
"PRIZE",
"PROOF",
"PROUD",
"PROVE",
"PULSE",
"PUNCH",
"PURSE",
"QUEEN",
"QUEST",
"QUICK",
"QUIET",
"QUITE",
"QUOTE",
"RADAR",
"RADIO",
"RAISE",
"RALLY",
"RANCH",
"RANGE",
"RAPID",
"RATIO",
"REACH",
"REACT",
"READY",
"REALM",
"REBEL",
"REFER",
"RELAX",
"REPLY",
"RIDER",
"RIDGE",
"RIFLE",
"RIGHT",
"RISKY",
"RIVAL",
"RIVER",
"ROBOT",
"ROMAN",
"ROUGH",
"ROUND",
"ROUTE",
"ROYAL",
"RUMOR",
"RURAL",
"SALAD",
"SALES",
"SAUCE",
"SCALE",
"SCARE",
"SCARY",
"SCENE",
"SCENT",
"SCOPE",
"SCORE",
"SCREW",
"SEIZE",
"SENSE",
"SERVE",
"SEVEN",
"SHADE",
"SHAKE",
"SHALL",
"SHAME",
"SHAPE",
"SHARE",
"SHARK",
"SHARP",
"SHEEP",
"SHEER",
"SHEET",
"SHELF",
"SHELL",
"SHIFT",
"SHINE",
"SHIRT",
"SHOCK",
"SHOOT",
"SHORE",
"SHORT",
"SHOUT",
"SHOVE",
"SHRUG",
"SIGHT",
"SILLY",
"SINCE",
"SIXTH",
"SKILL",
"SKIRT",
"SKULL",
"SLAVE",
"SLEEP",
"SLICE",
"SLIDE",
"SLOPE",
"SMALL",
"SMART",
"SMELL",
"SMILE",
"SMOKE",
"SNAKE",
"SNEAK",
"SOLAR",
"SOLID",
"SOLVE",
"SORRY",
"SOUND",
"SOUTH",
"SPACE",
"SPARE",
"SPARK",
"SPEAK",
"SPEED",
"SPELL",
"SPEND",
"SPILL",
"SPINE",
"SPITE",
"SPLIT",
"SPOON",
"SPORT",
"SPRAY",
"SQUAD",
"STACK",
"STAFF",
"STAGE",
"STAIR",
"STAKE",
"STAND",
"STARE",
"START",
"STATE",
"STEAK",
"STEAL",
"STEAM",
"STEEL",
"STEEP",
"STEER",
"STICK",
"STIFF",
"STILL",
"STOCK",
"STONE",
"STORE",
"STORM",
"STORY",
"STOVE",
"STRAW",
"STRIP",
"STUDY",
"STUFF",
"STYLE",
"SUGAR",
"SUITE",
"SUNNY",
"SUPER",
"SWEAR",
"SWEAT",
"SWEEP",
"SWEET",
"SWELL",
"SWING",
"SWORD",
"TABLE",
"TASTE",
"TEACH",
"TERMS",
"THANK",
"THEIR",
"THEME",
"THERE",
"THESE",
"THICK",
"THIGH",
"THING",
"THINK",
"THIRD",
"THOSE",
"THREE",
"THROW",
"THUMB",
"TIGHT",
"TIRED",
"TITLE",
"TODAY",
"TOOTH",
"TOPIC",
"TOTAL",
"TOUCH",
"TOUGH",
"TOWEL",
"TOWER",
"TOXIC",
"TRACE",
"TRACK",
"TRADE",
"TRAIL",
"TRAIN",
"TRAIT",
"TRASH",
"TREAT",
"TREND",
"TRIAL",
"TRIBE",
"TRICK",
"TROOP",
"TRUCK",
"TRULY",
"TRUNK",
"TRUST",
"TRUTH",
"TUMOR",
"TWICE",
"TWIST",
"UNCLE",
"UNDER",
"UNION",
"UNITE",
"UNITY",
"UNTIL",
"UPPER",
"UPSET",
"URBAN",
"USUAL",
"VALID",
"VALUE",
"VIDEO",
"VIRUS",
"VISIT",
"VITAL",
"VOCAL",
"VOICE",
"VOTER",
"WAGON",
"WAIST",
"WASTE",
"WATCH",
"WATER",
"WEAVE",
"WEIGH",
"WEIRD",
"WHALE",
"WHEAT",
"WHEEL",
"WHERE",
"WHICH",
"WHILE",
"WHITE",
"WHOLE",
"WHOSE",
"WIDOW",
"WOMAN",
"WORKS",
"WORLD",
"WORRY",
"WORTH",
"WOULD",
"WOUND",
"WRIST",
"WRITE",
"WRONG",
"YIELD",
"YOUNG",
"YOURS",
"YOUTH",
"ZEBRA",
"ZEROS",
"ZILCH",
"ZIPPY",
"ZONED",
"ZONES",
"ZOOMS",
"MRUBY",
NULL
};