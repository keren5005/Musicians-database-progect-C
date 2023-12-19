#include "Source.h"

//******************************************MISC*************************

//runs on file and counts amount of musicians.
unsigned int CountMusicians(char* musiciansFileName) {
	FILE* musiciansFile = fopen(musiciansFileName, "r");
	checkFile(musiciansFile);

	int ch = 0;
	int lines = 1;

	while ((ch = fgetc(musiciansFile)) != EOF)
		if (ch == '\n')
			lines++;

	fclose(musiciansFile);

	return lines;
}

//******************************************QUESION1*************************

//The function converts the data in the instruments file into a tree data structure.
InstrumentTree buildTree(char* InstrumentFileName) {
	InstrumentTree tree = makeEmptyTree();
	IDX index = 0;
	TreeNode* tempPtr;
	char* tempInstrument = (char*)malloc(SIZE * sizeof(char));
	checkMemoryAllocation(tempInstrument);
	FILE* instrumentsFile = fopen(InstrumentFileName, "r");
	checkFile(instrumentsFile);
	while (fgets(tempInstrument, SIZE, instrumentsFile) != NULL) {
		if (tempInstrument[strlen(tempInstrument) - 1] == '\n')
			tempInstrument[strlen(tempInstrument) - 1] = '\0'; //To override '\n' included in fgets

		tempPtr = buildInstrumentNode(tempInstrument, index);
		tree.root = addInstrumentToTree(tree.root, tempPtr);
		index++;
	}
	tree.nodesAmount = index;

	free(tempInstrument);
	fclose(instrumentsFile);

	return tree;
}

InstrumentTree makeEmptyTree() {
	InstrumentTree tree;
	tree.root = NULL;
	tree.nodesAmount = 0;
	return tree;
}
//Creates a new tree node.
TreeNode* buildInstrumentNode(char* instrument, IDX index) {
		
	TreeNode* instNode = (TreeNode*)malloc(sizeof(TreeNode));
	checkMemoryAllocation(instNode);

	instNode->instrument = (char*)malloc((strlen(instrument) + 1) * sizeof(char));
	checkMemoryAllocation(instNode->instrument);

	strcpy(instNode->instrument, instrument);
	instNode->insId = (unsigned short)index;
	instNode->left = instNode->right = NULL;

	return instNode;
}
//Connects new tree node to its place in the tree.
TreeNode* addInstrumentToTree(TreeNode* root, TreeNode* currInst) { //Helper
	if (root == NULL) 
		root = currInst;
	else {
		if (isRootFirst(root->instrument, currInst->instrument))
			root->left = addInstrumentToTree(root->left, currInst);
		else
			root->right = addInstrumentToTree(root->right, currInst);
	}
	return root;
}
//The function compares the string in an existing node to the string in the new node and 
//returns true if the string in the root node goes first in an alphabetical order.
bool isRootFirst(char* root, char* instName) {
	IDX ch = 0;

	while (root[ch] == instName[ch])
		ch++;

	if ((int)root[ch] < (int)instName[ch])
		return true;
	else return false;

}

//******************************************QUESION2************************* 

//Searches for the id of a instrument received from the user and returns it.
int findInsId(InstrumentTree tree, char* instrument) {
	return findInsIdHelper(tree.root, instrument);
}
//Recursive helper function to findInsId.
int findInsIdHelper(TreeNode* root, char* instrument) {
	int id;
	if (root == NULL) 
		return -1;

	if (strcmp(root->instrument, instrument) == 0) 
		return root->insId;

	if (isRootFirst(root->instrument, instrument))
		id = findInsIdHelper(root->left, instrument);
	else 
		id = findInsIdHelper(root->right, instrument);

	return id;
}

//******************************************QUESION3*************************

