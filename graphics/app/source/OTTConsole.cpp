#include "OTTConsole.hpp"
#include "OTTCharacterMap.hpp"

#include <core/OTTStringOps.hpp>
#include <core/OTTTextParser.hpp>
#include <graphics/core/OTTWindow.hpp>
#include <graphics/core/OTTImageBuffer.hpp>
#include <input/OTTKeyboard.hpp>

ott::Console::Console() :
	bEcho(false),
	nCols(20),
	nRows(18),
	nX(0),
	nY(0),
	strbuff(),
	line(),
	buffer(nRows, std::make_pair(std::string(nCols, ' '), true)),
	commands(),
	parser(),
	cmap(0x0),
	window(0x0)
{
}

ott::Console::Console(const unsigned short& W, const unsigned short& H) :
	bEcho(false),
	nCols(W),
	nRows(H),
	nX(0),
	nY(0),
	strbuff(),
	line(),
	buffer(nRows, std::make_pair(std::string(nCols, ' '), true)),
	commands(),
	parser(),
	cmap(0x0),
	window(0x0)
{
}

ott::Console::Console(const unsigned short& W, const unsigned short& H, CharacterMap* map) :
	bEcho(false),
	nCols(W),
	nRows(H),
	nX(0),
	nY(0),
	strbuff(),
	line(),
	buffer(nRows, std::make_pair(std::string(nCols, ' '), true)),
	commands(),
	parser(),
	cmap(map),
	window(0x0)
{
}

void ott::Console::Initialize() {
	this->AddConsoleCommand("cls",  0, -1, "", "Clear console screen");
	this->AddConsoleCommand("help", 0, -1, "[cmd]", "Print list of commands or syntax for (cmd)");
	this->AddConsoleCommand("echo", 0, -1, "", "Enable or disable echoing user input back to the console");
	this->AddConsoleCommand("quit", 0, -1, "", "Exit program");
	this->OnUserAddCommands();
	this->OnUserPrompt();
}

void ott::Console::Newline() {
	buffer.pop_front();
	this->Refresh(); // Redraw all lines
	buffer.push_back(std::make_pair(std::string(nCols, ' '), true));
	nX = 0;
}

void ott::Console::Put(const char& c) {
	if (nX >= nCols) { // New line
		this->Newline();
	}
	buffer.back().first[nX++] = c;
	if (nX >= nCols){
		this->Newline();
	}
	buffer.back().first[nX] = '_';
	buffer.back().second = true; // Redraw this line
}

void ott::Console::Put(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++) {
		this->Put(str[i]);
	}
}

void ott::Console::Unput() {
	if (nX <= 1) { // Do nothing (for now)
		return;
	}
	buffer.back().first[nX--] = ' ';
	buffer.back().first[nX] = '_';
	buffer.back().second = true; // Redraw this line
}

void ott::Console::Update() {
	// Poll the screen controller to check for button presses.
	Keyboard* keys = window->GetKeyboard();
	char keypress;
	while (keys->Get(keypress)) {
		this->Handle(keypress);
	}
}

void ott::Console::Draw(ImageBuffer* imgbuff, const unsigned short& x0/* = 0*/, const unsigned short& y0/* = 0*/) {
	if (!cmap)
		return;
	for (unsigned short y = 0; y < nRows; y++) {
		if (buffer[y].second) { // Do not redraw lines which have not changed
			cmap->DrawString(buffer[y].first, x0, y0 + y * cmap->CharacterHeight(), imgbuff);
			buffer[y].second = false;
		}
	}
}

void ott::Console::Refresh() {
	for (auto line = buffer.begin(); line != buffer.end(); line++)
		line->second = true;
}

void ott::Console::Print(ImageBuffer* imgbuff, const std::string& str, const unsigned short& x, const unsigned short& y, bool wrap/* = true*/) {
	if (x >= nCols || y >= nRows)
		return;
	if (x + str.length() < nCols) { // Entire string fits on a single line
		cmap->DrawString(
			str, 
			x * cmap->CharacterWidth(), 
			y * cmap->CharacterHeight(), 
			imgbuff
		);
	}
	else if (wrap){ // Print a portion of the string and then wrap to the next line
		unsigned short sx = x;
		unsigned short sy = y;
		size_t index = 0;
		while (index < str.length()) {
			cmap->DrawString(
				str.substr(index, nCols - sx), 
				sx * cmap->CharacterWidth(), 
				sy * cmap->CharacterHeight(), 
				imgbuff
			);
			index += (nCols - sx);
			sx = 0;
			if (++sy >= nRows) // Off bottom of screen
				return;
		}
	}
	else { // Print a portion of the input string
		cmap->DrawString(
			str.substr(0, nCols - x), 
			x * cmap->CharacterWidth(), 
			y * cmap->CharacterHeight(), 
			imgbuff
		);
	}
}

