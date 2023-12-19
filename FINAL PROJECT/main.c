#include "Source.h"

void main(int argc, char* argv[]) {
	int id;
	unsigned int musiciansAmount = CountMusicians(argv[2]);
	char instrumentNameQ2[SIZE]; //Instrument for Question 2

	InstrumentTree tree;
	Musician** MusiciansGroup;
	Musician*** MusiciansCollection;

	tree = buildTree(argv[1]); //The struct has a field for the number of nodes (AKA number of instruments) which is updated here

	printf("Please enter the instrument name you'd like to search for: ");
	gets(instrumentNameQ2);

	id = findInsId(tree, instrumentNameQ2);

	if (id == INST_NOT_FOUND) {
		printf("Could not find your instrument.\n");
		return;
	}

	printf("The instrument ID for %s is %d.\n", instrumentNameQ2, id);
	
	MusiciansGroup = buildMusicianArchive(argv[2], tree, musiciansAmount);

	MCS* MCSizes = makeMCSArr(tree.nodesAmount);
	MusiciansCollection = buildMusiciansCollection(tree.nodesAmount, MusiciansGroup, musiciansAmount, MCSizes);

	//Starting from here, the correct size of Musicians per instrument is the LogSize field in MCSizes!
	
	makeConcerts(MusiciansCollection, MCSizes, tree);

	freeAllDataStructures(tree, MusiciansGroup, musiciansAmount, MusiciansCollection, MCSizes);
}