//gets lines from musicians file and uses the data to create the data structure "musicianArchive".
Musician** buildMusicianArchive(char* musiciansFileName, InstrumentTree tree, int MusiciansAmount) { 
	IDX musician = 0;
	char* str = (char*)malloc(SIZE * sizeof(char));
	checkMemoryAllocation(str);
	Musician** MusiciansGroup = (Musician**)malloc(MusiciansAmount * sizeof(Musician*));
	checkMemoryAllocation(MusiciansGroup);

	FILE* musiciansFile = fopen(musiciansFileName, "r");
	checkFile(musiciansFile);

	const char* seps = " ,.;:?!-\t'()[]{}<>~_";
	
	while (fgets(str, SIZE, musiciansFile) != NULL) {
		if(str[strlen(str) - 1] == '\n') 
			str[strlen(str) - 1] = '\0'; //To override '\n' included in fgets

		MusiciansGroup[musician] = addMusicianToArchive(str, tree, seps);
		musician++;
	}

	free(str);
	fclose(musiciansFile);

	return MusiciansGroup;
}
//Adds a single musician to the archive.
Musician* addMusicianToArchive(char* musician, InstrumentTree tree, char* seps) {
	Musician* musicianPtr = makeEmptyMusician();
	bool isName = true;
	int ID;
	char* token = strtok(musician, seps);
	IDX i = 0;
	unsigned int logSize = 0;
	unsigned int physSize = MIN_MUSICIAN_NAME_NUM;
	MPINode* curr = musicianPtr->instruments.first;

	musicianPtr->name = (char**)malloc(physSize * sizeof(char*));
	checkMemoryAllocation(musicianPtr->name);

	while (isName) {
		ID = findInsId(tree, token);
		if (ID != INST_NOT_FOUND) isName = false;
		else {
			if (logSize == physSize) {
				physSize *= 2;
				musicianPtr->name = (char**)realloc(musicianPtr->name, physSize * sizeof(char*));
				checkMemoryAllocation(musicianPtr->name);
			}
			
			musicianPtr->name[i] = (char*)malloc((strlen(token)+1) * sizeof(char));
			checkMemoryAllocation(musicianPtr->name[i]);

			strcpy(musicianPtr->name[i], token);
			musicianPtr->name[i][strlen(token)] = '\0';

			token = strtok(NULL, seps);

			logSize++;
			i++;
		}
	}

	if (physSize != logSize)
		musicianPtr->name = (char**)realloc(musicianPtr->name, logSize * sizeof(char*));
	checkMemoryAllocation(musicianPtr->name);

	musicianPtr->namesAmount = logSize;

	while (token != NULL) {
		MPINode* node = makeEmptyNode();
		node->MPI.insId = (unsigned short)ID;

		token = strtok(NULL, seps);
		node->MPI.price = atof(token);

		insertNodeToEndList(&(musicianPtr->instruments), node);

		token = strtok(NULL, seps); //CHECKED
		if (token != NULL) ID = findInsId(tree, token);
	}

	return musicianPtr;
}
//makes a new, empty musician struct.
Musician* makeEmptyMusician() {
	Musician* musicianPtr = (Musician*)malloc(sizeof(Musician));
	checkMemoryAllocation(musicianPtr);

	musicianPtr->name = NULL;
	musicianPtr->instruments = makeEmptyMPIList();
	musicianPtr->playsInConcert = false; //Empty musicians don't participate in concerts

	return musicianPtr;
}
//makes a new, empty Musician Price Instrument list.
MPIList makeEmptyMPIList() {
	MPIList lst;
	lst.first = NULL;
	return lst;
}
//makes a new node.
MPINode* makeEmptyNode() {
	MPINode* nodePtr = (MPINode*)malloc(sizeof(MPINode));
	checkMemoryAllocation(nodePtr);

	nodePtr->next = NULL;
	return nodePtr;
}
//Inserts a node to the end of the list.
void insertNodeToEndList(MPIList* lst, MPINode* node) { //CHECKED
	if (lst->first == NULL)
		lst->first = lst->last = node;
	else {
		lst->last->next = node;
		lst->last = node;
	}
}

//******************************************QUESTION4*************************

