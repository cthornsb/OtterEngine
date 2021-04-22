#include "OTTConsole.hpp"
#include "OTTWindow.hpp"
#include "OTTKeyboard.hpp"
#include "OTTCharacterMap.hpp"
#include "OTTImageBuffer.hpp"

OTTConsole::OTTConsole() :
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

OTTConsole::OTTConsole(const unsigned short& W, const unsigned short& H) :
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

OTTConsole::OTTConsole(const unsigned short& W, const unsigned short& H, OTTCharacterMap* map) :
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

void OTTConsole::initialize() {
	addConsoleCommand("cls", 0, -1, "", "Clear console screen");
	addConsoleCommand("help", 0, -1, "[cmd]", "Print list of commands or syntax for (cmd)");
	addConsoleCommand("echo", 0, -1, "", "Enable or disable echoing user input back to the console");
	addConsoleCommand("quit", 0, -1, "", "Exit program");
	this->onUserAddCommands();
	this->onUserPrompt();
}

void OTTConsole::newline() {
	buffer.pop_front();
	refresh(); // Redraw all lines
	buffer.push_back(std::make_pair(std::string(nCols, ' '), true));
	nX = 0;
}

void OTTConsole::put(const char& c) {
	if (nX >= nCols) // New line
		newline();
	buffer.back().first[nX++] = c;
	if (nX >= nCols)
		newline();
	buffer.back().first[nX] = '_';
	buffer.back().second = true; // Redraw this line
}

void OTTConsole::put(const std::string& str) {
	for (size_t i = 0; i < str.length(); i++) {
		put(str[i]);
	}
}

void OTTConsole::unput() {
	if (nX <= 1) { // Do nothing (for now)
		return;
	}
	buffer.back().first[nX--] = ' ';
	buffer.back().first[nX] = '_';
	buffer.back().second = true; // Redraw this line
}

void OTTConsole::update() {
	// Poll the screen controller to check for button presses.
	OTTKeyboard* keys = window->getKeypress();
	char keypress;
	while (keys->get(keypress)) {
		handle(keypress);
	}
}

void OTTConsole::draw(OTTImageBuffer* imgbuff, const unsigned short& x0/* = 0*/, const unsigned short& y0/* = 0*/) {
	if (!cmap)
		return;
	for (unsigned short y = 0; y < nRows; y++) {
		if (buffer[y].second) { // Do not redraw lines which have not changed
			cmap->drawString(buffer[y].first, x0, y0 + y * cmap->getCharacterHeight(), imgbuff);
			buffer[y].second = false;
		}
	}
}

void OTTConsole::refresh() {
	for (auto line = buffer.begin(); line != buffer.end(); line++)
		line->second = true;
}

void OTTConsole::printString(OTTImageBuffer* imgbuff, const std::string& str, const unsigned short& x, const unsigned short& y, bool wrap/* = true*/) {
	if (x >= nCols || y >= nRows)
		return;
	if (x + str.length() < nCols) { // Entire string fits on a single line
		cmap->drawString(
			str, 
			x * cmap->getCharacterWidth(), 
			y * cmap->getCharacterHeight(), 
			imgbuff
		);
	}
	else if (wrap){ // Print a portion of the string and then wrap to the next line
		unsigned short sx = x;
		unsigned short sy = y;
		size_t index = 0;
		while (index < str.length()) {
			cmap->drawString(
				str.substr(index, nCols - sx), 
				sx * cmap->getCharacterWidth(), 
				sy * cmap->getCharacterHeight(), 
				imgbuff
			);
			index += (nCols - sx);
			sx = 0;
			if (++sy >= nRows) // Off bottom of screen
				return;
		}
	}
	else { // Print a portion of the input string
		cmap->drawString(
			str.substr(0, nCols - x), 
			x * cmap->getCharacterWidth(), 
			y * cmap->getCharacterHeight(), 
			imgbuff
		);
	}
}

void OTTConsole::handle(const char& c, bool flag/*=true*/) {
	if (c == '\t') { // Tab
		for (unsigned short i = 0; i < (nX / 4 + 1) * 4 - nX; i++)
			put(' ');
	}
	else if (c == '\n' || c == '\r') { // New line
		buffer.back().first[nX] = ' ';
		newline();
		if (flag) { // User input
			handleInput();
			this->onUserPrompt();
		}
		line = "";
	}
	else if (c == 0x8) { // Backspace
		if (!line.empty()) // Pop off the last character
			line.pop_back();
		unput();
	}
	else {
		line += c;
		put(c);
	}
}

void OTTConsole::handleInput() {
	//handle user input commands
	if (bEcho) {
		(*this) << line << "\n";
	}
	std::vector<std::string> args;
	unsigned int nArgs = ott::splitString(line, args, ' ');
	if (!nArgs)
		return;
	auto iter = commands.find(args.front()); // Search for valid command
	if (iter == commands.end()) { // Command not found		
		if (parser.isExpression(line)) { // Mathematical expression
			NumericalString value;
			if (parser.parse(line, value)) {
				if (value.type == NUMTYPE::INTEGER)
					(*this) << value.getUInt() << "\n";
				else if (value.type == NUMTYPE::BOOLEAN)
					(*this) << (value.getBool() ? "true" : "false") << "\n";
			}
			else {
				(*this) << "invalid syntax\n";
			}
			return;
		}
		else if (!this->onUserUnknownCommand(args)){
			(*this) << "unknown command\n";
		}		
		return;
	}
	ConsoleCommand* cmd = &iter->second;
	if (nArgs - 1 < cmd->getRequiredArgs()) {
		(*this) << "syntax error\n";
		(*this) << cmd->getName() << " " << cmd->getArgStr() << "\n";
		return;
	}
	if ((*cmd) == "cls") { // Clear the console
		clear();
	}
	else if ((*cmd) == "help") { // Help dialog
		if (nArgs >= 2) { // User specified command to print syntax for
			for (auto allcmd = commands.cbegin(); allcmd != commands.cend(); allcmd++) {
				if (allcmd->first == args.at(1)) {
					(*this) << "syntax:\n";
					(*this) << " " << allcmd->second.getName() << " " << allcmd->second.getArgStr() << "\n";
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
		window->close();
	}
	// Send command to derived class for processing
	this->onUserHandleInput(cmd, args);
}

bool OTTConsole::addConsoleCommand(
	const std::string& name,
	const unsigned short& args,
	const int& cmdID,
	const std::string& argstr,
	const std::string& helpstr)
{
	auto cmd = commands.find(name);
	if (cmd != commands.end()) // Command already exists
		return false;
	commands[name] = ConsoleCommand(cmdID, name, args, argstr, helpstr);
	return true;
}

void OTTConsole::flush() {
	for (size_t i = 0; i < strbuff.length(); i++) {
		handle(strbuff[i], false);
	}
	strbuff = "";
}

void OTTConsole::clear() {
	buffer.clear();
	for (unsigned short i = 0; i < nRows; i++)
		buffer.push_back(std::make_pair(std::string(nCols, ' '), true));
}

void OTTConsole::onUserPrompt() {
	put('>');
}
