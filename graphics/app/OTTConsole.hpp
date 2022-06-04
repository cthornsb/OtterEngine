#ifndef CORE_OTT_CONSOLE_HPP
#define CORE_OTT_CONSOLE_HPP

#include <core/OTTTextParser.hpp>

#include <deque>
#include <map>
#include <sstream>
#include <string>

namespace ott {

class Window;
class ImageBuffer;
class CharacterMap;

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

	bool operator == (const std::string& name) const {
		return (sName == name);
	}

	unsigned short RequiredArgs() const {
		return nReqArgs;
	}

	int Id() const {
		return nID;
	}

	std::string Name() const {
		return sName;
	}

	std::string ArgStr() const {
		return sArgs;
	}

	std::string HelpStr() const {
		return sHelp;
	}

private:
	int nID;

	std::string sName;
	std::string sArgs;
	std::string sHelp;

	unsigned short nReqArgs;
};

class Console {
public:
	Console();

	Console(const unsigned short& W, const unsigned short& H);

	Console(const unsigned short& W, const unsigned short& H, CharacterMap* map);

	template <typename T>
	Console& operator << (const T& val) {
		std::stringstream stream;
		stream << val;
		char c;
		while (stream.get(c)) {
			strbuff += c;
		}
		this->Flush();
		return (*this);
	}

	/** Initialize all commands and print console prompt
	  */
	void Initialize();

	/** Set the character map to use for text output 
	  */
	void SetCharacterMap(CharacterMap* map) {
		cmap = map;
	}

	/** Set the graphical window to use for text output
	  */
	void SetOutputWindow(Window* win) {
		window = win;
	}

	/** Check the keyboard key buffer for new characters and push them into the console buffer
	  */
	void Update();

	/** Draw all console lines which have been edited since the last draw onto an image buffer
	  */
	void Draw(ImageBuffer* imgbuff, const unsigned short& x0 = 0, const unsigned short& y0 = 0);

	/** Set all lines on the console to be redrawn during the next draw() call
	  */
	void Refresh();

	/** Print a string on the screen at a specified character row and column.
	  * Locations are indexed from column 0 and row 0 at top left of screen.
	  * If prameter wrap is set to true, text extending off the screen will wrap to a new line.
	  */
	void Print(ImageBuffer* imgbuff, const std::string& str, const unsigned short& x, const unsigned short& y, bool wrap = true);

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

	TextParser parser; ///< Text input parser

	CharacterMap* cmap; ///< Pointer to console font character bitmaps

	Window* window; ///< Pointer to output graphical window

	/** Push a single character into the input buffer
	  */
	void Put(const char& c);

	/** Push a string of characters into the input buffer
	  */
	void Put(const std::string& str);

	/** Remove the most recent character from the input buffer
	  */
	void Unput();

	/** Add a new line string to the back of the console deque, popping the oldest string off the front
	  */
	void Newline();

	/** Handle a single character of user input.
	  * Special characters include Tab (\t), New Line (\n), Carriage Return (\r), and Backspace (0x8); all
	  * other characters are output direclty to the console.
	  * @param flag If set, the current user input string will be processed and a new line will be added
	  */
	void Handle(const char& c, bool flag = true);

	/** Handle a user input string
	  */
	void HandleInput();

	/** Add a new console command
	  * @param name Console command string
	  * @param args Number of required command arguments (does not count optional args)
	  * @param cmdID Integer ID number associated with command
	  * @param argstr String to print for command argument syntax help (e.g. set <required> [optional])
	  * @param helpstr String to print for command help
	  * @return True if new command added successfully and return false if command with specified name already exists
	  */
	bool AddConsoleCommand(		
		const std::string& name, 
		const unsigned short& args, 
		const int& cmdID,
		const std::string& argstr,
		const std::string& helpstr
	);

	/** Flush the current user input buffer to the console.
	  * The user input buffer will only be flushed and will not be processed.
	  */
	void Flush();

	/** Clear all text from the console
	  */
	void Clear();

	/** Add all commands to command map using addConsoleCommand() e.g.
	  * addConsoleCommand("help", 0, 0x0, "[cmd]", "Print list of commands or syntax for (cmd)");
	  */
	virtual void OnUserAddCommands() {
	}

	/** Print a user prompt at the start of a new line
	  */
	virtual void OnUserPrompt();

	/** Handle unknown commands.
	  * If false is returned, the console will print an "unknown command" message
	  */
	virtual bool OnUserUnknownCommand(const std::vector<std::string>&) {
		return false;
	}

	/** 
	  */
	virtual void OnUserHandleInput(ConsoleCommand* cmd, const std::vector<std::string>& args) { }
};

} /* namespace ott */

#endif // #ifndef CORE_OTT_CONSOLE_HPP