//creates and allocates the musiciansCollection array.
Musician*** buildMusiciansCollection(int instrumentAmount, Musician** MusiciansGroup, int MusiciansAmount, MCS* MCSizes) {

	Musician*** MusiciansCollection = (Musician***)malloc(instrumentAmount * sizeof(Musician**));
	checkMemoryAllocation(MusiciansCollection);

	MusiciansCollection = fillMusiciansCollection(MusiciansGroup, MusiciansCollection, MCSizes, MusiciansAmount, instrumentAmount);

	return MusiciansCollection;
}
//The function runs on all musicians in musicianArchive and creates the matching pointers to them in musiciansCollection.
Musician*** fillMusiciansCollection(Musician** MusiciansGroup, Musician*** MusiciansCollection, MCS* MCSizes, int MusiciansAmount, int instrumentAmount) {
	IDX index;

	for (index = 0; index < MusiciansAmount; index++) 
		MusiciansCollection = addMusicianToCollection(MusiciansGroup[index], MusiciansCollection, MCSizes);
	
	for (index = 0; index < instrumentAmount; index++) {
		if ((MCSizes[index].logSize != MCSizes[index].physSize) && (MCSizes[index].physSize != 0)) {
			Musician** temp = (Musician**)realloc(MusiciansCollection[index], (MCSizes[index].logSize) * sizeof(Musician*));
			checkMemoryAllocation(temp);
			MusiciansCollection[index] = temp;
		}
		else if (MCSizes[index].physSize == 0)
			MusiciansCollection[index] = NULL;
	}
	return MusiciansCollection;
}
//Adds musician to collection using the MCSizes array (and sets the sizes for each array in the collection).
Musician*** addMusicianToCollection(Musician* musician, Musician*** MusiciansCollection, MCS* MCSizes) {
	unsigned short insId;
	MPINode* curr = musician->instruments.first;
	while (curr != NULL) {
		insId = curr->MPI.insId;
		if (MCSizes[insId].physSize == 0) {
			MCSizes[insId].physSize = 1;
			MusiciansCollection[insId] = (Musician**)malloc(sizeof(Musician*));
			checkMemoryAllocation(MusiciansCollection[insId]);
		}

		if (MCSizes[insId].logSize == MCSizes[insId].physSize) {
			MCSizes[insId].physSize *= 2;
			MusiciansCollection[insId] = (Musician**)realloc(MusiciansCollection[insId], MCSizes[insId].physSize * sizeof(Musician*));
			checkMemoryAllocation(MusiciansCollection[insId]);
		}

		MusiciansCollection[insId][MCSizes[insId].logSize] = musician;
	
		MCSizes[insId].logSize++;

		curr = curr->next;
	}

	return MusiciansCollection;
}
//Creates the MCSizes array that keeps track of the size of each array in the collection.
MCS* makeMCSArr(int instrumentAmount) {
	IDX index;
	MCS* MCSizes = (MCS*)malloc(instrumentAmount * sizeof(MCS));
	checkMemoryAllocation(MCSizes);

	for (index = 0; index < instrumentAmount; index++)
		MCSizes[index].logSize = MCSizes[index].physSize = 0;

	return MCSizes;
}

//******************************************QUESION5*************************

