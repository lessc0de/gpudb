#ifndef __SSB_COMMON__
#define __SSB_COMMON__

#define BILLION 	1000000000
#define BLOCKNUM	(16*1024*1024)	//the number of tuples in a block. the number should be a multiple of 4096
#define HSIZE 131072

enum {

//type supported in schema
	INT =0,
	FLOAT,
	STRING,

// supported relation in exp
	EQ,
	GTH,
	LTH,
	GEQ,
	LEQ,
	NOT_EQ,

// for where condition
	AND ,
	OR,
	EXP,

// groupby function
	MIN,
	MAX,
	COUNT,
	SUM,
	AVG,

// math operation
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,

// op type for mathExp
	COLUMN,
	CONS,

// data position
	GPU,
	MEM,
	PINNED,
	UVA,
	DISK,
	TMPFILE,

// order by sequence
	ASC,
	DESC,

// data format
	RLE,
	DICT,
	DELTA,
	UNCOMPRESSED,

	NOOP
};

//header of each block in the column 

struct columnHeader{
	long totalTupleNum;	//the total number of tuples in this column
	long tupleNum;		//the number of tuples in this block
	long blockSize;		//the size of the block in bytes
	int blockTotal;		//the total number of blocks that this column is divided into
	int blockId;		//the block id of the current block
	int format;		//the format of the current block
	char padding[4060];	//for futher use
};

//if the column is compressed using dictionary encoding method,
//distHeader will come behind the columnHeader.
//the size of the dictHeader varies depending on the size of the dictionary.

#define MAX_DICT_NUM	30000

struct dictHeader{
	int dictNum;			//number of distict values in the dictionary 
	int bitNum;			//the number of bits used to compress the data
	int hash[MAX_DICT_NUM];		//the hash table to store the dictionaries
};

struct rleHeader{
	int dictNum;
};

struct whereExp{
	int index;
	int relation;
	char content[32];
};

struct whereCondition{
	int andOr;
	int nested;
	int nestedRel;
	int expNum;
	struct whereExp *exp;
	struct whereCondition * con;
};

struct scanNode{
	struct tableNode *tn ;		// the tableNode to be scanned
	int hasWhere;			// whether the node has where condition
	int outputNum;			// the number of attributes that will be projected
	int *outputIndex;		// the index of projected attributes in the tableNode
	int whereAttrNum;		// the number of attributes in the where condition
	int * whereIndex;		// the index of each col in the table
	struct whereCondition * filter;	// the where conditioin
	int keepInGpu;			// whether all the results should be kept in GPU memory or not

};

struct mathExp {
	int op;				// the math operation
	int opNum;			// the number of operands

// 	when opNum is 2 ,
	struct mathExp exp[2];		// if the opNum is 2, this field stores the two operands

//	when opNum is 1,
	int opType;			// whether it is a regular column or a constant
	int opValue;			// it is the index of the column or the value of the constant 
};

struct tableNode{
	int totalAttr;			// the total number of attributes
	long tupleNum;			// the number of tuples in the relation
	int tupleSize;			// the size of each tuple
	int * attrType;			// the type of each attributes
	int * attrSize;			// the size of each attributes
	int * attrTotalSize;		// the total size of each attribute
	int * attrIndex;		// the index of each attribute in the table
	char **content;			// the actual content of each attribute, organized by columns
	int * dataPos;			// the position of the data, whether in disk, memory or GPU global memory
	int * dataFormat;		// the format of each column

};

struct groupByExp{
	int func;			// the group by function
	struct mathExp exp;		// the math exp. 
};

struct groupByNode{
	struct tableNode * table;	// the table node to be grouped by

	int groupByColNum;		// the number of columns that will be grouped on
	int * groupByIndex;		// the index of the columns that will be grouped on
					// -1 means group by a constant
	int * groupByType;		// the type of the group by column
	int * groupBySize;		// the size of the group by column

	int outputAttrNum;		// the number of output attributes
	int *attrType;			// the type of each output attribute
	int *attrSize;			// the size of each output attribute
	struct groupByExp * gbExp;	// the group by expression

	int tupleSize;			// the size of the tuple in the join result

	int * keepInGpu;		// whether the results should be kept in gpu

};

struct sortRecord{
	unsigned int key;		// the key to be sorted
	unsigned int pos;		// the position of the corresponding record
};

struct orderByNode{
	struct tableNode * table;	// the table node to be ordered by

	int orderByNum;			// the number of columns that will be ordered on
	int *orderBySeq;		// asc or desc
	int *orderByIndex;		// the index of each order by column in the table

};

struct materializeNode{
	struct tableNode * table;	// the table node to be materialized

};

struct statistic{
	double kernel;
	double total;
};


#endif