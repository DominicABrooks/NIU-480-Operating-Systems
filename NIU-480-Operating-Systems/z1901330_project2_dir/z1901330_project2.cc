/********************************************************************
CSCI 480 - Assignment 2 - Semester Fall 2023
Programmer: Dominic Brooks
Section: 0001
TA: Sai Dinesh Reddy Bandi, Ajay Kuma Reddy Kandula, Yuva Krishna Thanneru
Date Due: 9/27/23
Purpose: Practice the use of fork() and several other system calls to implementa microshell in C/C++, and practice FCFS CPU scheduling algorithm.
*********************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/**
 * Executes a command specified by a vector of strings representing tokens.
 *
 * @param tokens A vector of strings representing the command tokens.
 */
void execute_command(const std::vector<std::string>& tokens);

/**
 * Redirects program output to a specified file if a redirection operator '>' is found in the tokens.
 *
 * @param tokens A vector of strings representing the command tokens.
 */
void redirect_output(const std::vector<std::string>& tokens);


/**
 * Simulates First-Come-First-Served (FCFS) CPU scheduling for a given number of processes.
 *
 * @param tokens A vector of strings representing the command tokens.
 */
void fcfs_simulation(const std::vector<std::string>& tokens);

int main() {
    // Start an infinite loop for the shell
    while (true) {
        try {
            // Print the shell prompt
            std::cout << "myshell> ";
            
            // Read user input into a string
            std::string user_input;
            std::getline(std::cin, user_input);

            // Handle empty input by continuing to the next iteration
            if (user_input.empty()) {
                continue;
            }

            // Tokenize the user input
            std::istringstream iss(user_input);
            std::vector<std::string> tokens;
            std::string token;

            // Split the user input into tokens and store them in a vector
            while (iss >> token) {
                tokens.push_back(token);
            }

            // Check for special commands
            if (tokens[0] == "quit" || tokens[0] == "q") {
                // If the user entered 'quit' or 'q', exit the shell
                break;
            } else if (tokens[0] == "fcfs") {
                // If the user entered 'fcfs', call the fcfs_simulation function
                fcfs_simulation(tokens);
            } else {
                // If it's not a special command, execute the entered command
                execute_command(tokens);
            }
        } catch (const std::exception& e) {
            // Handle exceptions and display error messages
            std::cerr << e.what() << std::endl;
        }
    }

    // Exit the shell
    return 0;
}


void execute_command(const std::vector<std::string>& tokens) {
    // Create a child process
    pid_t pid = fork();

    // Check for errors in fork()
    if (pid == -1) {
        std::cerr << "Fork Error" << std::endl;
    } else if (pid == 0) {
        // Child process
        // Handle output redirection if the '>' operator is present in tokens
        redirect_output(tokens);

        // Create a vector to store the command arguments
        std::vector<char*> args;

        // Iterate through tokens and construct the argument list, excluding redirection
        for (const std::string& token : tokens) {
            // If the token starts with '>', stop processing (redirection handled earlier)
            if (token.find(">") == 0) {
                break;
            }

            // Add the token as a C-style string to the argument list
            args.push_back(const_cast<char*>(token.c_str()));
        }

        // Add a nullptr at the end of the argument list to mark the end
        args.push_back(nullptr);

        // Execute the command specified by the argument list
        execvp(args[0], args.data());

        // If execvp() fails, print an error message and exit the child process
        std::cerr << "Couldn't execute: " << args[0] << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;

        // Wait for the child process to complete
        waitpid(pid, &status, 0);
    }
}

