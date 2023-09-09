/********************************************************************
CSCI 480 - Assignment 1 - Semester Fall 2023
Programmer: Dominic Brooks
Section: 0001
TA: Sai Dinesh Reddy Bandi, Ajay Kuma Reddy Kandula, Yuva Krishna Thanneru
Date Due: 9/10/23
Purpose: Reads and parses various virtual files in the Linux /proc file system to gather detailed  
    information about the system it runs on. It provides human-readable answers to questions 
    related to the system's operating system, processors, uptime, CPU statistics, and swap device size.
*********************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <array>
#include <map>
#include <sstream>
#include <limits>
#include <cmath>
#include <unistd.h>

// Function to read and print the contents of a file
void readAndPrintFile(const std::string& filePath);

// Parse information from /proc/cpuinfo into an array of maps
std::array<std::map<std::string, std::string>, 8> parseCpuInfo();

// Convert seconds into a formatted time string
std::string convertSecondsToTimeString(double seconds);

// Print questions from section A
void printSectionA();

// Print questions from section B
void printSectionB();

// Print questions from section C
void printSectionC();

// Print questions from section D
void printSectionD();

// Print questions from section E
void printSectionE();

// Initialize and populate an array of maps with CPU information
std::array<std::map<std::string, std::string>, 8> parsedCpuInfo = parseCpuInfo();

int main() {
    std::cout << "A: Questions about OS:" << std::endl;
    printSectionA();

    std::cout << "B: Questions about processors:" << std::endl;
    printSectionB();

    std::cout << "C: Questions about processor 0:" << std::endl;
    printSectionC();

    std::cout << "D: Questions about processor 5:" << std::endl;
    printSectionD();

    std::cout << "E: Size of swap device in MB: ";
    printSectionE();

    return 0;
}

// Function to print information about the OS
void printSectionA() {
    // SECTION A ------------------------
    std::string ostypeFilePath = "/proc/sys/kernel/ostype";
    std::string hostnameFilePath = "/proc/sys/kernel/hostname";
    std::string osreleaseFilePath = "/proc/sys/kernel/osrelease";
    std::string versionFilePath = "/proc/sys/kernel/version";

    // Read and print the contents of each file
    std::cout << "1. ostype: ";
    readAndPrintFile(ostypeFilePath);
    std::cout << "2. hostname: ";
    readAndPrintFile(hostnameFilePath);
    std::cout << "3. osrelease: ";
    readAndPrintFile(osreleaseFilePath);
    std::cout << "4. version: ";
    readAndPrintFile(versionFilePath);
    // -----------------------------------
    std::cout << std::endl;
}

// Function to print information about processors
void printSectionB() {
    // SECTION B ------------------------
    std::string uptimeFilePath = "/proc/uptime";

    // Accessing and printing the data
    // Question 1. Number of processors
    std::cout << "1. num of processors: " << parsedCpuInfo.size() << std::endl;

    // Question 2. Number of multi-core chips
    int count = 0;
    for (const auto& cpuInfoMap : parsedCpuInfo) {
        auto physicalIdIter = cpuInfoMap.find("physical id");
        if (physicalIdIter != cpuInfoMap.end() && physicalIdIter->second == "1") {
            count++;
        }
    }
    std::cout << "2. num of physical multi-core chips: " << count << std::endl;

    // Question 3. Uptime in seconds
    // Get uptime from /proc/uptime
    std::ifstream file(uptimeFilePath);
    std::string line;

    // Get the first line from /proc/uptime
    std::getline(file, line);

    // Reading the formatted data into uptimeInSeconds
    std::istringstream iss(line);
    double uptimeInSeconds;

    // If the file isn't empty, read it into uptimeInSeconds and print it
    if (iss >> uptimeInSeconds) {
        std::cout << std::fixed << std::setprecision(2) << "3. uptime in seconds: " << uptimeInSeconds << std::endl;
    } else {
        std::cerr << "Failed to extract seconds." << std::endl;
    }

    // Question 4. Formatted uptime
    std::cout << "4. formatted uptime: ";
    std::cout << convertSecondsToTimeString(uptimeInSeconds) << std::endl;
    // -----------------------------------
    std::cout << std::endl;
}

// Function to convert seconds into a formatted time string
std::string convertSecondsToTimeString(double seconds) {
    double remainingSeconds = fmod(seconds, 60.0);
    double totalMinutes = (seconds - remainingSeconds) / 60.0;
    double minutes = fmod(totalMinutes, 60.0);
    double totalHours = (totalMinutes - minutes) / 60.0;
    double hours = fmod(totalHours, 24.0);
    double days = (totalHours - hours) / 24.0;

    std::ostringstream formattedTime;
    formattedTime << std::fixed << std::setprecision(0);
    if (days > 0) {
        formattedTime << days << " days, ";
    }
    if (hours > 0) {
        formattedTime << hours << " hours, ";
    }
    if (minutes > 0) {
        formattedTime << minutes << " minutes, ";
    }
    formattedTime << remainingSeconds << " seconds";

    return formattedTime.str();
}

// Function to parse information from /proc/cpuinfo into an array of maps
std::array<std::map<std::string, std::string>, 8> parseCpuInfo() {
    // SECTION B -------------------------
    std::string cpuinfoFilePath = "/proc/cpuinfo";
    std::ifstream file(cpuinfoFilePath);

    // -----------------------------------
    std::array<std::map<std::string, std::string>, 8> arrayOfCpuInfo;
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file /proc/cpuinfo" << std::endl;
        return arrayOfCpuInfo;
    }

    // -----------------------------------
    std::string line;
    int numProcessor = 0;
    while (std::getline(file, line)) {
        if (line.find("processor") != std::string::npos) {
            numProcessor++;
        } else {
            // Extract key and value pairs from each line
            size_t colonPos = line.find(":");
            std::string lineKey = line.substr(0, colonPos);
            std::string lineValue = line.substr(colonPos + 1); // Skip the colon itself

            // Remove leading and trailing spaces from lineKey and lineValue
            lineKey.erase(lineKey.find_last_not_of(" \t") + 1);
            lineKey.erase(0, lineKey.find_first_not_of(" \t"));
            lineValue.erase(lineValue.find_last_not_of(" \t") + 1);
            lineValue.erase(0, lineValue.find_first_not_of(" \t"));

            arrayOfCpuInfo[numProcessor - 1].insert(std::make_pair(lineKey, lineValue));
        }
    }

    // -----------------------------------
    file.close();
    // -----------------------------------
    return arrayOfCpuInfo;
}

// Function to print information about processor 0
void printSectionC() {
    // SECTION C ------------------------
    // Read and print processor-specific information

    std::cout << "1. vendor: " << parsedCpuInfo[0]["vendor_id"] << std::endl;
    std::cout << "2. model name: " << parsedCpuInfo[0]["model name"] << std::endl;

    // Parse address sizes to extract physical and virtual address sizes
    std::string addressSizes = parsedCpuInfo[0]["address sizes"];
    std::istringstream iss(addressSizes);

    int physicalSize, virtualSize;

    // Read the first number
    iss >> physicalSize;

    // Read the 'bits physical' part and discard it
    iss.ignore(std::numeric_limits<std::streamsize>::max(), ',');
    iss.ignore(std::numeric_limits<std::streamsize>::max(), ' ');

    // Read the second number
    iss >> virtualSize;

    if (iss.fail()) {
        std::cerr << "Error: Unable to extract numbers from the string." << std::endl;
    }

    std::cout << "3. physical address size: " << physicalSize << " bits" << std::endl;
    std::cout << "4. virtual address size: " << virtualSize << " bits" << std::endl;
    // -----------------------------------
    std::cout << std::endl;
}

// Function to print information about processor 5
void printSectionD() {
    // SECTION D ------------------------
    std::string statFilePath = "/proc/stat";

    // Read the content from the file
    std::ifstream file(statFilePath);

    if (!file.is_open()) {
        std::cerr << "Failed to open " << statFilePath << std::endl;
    }

    std::string line;

    // Search for the line that starts with "cpu5"
    while (std::getline(file, line)) {
        if (line.find("cpu5") == 0) {
            // Extract CPU statistics from the line
            std::istringstream line_stream(line);
            std::string cpu5;
            int userMode, niceMode, systemMode, idle;
            line_stream >> cpu5 >> userMode >> niceMode >> systemMode >> idle;

            // Print or store the values as needed
            std::cout << "1. seconds spent in user mode: " << static_cast<double>(userMode) / sysconf(_SC_CLK_TCK) << std::endl;
            std::cout << "2. seconds spent in system mode: " << static_cast<double>(systemMode) / sysconf(_SC_CLK_TCK) << std::endl;
            std::cout << "3. seconds spent idle: " << static_cast<double>(idle) / sysconf(_SC_CLK_TCK) << std::endl;

            double idleSeconds = static_cast<double>(idle) / sysconf(_SC_CLK_TCK);
            std::cout << "4. formatted time spent in idle: " << convertSecondsToTimeString(idleSeconds) << std::endl;
        }
    }

    // -----------------------------------
    std::cout << std::endl;
}

// Function to print the size of the swap device in MB
void printSectionE() {
    // SECTION E ------------------------
    std::string swapsFilePath = "/proc/swaps";
    std::ifstream file(swapsFilePath);

    // Check if the file is open before proceeding
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << swapsFilePath << std::endl;
        return;
    }

    // Read the first line and discard it
    std::string line;
    std::getline(file, line);

    // Read the second line
    if (std::getline(file, line)) {
        double swapSizeKb;
        std::istringstream iss(line);

        // Skip the first two columns (/dev/md2 and partition)
        std::string temp;
        iss >> temp >> temp;

        // Read in swapSizeKb
        if (iss >> swapSizeKb) {
            // Convert to MB and print
            std::cout << swapSizeKb / 1000 << std::endl;
        } else {
            std::cerr << "Failed to extract swap size from the second line." << std::endl;
        }
    } else {
        std::cerr << "Failed to read the second line from the file." << std::endl;
    }
}

// Function to read and print the contents of a file
void readAndPrintFile(const std::string& filePath) {
    std::ifstream file(filePath);

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
}