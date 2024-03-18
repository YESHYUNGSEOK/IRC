#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <string>
# include <iostream>
# include <vector>
# include "CommandHandler.hpp"

class Message {
	public:
		Message(std::string &chunk);
		~Message();

		const std::string		&get_prefix() const;
		const std::string		&get_command() const;
		const std::string		&get_middleParam() const;
		const std::string		&get_trailingParam() const;
		const bool				is_valid() const;

	private:
		bool					_is_valid;
		std::string				_prefix;
		e_cmd					_command;
		std::string				_middleParam;
		std::string				_trailingParam;

		bool					check_crlf(std::string &chunk);
		bool					check_prefix(std::string &chunk);
		bool					check_command(std::string &chunk);
		bool					check_param(std::string &chunk);
		bool					check_middle_param(std::string &chunk);
		bool					check_trailing_param(std::string &chunk);
};

# endif
