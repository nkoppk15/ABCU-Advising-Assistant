/*
* 
* Name: ABCUAdvisingAssistant.cpp
* Author: Nicholas Kopp
* Version : 1.0
* Description : This program will implement abstract data structures,
* specifically a binary search tree, to import data from a .csv file
* to sort and display course information for use in the ABCU Advising
* department.
* 
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_set>
#include <cctype>

using namespace std;

/*========================================================
* Global definitions
* ========================================================
*/

// Define structure for course information
struct Course {
	string courseNumber;
	string courseName;
	vector<string> prerequisites;

	// Default course constructor
	Course() {
		courseNumber = "";
		courseName = "";
	}
};

// Define Binary Search Tree node stucture with left and right node pointers.
struct Node {
	Course course;
	Node* left;
	Node* right;

	// Default node constructor
	Node() {
		left = nullptr;
		right = nullptr;
	}

	// Node constructor with a course
	Node(Course aCourse) :
		Node() {
			course = aCourse;
	}
};

/*---------------------------------------
* Class Definition for Binary Search Tree
----------------------------------------*/

/*
* Description: A class to hold attributes and methods
* necessary for BST implementation
*/

class BinarySearchTree {

private:
	Node* root;

	void inOrder(Node* node);
	void addNode(Node* node, const Course& course);
	void validateNode(Node* node, const unordered_set<string>& courseNumbers);
	void destroyTree(Node* node);

public:
	BinarySearchTree();
	~BinarySearchTree();
	void printCourses();
	void insert(const Course& course);
	void displayCourse(const Course& course);
	void validatePrerequisites(const unordered_set<string>& courseNumbers);
	Course search(const string& courseNumber);
};
	// Default Constructor

	BinarySearchTree::BinarySearchTree() {
		//Initialize root to nullptr
		root = nullptr;
	}

	BinarySearchTree::~BinarySearchTree() {
		destroyTree(root);
	}

	void BinarySearchTree::destroyTree(Node* node) {
		if (node == nullptr) {
			return;
		}

		destroyTree(node->left);
		destroyTree(node->right);
		delete node;
	}

	void BinarySearchTree::inOrder(Node* node) {
		// Tree traversal to visit each Course node and print in
		// alphanumeric order based on courseNumber using recursion

		if (node != nullptr) {
			// Recursive call starting with left node
			inOrder(node->left);
			// Display course information
			displayCourse(node->course);
			// Recursive call to right node
			inOrder(node->right);
		}
	}

	void BinarySearchTree::addNode(Node* node, const Course& course) {
		// Logic to traverse tree and find an empty node using
		// courseNumber for sorting starting with the root
		Node* current = node;

		if (course.courseNumber < current->course.courseNumber) {
			// If root course number is greater than current
			// insert course on left side if empty
			if (current->left == nullptr) {
				Node* newNode = new Node(course);
				current->left = newNode;
			}
			// If not empty, recursively traverse left node
			else {
				addNode(current->left, course);
			}
		}
		else {
			// If root course number is less than current
			// insert course on right side if empty
			if (current->right == nullptr) {
				Node* newNode = new Node(course);
				current->right = newNode;
			}
			// If not empty, recursively traverse right node
			else {
				addNode(current->right, course);
			}
		}
	}

	void BinarySearchTree::printCourses() {
		//Call inOrder print starting with root
		inOrder(root);
	}

	void BinarySearchTree::insert(const Course& course) {
		// Function to insert a new node holding a course into the
		// binary tree by traversing the tree starting with root

		// If root is empty, place new node in root
		if (root == nullptr) {
			Node* newNode = new Node(course);
			root = newNode;
		}
		// Otherwise, call addNode() to find insertion point
		else {
			addNode(root, course);
		}

	}

	void BinarySearchTree::displayCourse(const Course &course) {
		// Helper function to output formatted course information 
		// when using search() and printCourses()
		cout << left << setw(15) << course.courseNumber
			<< setw(45) << course.courseName;

		// If prerequisites is empty, print none 
		if (course.prerequisites.size() == 0) {
			cout << "None";
		}
		// Otherwise, use a for loop to iterate through all prerequisites
		// using a comma and space to separate fields until final index
		else {
			for (size_t i = 0; i < course.prerequisites.size(); i++) {
				cout << course.prerequisites[i];

				if (i < course.prerequisites.size() - 1)
					cout << ", ";
			}
		}	 
		// End each displayCourses() with a new line
		cout << endl;
	}

	Course BinarySearchTree::search(const string& courseNumber) {
		/*
		* Search function that traverses the tree to find a course
		* that matches user's input courseNumber and return course
		* information using displayCourse()
		* 
		*/

		// Start traversal with root
		Node* current = root;

		// While loop that compares input course number to current
		// and navigates left or right based on that comparison
		while (current != nullptr) {
			if (courseNumber == current->course.courseNumber) {
				return current->course;
			}
			if (courseNumber < current->course.courseNumber) {
				current = current->left;
			}
			else {
				current = current->right;
			}
		}

		// If no matching course is found, return an empty course
		Course emptyCourse;
		return emptyCourse;

	}

	void loadFile(const string& fileName, BinarySearchTree& bst) {

		// Function to open a provided .csv file and import course information
		// into the binary search tree data structure based on courseNumber

		// Initialize file input
		ifstream inputFile;
		
		// Attempt to open the file (file name currently declared in main)
		inputFile.open(fileName);
		
		// Error handling for file that cannot be opened
		if (!inputFile.is_open()) {
			throw runtime_error("ERROR: Cannot open file.");
		}

		// Validation message to communicate successful file opening to user
		cout << "File loaded successfully. \nImporting course list from \"" << fileName << "\".\n" << endl;
		// Initialize unordered set to hold all course numbers for reference during validation
		unordered_set<string> allCourseNumbers;

		/*
		*  Using getline to pull lines from the .csv file and break them into 'cells' using
		* a comma as the delimiter. Local vector rowData holds each field and adds
		* the information to a new Course object. If > 2 fields, add to course.prerequisites.
		*/

		string line;
		while (getline(inputFile, line)) {
			stringstream ss(line);
			string cell;
			vector<string> rowData;

			while (getline(ss, cell, ',')) {
				rowData.push_back(cell);
			}
			
			Course newCourse;
			newCourse.courseNumber = rowData[0];
			newCourse.courseName = rowData[1];

			if (rowData.size() > 2) {
				for (int i = 2; i < rowData.size(); i++) {
					
					//Check if row is empty to prevent inaccurate prerequiste elements
					if (!rowData[i].empty()) {
						newCourse.prerequisites.push_back(rowData[i]);
					}
				}
			}
			
			// Insert each course number into unordered set for validation
			allCourseNumbers.insert(rowData[0]);
			// Pass new course to insert() to add to the tree
			bst.insert(newCourse);
		}

		// Run validation check to ensure all prerequisites exist in course list
		bst.validatePrerequisites(allCourseNumbers);
	}

	void BinarySearchTree::validatePrerequisites(const unordered_set<string>& courseNumbers) {
		// Call validateNode() to traverse the tree from root checking each node's prerequisites
		validateNode(root, courseNumbers);
	}

	void BinarySearchTree::validateNode(Node* node, const unordered_set<string>& courseNumbers) {

		/*
		* This function uses in order traversal to check the prerequisites of each
		* node->course against the list of allCourseNumbers using find. If a match
		* is not found, submit an error message including the errant prerequisite.
		*/


		if (node == nullptr) {
			return;
		}

		validateNode(node->left, courseNumbers);

		// Enhanced for loop using find from #algorithm to compare each prerequisite string
		// passed by reference
		for (const string& prerequisite : node->course.prerequisites) {
			string target = prerequisite;
			
			if (courseNumbers.find(target) == courseNumbers.end()) {
				cout << "Error: " << prerequisite << " not found in course list. " << endl;
			}

		}
		validateNode(node->right, courseNumbers);
	}


	void mainMenu(const string& fileName, BinarySearchTree& bst) {

		/*
		* This function serves as a main menu that gives the user a number of options
		* using a switch and integer menuSelection. This menu will loop while menuSelection
		* is not 9 (exit). The binary search tree and file name are passed by reference.
		*/


		int menuSelection;
		string prereqSearch;
		Course result;

		// Do/While ensures the loop runs until the user chooses 9 to exit
		do {
			cout << "Welcome to the ABCU Advising Assistant!" << endl;
			cout << "Please select from the following options:" << endl;
			cout << "\n1: Load File" << endl;
			cout << "2: Print All Courses" << endl;
			cout << "3: Search Course Prerequisites" << endl;
			cout << "9: Exit" << endl;

			cout << "\nMake a selection: ";
			cin >> menuSelection;
			cout << endl;
			switch (menuSelection) {

			case 1:

				// Calls loadFile() to import the file provided by ABCU. 
				cout << "Attempting to import information from file...\n" << endl;
				loadFile(fileName, bst);
				break;

			case 2:

				// Formatted string with header to print course list using printCourses()
				cout << "Printing courses...." << endl;
				cout << left << setw(15) << "Course Number"
					<< setw(45) << "Course Name"
					<< setw(20) << "Prerequisites" << endl;
				bst.printCourses();
				cout << endl;
				break;

			case 3:

				// Search for a course using toupper for input validation
				cout << "Enter a course number to view prerequisites: ";
				cin >> prereqSearch;

				for (char& c : prereqSearch) {
					c = toupper(c);
				}

				// Call search to find course based on courseNumber
				result = bst.search(prereqSearch);

				// If search() returns empty course with sentinel value, error message prints
				if (result.courseNumber == "") {
					cout << "Course not found. Please try again." << endl;
				}

				else {
					bst.displayCourse(result);
				}
				cout << endl;
				break;

			case 9:

				//Print exit message and break do/while loop to exit program
				cout << "Thank you for using our software! Goodbye!" << endl;
				break;


			default:

				// Default error message to account for any input not in the switch
				cout << "Invalid menu selection. Please review the options and try again!" << endl;
			}
			
			

		} while (menuSelection != 9);
	}

	int main() {

		/*
		* Driver function to initialize file name and Binary Search Tree
		* object. Call to main menu where all function calls are made. 
		*/

		string csvFileName = "CS 300 ABCU_Advising_Program_Input.csv";
		BinarySearchTree bst;

		mainMenu(csvFileName, bst);

		return 0;
	}