void redirect_output(const std::vector<std::string>& tokens) {
    int output_fd = STDOUT_FILENO;
    bool found_redirect = false;

    // Iterate through the 'tokens' vector using an index 'i'
    for (size_t i = 0; i < tokens.size(); ++i) {
        // Check if the current token starts with '>'
        if (tokens[i].find(">") == 0) {
            // If there are more tokens after the current one
            if (i + 1 < tokens.size()) {
                // Get a reference to the next token
                const std::string& next_token = tokens[i + 1];

                // Open the specified file for writing
                output_fd = open(next_token.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (output_fd == -1) {
                    std::cerr << "Couldn't open: " << next_token.c_str() << std::endl;
                    exit(EXIT_FAILURE);
                }

                // Set a flag to indicate that a redirection operator was found and exit the loop
                found_redirect = true;
                break;
            } else {
                // Get the part of the token that follows '>'
                const std::string& output_filename = tokens[i].substr(1);

                // Check if the extracted filename is empty
                if (output_filename.empty()) {
                    // Error message for a missing output file name
                    std::cerr << "Missing output file name after '>'." << std::endl;
                    exit(EXIT_FAILURE);
                }

                // Open the specified file for writing
                output_fd = open(output_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (output_fd == -1) {
                    std::cerr << "Couldn't open: " << output_filename.c_str() << std::endl;
                    exit(EXIT_FAILURE);
                }

                // Set a flag to indicate that a redirection operator was found and exit the loop
                found_redirect = true;
                break;
            }
        }
    }

    // If redirection was requested and the output file descriptor is not the standard output (STDOUT_FILENO)
    if (found_redirect && output_fd != STDOUT_FILENO) {
        // Duplicate the output file descriptor to standard output (STDOUT_FILENO)
        // This redirects program output to the specified file
        dup2(output_fd, STDOUT_FILENO);

        // Close the original output file descriptor since it's no longer needed
        close(output_fd);
    }
}

void fcfs_simulation(const std::vector<std::string>& tokens) {
    // Initialize the number of processes to the default value
    int num_processes = 5;

    // Check if a number of processes is specified in tokens
    if (tokens.size() > 1) {
        // Check if the second token is not a redirection token
        if (tokens[1] != ">") {
            try {
                // Attempt to parse the second token as the number of processes
                num_processes = std::stoi(tokens[1]);
            } catch (const std::exception& e) {
                // Handle the exception if parsing fails
                std::cerr << "Invalid number of processes." << std::endl;
                return; // Return from the function in case of an error
            }
        }
    }

    // Seed the random number generator
    srand(10);

    // Create a vector to store burst times for each process
    std::vector<int> burst_times(num_processes);

    // Store the original stdout file descriptor
    int original_stdout = dup(STDOUT_FILENO);

    // Check for output redirection
    std::string output_file;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == ">") {
            if (i + 1 < tokens.size()) {
                // Get the output file name after the ">" token
                output_file = tokens[i + 1];
                break;
            }
        }
    }

    // If an output file is specified, open it for writing
    if (!output_file.empty()) {
        int output_fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (output_fd == -1) {
            std::cerr << "Couldn't open: " << output_file.c_str() << std::endl;
            exit(EXIT_FAILURE);
        }

        // Redirect stdout to the output file
        dup2(output_fd, STDOUT_FILENO);

        // Close the output file descriptor since stdout is redirected
        close(output_fd);
    }

    // Input burst times for each process
    for (int i = 0; i < num_processes; ++i) {
        burst_times[i] = rand() % 100 + 1;
    }

    // Print a message indicating the start of the simulation
    std::cout << "FCFS CPU scheduling simulation with " << num_processes << " processes" << std::endl;

    // Initialize variables for tracking total waiting time and current time
    int total_waiting_time = 0;
    int current_time = 0;

    // Simulate the execution of each process
    for (int i = 0; i < num_processes; ++i) {
        std::cout << "Executing Process " << i + 1 << " (Burst Time: " << burst_times[i] << " ms)" << std::endl;

        // Calculate waiting time for the current process
        int waiting_time = current_time;
        total_waiting_time += waiting_time;

        // Update current time with the burst time of the current process
        current_time += burst_times[i];
    }

    // Calculate and display the average waiting time
    double average_waiting_time = static_cast<double>(total_waiting_time) / num_processes;

    // Print the total and average waiting times
    std::cout << "Total waiting time in the ready queue: " << total_waiting_time << " ms" << std::endl;
    std::cout << "Average waiting time in the ready queue: " << average_waiting_time << " ms" << std::endl;

    // Reset the output file descriptor to stdout
    dup2(original_stdout, STDOUT_FILENO);

    // Close the duplicated file descriptor for the original stdout
    close(original_stdout);
}