//Function that gets data for concerts and creates them.
void makeConcerts(Musician*** MusCollection, MCS* MCSizes, InstrumentTree tree) {
	char str[SIZE];
	char* token;
	Concert currConcert = makeEmptyConcert();
	
	printf("Please enter concerts. To stop, hit enter.\n");
	gets(str);
	token = str;

	while (str[0] != '\0') {
		currConcert.name = strtok(str, " ");

		token = addTimeToConcert(&currConcert, token);

		token = strtok(NULL, " ");

		while (token != NULL) {
			token = addConcertInstrumentToList(&(currConcert.instruments), token, tree);

			MusCollection[currConcert.instruments.last->inst] = sortArrOfMusicians(MusCollection[currConcert.instruments.last->inst], MCSizes[currConcert.instruments.last->inst].logSize, currConcert.instruments.last->inst, currConcert.instruments.last->importance);

			token = strtok(NULL, " ");
		}

		getMusiciansForConcert(&currConcert, MusCollection, MCSizes);

		gets(str);
	}
	//reminder- str is a static array and does not need to be freed
}
//Males an empty concert.
Concert makeEmptyConcert() {
	Concert concert;
	concert.name = (char*)malloc(SIZE * sizeof(char));
	checkMemoryAllocation(concert.name);
	concert.name[0] = '\0';
	concert.date_of_concert.hour = (float)concert.date_of_concert.day = concert.date_of_concert.month = concert.date_of_concert.year = 0;
	concert.instruments.first = concert.instruments.last = NULL;

	return concert;
}
//Gets the time for the concert and inserts the data into the date_of_concert struct.
char* addTimeToConcert(Concert* concert, char* token) {
	token = strtok(NULL, " ");
	concert->date_of_concert.day = atoi(token);
	token = strtok(NULL, " ");
	concert->date_of_concert.month = atoi(token);
	token = strtok(NULL, " ");
	concert->date_of_concert.year = atoi(token);
	token = strtok(NULL, " ");
	concert->date_of_concert.hour = getHourMinutesForConcert(token);

	return token;
}
//Gets the hours and minutes for the concert and converts them to a float in the concert struct.
float getHourMinutesForConcert(char* time) {
	//Format reminder: hh:mm
	//Indexes:         01234
	time[2] = '\0'; //changing ':' into '\0' to only receive the hour string
	int hour = atoi(time);
	int minutes = atoi(time + 3); //getting minutes string
	return hour + (minutes / MINS_PER_HOUR);
}
//Puts an instrument that is needed for the concert and its data into the list of instruments.
char* addConcertInstrumentToList(CIList* lst, char* token, InstrumentTree tree) {
	if (lst->first == NULL) {
		lst->first = lst->last = (ConcertInstrument*)malloc(sizeof(ConcertInstrument));
		checkMemoryAllocation(lst->last);
	}
	else {
		lst->last->next = (ConcertInstrument*)malloc(sizeof(ConcertInstrument));
		checkMemoryAllocation(lst->last->next);
		lst->last = lst->last->next;
	}

	lst->last->instName = (char*)malloc((strlen(token) + 1) * sizeof(char));
	checkMemoryAllocation(lst->last->instName);

	strcpy(lst->last->instName, token);
	lst->last->inst = findInsId(tree, token);
	token = strtok(NULL, " ");
	lst->last->num = atoi(token);
	token = strtok(NULL, " ");
	lst->last->importance = token[0];
	lst->last->next = NULL;

	lst->last->musArr = (InstPriceForMus*)malloc(lst->last->num * sizeof(InstPriceForMus));
	checkMemoryAllocation(lst->last->musArr);
	
	return token;
}
//Creates the pricesArr array, sorts musiciansCollection using it and frees it at the end of use.
Musician** sortArrOfMusicians(Musician** arrOfPtrs, unsigned int size, IDX instIdx, char importance) {
	InstPriceForMus* pricesArr = buildPricesArr(arrOfPtrs, size, instIdx);
	
	if (importance == IMPORTANT)
		qsort(pricesArr, size, sizeof(InstPriceForMus), sortMusiciansFromExpensiveToCheap);
	else
		qsort(pricesArr, size, sizeof(InstPriceForMus), sortMusiciansFromCheapToExpensive);

	arrOfPtrs = rearrangeArrOfPtrs(arrOfPtrs, pricesArr, size);

	free(pricesArr);
	return arrOfPtrs;
}
//Fills info into the pricesArr.
InstPriceForMus* buildPricesArr(Musician** arrOfPtrs, unsigned int size, IDX insId) {
	IDX idx;
	MPINode* curr;
	InstPriceForMus* pricesArr = (InstPriceForMus*)malloc(size * sizeof(InstPriceForMus));
	checkMemoryAllocation(pricesArr);
	

	for (idx = 0; idx < size; idx++) {


		pricesArr[idx].MusPtr = arrOfPtrs[idx];

		curr = arrOfPtrs[idx]->instruments.first; //Initializing curr to be first node

		while (curr->MPI.insId != insId) //a node with the ID has to be in the list or else the musician wouldn't be in the array.
			curr = curr->next;

		pricesArr[idx].price = curr->MPI.price;
	}
	
	return pricesArr;
}
//generic swap function between two MusicianPriceInstruments.
void swapInstruments(MusicianPriceInstrument* first, MusicianPriceInstrument* curr) {
	MusicianPriceInstrument temp = *first;
	*first = *curr;
	*curr = temp;
}
//sorting function for qsort.
int sortMusiciansFromCheapToExpensive(InstPriceForMus* mus1, InstPriceForMus* mus2) {
	if (mus1->price > mus2->price)
		return 1;
	if (mus1->price < mus2->price)
		return -1;
	else
		return 0;
}
//sorting function for qsort.
int sortMusiciansFromExpensiveToCheap(InstPriceForMus* mus1, InstPriceForMus* mus2) {
	if (mus1->price < mus2->price)
		return 1;
	if (mus1->price > mus2->price)
		return -1;
	else
		return 0;
}
//places the now sorted musicians back into the array.
Musician** rearrangeArrOfPtrs(Musician** arrOfPtrs, InstPriceForMus* pricesArr, unsigned int size) {
	IDX idx;

	for (idx = 0; idx < size; idx++)
		arrOfPtrs[idx] = pricesArr[idx].MusPtr;

	return arrOfPtrs;
}
//Chooses the musicians for the concert. If everything's okay, prints the concert. Else, gives an error message.
//Either way, the function resets the changed data to prepare for the next concert.
void getMusiciansForConcert(Concert* concert, Musician*** MusCollection, MCS* MCSizes) {
	ConcertInstrument* curr = concert->instruments.first;
	IDX musician, counter;
	bool error = false; //If there's an issue with assigning musicians
	while (curr != NULL && !error) { //loop on instruments in concert
		
		if (curr->num > MCSizes[curr->inst].logSize) { //If there aren't enough musicians to begin with
			error = true;
			initializeNextInstruments(curr); //To initiallize instruments amount for the reset func
		}
		else {
			for (musician = 0, counter = 0; counter != curr->num && musician < MCSizes[curr->inst].logSize; musician++) {
				if (!MusCollection[curr->inst][musician]->playsInConcert) {
					curr->musArr[counter].MusPtr = MusCollection[curr->inst][musician];
					curr->musArr[counter].price = findPrice(curr->musArr[counter].MusPtr, curr->inst);
					MusCollection[curr->inst][musician]->playsInConcert = true;
					counter++;
				}
			}

			if (counter != curr->num) {
				error = true; //This means we couldn't find enough musicians for the concert.
				curr->num = counter; //To override the unreached size for freeing the data
				initializeNextInstruments(curr->next); //To change actual amount of next instruments 
				//(if any) to 0, since they weren't filled due to exiting the loop.
			}
			else
				curr = curr->next;
		}
	}
	if (error == true)
		printf("Could not find musicians for the concert %s\n", concert->name);
	else {
		printConcert(*concert, MusCollection);
	}

	resetConcertData(concert);
}
//If creating a concert was unsuccessful, the musicians amount does not match the expected amount 
//and need to be initialized. 
void initializeNextInstruments(ConcertInstrument* curr) {
	while (curr != NULL) {
		curr->num = 0;
		curr = curr->next;
	}
}
//Prints the concert data
void printConcert(Concert concert, Musician*** musCollection) { //I have no idea if this works lol
	ConcertInstrument* curr = concert.instruments.first;
	IDX musician, name, instrument;
	float totalPrice = 0.0;
	int minutes = 60 * ((concert.date_of_concert.hour) - (int)(concert.date_of_concert.hour));
	
	printf("%s %02d %02d %d %2d:%02d: ", concert.name, concert.date_of_concert.day, concert.date_of_concert.month, concert.date_of_concert.year, (int)concert.date_of_concert.hour, minutes);

	while (curr != NULL) {
		for (musician = 0; musician < curr->num; musician++) {
			for (name = 0; name < curr->musArr[musician].MusPtr->namesAmount; name++) {
				printf("%s ", curr->musArr[musician].MusPtr->name[name]);
			}
			printf("- ");
			printf("%s (%d)", curr->instName, (unsigned int)(curr->musArr[musician].price));
			totalPrice += curr->musArr[musician].price;
			if (musician == curr->num - 1 && curr->next == NULL) printf(". ");
			else printf(", ");
		}
		curr = curr->next;
	}
	printf("Total cost: %d.\n", (unsigned int)totalPrice);
}
//Finds the price of the requested instrument from the musician.
float findPrice(Musician* musician, IDX instId) {
	MPINode* curr = musician->instruments.first;

	while (curr != NULL) {
		if (curr->MPI.insId == instId) return curr->MPI.price;
		curr = curr->next;
	}
	return -1.0; //Impossible to reach here because the instId has to be present in the list.
}
//Resets concert data for the next concert creation.
void resetConcertData(Concert* concert) {
	//Date of concert will be overriden with next concert data.
	//Name of concert is a pointer to str, no need to free it.
	freeCIList(&(concert->instruments));
}
//Frees the concert intrument list.
void freeCIList(CIList* lst) {
	if (lst->first != NULL) freeCIListHelper(lst->first);
	lst->first = lst->last = NULL;
}
//Recursive helper function for freeCIList.
void freeCIListHelper(ConcertInstrument* curr) {
	IDX elem; //InstPriceForMusician element in array
	if (curr == NULL) 
		return;

	freeCIListHelper(curr->next);

	for (elem = 0; elem < curr->num; elem++)
			((curr->musArr)[elem].MusPtr)->playsInConcert = false; //Reset status

	free(curr->musArr);
	free(curr->instName);
	free(curr);
}

