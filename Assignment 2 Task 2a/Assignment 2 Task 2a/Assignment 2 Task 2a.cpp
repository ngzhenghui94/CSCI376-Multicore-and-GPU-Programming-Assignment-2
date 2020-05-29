#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <regex>



int main() {

	//Create data struct for Host (Task 2a)
	std::vector<char> plainText;
	std::vector<char> cipherText;
	std::vector<char> decryptedText;

	//Our shift var
	signed int key;
	//Our plaintext filename.
	std::string fileName = "plaintext.txt";
	//Task 2a, file names for caesar cipher on Host/platform.
	std::string outputCipherTextHost = "cipherTextHost.txt";
	std::string outputDecryptedTextHost = "decryptedTextHost.txt";


	//Start of Assignment 2 Task 2a.
	//Caesar Cipher in Host
	//Read in pt file to plainText vector.
	std::cout << "Assignment 2 Task 2a: " << std::endl;
	std::ifstream plainTextFile;
	plainTextFile.open(fileName, std::ifstream::in);
	//char var to store and check each Character in the file.
	char getCharOne;
	if (plainTextFile.is_open()) {
		while (plainTextFile.get(getCharOne)) {
			if (getCharOne >= 'a' && getCharOne <= 'z') {
				getCharOne = getCharOne - 32;
				plainText.push_back(getCharOne);
			}
			else if (getCharOne >= 'A' && getCharOne <= 'Z') {
				plainText.push_back(getCharOne);
			}
			else if (isblank(getCharOne)) {
				plainText.push_back(' ');
			}
			else if (ispunct(getCharOne)) {
				plainText.push_back(getCharOne);
			}
			else if (isspace(getCharOne)) {
				plainText.push_back(getCharOne);
			}
			else if (isdigit(getCharOne)) {
				plainText.push_back(getCharOne);
			}
		}
		plainTextFile.close();
	}

	//Print out the plaintext we have read into plainText (vector)
	/*
	std::cout << "Plain text:" << std::endl;
	for (int i = 0; i < plainText.size(); i++) {
		std::cout << plainText[i];
	}
	std::cout << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	*/

	//Task 2a get user's shift value.
	std::cout << "Please enter the shift value: ";
	std::cin >> key;
	//Check input, shift should be at most 26.
	while (1) {
		if (std::cin.fail() || key >= 26) {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			std::cout << "Please enter a valid integer between 1 and 26. " << std::endl;
			std::cin >> key;
		}
		else {
			std::cin.clear();
			std::cin.ignore(INT_MAX, '\n');
			break;
		}
	}
	std::cout << "You have input a shift value of: " << key << std::endl;
	std::cout << "\n";

	//Task 2a caesar cipher encryption.
	std::cout << "Encrypting Plain Text on Host from: " << fileName << std::endl;
	//var to store and check each char in plainText vector and add it into cipherText vector (along with it's shift)
	char getCharTwo;
	for (int i = 0; i < plainText.size(); i++) {
		getCharTwo = plainText[i];
		if (getCharTwo >= 'a' && getCharTwo <= 'z') {
			getCharTwo = getCharTwo + key; //add to. (shifting)
			if (getCharTwo > 'z') {			//if shifted past z
				getCharTwo = getCharTwo - 'z' + 'a' - 1;
			}
			else if (getCharTwo < 'a') {   //if shifted to negative, wrap around
				getCharTwo = getCharTwo + 26;
			}
			cipherText.push_back(getCharTwo);
		}
		else if (getCharTwo >= 'A' && getCharTwo <= 'Z') {
			getCharTwo = getCharTwo + key; //add to. (shifting)
			if (getCharTwo > 'Z') {			//if shifted past Z
				getCharTwo = getCharTwo - 'Z' + 'A' - 1;
			}
			else if (getCharTwo < 'A') {   //if shifted to negative, wrap around
				getCharTwo = getCharTwo + 26;
			}
			cipherText.push_back(getCharTwo);
		}
		//treating the other characters as it is.
		else if (isblank(getCharTwo)) {
			cipherText.push_back(' ');
		}
		else if (ispunct(getCharTwo)) {
			cipherText.push_back(getCharTwo);
		}
		else if (isspace(getCharTwo)) {
			cipherText.push_back(getCharTwo);
		}
		else if (isdigit(getCharTwo)) {
			cipherText.push_back(getCharTwo);
		}
	}

	//Print out cipherText.
	/*
	std::cout << "Cipher text:" << std::endl;
	for (int i = 0; i < cipherText.size(); i++) {
		std::cout << cipherText[i];
	}
	std::cout << std::endl;
	*/
	//Saving cipher text for task 2a.
	std::ofstream myfile;
	myfile.open(outputCipherTextHost);
	for (int i = 0; i < cipherText.size(); i++) {
		myfile << cipherText[i];
	}
	std::cout << "Output Encrypted Text on Host to: " << outputCipherTextHost << std::endl;
	myfile.close();

	//Task 2a caesar cipher decryption.
	std::cout << "Decrypting Cipher Text on Host... " << std::endl;
	char getCharThird;
	for (int i = 0; i < cipherText.size(); i++) {
		getCharThird = cipherText[i];
		//Processing lower case chars
		if (getCharThird >= 'a' && getCharThird <= 'z') {
			getCharThird = getCharThird - key; //minus to. (shifting)
			if (getCharThird < 'a') {			//if shifted past a
				getCharThird = getCharThird + 'z' - 'a' + 1;
			}
			else if (getCharThird > 'z') {		//else if go past z, wrap ard
				getCharThird = getCharThird - 26;
			}
			decryptedText.push_back(getCharThird);
		}
		else if (getCharThird >= 'A' && getCharThird <= 'Z') {
			getCharThird = getCharThird - key; //minus to. (shifting)
			if (getCharThird < 'A') {			//if shifted past A
				getCharThird = getCharThird + 'Z' - 'A' + 1;
			}
			else if (getCharThird > 'Z') {		//else if go past Z, wrap
				getCharThird = getCharThird - 26;
			}
			decryptedText.push_back(getCharThird);
		}
		//treating the other characters as it is.
		else if (isblank(getCharThird)) {
			decryptedText.push_back(' ');
		}
		else if (ispunct(getCharThird)) {
			decryptedText.push_back(getCharThird);
		}
		else if (isspace(getCharThird)) {
			decryptedText.push_back(getCharThird);
		}
		else if (isdigit(getCharThird)) {
			decryptedText.push_back(getCharThird);
		}
	}

	//Saving decrypted text for task 2a.
	std::ofstream myfileTwo;
	myfileTwo.open(outputDecryptedTextHost);
	for (int i = 0; i < decryptedText.size(); i++) {
		myfileTwo << decryptedText[i];
	}
	std::cout << "Output Decrypted Text on Host to: " << outputDecryptedTextHost << std::endl;
	myfileTwo.close();
	
}
