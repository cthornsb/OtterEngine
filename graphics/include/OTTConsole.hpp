#ifndef OTT_CONSOLE_HPP
#define OTT_CONSOLE_HPP

#include <string>
#include <sstream>
#include <deque>
#include <map>

#include "OTTTextParser.hpp"

class OTTWindow;

class OTTImageBuffer;

class OTTCharacterMap;

class ConsoleCommand {
public:
	ConsoleCommand() :
		nID(-1),
		sName(),
		sArgs(),
		sHelp(),
		nReqArgs(0)
	{
	}

	ConsoleCommand(const int& id, const std::string& name, const unsigned short& args, const std::string& argstr, const std::string& helpstr) :
		nID(id),
		sName(name),
		sArgs(argstr),
		sHelp(helpstr),
		nReqArgs(args)
	{
	}

	unsigned short getRequiredArgs() const {
		return nReqArgs;
	}

	bool operator == (const std::string& name) const {
		return (sName == name);
	}

	int getID() const {
		return nID;
	}

	std::string getName() const {
		return sName;
	}

	std::string getArgStr() const {
		return sArgs;
	}

	std::string getHelpStr() const {
		return sHelp;
	}

private:
	int nID;

	std::string sName;
	std::string sArgs;
	std::string sHelp;

	unsigned short nReqArgs;
};

class OTTConsole {
public:
	OTTConsole();

	OTTConsole(const unsigned short& W, const unsigned short& H);

	OTTConsole(const unsigned short& W, const unsigned short& H, OTTCharacterMap* map);

	template <typename T>
	OTTConsole& operator << (const T& val) {
		std::stringstream stream;
		stream << val;
		char c;
		while (stream.get(c)) {
			strbuff += c;
		}
		flush();
		return (*this);
	}

	/** Initialize all commands and print console prompt
	  */
	void initialize();

	/** Set the character map to use for text output 
	  */
	void setCharacterMap(OTTCharacterMap* map) {
		cmap = map;
	}

	/** Set the graphical window to use for text output
	  */
	void setOutputWindow(OTTWindow* win) {
		window = win;
	}

	/** Check the keyboard key buffer for new characters and push them into the console buffer
	  */
	void update();

	/** Draw all console lines which have been edited since the last draw onto an image buffer
	  */
	void draw(OTTImageBuffer* imgbuff, const unsigned short& x0 = 0, const unsigned short& y0 = 0);

	/** Redraw all lines of the console onto an image buffer
	  */
	void redraw(OTTImageBuffer* imgbuff, const unsigned short& x0 = 0, const unsigned short& y0 = 0);

protected:
	bool bEcho; ///< Set if command input will be echoed back

	unsigned short nCols; ///< Number of console columns

	unsigned short nRows; ///< Number of console rows

	unsigned short nX; ///< Current console column position

	unsigned short nY; ///< Current console row position

	std::string strbuff; ///< Console character stream buffer

	std::string line; ///< Currently active console line

	std::deque<std::pair<std::string, bool> > buffer; ///< Console row strings

	std::map<std::string, ConsoleCommand> commands; ///< Map of all console commands

	OTTTextParser parser; ///< Text input parser

	OTTCharacterMap* cmap; ///< Pointer to console font character bitmaps

	OTTWindow* window; ///< Pointer to output graphical window

	/** Push a single character into the input buffer
	  */
	void put(const char& c);

	/** Push a string of characters into the input buffer
	  */
	void put(const std::string& str);

	/** Remove the most recent character from the input buffer
	  */
	void unput();

	/** Add a new line string to the back of the console deque, popping the oldest string off the front
	  */
	void newline();

	/** Handle a single character of user input.
	  * Special characters include Tab (\t), New Line (\n), Carriage Return (\r), and Backspace (0x8); all
	  * other characters are output direclty to the console.
	  * @param flag If set, the current user input string will be processed and a new line will be added
	  */
	void handle(const char& c, bool flag = true);

	/** Handle a user input string
	  */
	void handleInput();

	/** Add a new console command
	  * @param name Console command string
	  * @param args Number of required command arguments (does not count optional args)
	  * @param cmdID Integer ID number associated with command
	  * @param argstr String to print for command argument syntax help (e.g. set <required> [optional])
	  * @param helpstr String to print for command help
	  * @return True if new command added successfully and return false if command with specified name already exists
	  */
	bool addConsoleCommand(		
		const std::string& name, 
		const unsigned short& args, 
		const int& cmdID,
		const std::string& argstr,
		const std::string& helpstr
	);

	/** Flush the current user input buffer to the console.
	  * The user input buffer will only be flushed and will not be processed.
	  */
	void flush();

	/** Clear all text from the console
	  */
	void clear();

	/** Add all commands to command map using addConsoleCommand() e.g.
	  * addConsoleCommand("help", 0, 0x0, "[cmd]", "Print list of commands or syntax for (cmd)");
	  */
	virtual void onUserAddCommands() {
	}

	/** Print a user prompt at the start of a new line
	  */
	virtual void onUserPrompt();

	/** Handle unknown commands.
	  * If false is returned, the console will print an "unknown command" message
	  */
	virtual bool onUserUnknownCommand(const std::vector<std::string>&) {
		return false;
	}

	/** 
	  */
	virtual void onUserHandleInput(ConsoleCommand* cmd, const std::vector<std::string>& args) {
	}
};

#endif // ifndef OTT_CONSOLE_HPP