//*****************************FREE ALL DATA FUNCTIONS**************************

//Frees all data structures before exiting the program.
void freeAllDataStructures(InstrumentTree tree, Musician** MusiciansGroup, unsigned int musiciansAmount, Musician*** MusiciansCollection, MCS* MCSizes) {
	freeMusiciansCollection(tree, MusiciansCollection, MCSizes);
	freeMusiciansGroup(MusiciansGroup, musiciansAmount);
	freeTree(tree);
}
//Frees the MusiciansCollection.
void freeMusiciansCollection(InstrumentTree tree, Musician*** MusCollection, MCS* MCSizes) {
	IDX idx;

	for (idx = 0; idx < tree.nodesAmount; idx++)
		free(MusCollection[idx]);

	free(MusCollection);
	free(MCSizes);
}
//Frees the MusiciansGroup.
void freeMusiciansGroup(Musician** MusiciansGroup, unsigned int musiciansAmount) {
	IDX musician, nameIdx;

	for (musician = 0; musician < musiciansAmount; musician++) {
		//free names
		for (nameIdx = 0; nameIdx < MusiciansGroup[musician]->namesAmount; nameIdx++)
			free(MusiciansGroup[musician]->name[nameIdx]);

		free(MusiciansGroup[musician]->name);
		// free list
		freeMPIList(&(MusiciansGroup[musician]->instruments));
		// free structs
		free(MusiciansGroup[musician]);
	}
	free(MusiciansGroup);
}
//Frees the MPIList.
void freeMPIList(MPIList* list) {
	freeMPIListHelper(list->first);
}
//Recursive free to list nodes.
void freeMPIListHelper(MPINode* curr) {
	if (curr == NULL)
		return;

	freeMPIListHelper(curr->next);
	free(curr);
}
//Frees the instruments tree.
void freeTree(InstrumentTree tree) {
	freeTreeHelper(tree.root);
}
//Recursive free to tree nodes.
void freeTreeHelper(TreeNode* root) {
	if (root == NULL) return;

	freeTreeHelper(root->left);
	freeTreeHelper(root->right);
	free(root->instrument);
	free(root);
}

//*****************************MEMORY & FILES CHECK**************************

//Checks if the malloc/realloc was successful.
void checkMemoryAllocation(void* ptr) {
	if (ptr == NULL) {
		printf("FATAL ERROR! MEMORY ALLOCATION FAILED!\n");
		exit(1);
	}
}
//Checks if the file was opened correctly.
void checkFile(FILE* filePtr) {
	if (filePtr == NULL) {
		printf("Failed opening the file. Exiting!\n");
		exit(1);
	}
}