void ott::Console::Handle(const char& c, bool flag/*=true*/) {
	if (c == '\t') { // Tab
		for (unsigned short i = 0; i < (nX / 4 + 1) * 4 - nX; i++) {
			this->Put(' ');
		}
	}
	else if (c == '\n' || c == '\r') { // New line
		buffer.back().first[nX] = ' ';
		this->Newline();
		if (flag) { // User input
			this->HandleInput();
			this->OnUserPrompt();
		}
		line = "";
	}
	else if (c == 0x8) { // Backspace
		if (!line.empty()) { // Pop off the last character
			line.pop_back();
		}
		this->Unput();
	}
	else {
		line += c;
		this->Put(c);
	}
}

void ott::Console::HandleInput() {
	//handle user input commands
	if (bEcho) {
		(*this) << line << "\n";
	}
	std::vector<std::string> args;
	unsigned int nArgs = ott::SplitString(line, args, ' ');
	if (!nArgs)
		return;
	auto iter = commands.find(args.front()); // Search for valid command
	if (iter == commands.end()) { // Command not found		
		if (parser.StringIsExpression(line)) { // Mathematical expression
			NumericalString value;
			if (parser.Parse(line, value)) {
				if (value.type == NUMTYPE::INTEGER)
					(*this) << value.GetUInt() << "\n";
				else if (value.type == NUMTYPE::BOOLEAN)
					(*this) << (value.GetBoolean() ? "true" : "false") << "\n";
			}
			else {
				(*this) << "invalid syntax\n";
			}
			return;
		}
		else if (!this->OnUserUnknownCommand(args)){
			(*this) << "unknown command\n";
		}		
		return;
	}
	ConsoleCommand* cmd = &iter->second;
	if (nArgs - 1 < cmd->RequiredArgs()) {
		(*this) << "syntax error\n";
		(*this) << cmd->Name() << " " << cmd->ArgStr() << "\n";
		return;
	}
	if ((*cmd) == "cls") { // Clear the console
		this->Clear();
	}
	else if ((*cmd) == "help") { // Help dialog
		if (nArgs >= 2) { // User specified command to print syntax for
			for (auto allcmd = commands.cbegin(); allcmd != commands.cend(); allcmd++) {
				if (allcmd->first == args.at(1)) {
					(*this) << "syntax:\n";
					(*this) << " " << allcmd->second.Name() << " " << allcmd->second.ArgStr() << "\n";
					return;
				}
			}
			(*this) << "unknown command\n";
		}
		else {
			//clear();
			int rowCount = 0;
			for (auto allcmd = commands.cbegin(); allcmd != commands.cend(); allcmd++) {
				if (rowCount++ >= nRows - 1)
					continue;
				(*this) << allcmd->first << "\n";
			}
		}
	}
	else if ((*cmd) == "echo") { // Command input echo
		bEcho = !bEcho;
	}
	else if ((*cmd) == "quit") { // Exit program
		window->Close();
	}
	// Send command to derived class for processing
	this->OnUserHandleInput(cmd, args);
}

bool ott::Console::AddConsoleCommand(
	const std::string& name,
	const unsigned short& args,
	const int& cmdID,
	const std::string& argstr,
	const std::string& helpstr)
{
	auto cmd = commands.find(name);
	if (cmd != commands.end()) { // Command already exists
		return false;
	}
	commands[name] = ConsoleCommand(cmdID, name, args, argstr, helpstr);
	return true;
}

void ott::Console::Flush() {
	for (size_t i = 0; i < strbuff.length(); i++) {
		this->Handle(strbuff[i], false);
	}
	strbuff = "";
}

void ott::Console::Clear() {
	buffer.clear();
	for (unsigned short i = 0; i < nRows; i++) {
		buffer.push_back(std::make_pair(std::string(nCols, ' '), true));
	}
}

void ott::Console::OnUserPrompt() {
	this->Put('>');
}
