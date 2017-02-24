#ifndef COMMANDRUNNING_H
#define COMMANDRUNNING_H

#include <string>
#include <cstdlib>
#include <sstream>
#include <iostream>

std::string getOutput(std::string command) {
  FILE *fp;
  char buf[1024];

  std::stringstream output;

  fp = popen(command.c_str(), "r");
  if (fp == NULL) {
    std::cerr << "Failed to run command\n";
    exit(1);
  }

  while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
    output << buf;
  }

  /* close */
  pclose(fp);
  return output.str();
}

#endif // COMMANDRUNNING_H
