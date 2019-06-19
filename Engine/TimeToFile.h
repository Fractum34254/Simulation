#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include "FrameTimer.h"

class TimeToFile
{
public:
	TimeToFile(std::string name)
	{
		filename = name;
		ft.Mark();
	}
	void Stop(std::string text)
	{
		float dt = ft.Mark();
		marks++;
		std::string content = "";
		std::ifstream fileIn(filename);
		if (fileIn)
		{
			std::stringstream buffer;
			buffer << fileIn.rdbuf();
			content = buffer.str();
		}
		fileIn.close();
		content += "Mark ";
		content += std::to_string(marks);
		content += ": ";
		content += text;
		content += ":\n";
		content += std::to_string(dt);
		content += "s\n";
		std::ofstream fileOut(filename);
		fileOut << content;
		fileOut.close();
		ft.Mark();
	}
	~TimeToFile()
	{
		Stop("Deconstructor");
		std::string content = "";
		std::ifstream fileIn(filename);
		if (fileIn)
		{
			std::stringstream buffer;
			buffer << fileIn.rdbuf();
			content = buffer.str();
		}
		fileIn.close();
		content += "------------------------------------------\n";
		std::ofstream fileOut(filename);
		fileOut << content;
		fileOut.close();
	}
private:
	FrameTimer ft;
	std::string filename;
	int marks = 0;
};
