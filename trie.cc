#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
using namespace std;

/*
 *	Node Class
 */

class Node {
public:
	Node ();
	~Node ();
	vector< Node* > getChildren () { return children; }
	Node* findChild( char c );
	void addChild( Node* n );
	char getContent() { return content; }
	void setContent( char c ) { content = c; }
    bool checkIsWord() { return isWord; }
    void setWord( string s );
    string getWord();
private:
	char content;
	vector< Node* > children;	
	bool isWord;
    string word;
};


Node::Node() : isWord( false ) {}

Node::~Node() {
    for( vector< Node* >::iterator it = children.begin(); it != children.end(); it++ ) {
        delete *it;
    }
    children.clear();
}

Node* Node::findChild( char c ) {
	for( int i = 0; i < children.size(); i++ ) {
		Node* temp = children.at( i );
		if( temp->getContent() == c ) return temp;
	}
	return NULL;
}

void Node::addChild( Node* n ) {
	children.push_back( n );
}

void Node::setWord( string s ) {
    isWord = true;
    word = s;
}

string Node::getWord() {
    if( !isWord ) return "";
    return word;
}

/*
 * Trie class
 */

/*
 * Idea: Modified Trie
 * Keep a counter for each letter input (when adding a word) to show the number of times that letter appears in a row in the word
 * 
 */


class Trie {
public:
	Trie();
	~Trie();
	void addWord( string s );
	bool isWord( string s );
    void printAllWords();
    string checkSpelling( string s );
private:
	Node* root;	
    vector< string > words;
    vector< char > vowels;
};


Trie::Trie() {
	root = new Node();
    vowels.push_back( 'a' );
    vowels.push_back( 'e' );
    vowels.push_back( 'i' );
    vowels.push_back( 'o' );
    vowels.push_back( 'u' );
}


Trie::~Trie() {
    delete root;
}

bool Trie::isWord( string s ) {
    Node* temp = root;
    for( int i = 0; i < s.length(); i++ ) {
        Node* next = temp->findChild( s.at( i ) );
        if( next == NULL ) return false;
        temp = next;
        if( i == s.length() - 1 ) {
            if ( temp->checkIsWord() ) return true;
            else return false;
        }
    }
    return false;
}

void Trie::addWord( string s ) {
	Node* temp = root;
	if ( s.length() == 0 ) {
		temp->setWord( s );
		return;
    }	

	for ( int i = 0; i < s.length(); i++ ) {
		Node* child = temp->findChild( s.at( i ) );
		if( child != NULL ) temp = child;
		else {
			Node* node = new Node();
			node->setContent( s.at( i ) );
			temp->addChild( node );
            temp = node;
		}
		if( i == s.length() - 1 ) temp->setWord( s );
    }
    words.push_back( s );
}

void Trie::printAllWords() {
    for ( vector< string >::iterator it = words.begin(); it != words.end(); it++ ) {
        cout << (*it) << endl;
    }
}

bool isVowel( char c ) {
    if( c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ) return true;
    return false;
}

string Trie::checkSpelling( string s ) {
    if( isWord( s ) ) return s;
    vector< Node* > currentNodes;
    currentNodes.push_back( root );

    // for each letter
    for( int i = 0; i < s.length(); i++ ) {
        
        char currChar = s.at( i );
        vector< Node* > nextList;

        // for each node
        for( vector<Node*>::iterator it = currentNodes.begin(); it != currentNodes.end(); it++ ){
            
            Node* currNode = (*it);
            Node* child = NULL; 
            child = currNode->findChild( currChar );  // natural child, no moving around
           
            // if currChar is a vowel, it's a wildcard - can be ANY vowel
            if( isVowel( tolower( currChar ) ) ) {
                
                for( vector< char >::iterator iterator = vowels.begin(); iterator != vowels.end(); iterator++ ) {
                    char v = ( *iterator );             
                    
                    Node* lowerChild = currNode->findChild( v );
                    Node* upperChild = currNode->findChild( toupper( v ) ); 
            
                    if( lowerChild != NULL ) nextList.push_back( lowerChild );
                    if( upperChild != NULL ) nextList.push_back( upperChild );
            
                    if( i != 0 && isVowel( tolower( s.at( i - 1 ) ) ) ) {
                        nextList.push_back( currNode );
                    }
                }
            
            // not a vowel    
            } else {
                
                // case changes
                Node* caseChange = NULL;
                if( isupper( currChar ) ) {
                    caseChange = currNode->findChild( tolower( currChar ) );
                } else {
                    caseChange = currNode->findChild( toupper( currChar ) );
                }
           
                if( child != NULL ) nextList.push_back( child );
                if( caseChange != NULL ) nextList.push_back( caseChange );

                // repeated characters
                if( i != 0 && ( s.at( i - 1 ) == tolower( currChar ) || s.at( i - 1 ) == toupper( currChar ) ) ) {
                    nextList.push_back( currNode );
                }
            } 
        }
        currentNodes = nextList; 
    }
    for( vector< Node* >::iterator it = currentNodes.begin(); it != currentNodes.end(); it++ ) {
        if( (*it)->checkIsWord() ) return (*it)->getWord();
    }
    return "NO SUGGESTION";
} 
 

/*
 * Spellcheck Main Function
 */

int main() {
    Trie* trie = new Trie();
    ifstream wordFile( "/usr/share/dict/words" );
    string word;
    if( wordFile.is_open() ) {
        while( getline( wordFile, word ) ) {
            trie->addWord( word );    
        }
    }
    cout << "Enter words one at a time and press ENTER. Type QUIT to exit program" << endl; 
    string checkWord;
    cout << ">";
    while( cin >> checkWord ) {
        if ( checkWord == "QUIT" ) break;
        cout << trie->checkSpelling( checkWord ) << endl;
        cout << ">";
    }
    delete trie;
    return 0;
}
