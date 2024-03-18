#include "Message.hpp"

Message::Message(std::string &chunk)
	: _is_valid(true), _prefix(""), _command(NONE), _middleParam(""), _trailingParam("")
	{
	//check the end(\r\n)
	if (check_crlf(chunk) == false)
		_is_valid = false;
	//check the prefix

	//check the command if it exists in cmd list

	//check param
	//check middle param
	//check trailing param

	//if everything is valid, set _is_valid to true
	//else set _is_valid to false

}

Message::~Message() {

}

bool check_crlf(std::string &chunk) {
	if (chunk.size() < 2
		|| !(chunk[chunk.size() - 2] == '\r' && chunk[chunk.size() - 1] == '\n'))
	{
		this->_is_valid = false;
		return false;
	}
	return true;
}
