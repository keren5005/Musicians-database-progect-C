#ifndef defbool
#define defbool
#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#endif

#define SIZE 150
#define INST_NOT_FOUND -1
#define MIN_MUSICIAN_NAME_NUM 2
#define MINS_PER_HOUR 60.0
#define IMPORTANT '1'

typedef unsigned int IDX;

typedef struct treeNode {
	char* instrument; //Instrument name
	unsigned short insId; //Instrument ID
	struct treeNode* left;
	struct treeNode* right;
} TreeNode;

typedef struct tree {
	TreeNode* root;
	unsigned int nodesAmount;
} InstrumentTree;

typedef struct {
	unsigned short insId;
	float price;
} MusicianPriceInstrument;

typedef struct mpinode {
	MusicianPriceInstrument MPI;
	struct mpinode* next;
}MPINode;

typedef struct mpilist {
	MPINode* first;
	MPINode* last;
}MPIList;

typedef struct {
	char** name;
	unsigned int namesAmount;
	MPIList instruments;
	bool playsInConcert; //True if the musician is playing in the concert.
} Musician;

typedef struct {
	unsigned int logSize;
	unsigned int physSize;
}MCS; //MusicianCollectionSizes


typedef struct {
	Musician* MusPtr;
	float price;
} InstPriceForMus; //The price for the instrument for the musician

typedef struct {
	int day, month, year;
	float hour;
} Date;

typedef struct concertInstrument{
	int inst; //Instrument ID
	char* instName; //Instrument name
	int num; //Instrument amount
	char importance; //1 = important, 0 = unimportant
	InstPriceForMus* musArr;
	struct concertInstrument* next;
} ConcertInstrument;

typedef struct {
	ConcertInstrument* first;
	ConcertInstrument* last;
}CIList;

typedef struct {
	Date date_of_concert;
	char* name;
	CIList instruments;
} Concert;

//Misc.
unsigned int CountMusicians(char* musiciansFileName);

//1st question //TESTED
InstrumentTree buildTree(char* instrumentsFileName);
InstrumentTree makeEmptyTree();
TreeNode* buildInstrumentNode(char* instrument, IDX index);
TreeNode* addInstrumentToTree(TreeNode* root, TreeNode* Instrument); //Helper
bool isRootFirst(char* root, char* instName);

//2nd question //TESTED
int findInsId(InstrumentTree tree, char* instrument);
int findInsIdHelper(TreeNode* root, char* instrument);

//3rd question //TESTED
Musician** buildMusicianArchive(char* MusiciansFileName, InstrumentTree tree, int MusiciansAmount);
Musician* addMusicianToArchive(char* musician, InstrumentTree tree, char* seps);
Musician* makeEmptyMusician();
MPIList makeEmptyMPIList();
MPINode* makeEmptyNode();
void insertNodeToEndList(MPIList* lst, MPINode* node);

//4th Question //TESTED
Musician*** buildMusiciansCollection(int instrumentAmount, Musician** MusiciansGroup, int MusiciansAmount, MCS* MCSizes);
Musician*** fillMusiciansCollection(Musician** MusiciansGroup, Musician*** MusiciansCollection, MCS* MCSizes, int MusiciansAmount, int instrumentAmount);
Musician*** addMusicianToCollection(Musician* musician, Musician*** MusiciansCollection, MCS* MCSizes);
MCS* makeMCSArr(int instrumentAmount);

//5th Question
void makeConcerts(Musician*** MusCollection, MCS* MCSizes, InstrumentTree tree);
Concert makeEmptyConcert();
char* addTimeToConcert(Concert* concert, char* token);
float getHourMinutesForConcert(char* time);
char* addConcertInstrumentToList(CIList* lst, char* token, InstrumentTree tree);
Musician** sortArrOfMusicians(Musician** arrOfPtrs, unsigned int size, IDX instIdx, char importance);
InstPriceForMus* buildPricesArr(Musician** arrOfPtrs, unsigned int size, IDX insId);
void swapInstruments(MusicianPriceInstrument* first, MusicianPriceInstrument* curr);
int sortMusiciansFromCheapToExpensive(InstPriceForMus* mus1, InstPriceForMus* mus2);
int sortMusiciansFromExpensiveToCheap(InstPriceForMus* mus1, InstPriceForMus* mus2);
Musician** rearrangeArrOfPtrs(Musician** arrOfPtrs, InstPriceForMus* pricesArr, unsigned int size);
void getMusiciansForConcert(Concert* concert, Musician*** MusCollection, MCS* MCSizes);
void initializeNextInstruments(ConcertInstrument* curr);
void printConcert(Concert concert, Musician*** musCollection);
float findPrice(Musician* musician, IDX instId);
void resetConcertData(Concert* concert);
void freeCIList(CIList* lst);
void freeCIListHelper(ConcertInstrument* curr);

//Free data
void freeAllDataStructures(InstrumentTree tree, Musician** MusiciansGroup, unsigned int musiciansAmount, Musician*** MusiciansCollection, MCS* MCSizes);
void freeMusiciansCollection(InstrumentTree tree, Musician*** MusCollection, MCS* MCSizes);
void freeMusiciansGroup(Musician** MusiciansGroup, unsigned int musiciansAmount);
void freeMPIList(MPIList* list);
void freeMPIListHelper(MPINode* curr);
void freeTree(InstrumentTree tree);
void freeTreeHelper(TreeNode* root);

//Memory
void checkMemoryAllocation(void* ptr);
void checkFile(FILE* filePtr);