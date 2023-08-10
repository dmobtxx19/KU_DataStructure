#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p; // # of permuterms
	int word_index = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	while (fscanf( fp, "%s", str) != EOF)
	{	
		trieInsert(permute_trie, str, word_index);
		num_p = make_permuterms( str, permuterms);
		
		for (int i = 0; i < num_p; i++)
			trieInsert( permute_trie, permuterms[i], word_index);
		
		clear_permuterms( permuterms, num_p);
		
		dic[word_index++] = strdup( str);
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 
		{
			trieSearchWildcard( permute_trie, str, dic);
		}
		// keyword search
		else 
		{
			ret = trieSearch( permute_trie, str);
			
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < word_index; i++)
		free( dic[i]);
	
	trieDestroy( permute_trie);
	
	return 0;
}


/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE* trieCreateNode(void) {
	TRIE* trie = (TRIE*)malloc(sizeof(TRIE));
	if (trie == NULL) return NULL;

	for (int i = 0; i < MAX_DEGREE; i++) {
		trie->subtrees[i] = NULL;
	}
	trie->index = -1;
	return trie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE* root) {
	if (root != NULL) {
		for (int i = 0; i < MAX_DEGREE; i++) {
			if (root->subtrees[i] != NULL)
				trieDestroy(root->subtrees[i]);
		}
		free(root);
	}
	return;
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE* root, char* str, int dic_index) {
	TRIE* r = root;

	for (int i = 0; i < strlen(str); i++) {
		if (getIndex(str[i]) < 0 || getIndex(str[i]) > MAX_DEGREE - 1) 
			return 0;
		
		if (r->subtrees[getIndex(str[i])] == NULL)
			r->subtrees[getIndex(str[i])] = trieCreateNode();
		r = r->subtrees[getIndex(str[i])];
	}

	r->index = dic_index;
	return 1;
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch(TRIE* root, char* str) {
	TRIE* r = root;

	for (int i = 0; i < strlen(str); i++) {
		if (r->subtrees[getIndex(str[i])] == NULL)
			return -1;
		r = r->subtrees[getIndex(str[i])];
	}

	return r->index;
}

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE* root, char* dic[]) {
	TRIE *r = root;

	if (root->index != -1) 
		printf("%s\n", dic[root->index]);

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (r->subtrees[i] != NULL)
			trieList(r->subtrees[i], dic);
	}
}

/* prints all entries starting with str (as prefix) in trie
	ex) "abb" -> "abbas", "abbasid", "abbess", ...
	this function uses trieList function
*/
void triePrefixList(TRIE* root, char* str, char* dic[]) {
	TRIE* r = root;
	
	for (int i = 0; i < strlen(str); i++) {
		
		if (r == NULL) return;
		r = r->subtrees[getIndex(str[i])];
	}
	
	if (r != NULL)
		trieList(r, dic);
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms(char* str, char* permuterms[]) {
	char c;
	char* tmp = (char*)malloc((strlen(str) + 2) * sizeof(char));
	strcpy(tmp, str);
	strcat(tmp, "$");

	for (int i = 0; i <= strlen(str); i++) {
		permuterms[i] = strdup(tmp);
		c = tmp[0];
		for (int j = 0; j < strlen(str); j++)
			tmp[j] = tmp[j + 1];
		tmp[strlen(str)] = c;
	}

	free(tmp);
	return strlen(str) + 1;
}

/* recycles memory for permuterms
*/
void clear_permuterms(char* permuterms[], int size) {
	for (int i = 0; i < size; i++) {
		free(permuterms[i]);
	}
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard(TRIE* root, char* str, char* dic[]) {
	char prefix[100];
	int index = -1, cnt = 0;

	for (int i = 0; i < strlen(str); i++) {
		if (str[i] == '*') {
			if (!cnt) { 
				cnt++;
				index = i;
			}
			else {
				cnt++;
				for (int j = 1; j < strlen(str) - 1; j++)
					prefix[j - 1] = str[j];
				prefix[strlen(str) - 2] = '\0';
				//strncpy(prefix, str + 1, strlen(str) - 3);
				break;
			}
		}
	}
		
	if (cnt == 1) {
	
		// strncpy(prefix, str + index, strlen(str) - index - 2);
		for (int j = index + 1; j < strlen(str); j++)
			prefix[j - index - 1] = str[j];
		
		prefix[strlen(str) - index - 1] = '$';
		//strcat(prefix, "$");
		
		// strncat(prefix, str, index);
		for (int j = 0; j < index; j++)
			prefix[strlen(str) - index + j] = str[j];
		
		prefix[strlen(str)] = '\0';
	}
	triePrefixList(root, prefix, dic);

	return;
	/*
	if (str[0] != '*' && str[strlen(str) - 1] == '*') {
		prefix[0] = EOW;
		for (int i = 0; i < strlen(str) - 1; i++)
			prefix[i + 1] = str[i];
	}
	else if (str[0] == '*' && str[strlen(str) - 1] != '*') {
		for (int i = 0; i < strlen(str) - 1; i++)
			prefix[i] = str[i + 1];
		prefix[strlen(str) - 1] = EOW;
	}
	else if (str[0] == '*' && str[strlen(str) - 1] == '*') {
		for (int i = 0; i < strlen(str) - 1; i++)
			prefix[i] = str[i + 1];
	}
	else {
		for (int i = 0; i < strlen(str); i++) {
			if()
		}
	}
	*/
}
